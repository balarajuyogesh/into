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

#include "PiiLocalServer.h"
#include <QLocalSocket>

PiiLocalServer::Data::Data(PiiLocalServer* owner, PiiNetworkProtocol* protocol) :
  PiiNetworkServer::Data(protocol),
  server(owner)
{}

PiiLocalServer::PiiLocalServer(PiiNetworkProtocol* protocol) :
  PiiNetworkServer(new Data(this, protocol))
{}

PiiLocalServer::~PiiLocalServer()
{
  _d()->server.close();
}

bool PiiLocalServer::startListening()
{
  PII_D;
  return d->server.listen(d->strSocketName);
}

void PiiLocalServer::stopListening()
{
  _d()->server.close();
}
  
void PiiLocalServer::incomingConnection(quintptr socketDescriptor)
{
  PiiNetworkServer::incomingConnection(PiiGenericSocketDescriptor(socketDescriptor));
}

QIODevice* PiiLocalServer::createSocket(PiiGenericSocketDescriptor socketDescriptor)
{
  QLocalSocket* pSocket = new QLocalSocket;
  pSocket->setSocketDescriptor(socketDescriptor.localSocketDescriptor);
  return pSocket;
}

bool PiiLocalServer::setServerAddress(const QString& serverAddress)
{
  _d()->strSocketName = serverAddress;
  return true;
}

QString PiiLocalServer::serverAddress() const
{
  return _d()->strSocketName;
}


PiiLocalServer::EntryPoint::EntryPoint(PiiLocalServer* owner) : _pOwner(owner) {}
void PiiLocalServer::EntryPoint::incomingConnection(quintptr sockedFd) { _pOwner->incomingConnection(sockedFd); }

