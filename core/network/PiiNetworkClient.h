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

#ifndef _PIINETWORKCLIENT_H
#define _PIINETWORKCLIENT_H

#include <QString>
#include <QObject>
#include <QAbstractSocket>

#include "PiiSocketDevice.h"

/**
 * A generic client for network connections. PiiNetworkClient tries
 * to keep a connection open to avoid unnecessary reconnection delays.
 *
 */
class PII_NETWORK_EXPORT PiiNetworkClient : public QObject
{
  Q_OBJECT
public:
  /**
   * Creates a new network client that is used to communicate with the
   * server at the given address. See PiiHttpServer for valid address
   * formats. PiiNetworkClient also recognizes http, https, ftp, and
   * ftps protocols and automatically uses the corresponding default
   * port numbers. Note that there must be no slash at the end of the
   * address.
   *
   * ~~~
   * PiiNetworkClient client("http://intopii.com");
   * ~~~
   */
  PiiNetworkClient(const QString& serverAddress = "");

  /**
   * Destroys the device.
   */
  ~PiiNetworkClient();
  
  /**
   * Opens a connection to the server. The connection will be kept
   * alive if possible. Subsequent calls to this function check if the
   * connection is still open and try to reopen it if possible. The
   * function finishes only after the connection has been established
   * or failed.
   *
   * @return a pointer to the communication device, or zero if the
   * connection failed. The QIODevice pointer in the returned device
   * is owned by PiiNetworkClient and must not be deleted by the
   * caller.
   */
  PiiSocketDevice openConnection();

  /**
   * Sets the maximum number of milliseconds the PiiNetworkClient
   * will wait for a successful connection. Set this value before
   * calling [openConnection()]. The default value is 5000.
   */
  void setConnectionTimeout(int connectionTimeout);
  /**
   * Returns the connection time-out.
   */
  int connectionTimeout() const;

  /**
   * Sets the server address. The server at the new address will be
   * first contacted when [openConnection()] is called next time. See
   * PiiHttpServer for valid address formats. If the server address
   * changes, the next [openConnection()] call will delete the current
   * communication device and create a new one.
   *
   * ~~~
   * client.setServerAddress("tcp://127.0.0.1:3142");
   * // http, https, ftp, and ftps are recognized
   * client.setServerAddress("ftps://127.0.0.1");
   * ~~~
   */
  void setServerAddress(const QString& serverAddress);
  /**
   * Returns the current server address.
   */
  QString serverAddress() const;

public slots:
  /**
   * Closes the connection to the server.
   */
  void closeConnection();

private:
  PiiSocketDevice connectToServer();
  
  /// @internal
  class Data
  {
  public:
    Data(const QString& serverAddress);
    QString strServerAddress, strOldAddress;
    PiiSocketDevice pDevice;
    int iConnectionTimeout;
  } *d;
};

#endif //_PIINETWORKCLIENT_H
