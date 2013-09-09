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

#include "PiiObjectServer.h"

#include "PiiHttpDevice.h"
#include "PiiHttpException.h"
#include "PiiStreamBuffer.h"

#include <PiiUtil.h>
#include <PiiMetaTypeUtil.h>
#include <PiiInvalidArgumentException.h>

#include <QTimer>
#include <QThread>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QUuid>

#include "PiiNetworkEncoding.h"

PiiObjectServer::Data::Data() :
  iChannelTimeout(10000),
  safetyLevel(AccessFromAnyThread)
{}

PiiObjectServer::Data::~Data()
{
}

PiiObjectServer::PiiObjectServer() :
  d(new Data)
{
  init();
}

PiiObjectServer::PiiObjectServer(Data* data) :
  d(data)
{
  init();
}

void PiiObjectServer::init()
{
  QTimer* pTimer = new QTimer(this);
  connect(pTimer, SIGNAL(timeout()), this, SLOT(collectGarbage()));
  pTimer->start(1000);
}

PiiObjectServer::~PiiObjectServer()
{
  removeFunctions();
  removeCallbacks();
  killChannels();
  delete d;
}

void PiiObjectServer::killChannels()
{
  QMutexLocker lock(&d->channelMutex);
  for (QHash<QString,ChannelImpl*>::iterator i = d->hashChannelsById.begin(); i != d->hashChannelsById.end(); ++i)
    i.value()->quit();
  for (QHash<QString,ChannelImpl*>::iterator i = d->hashChannelsById.begin(); i != d->hashChannelsById.end(); ++i)
    {
      i.value()->wait();
      delete i.value();
    }
  d->hashChannelsById.clear();
}

QString PiiObjectServer::addFunction(const QString& name, PiiGenericFunction* function)
{
  d->lstFunctions << qMakePair(name, function);
  return function->signature(name);
}

void PiiObjectServer::removeFunction(const QString& signature)
{
  removeFunction(signature, d->lstFunctions);
}

void PiiObjectServer::removeFunctions()
{
  removeFunctions(d->lstFunctions);
}

void PiiObjectServer::removeFunction(const QString& signature, FunctionList& lst)
{
  int i = indexOf(signature, lst);
  if (i != -1)
    delete lst.takeAt(i).second;
}

void PiiObjectServer::removeFunctions(FunctionList& lst)
{
  for (int i=0; i<lst.size(); ++i)
    delete lst[i].second;
  lst.clear();
}

QStringList PiiObjectServer::listRoot() const
{
  return QStringList() << "functions/" << "callbacks/" << "channels/" << "ping";
}

// handles requests to /channels/
// *lock* must be held when calling this function
void PiiObjectServer::handleChannelCommand(const QString& uri, PiiHttpDevice* dev,
                                           PiiHttpProtocol::TimeLimiter* controller,
                                           QMutexLocker* lock)
{
  PII_REQUIRE_HTTP_METHOD("GET");
  
  int iSlashIndex = uri.indexOf('/');
  // No slash -> the request was to /channels/id -> reconnect to channel
  if (iSlashIndex == -1)
    {
      channelById(uri)->push(dev, controller, lock); // may throw
      return;
    }

  QString strChannelId = uri.left(iSlashIndex);
  QString strFunction = uri.mid(iSlashIndex+1);

  if (strFunction == "sources/")
    dev->print(channelById(strChannelId)->lstSources.join("\n")); // may throw
  else if (strFunction == "connect")
    {
      QString strSourceId = dev->queryString();
      ChannelImpl* pChannel = channelById(strChannelId); // may throw

      // Disallow multiple registrations to a channel.
      if (pChannel->lstSources.contains(strSourceId))
        return;

      connectToChannel(pChannel, strSourceId);
      pChannel->lstSources << strSourceId;
    }      
  else if (strFunction == "disconnect")
    {
      QString strSourceId = dev->queryString();
      ChannelImpl* pChannel = channelById(strChannelId); // may throw
      pChannel->lstSources.removeOne(strSourceId);
      pChannel->removeObjectsQueuedTo(strSourceId);
      disconnectFromChannel(pChannel, strSourceId);
    }
  else if (strFunction == "delete")
    {
      // Quit the thread and destroy it.
      ChannelImpl* pChannel = channelById(strChannelId); // may throw
      pChannel->quit();
      pChannel->wait();
      channelDeleted(pChannel);
      delete pChannel;
      d->hashChannelsById.remove(strChannelId);
    }
  else
    PII_THROW_HTTP_ERROR(NotFoundStatus);
}

void PiiObjectServer::createExceptionResponse(PiiHttpDevice* dev, const PiiException& ex)
{
  try
    {
      QByteArray aData = PiiNetwork::toByteArray(&ex, PiiNetwork::BinaryFormat);
      dev->setStatus(PiiNetwork::RemoteExceptionStatus);
      dev->write(aData);
    }
  catch (PiiException& ex2)
    {
      PII_THROW_HTTP_ERROR_MSG(InternalServerErrorStatus,
                               tr("Could not encode the exception thrown by a function.\n"
                                  "%1\n"
                                  "The original error was: %2")
                               .arg(ex2.message())
                               .arg(ex.message()));
    }
}

void PiiObjectServer::handleRequest(const QString& uri, PiiHttpDevice* dev,
                                    PiiHttpProtocol::TimeLimiter* controller)
{
  bool bPostRequest = dev->requestMethod() == "POST";

  if (!bPostRequest && dev->requestMethod() != "GET")
    PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);

  dev->startOutputFiltering(new PiiStreamBuffer);

  QString strRequestPath = dev->requestPath(uri);

  if (strRequestPath.isEmpty())
    {
      PII_REQUIRE_HTTP_METHOD("GET");
      dev->print(listRoot().join("\n"));
      return;
    }

  int iSlashIndex = strRequestPath.indexOf('/');
  if (iSlashIndex == -1)
    {
      if (strRequestPath == "ping" || strRequestPath == "new")
        // Do nothing. "new" allows the client to always request a new
        // object even if the server provides just a single instance.
        return;
      PII_THROW_HTTP_ERROR(NotFoundStatus);
    }

  QString strSubDir = strRequestPath.left(iSlashIndex);

  // Function calls
  if (strSubDir == "functions")
    {
      QString strFunction = strRequestPath.mid(iSlashIndex+1);
      // Empty function name -> list all functions
      if (strFunction.isEmpty())
        {
          PII_REQUIRE_HTTP_METHOD("GET");
          dev->print(functionSignatures().join("\n"));
        }
      else
        {
          QVariant varReturn;
          QVariantList lstParams;
          // GET request -> parameters in URL
          if (!bPostRequest)
            lstParams = paramList(dev, dev->queryItems(), dev->queryValues());
          // POST form -> parameters in body
          else if (dev->requestHeader().contentType() == PiiNetwork::pFormContentType)
            lstParams = paramList(dev, dev->formItems(), dev->formValues());
          // Other POST -> decode a QVariantList
          else
            {
              try
                {
                  lstParams = PiiNetwork::fromByteArray<QVariantList>(dev->readBody());
                }
              catch (PiiSerializationException& ex)
                {
                  PII_THROW_HTTP_ERROR_MSG(BadRequestStatus, ex.message() + " (" + ex.info() + ")");
                }
            }
          try
            {
              varReturn = call(strFunction, lstParams);
              if (varReturn.isValid())
                dev->write(dev->encode(varReturn));
            }
          catch (PiiHttpException& ex)
            {
              throw;
            }
          catch (PiiException& ex)
            {
              createExceptionResponse(dev, ex); // may throw
            }
          catch (...)
            {
              PII_THROW_HTTP_ERROR_MSG(InternalServerErrorStatus,
                                       tr("Uncaught exception while processing function call."));
            }
        }
    }
  else if (strSubDir == "channels")
    {
      QString strFunction = strRequestPath.mid(iSlashIndex+1);
      if (strFunction.isEmpty())
        {
          PII_REQUIRE_HTTP_METHOD("GET");
          dev->print("new");
        }
      else
        {
          QMutexLocker lock(&d->channelMutex);
          // new and reconnect behave differently from other
          // "functions". They hang the calling thread.
          if (strFunction == "new")
            {
              PII_REQUIRE_HTTP_METHOD("GET");
              QString strId = createNewChannel();
              dev->print(strId);
              dev->putChar('\n');
              channelById(strId)->push(dev, controller, &lock); // may throw
            }
          else
            // strFunction must be of the form channel-id/action
            handleChannelCommand(strFunction, dev, controller, &lock); // may throw
        }
    }
  else if (strSubDir == "callbacks")
    {
    }
  else
    PII_THROW_HTTP_ERROR(NotFoundStatus);
}

// channelMutex must be held when calling this function
PiiObjectServer::ChannelImpl* PiiObjectServer::channelById(const QString& channelId)
{
  QHash<QString,ChannelImpl*>::const_iterator i = d->hashChannelsById.find(channelId);
  if (i == d->hashChannelsById.end())
    PII_THROW_HTTP_ERROR(NotFoundStatus);
  return i.value();
}

QVariantList PiiObjectServer::paramList(PiiHttpDevice* dev, const QStringList& names, const QVariantMap& map)
{
  QVariantList lstResult;
  //qDebug() << names << map;
  for (int i=0; i<names.size(); ++i)
    {
      QVariant varValue = map[names[i]];
      if (varValue.isValid())
        lstResult << varValue;
      else
        lstResult << dev->decodeVariant(names[i]);
    }
  return lstResult;
}

QStringList PiiObjectServer::functionSignatures() const
{
  QStringList lstResult;
  for (int i=0; i<d->lstFunctions.size(); ++i)
    lstResult << d->lstFunctions[i].second->signature(d->lstFunctions[i].first);
  return lstResult;
}

QVariant PiiObjectServer::callFromMainThread(void* function, void* params)
{
  return reinterpret_cast<PiiGenericFunction*>(function)->call(*reinterpret_cast<QVariantList*>(params));
}

QVariant PiiObjectServer::call(const QString& function, QVariantList& params)
{
  PiiNetwork::ResolutionError errorCode = PiiNetwork::NoResolutionError;
  PiiGenericFunction* pFunction = PiiNetwork::resolveFunction(d->lstFunctions, function, params, &errorCode);
  if (pFunction != 0)
    {
      switch (functionSafetyLevel(pFunction->signature(function)))
        {
        case AccessFromMainThread:
          {
            Q_ASSERT(thread() == qApp->thread());
            QVariant varResult;
            QMetaObject::invokeMethod(this, "callFromMainThread",
                                      Qt::BlockingQueuedConnection,
#if (QT_VERSION >= 0x040800)
                                      Q_RETURN_ARG(QVariant, varResult),
#endif
                                      Q_ARG(void*, pFunction),
                                      Q_ARG(void*, &params));
            return varResult;
          }
        case AccessFromAnyThread:
          synchronized (d->accessMutex) return pFunction->call(params);
        case AccessConcurrently:
          return pFunction->call(params);
        }
    }
  
  if (errorCode == PiiNetwork::FunctionNameNotFound)
    PII_THROW_HTTP_ERROR(NotFoundStatus);
  //else if (errorCode == OverloadNotFound)
  PII_THROW_HTTP_ERROR_MSG(BadRequestStatus, tr("No acceptable overload found."));
}

void PiiObjectServer::collectGarbage()
{
  QMutexLocker lock(&d->channelMutex);
  // Kill all inactive channels
  QHash<QString,ChannelImpl*>::iterator i = d->hashChannelsById.begin();
  while (i != d->hashChannelsById.end())
    {
      //piiDebug("Checking %s (%p)", piiPrintable(i.key()), i.value());
      ChannelImpl* pChannel = *i;
      if (!pChannel->isAlive(d->iChannelTimeout))
        {
          piiDebug(QString("Deleting channel %1 due to a time-out.").arg(i.key()));
          channelDeleted(pChannel);
          pChannel->wait();
          delete pChannel;
          i = d->hashChannelsById.erase(i);
        }
      else
        ++i;
    }
}

// channelMutex must be held when calling this function
QString PiiObjectServer::createNewChannel()
{
  // Remove curly braces around the uuid
  QString strId = QUuid::createUuid().toString().mid(1);
  strId.chop(1);
  d->hashChannelsById.insert(strId, createChannel());
  return strId;
}

PiiObjectServer::ChannelImpl* PiiObjectServer::createChannel() const
{
  return new ChannelImpl;
}

int PiiObjectServer::indexOf(const QString& signature, const FunctionList& lst) const
{
  for (int i=0; i<lst.size(); ++i)
    if (lst[i].second->signature(lst[i].first) == signature)
      return i;
  return -1;
}

void PiiObjectServer::disconnectFromChannel(Channel* channel, const QString& sourceId)
{
  int iCallBackIndex = -1;
  if (sourceId.startsWith("callbacks/"))
    iCallBackIndex = indexOf(sourceId.mid(10), d->lstCallbacks);
  if (iCallBackIndex == -1)
    return;

  static_cast<CallbackFunction*>(d->lstCallbacks[iCallBackIndex].second)->lstChannels.removeOne(channel);
}

void PiiObjectServer::connectToChannel(Channel* channel, const QString& sourceId)
{
  int iCallBackIndex = -1;
  if (sourceId.startsWith("callbacks/"))
    iCallBackIndex = indexOf(sourceId.mid(10), d->lstCallbacks);
  if (iCallBackIndex == -1)
    PII_THROW_HTTP_ERROR_MSG(BadRequestStatus, tr("\"%1\" is not a valid callback.").arg(sourceId));
  static_cast<CallbackFunction*>(d->lstCallbacks[iCallBackIndex].second)->lstChannels << channel;
}

void PiiObjectServer::channelDeleted(Channel* channel)
{
  // Remove occurrences of channel from all callbacs.
  for (FunctionList::const_iterator i=d->lstCallbacks.constBegin(); i != d->lstCallbacks.constEnd(); ++i)
    static_cast<CallbackFunction*>(i->second)->lstChannels.removeOne(channel);
}

void PiiObjectServer::setChannelTimeout(int channelTimeout) { d->iChannelTimeout = channelTimeout; }
int PiiObjectServer::channelTimeout() const { return d->iChannelTimeout; }

bool PiiObjectServer::addCallback(const QString& signature)
{
  QRegExp re("([^(]+)\\(([^)]*)\\)");
  if (!re.exactMatch(signature))
    return false;

  // Ensure no such callback is already registered
  if (indexOf(signature, d->lstCallbacks) != -1)
    return false;

  QString strName = re.cap(1), strTypes = re.cap(2);
  QList<int> lstTypes = Pii::parseTypes(strTypes.toLatin1());
  if (!strTypes.isEmpty() && lstTypes.isEmpty()) // Incorrect type names
    return false;
  d->lstCallbacks << qMakePair<QString,PiiGenericFunction*>(strName,
                                                            new CallbackFunction(strName, lstTypes));
  return true;
}

void PiiObjectServer::removeCallback(const QString& signature)
{
  removeFunction(signature, d->lstCallbacks);
}

void PiiObjectServer::removeCallbacks()
{
  removeFunctions(d->lstCallbacks);
}

QVariant PiiObjectServer::callBackList(const QString& function, QVariantList& params)
{
  QMutexLocker lock(&d->channelMutex);
  PiiNetwork::ResolutionError errorCode;
  PiiGenericFunction* pFunction = PiiNetwork::resolveFunction(d->lstCallbacks, function, params, &errorCode);
  if (pFunction != 0)
    return pFunction->call(params);
  if (errorCode == PiiNetwork::FunctionNameNotFound)
    PII_THROW(PiiInvalidArgumentException, tr("No such function."));
  PII_THROW(PiiInvalidArgumentException, tr("No acceptable overload found."));
}


PiiObjectServer::Channel::~Channel()
{}

bool PiiObjectServer::Channel::enqueuePushData(const QString& uri, const QByteArray& data)
{
  QMutexLocker lock(&_queueMutex);
  if (_dataQueue.size() >= 20)
    return false;

  _dataQueue.enqueue(qMakePair(uri, data));
  if (_dataQueue.size() == 20)
    piiWarning("Maximum size of channel buffer reached.");

  //if (_dataQueue.size() > 1)
  //  piiDebug("%d objects in queue", _dataQueue.size());
  _queueCondition.wakeOne();
  return true;
}

PiiObjectServer::ChannelImpl::ChannelImpl() :
  _bPushing(false),
  _bKilled(false)
{
  _idleTimer.stop();
}

void PiiObjectServer::ChannelImpl::removeObjectsQueuedTo(const QString& uri)
{
  QMutexLocker lock(&_queueMutex);
  for (int i=_dataQueue.size(); i--; )
    if (_dataQueue[i].first == uri)
      _dataQueue.removeAt(i);
}

bool PiiObjectServer::ChannelImpl::isAlive(int timeout) const
{
  QMutexLocker lock(&_queueMutex);
  return _bPushing || _idleTimer.milliseconds() < timeout;
}

void PiiObjectServer::ChannelImpl::quit()
{
  QMutexLocker lock(&_queueMutex);
  _bKilled = true;
  _queueCondition.wakeOne();
}

void PiiObjectServer::ChannelImpl::wait()
{
  QMutexLocker lock(&_queueMutex);
  if (_bPushing)
    _pushEndCondition.wait(&_queueMutex);
}

// *lock* must be held when calling this function
void PiiObjectServer::ChannelImpl::push(PiiHttpDevice* dev,
                                        PiiHttpProtocol::TimeLimiter* controller,
                                        QMutexLocker* lock)
{
  static const char* pBoundary = "--243F6A8885A308D3";

  /* If we are currently pushing, it means either of the following:
     1) an unauthorized client figured out the channel ID and is trying to steal it.
     2) an authorized client is reconnecting to the channel.
     2.1) the client is trying to read the same channel twice accidentally or on purpose
     2.2) the client has lost connection to the channel, but we haven't noticed it yet.

     1 is unlikely, albeit possible
     2.1 is a programming error or an attempt to break the system
     2.2 happens in practice

     In all cases we just drop the old connection and move the channel
     to the new thread.
  */
     
  if (_bPushing)
    {
      quit();
      wait();
      piiWarning("Reconnecting to an active channel. Old connection will break.");
    }
  _bPushing = true;
  _idleTimer.stop();
  
  lock->unlock();
  
  controller->setMaxTime(-1);
  dev->setHeader("Content-Type", QString("multipart/mixed-replace; boundary=\"%1\"").arg(pBoundary+2));
  dev->write(pBoundary);
  dev->write("\r\n");
  // Flush all buffered data to socket and pass all remaining data
  // unfiltered.
  dev->outputFilter()->flushFilter();
  dev->endOutputFiltering();
  // Flush the socket too.
  dev->flushFilter();
  
  _queueMutex.lock();

  forever
    {
      while (!_bKilled && !_dataQueue.isEmpty() && dev->isWritable() && controller->canContinue())
        {
          QPair<QString,QByteArray> pair(_dataQueue.dequeue());

          // Writing to the device may take time. Let new data appear
          // meanwhile.
          _queueMutex.unlock();
          //piiDebug("PiiObjectServer::push() writing");
          dev->print(QString("X-ID: %1\r\nContent-Length: %2\r\n\r\n")
                     .arg(pair.first).arg(pair.second.size()));
          qint64 iBytesWritten = dev->write(pair.second);
          // Couldn't write all data -> warn and put the data back to
          // the queue.
          if (iBytesWritten != pair.second.size())
            {
              piiWarning("Failed to push data to channel. Only %d bytes written out of %d.",
                         int(iBytesWritten), pair.second.size());
              synchronized (_queueMutex) _dataQueue.prepend(pair);
              continue;
            }
          dev->write("\r\n");
          dev->write(pBoundary);
          dev->write("\r\n");
          dev->flushFilter();
          //piiDebug("Wrote %d of %d bytes to %s", (int)iBytesWritten, pair.second.size(), piiPrintable(pair.first));
          _queueMutex.lock();
        }
      if (_bKilled || !dev->isWritable() || !controller->canContinue())
        break;

      _queueCondition.wait(&_queueMutex, 50);
    }
  
  _bKilled = false;
  _bPushing = false;
  _idleTimer.restart();

  _pushEndCondition.wakeAll();
  _queueMutex.unlock();
  
  dev->setHeader("Connection", "close");
}

void PiiObjectServer::CallbackFunction::call(void** args)
{
  QByteArray aData = PiiNetwork::toByteArray(Pii::argsToList(d->lstParamTypes, args), PiiNetwork::BinaryFormat);
  
  for (int i=0; i<lstChannels.size(); ++i)
    lstChannels[i]->enqueuePushData(_strSourceId, aData);
}

void PiiObjectServer::setSafetyLevel(ThreadSafetyLevel safetyLevel)
{
  if (d->safetyLevel == AccessFromMainThread) return;
  d->safetyLevel = safetyLevel;
  if (safetyLevel == AccessFromMainThread)
    moveToMainThread();
}

PiiObjectServer::ThreadSafetyLevel PiiObjectServer::safetyLevel() const
{
  return d->safetyLevel;
}

void PiiObjectServer::setFunctionSafetyLevel(const QString& functionSignature, ThreadSafetyLevel safetyLevel)
{
  d->mapFunctionSafetyLevels.insert(functionSignature, safetyLevel);
}

PiiObjectServer::ThreadSafetyLevel PiiObjectServer::functionSafetyLevel(const QString& functionSignature) const
{
  SafetyLevelMap::const_iterator it = d->mapFunctionSafetyLevels.find(functionSignature);
  return it == d->mapFunctionSafetyLevels.end() ?
    d->safetyLevel :
    *it;
}

void PiiObjectServer::removeFunctionSafetyLevel(const QString& functionSignature)
{
  d->mapFunctionSafetyLevels.remove(functionSignature);
}

void PiiObjectServer::moveToMainThread()
{
  moveToThread(qApp->thread());
}
