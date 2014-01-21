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

#include "PiiNetworkClient.h"
#include <QIODevice>
#include <QUrl>
#include <QTcpSocket>
#include <QSslSocket>
#include <QLocalSocket>
#include <QCoreApplication>

PiiNetworkClient::Data::Data(const QString& serverAddress) :
  strServerAddress(serverAddress),
  iConnectionTimeout(5000)
{
}

PiiNetworkClient::PiiNetworkClient(const QString& serverAddress) :
  d(new Data(serverAddress))
{
}

PiiNetworkClient::~PiiNetworkClient()
{
  closeConnection();
  delete d->pDevice.device();
  delete d;
}

void PiiNetworkClient::closeConnection()
{
  if (d->pDevice == 0)
    return;
  d->pDevice.disconnect();
  d->pDevice.waitForDisconnected(500);
  d->pDevice->close();
}

PiiSocketDevice PiiNetworkClient::openConnection()
{
  if (d->strServerAddress.isEmpty())
    return PiiSocketDevice();

  if (d->pDevice == 0 || !d->pDevice.isWritable() || d->strServerAddress != d->strOldAddress)
    {
      //qDebug("PiiNetworkClient::openConnection(): creating new device");
      delete d->pDevice;
      d->pDevice = connectToServer();
      d->strOldAddress = d->strServerAddress;
    }
  return d->pDevice;
}

PiiSocketDevice PiiNetworkClient::connectToServer()
{
  QUrl serverUrl(d->strServerAddress);
  QString strScheme = serverUrl.scheme();

  int iPort = serverUrl.port();
  if (iPort == -1)
    {
      if (strScheme == "http") iPort = 80;
      else if (strScheme == "https") iPort = 443;
      else if (strScheme == "ftp") iPort = 21;
      else if (strScheme == "ftps") iPort = 990;
    }
  if (strScheme member_of<QString> ("tcp", "http", "ftp"))
    {
      if (iPort == -1)
        return PiiSocketDevice();
      QTcpSocket* pSocket = new QTcpSocket;
      pSocket->connectToHost(serverUrl.host(), iPort);
      if (!pSocket->waitForConnected(d->iConnectionTimeout))
        {
          delete pSocket;
          return PiiSocketDevice();
        }
      return pSocket;
    }
#ifndef QT_NO_OPENSSL
  else if (strScheme member_of<QString> ("ssl", "https", "ftps"))
    {
      if (iPort == -1)
        return PiiSocketDevice();
      QSslSocket* pSocket = new QSslSocket;
      pSocket->connectToHostEncrypted(serverUrl.host(), iPort);
      if (!pSocket->waitForEncrypted(d->iConnectionTimeout))
        {
          delete pSocket;
          return PiiSocketDevice();
        }
      return pSocket;
    }
#endif
  else if (strScheme == "local")
    {
      QLocalSocket* pSocket = new QLocalSocket;
      pSocket->connectToServer(d->strServerAddress.mid(8)); // strlen("local://")
      if (!pSocket->waitForConnected(d->iConnectionTimeout))
        {
          delete pSocket;
          return PiiSocketDevice();
        }
      return pSocket;
    }

  return PiiSocketDevice();
}

void PiiNetworkClient::setConnectionTimeout(int connectionTimeout) { d->iConnectionTimeout = connectionTimeout; }
int PiiNetworkClient::connectionTimeout() const { return d->iConnectionTimeout; }
void PiiNetworkClient::setServerAddress(const QString& serverAddress) { d->strServerAddress = serverAddress; }
QString PiiNetworkClient::serverAddress() const { return d->strServerAddress; }
