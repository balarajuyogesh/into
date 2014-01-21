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

#ifndef _PIILOCALSERVER_H
#define _PIILOCALSERVER_H

#include "PiiNetworkServer.h"
#include <QLocalServer>

/**
 * A threaded server that runs an application layer protocol over a
 * local socket. On Unix, this means a Unix-domain socket. On Windows,
 * pipes are used.
 *
 */
class PII_NETWORK_EXPORT PiiLocalServer : public PiiNetworkServer
{
  Q_OBJECT

public:
  PiiLocalServer(PiiNetworkProtocol* protocol);
  ~PiiLocalServer();

  /**
   * Set the name of the socket/pipe to bind to. On Unix, this is the
   * name of the socket file. On windows, use a pipe path.
   *
   * @see QLocalServer::listen()
   */
  bool setServerAddress(const QString& serverAddress);
  QString serverAddress() const;

  /**
   * Creates a new QLocalSocket and assigns `socketDescriptor` to it.
   */
  QIODevice* createSocket(PiiGenericSocketDescriptor socketDescriptor);

protected:
  bool startListening();

  void stopListening();

private:
  class EntryPoint : public QLocalServer
  {
  public:
    EntryPoint(PiiLocalServer* owner);

  protected:
    /// Calls PiiLocalServer::incomingConnection().
    void incomingConnection(quintptr socketDescriptor);

  private:
    PiiLocalServer* _pOwner;
  };

  /// @internal
  class Data : public PiiNetworkServer::Data
  {
  public:
    Data(PiiLocalServer* owner, PiiNetworkProtocol* protocol);

    EntryPoint server;
    QString strSocketName;
  };
  PII_D_FUNC;

  /**
   * Calls PiiNetworkServer::incomingConnection().
   */
  void incomingConnection(quintptr socketDescriptor);

};


#endif //_PIILOCALSERVER_H
