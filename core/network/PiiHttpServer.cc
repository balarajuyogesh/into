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

#include "PiiHttpServer.h"
#include "PiiTcpServer.h"
#include "PiiLocalServer.h"

QMutex PiiHttpServer::_mapLock;
PiiHttpServer* PiiHttpServer::_pDefaultServer = 0;

PiiHttpServer::Data::Data(PiiNetworkServer* server,
                          PiiHttpProtocol* protocol) :
  pServer(server), pProtocol(protocol)
{
}

PiiHttpServer::Data::~Data()
{
  delete pServer;
  delete pProtocol;
}


PiiHttpServer::PiiHttpServer(PiiNetworkServer* server, PiiHttpProtocol* protocol) :
  d(new Data(server, protocol))
{
  server->setBusyMessage("HTTP/1.1 503 Service Unavailable\r\n\r\n");
}

PiiHttpServer::~PiiHttpServer()
{
  delete d;
}

PiiHttpProtocol* PiiHttpServer::protocol() const
{
  return d->pProtocol;
}

PiiNetworkServer* PiiHttpServer::networkServer() const
{
  return d->pServer;
}

PiiHttpServer::ServerMap* PiiHttpServer::serverMap()
{
  static ServerMap map;
  return &map;
}

PiiHttpServer* PiiHttpServer::addServer(const QString& serverName, const QString& address)
{
  QMutexLocker lock(&_mapLock);

  deleteServer(serverName);
  PiiHttpServer* pServer = createServer(address);
  if (pServer != 0)
    {
      // First will become default. So will an empty name.
      if (serverMap()->size() == 0 || serverName.isEmpty())
        _pDefaultServer = pServer;
      serverMap()->insert(serverName, pServer);
    }
  return pServer;
}

PiiHttpServer* PiiHttpServer::server(const QString& serverName)
{
  QMutexLocker lock(&_mapLock);
  if (serverName.isEmpty())
    return _pDefaultServer;
  return serverMap()->value(serverName, 0);
}

void PiiHttpServer::removeServer(const QString& serverName)
{
  QMutexLocker lock(&_mapLock);
  deleteServer(serverName);
}

void PiiHttpServer::deleteServer(const QString& serverName)
{
  ServerMap* pMap = serverMap();
  if (pMap->contains(serverName))
    {
      PiiHttpServer* pServer = pMap->take(serverName);
      pServer->networkServer()->stop(PiiNetwork::InterruptClients);
      // Default was deleted
      if (pServer == _pDefaultServer)
        {
          // Reassign default if possible
          if (pMap->size() > 0)
            _pDefaultServer = *pMap->begin();
          else
            _pDefaultServer = 0;
        }
      delete pServer;
    }
}

PiiHttpServer* PiiHttpServer::createServer(const QString& address)
{
  int schemeIndex = address.indexOf("://");
  if (schemeIndex == -1)
    return 0;

  QString strScheme = address.left(schemeIndex);
  PiiHttpProtocol* pProtocol = new PiiHttpProtocol;
  PiiNetworkServer* pServer = 0;

  if (strScheme == "tcp")
    pServer = new PiiTcpServer(pProtocol);
  else if (strScheme == "ssl")
    pServer = new PiiTcpServer(pProtocol, PiiTcpServer::SslEncryption);
  else if (strScheme == "local")
    pServer = new PiiLocalServer(pProtocol);
  else
    {
      delete pProtocol;
      return 0;
    }

  if (!pServer->setServerAddress(address.mid(schemeIndex+3)))
    {
      delete pServer;
      delete pProtocol;
      return 0;
    }

  return new PiiHttpServer(pServer, pProtocol);
}

bool PiiHttpServer::start() { return d->pServer->start(); }
bool PiiHttpServer::stop(PiiNetwork::StopMode mode) { return d->pServer->stop(mode); }
