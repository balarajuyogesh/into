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

#include "PiiInstantiableObjectServer.h"
#include "PiiHttpException.h"
#include "PiiHttpDevice.h"
#include "PiiStreamBuffer.h"

#include <QUuid>
#include <QTimer>

PiiInstantiableObjectServer::Data::Data() :
  iInstanceTimeout(10000),
  iMaxInstances(100),
  iInstanceCount(0)
{}

PiiInstantiableObjectServer::Data::~Data()
{}

PiiInstantiableObjectServer::PiiInstantiableObjectServer() :
  d(new Data)
{
  QTimer* pTimer = new QTimer(this);
  connect(pTimer, SIGNAL(timeout()), this, SLOT(collectGarbage()));
  pTimer->start(1000);
}

PiiInstantiableObjectServer::~PiiInstantiableObjectServer()
{
  deleteServers();
  delete d;
}

QString PiiInstantiableObjectServer::createNewInstance(const QVariantMap& parameters)
{
  QMutexLocker lock(&d->instanceMutex);
  if (d->iInstanceCount >= d->iMaxInstances)
    PII_THROW_HTTP_ERROR_MSG(ServiceUnavailableStatus, tr("Maximum number of remote object instances reached."));
  PiiHttpProtocol::UriHandler* pNewServer = createServer(parameters);
  if (pNewServer == 0)
    PII_THROW_HTTP_ERROR_MSG(InternalServerErrorStatus, tr("Unable to create remote object instance."));
  ++d->iInstanceCount;
  // Remove curly braces around the uuid
  QString strId = QUuid::createUuid().toString().mid(1);
  strId.chop(1);
  d->hashInstances.insert(strId, Instance(pNewServer));
  return strId;
}

PiiHttpProtocol::UriHandler* PiiInstantiableObjectServer::findInstance(const QString& instanceId) const
{
  QHash<QString,Instance>::iterator i = d->hashInstances.find(instanceId);
  if (i == d->hashInstances.end())
    PII_THROW_HTTP_ERROR(NotFoundStatus);
  i->idleTime.restart();
  return i->pServer;
}


void PiiInstantiableObjectServer::handleRequest(const QString& uri, PiiHttpDevice* dev,
                                                PiiHttpProtocol::TimeLimiter* controller)
{
  QString strRequestPath = dev->requestPath(uri);
  dev->startOutputFiltering(new PiiStreamBuffer);
 
  // This class provides only "/new" and "/delete" functionality and
  // passes other requests to secondary servers.
  if (strRequestPath.isEmpty())
    dev->print("new\ndelete");
  else if (strRequestPath == "new")
    dev->print(createNewInstance(dev->requestValues()));
  else if (strRequestPath == "delete")
    deleteServer(dev->queryString());
  // Find the server that handles request to the given object
  // instance.
  else
    {
      int iSlashIndex = strRequestPath.indexOf('/');
      if (iSlashIndex == -1)
        PII_THROW_HTTP_ERROR(NotFoundStatus);
      QString strServerId = strRequestPath.left(iSlashIndex);
      synchronized (d->instanceMutex)
        {
          PiiHttpProtocol::UriHandler* pServer = findInstance(strServerId); // may throw
          // Pass the request to the selected server
          pServer->handleRequest(uri + strServerId + "/", dev, controller);
        }
    }
}

void PiiInstantiableObjectServer::deleteServers()
{
  QMutexLocker lock(&d->instanceMutex);
  for (QHash<QString,Instance>::iterator i = d->hashInstances.begin();
       i != d->hashInstances.end(); ++i)
    delete i->pServer;
}

void PiiInstantiableObjectServer::deleteServer(const QString& serverId)
{
  QMutexLocker lock(&d->instanceMutex);
  QHash<QString,Instance>::iterator i = d->hashInstances.find(serverId);
  if (i != d->hashInstances.end())
    {
      delete i->pServer;
      d->hashInstances.erase(i);
    }
}

void PiiInstantiableObjectServer::collectGarbage()
{
  QMutexLocker lock(&d->instanceMutex);
  // Kill all inactive server instances
  QHash<QString,Instance>::iterator i = d->hashInstances.begin();
  while (i != d->hashInstances.end())
    {
      if (!i.value().idleTime.elapsed() > d->iInstanceTimeout)
        {
          delete i.value().pServer;
          --d->iInstanceCount;
          i = d->hashInstances.erase(i);
        }
      else
        ++i;
    }
}

void PiiInstantiableObjectServer::setInstanceTimeout(int instanceTimeout) { d->iInstanceTimeout = instanceTimeout; }
int PiiInstantiableObjectServer::instanceTimeout() const { return d->iInstanceTimeout; }

void PiiInstantiableObjectServer::setMaxInstances(int maxInstances) { d->iMaxInstances = maxInstances; }
int PiiInstantiableObjectServer::maxInstances() const { return d->iMaxInstances; }
