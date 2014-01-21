/* This file is part of Into.
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#include "PiiNetworkServer.h"
#include "PiiNetworkServerThread.h"
#include <QIODevice>

PiiNetworkServer::Data::Data(PiiNetworkProtocol* protocol) :
  iMinWorkers(0), iMaxWorkers(10),
  iWorkerMaxIdleTime(20),
  iMaxPendingConnections(0),
  aBusyMessage("Server busy\n"),
  pProtocol(protocol),
  state(Stopped)
{}

PiiNetworkServer::Data::~Data()
{}

PiiNetworkServer::PiiNetworkServer(PiiNetworkProtocol* protocol) :
  d(new Data(protocol))
{
  // Deletes finished threads.
  startTimer(1000);
}

PiiNetworkServer::PiiNetworkServer(Data* data) :
  d(data)
{}

PiiNetworkServer::~PiiNetworkServer()
{
  QList<PiiNetworkServerThread*> lstThreads;
  synchronized (d->threadListLock)
    {
      d->state = Stopping;
      lstThreads = d->lstAllThreads;
      for (int i=0; i<lstThreads.size(); ++i)
        lstThreads[i]->stop(PiiNetwork::InterruptClients);
    }

  waitAll(lstThreads);
  qDeleteAll(lstThreads);

  delete d;
}

bool PiiNetworkServer::start()
{
  QMutexLocker lock(&d->threadListLock);

  if (d->state == Running)
    return true;
  if (d->state == Stopping)
    return false;

  d->lstPendingConnections.clear();

  // Add threads to the pool.
  while (d->lstFreeThreads.size() < d->iMinWorkers)
    {
      PiiNetworkServerThread *newWorker = createWorker(d->pProtocol);
      newWorker->setController(this);
      d->lstFreeThreads << newWorker;
    }

  if (startListening())
    {
      d->state = Running;
      return true;
    }

  return false;
}

bool PiiNetworkServer::isRunning() const { return d->state == Running; }

void PiiNetworkServer::waitAll(const QList<PiiNetworkServerThread*>& threads)
{
  for (int i=0; i<threads.size(); ++i)
    threads[i]->wait();
}

bool PiiNetworkServer::stop(PiiNetwork::StopMode mode)
{
  QList<PiiNetworkServerThread*> lstThreads;
  synchronized (d->threadListLock)
    {
      if (d->state != Running)
        return false;

      d->state = Stopping;
      stopListening();

      // Copy the thread list
      lstThreads = d->lstAllThreads;
      // Send stop signal to each
      for (int i=0; i<lstThreads.size(); ++i)
        lstThreads[i]->stop(mode);
    }

  // Wait until all threads are done. We can't use d->lstAllThreads here
  // because it is modified by threadFinished().
  waitAll(lstThreads);

  synchronized (d->threadListLock)
    {
      qDeleteAll(d->lstAllThreads);
      d->lstAllThreads.clear();
      d->lstFinishedThreads.clear();
      d->lstFreeThreads.clear();
      d->state = Stopped;
    }
  return true;
}

void PiiNetworkServer::deleteFinishedThreads()
{
  synchronized (d->threadListLock)
    {
      for (int i=0; i<d->lstFinishedThreads.size(); ++i)
        {
          d->lstFinishedThreads[i]->wait();
          delete d->lstFinishedThreads[i];
        }
      d->lstFinishedThreads.clear();
    }
}

void PiiNetworkServer::timerEvent(QTimerEvent*)
{
  deleteFinishedThreads();
}

// Add thread to the list of idle threads.
void PiiNetworkServer::threadAvailable(PiiNetworkServerThread* worker)
{
  QMutexLocker lock(&d->threadListLock);

  if (d->state != Running) return;

  // If there is a pending connection, start serving it immediately.
  if (d->lstPendingConnections.size() > 0)
    worker->startRequest(d->lstPendingConnections.dequeue());
  else
    // If there are no pending connections, add the thread to the list
    // of free threads.
    d->lstFreeThreads << worker;
}

void PiiNetworkServer::threadFinished(PiiNetworkServerThread* worker)
{
  QMutexLocker lock(&d->threadListLock);

  if (d->lstAllThreads.size() > d->iMinWorkers)
    {
      d->lstFinishedThreads << worker;
      // Take it out of our thread lists.
      d->lstFreeThreads.removeOne(worker);
      d->lstAllThreads.removeOne(worker);
    }
}

void PiiNetworkServer::incomingConnection(PiiGenericSocketDescriptor socketDescriptor)
{
  QMutexLocker lock(&d->threadListLock);

  //piiDebug("Connection attempt.");

  if (d->state != Running) return;

  // If at least one thread is available, use it.
  if (d->lstFreeThreads.size() > 0)
    {
      //piiDebug("Picked an idle thread");
      PiiNetworkServerThread *worker = d->lstFreeThreads.takeLast();
      worker->startRequest(socketDescriptor);
    }
  // If no free threads are available, and we are still allowed to
  // create a new one, do it.
  else if (d->lstAllThreads.size() < d->iMaxWorkers)
    {
      //piiDebug("Created a new thread");
      // Stateful protocol needs to be cloned to maintain state
      // separately for each client.
      PiiNetworkServerThread *newWorker = createWorker(d->pProtocol);
      newWorker->setController(this);
      newWorker->startRequest(socketDescriptor);
      d->lstAllThreads << newWorker;
    }
  // No more threads, please. Are we allowed to queue pending connections?
  else if (d->lstPendingConnections.size() < d->iMaxPendingConnections)
    {
      //piiDebug("Queuing the connection.");
      /* It may happen that a connection is first accepted and placed
         to the list of pending connections. If the client then
         happens to get tired of waiting, it may go away. If the
         server is extremely busy, it may happen that the same socket
         descriptor is allocated to another client. We must thus check
         that the descriptor isn't already in our list.
      */
      if (!d->lstPendingConnections.contains(socketDescriptor))
        d->lstPendingConnections.enqueue(socketDescriptor);
    }
  // No luck this time.
  else
    {
      //piiWarning(tr("Too many concurrent clients."));
      serverBusy(socketDescriptor);
    }
}

PiiNetworkServerThread* PiiNetworkServer::createWorker(PiiNetworkProtocol* protocol)
{
  return new PiiNetworkServerThread(protocol);
}

void PiiNetworkServer::serverBusy(PiiGenericSocketDescriptor socketDescriptor)
{
  QIODevice* dev = createSocket(socketDescriptor);
  dev->write(d->aBusyMessage);
  delete dev;
}

void PiiNetworkServer::setMinWorkers(int minWorkers) { if (minWorkers >= 0 && minWorkers < 1000) d->iMinWorkers = minWorkers; }
int PiiNetworkServer::minWorkers() const { return d->iMinWorkers; }
void PiiNetworkServer::setMaxWorkers(int maxWorkers) { if (maxWorkers > 0 && maxWorkers < 1000) d->iMaxWorkers = maxWorkers; }
int PiiNetworkServer::maxWorkers() const { return d->iMaxWorkers; }
void PiiNetworkServer::setWorkerMaxIdleTime(int workerMaxIdleTime) { d->iWorkerMaxIdleTime = workerMaxIdleTime; }
int PiiNetworkServer::workerMaxIdleTime() const { return d->iWorkerMaxIdleTime; }
void PiiNetworkServer::setMaxPendingConnections(int maxPendingConnections) { d->iMaxPendingConnections = maxPendingConnections; }
int PiiNetworkServer::maxPendingConnections() const { return d->iMaxPendingConnections; }
void PiiNetworkServer::setBusyMessage(const QString& busyMessage) { d->aBusyMessage = busyMessage.toUtf8(); }
QString PiiNetworkServer::busyMessage() const { return QString::fromUtf8(d->aBusyMessage.constData(), d->aBusyMessage.size()); }
PiiNetworkProtocol* PiiNetworkServer::protocol() const { return d->pProtocol; }
