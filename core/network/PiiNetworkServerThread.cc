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

#include "PiiNetworkServerThread.h"
#include <QIODevice>
#include <QCoreApplication>

PiiNetworkServerThread::Data::Data(PiiNetworkProtocol* protocol) :
  pProtocol(protocol->clone()),
  bOwnProtocol(protocol != pProtocol),
  pController(0),
  requestCondition(PiiWaitCondition::Queue),
  bRunning(false), bInterrupted(false),
  iMaxIdleTime(10000)
{}

PiiNetworkServerThread::PiiNetworkServerThread(PiiNetworkProtocol* protocol) :
  d(new Data(protocol))
{}

PiiNetworkServerThread::~PiiNetworkServerThread()
{
  stop();
  wait();
  
  if (d->bOwnProtocol)
    delete d->pProtocol;
}

void PiiNetworkServerThread::startRequest(PiiGenericSocketDescriptor socketDescriptor)
{
  d->socketDescriptor = socketDescriptor;

  // If the thread is not running, start it.
  if (!d->bRunning)
    {
      d->requestCondition.wakeAll();
      d->bRunning = true;
      d->bInterrupted = false;
      start();
    }

  // Wake it up.
  d->requestCondition.wakeOne();
}

void PiiNetworkServerThread::stop(PiiNetwork::StopMode mode)
{
  d->bRunning = false;
  d->bInterrupted = mode == PiiNetwork::InterruptClients;
  d->requestCondition.wakeOne();
}

bool PiiNetworkServerThread::canContinue(double /*percentage*/) const
{
  return !d->bInterrupted;
}

void PiiNetworkServerThread::run()
{
  while (d->bRunning)
    {
      // Wait for a wake-up signal. If we got no new clients within
      // the time limit, kill the thread.
      if (!d->requestCondition.wait(d->iMaxIdleTime))
        break;
      
      if (!d->bRunning)
        break;

      QIODevice* pSocket = d->pController->createSocket(d->socketDescriptor);
      if (pSocket != 0)
        {
          d->pProtocol->communicate(pSocket, this);
          delete pSocket;
        }

      // We are done with the client. Tell mama.
      d->pController->threadAvailable(this);
    }

  d->pController->threadFinished(this);
}

void PiiNetworkServerThread::setController(Controller* controller) { d->pController = controller; }
PiiNetworkServerThread::Controller* PiiNetworkServerThread::controller() const { return d->pController; }
void PiiNetworkServerThread::setMaxIdleTime(int maxIdleTime) { d->iMaxIdleTime = maxIdleTime; }
int PiiNetworkServerThread::maxIdleTime() const { return d->iMaxIdleTime; }
