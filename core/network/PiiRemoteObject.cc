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

#include "PiiRemoteObject.h"

#include "PiiNetworkException.h"
#include "PiiHttpProtocol.h"
#include "PiiMultipartDecoder.h"
#include "PiiStreamBuffer.h"

#include <PiiAsyncCall.h>
#include <PiiSerializationUtil.h>
#include <PiiGenericTextInputArchive.h>
#include <PiiGenericTextOutputArchive.h>
#include <PiiInvalidArgumentException.h>

#include <QUrl>
#include <QUuid>

#include "PiiNetworkEncoding.h"
#include "PiiQObjectServer.h"

PiiRemoteObject::Data::Data() :
  pHttpDevice(0),
  pChannelThread(0),
  bChannelRunning(false),
  iRetryCount(3),
  iRetryDelay(2000),
  iMaxFailureCount(-1),
  strClientId(QUuid::createUuid().toString()),
  pLocalServer(0)
{}

PiiRemoteObject::PiiRemoteObject() : d(new Data)
{}

PiiRemoteObject::PiiRemoteObject(const QString& serverUri) : d(new Data)
{
  try { setServerUriImpl(serverUri); } catch (...) { delete d; throw; }
}

PiiRemoteObject::PiiRemoteObject(Data* data) : d(data)
{}

PiiRemoteObject::PiiRemoteObject(Data* data, const QString& serverUri) : d(data)
{
  try { setServerUriImpl(serverUri); } catch (...) { delete d; throw; }
}

PiiRemoteObject::~PiiRemoteObject()
{
  // Explicitly close the channel on server side.
  synchronized (d->channelMutex)
    if (d->bChannelRunning)
      {
        closeChannel();
        try { call<void>(QString("channels/%1/delete").arg(d->strChannelId)); } catch (...) {}
      }

  delete d->pHttpDevice;
  for (int i = 0; i < d->lstCallbacks.size(); ++i)
    delete d->lstCallbacks[i].second;
  delete d;
}

// PENDING this should use a global pool of connections to avoid
// establishing a new one for each remote object on the same server.
PiiRemoteObject::HttpDevicePtr PiiRemoteObject::openConnection()
{
  HttpDevicePtr pDev(&d->deviceMutex);
  // If the server is in the same process and this call is being made
  // from the main thread, we could deadlock otherwise...
  if (d->pLocalServer &&
      QThread::currentThread() == qApp->thread())
    {
      d->buffer.close();
      d->buffer.setData(QByteArray());
      d->buffer.open(QIODevice::ReadWrite);
      // This isn't actually possible currently because we can't know
      // if the server is local before contacting it through the
      // network first. But if we later add support e.g. for saving
      // and restoring client state, we may be able to contact a local
      // server directly at the first attempt.
      if (!d->pHttpDevice)
        d->pHttpDevice = new PiiHttpDevice(&d->buffer, PiiHttpDevice::Client);
      else if (d->pHttpDevice->device() != &d->buffer)
        d->pHttpDevice->setDevice(&d->buffer);
    }
  else
    {
      if (unsigned(d->iFailureCount.load()) > unsigned(d->iMaxFailureCount))
        PII_THROW(PiiNetworkException, tr("Maximum number of failures reached."));

      QIODevice* pSocket = 0;
      for (int iTry = 0; iTry <= d->iRetryCount; ++iTry)
        {
          pSocket = d->networkClient.openConnection();
          if (pSocket != 0)
            break;
          else if (iTry != d->iRetryCount)
            PiiDelay::msleep(d->iRetryDelay);
        }
      if (pSocket == 0)
        {
          addFailure();
          PII_THROW(PiiNetworkException,
                    tr("Connection to the server object at %1 could not be established.").arg(serverUri()));
        }
      if (d->pHttpDevice == 0)
        d->pHttpDevice = new PiiHttpDevice(pSocket, PiiHttpDevice::Client);
      else if (d->pHttpDevice->device() != pSocket)
        d->pHttpDevice->setDevice(pSocket);
    }

  // Add an X-Client-ID header to all outgoing requests.
  d->pHttpDevice->setHeader("X-Client-ID", d->strClientId);
  return pDev = d->pHttpDevice;
}

void PiiRemoteObject::closeConnection()
{
  QMutexLocker lock(&d->deviceMutex);
  d->networkClient.closeConnection();
}

void PiiRemoteObject::finishRequest(PiiHttpDevice* dev)
{
  dev->finish();
  // If the server is actually in the same process and we are in the
  // main thread, avoid a deadlock by letting it handle the request
  // directly.
  if (d->pLocalServer &&
      QThread::currentThread() == qApp->thread())
    {
      // This is the size of the request in bytes.
      qint64 iPos = d->buffer.pos();
      // Position the buffer at the start of the HTTP request.
      d->buffer.seek(0);
      // Create a http device for the server's side of the protocol.
      PiiProgressController controller;
      PiiHttpProtocol::TimeLimiter limiter(&controller, INT_MAX);
      PiiHttpDevice serverDevice(&d->buffer, PiiHttpDevice::Server);
      serverDevice.readHeader(); // Should be OK, since we formatted the header.
      d->pLocalServer->handleRequest(d->strPath, &serverDevice, &limiter);
      serverDevice.finish();
      // Position the buffer at the start of the server's response.
      d->buffer.seek(iPos);
    }
}

QList<QByteArray> PiiRemoteObject::readDirectoryList(const QString& path)
{
  // Try twice
  for (int iTry = 0; iTry < 2; ++iTry)
    {
      HttpDevicePtr pDev = openConnection();

      pDev->setRequest("GET", d->strPath + path);
      finishRequest(pDev);

      if (!pDev->isReadable())
        continue;

      if (!pDev->readHeader())
        {
          addFailure();
          PII_THROW(PiiNetworkException, tr("Couldn't receive a response header."));
        }
      if (pDev->status() != PiiHttpProtocol::OkStatus)
        {
          addFailure();
          PII_THROW(PiiNetworkException, tr("Server responded with status code %1.").arg(pDev->status()));
        }

      return pDev->readBody().split('\n');
    }
  return QList<QByteArray>();
}

QString PiiRemoteObject::addCallback(const QString& name, PiiGenericFunction* function)
{
  try
    {
      connectToChannel("callbacks/" + function->signature(name));
    }
  catch (...)
    {
      delete function; // PENDING ?
      throw;
    }

  d->lstCallbacks << qMakePair(name, function);
  return function->signature(name);
}

void PiiRemoteObject::removeCallback(const QString& signature)
{
  for (int i = 0; i < d->lstCallbacks.size(); ++i)
    if (d->lstCallbacks[i].second->signature(d->lstCallbacks[i].first) == signature)
      {
        try { disconnectFromChannel("callbacks/" + signature); }
        catch (PiiException& ex)
          {
            addFailure();
            piiWarning(ex.message());
          }

        delete d->lstCallbacks[i].second;
        d->lstCallbacks.removeAt(i);
        return;
      }
}

void PiiRemoteObject::removeCallbacks()
{
  for (int i = 0; i < d->lstCallbacks.size(); ++i)
    {
      try { disconnectFromChannel("callbacks/" + d->lstCallbacks[i].second->signature(d->lstCallbacks[i].first)); }
      catch (PiiException& ex)
        {
          addFailure();
          piiWarning(ex.message());
        }
      delete d->lstCallbacks[i].second;
    }
  d->lstCallbacks.clear();
}

void PiiRemoteObject::connectToChannel(const QString& sourceId)
{
  if (!d->lstConnectedSources.contains(sourceId))
    {
      manageChannel("connect", sourceId); // may throw
      d->lstConnectedSources << sourceId;
    }
}

void PiiRemoteObject::disconnectFromChannel(const QString& sourceId)
{
  manageChannel("disconnect", sourceId); // may throw
  d->lstConnectedSources.removeOne(sourceId);
}

void PiiRemoteObject::manageChannel(const QString& operation, const QString& sourceId)
{
  openChannel(); // may throw
  QString strUri = "channels/" + d->strChannelId + '/' + operation + '?' +
    QUrl::toPercentEncoding(sourceId);
  call<void>(strUri); // may throw
}

void PiiRemoteObject::openChannel()
{
  QMutexLocker lock(&d->channelMutex);

  if (d->pChannelThread)
    return;

  closeChannel();
  d->strChannelId.clear();

  d->pChannelThread = Pii::createAsyncCall(this, &PiiRemoteObject::readChannel);
  d->pChannelThread->start();
  d->channelUpCondition.wait(&d->channelMutex);
  if (!d->bChannelRunning)
    {
      lock.unlock();
      d->pChannelThread->wait();
      lock.relock();
      delete d->pChannelThread;
      d->pChannelThread = 0;
      addFailure();
      PII_THROW(PiiNetworkException, tr("Failed to request a return channel from %1.").arg(d->networkClient.serverAddress()));
    }
}

// channelMutex must be held when calling this function
void PiiRemoteObject::closeChannel()
{
  d->bChannelRunning = false;
  if (d->pChannelThread != 0)
    {
      d->channelMutex.unlock();
      d->pChannelThread->wait();
      d->channelMutex.lock();
      delete d->pChannelThread;
      d->pChannelThread = 0;
    }
}

bool PiiRemoteObject::canContinue(double) const
{
  return d->bChannelRunning;
}

bool PiiRemoteObject::requestNewChannel(PiiHttpDevice& dev)
{
  dev.setRequest("GET", d->strPath + "channels/new");
  dev.finish();

  if (!dev.readHeader() || !checkChannelResponse(dev))
    return false;

  // Read preamble (should contain the channel ID)
  QByteArray aChannelId = dev.readLine();
  while (aChannelId.size() > 0 && char(aChannelId[aChannelId.size()-1]) member_of ('\n', '\r'))
    aChannelId.chop(1);

  if (aChannelId.isEmpty())
    {
      piiWarning(tr("Could not read channel ID from input stream."));
      return false;
    }

  d->strChannelId = aChannelId;
  return true;
}

bool PiiRemoteObject::reconnectSources()
{
  try
    {
      QStringList lstOldSources = d->lstConnectedSources;
      d->lstConnectedSources.clear();
      foreach (QString strSource, lstOldSources)
        connectToChannel(strSource); // may throw
      return true;
    }
  catch (PiiException& ex)
    {
      piiCritical(ex.message());
    }
  return false;
}

void PiiRemoteObject::readChannel()
{
  QMutexLocker lock(&d->channelMutex);

  PiiNetworkClient networkClient(d->networkClient.serverAddress());
  PiiSocketDevice pSocket = networkClient.openConnection();

  if (pSocket == 0)
    {
      d->channelUpCondition.wakeOne();
      return;
    }

  PiiHttpDevice dev(pSocket, PiiHttpDevice::Client);
  // We are going to accept any number of bytes
  dev.setMessageSizeLimit(0);

  if (!requestNewChannel(dev))
    {
      d->channelUpCondition.wakeOne();
      return;
    }

  dev.setController(this);

  // Now we are finally up and running
  d->bChannelRunning = true;
  d->channelUpCondition.wakeOne();
  lock.unlock();

  forever
    {
      PiiMultipartDecoder decoder(&dev, dev.responseHeader());
      while (d->bChannelRunning && dev.isReadable())
        {
          try
            {
              // Fetch next message
              if (!decoder.nextMessage()) // may throw
                continue;

              // Read and decode body contents
              QByteArray aBody = decoder.readAll();
              /*piiDebug("Received message (%d bytes), read %d to %s",
                       decoder.header().contentLength(),
                       aBody.size(), piiPrintable(decoder.header().value("X-ID")));
              */
              decodePushedData(decoder.header().value("X-ID"), aBody);
            }
          catch (PiiException& ex)
            {
              // Oops, couldn't decode server's message. Let's try
              // to restore the situation.
              piiWarning(ex.location("", ": ") + ex.message());
              networkClient.closeConnection();
              break;
            }
        }
      if (!d->bChannelRunning)
        break;
      if (!dev.isReadable())
        {
          piiWarning("Lost connection to server's return channel. Trying to reconnect.");
          // Try to reconnect
          for (int iTry = 0; iTry <= d->iRetryCount; ++iTry)
            {
              if (iTry != 0)
                PiiDelay::msleep(d->iRetryDelay);

              QMutexLocker channelLock(&d->channelMutex);
              networkClient.setServerAddress(d->networkClient.serverAddress());
              pSocket = networkClient.openConnection();

              if (pSocket != 0)
                {
                  dev.setDevice(pSocket);
                  dev.setRequest("GET", d->strPath + "channels/" + d->strChannelId);
                  dev.finish();
                  if (dev.readHeader())
                    break;
                  else
                    {
                      networkClient.closeConnection();
                      pSocket = 0;
                    }
                }
            }
          if (pSocket == 0)
            {
              piiCritical(tr("Reconnecting to %1 failed.").arg(d->networkClient.serverAddress()));
              addFailure();
              break;
            }

          // Couldn't reconnect to the lost channel. Try to create a
          // new one and reconnect all connected sources to it.
          if (!checkChannelResponse(dev))
            {
              piiWarning(tr("Could not reconnect to the old channel. Trying to create a new one."));
              if (requestNewChannel(dev))
                {
                  if (!reconnectSources())
                    {
                      d->lstConnectedSources.clear();
                      piiCritical(tr("Could not reconnect registered sources."));
                      addFailure();
                      break;
                    }
                  piiDebug(tr("New return channel created successfully."));
                }
              else
                {
                  piiCritical(tr("Could not create a new channel."));
                  addFailure();
                  break;
                }
            }
        }
    }

  synchronized (d->channelMutex) d->bChannelRunning = false;
}

bool PiiRemoteObject::checkChannelResponse(PiiHttpDevice& dev)
{
  if (dev.status() != PiiHttpProtocol::OkStatus)
    {
      piiWarning(tr("Cannot set up a channel. Remote server responded with status code %1.").arg(dev.status()));
      return false;
    }
  else if (!dev.responseHeader().contentType().startsWith("multipart/"))
    {
      piiWarning(tr("Cannot set up a channel. Unrecognized content type \"%1\".").arg(dev.responseHeader().contentType()));
      return false;
    }

  return true;
}

void PiiRemoteObject::decodePushedData(const QString& sourceId, const QByteArray& data)
{
  //piiDebug(QString("Received %1 bytes to %2.").arg(data.size()).arg(strId));
  if (sourceId.startsWith("callbacks/"))
    {
      QVariantList lstParams;
      if (!data.isEmpty())
        lstParams = PiiNetwork::fromByteArray<QVariantList>(data);

      PiiNetwork::resolveFunction(d->lstCallbacks, sourceId.mid(10), lstParams)->call(lstParams);
    }
}

void PiiRemoteObject::setServerUriImpl(const QString& uri)
{
  QRegExp uriExp("([^:]+://[^/]+)(/[^ ]*)");
  if (!uriExp.exactMatch(uri))
    PII_THROW(PiiInvalidArgumentException, tr("The provided server URI (%1) is invalid.").arg(uri));

  d->networkClient.setServerAddress(uriExp.cap(1));
  d->strPath = uriExp.cap(2);
  if (d->strPath[d->strPath.size()-1] != '/')
    d->strPath.append('/');

  HttpDevicePtr pDev = openConnection();
  pDev->setRequest("GET", d->strPath + "id");
  pDev->finish();
  if (!pDev->readHeader() || pDev->status() != PiiHttpProtocol::OkStatus)
    {
      addFailure();
      PII_THROW(PiiNetworkException, tr("Unable to read remote object ID."));
    }
  QString strServerId(pDev->readBody());
  if (!d->strServerId.isEmpty() && strServerId != d->strServerId)
    PII_THROW(PiiNetworkException,
              tr("Remote object ID changed (was %1, now %2).").arg(d->strServerId, strServerId));
  d->strServerId = strServerId;

  /* You may call this either a stupid hack, a necessary work-around
     or just an optimization. The problem is that certain things in Qt
     can only be done in the main thread. If we called a remote
     function through the network stack, the request would be served
     by an arbitrary thread, which may need to pass the call to the
     main thread and wait for the result. But we may be in the main
     thread already. Deadlock, anyone?
   */
  PiiObjectServer* pServer = PiiObjectServer::server(strServerId);
  if (pServer)
    {
      if (pServer->strictestSafetyLevel() == PiiObjectServer::AccessFromMainThread ||
          (qobject_cast<PiiQObjectServer*>(pServer) &&
           static_cast<PiiQObjectServer*>(pServer)->strictestPropertySafetyLevel() ==
           PiiQObjectServer::AccessPropertyFromMainThread))
        d->pLocalServer = pServer;
    }
}

void PiiRemoteObject::setServerUri(const QString& uri)
{
  QMutexLocker lock(&d->channelMutex);
  setServerUriImpl(uri);
  serverUriChanged(uri);
}

QString PiiRemoteObject::serverUri() const
{
  return d->networkClient.serverAddress() + d->strPath;
}

void PiiRemoteObject::handleException(PiiHttpDevice* dev)
{
  QByteArray aData = dev->readBody();
  if (!aData.isEmpty())
    {
      PiiException* pException;
      pException = PiiNetwork::fromByteArray<PiiException*>(aData); // may throw
      pException->throwIt(); // throws
    }
  PII_THROW(PiiSerializationException, tr("The remote function threw an exception, but no data was received."));
}

QVariant PiiRemoteObject::callList(const QString& uri, const QVariantList& params)
{
  HttpDevicePtr pDev = openConnection();

  if (params.size() > 0)
    {
      pDev->setRequest("POST", d->strPath + uri);
      pDev->startOutputFiltering(new PiiStreamBuffer);
      try { pDev->write(PiiNetwork::toByteArray(params, PiiNetwork::BinaryFormat)); }
      catch (...) { finishRequest(pDev); throw; }
    }
  else
    {
      pDev->setRequest("GET", d->strPath + uri);
    }

  finishRequest(pDev);

  PII_THROW_IF_NOT_CONNECTED;
  if (!pDev->readHeader())
    PII_THROW(PiiNetworkException, tr(PiiNetwork::pErrorReadingResponseHeader));

  switch (pDev->status())
    {
    case PiiHttpProtocol::OkStatus:
      if (pDev->responseHeader().contentLength() > 0)
        return pDev->decodeVariant(pDev->readBody()); // may throw
      else
        pDev->discardBody();
      return QVariant();
    case PiiNetwork::RemoteExceptionStatus:
      handleException(pDev); // throws
    default:
      PII_THROW(PiiNetworkException, tr(PiiNetwork::pServerRepliedWithStatus).arg(pDev->status()));
    }
}

void PiiRemoteObject::setRetryCount(int retryCount) { d->iRetryCount = qBound(0,retryCount,5); }
int PiiRemoteObject::retryCount() const { return d->iRetryCount; }
void PiiRemoteObject::setRetryDelay(int retryDelay) { d->iRetryDelay = qBound(0,retryDelay,2000); }
int PiiRemoteObject::retryDelay() const { return d->iRetryDelay; }

void PiiRemoteObject::serverUriChanged(const QString&) {}

int PiiRemoteObject::failureCount() const { return d->iFailureCount.load(); }
void PiiRemoteObject::resetFailureCount() { d->iFailureCount = 0; }
void PiiRemoteObject::addFailure() { d->iFailureCount.ref(); }

void PiiRemoteObject::setMaxFailureCount(int maxFailureCount) { d->iMaxFailureCount = maxFailureCount; }
int PiiRemoteObject::maxFailureCount() const { return d->iMaxFailureCount; }

QString PiiRemoteObject::serverId() const { return d->strServerId; }
