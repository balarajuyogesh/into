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

#ifndef _PIIHTTPSERVER_H
#define _PIIHTTPSERVER_H

#include <QMutex>
#include <QMap>
#include "PiiHttpProtocol.h"
#include "PiiNetworkServer.h"

/**
 * A HTTP server. This class holds an application-wide registry of
 * HTTP servers. The servers can be created and accessed with
 * user-defined names.
 *
 */
class PII_NETWORK_EXPORT PiiHttpServer
{
public:
  ~PiiHttpServer();

  /**
   * Returns the protocol instance that parses requests to this server
   * and communicates with the clients.
   */
  PiiHttpProtocol* protocol() const;

  /**
   * Returns the network server that manages incoming connections.
   */
  PiiNetworkServer* networkServer() const;

  /**
   * A shorthand for networkServer()->start().
   */
  bool start();
  
  /**
   * A shorthand for networkServer()->stop().
   */
  bool stop(PiiNetwork::StopMode mode = PiiNetwork::WaitClients);

  /**
   * Adds a new server to the list of HTTP servers using the given 
   * `serverName` and the binding address `address`. I there already
   * exists a server with the same name, the old one will be
   * destroyed. If there are no servers, the first one will be set as
   * the default server.
   *
   * @param serverName the name of the server. The server instance can
   * be later retrieved with [server()]. If `serverName` is empty,
   * the new server will become the default server.
   *
   * @param address the low level protocol and the address to bind the
   * server to. The address may be either an IPv4 address
   * (tcp://123.123.123.123:80 or ssl://0.0.0.0:443), an IPv6 address
   * (tcp://[2001:db8::1428:57ab]:80 or ssl://[::1]:443), or the name
   * of a local socket (local:///tmp/server.sock on Linux,
   * local://\\\\.\\pipe\\socket on Windows). Network addresses must
   * contain a port number and no trailing slash. The server currently
   * supports `tcp`, `ssl`, and `local` connections.
   *
   * @return a pointer to a new PiiHttpServer instance, or zero if the
   * address is not valid. The pointer is still owned by
   * PiiHttpServer; you must not delete it yourself.
   *
   * ~~~
   * // Create a HTTPS server (0.0.0.0 binds to all network interfaces)
   * PiiFileSystemUriHandler handler("/var/www/securehtml");
   * PiiHttpServer* server = PiiHttpServer::addServer("secure", "ssl://0.0.0.0:443/");
   * server->protocol()->registerUriHandler("/", &handler);
   *
   * // Create another server that uses the same handler, but
   * // communicates through a local socket.
   * server = PiiHttpServer::addServer("local", "local:///tmp/http.sock");
   * server->protocol()->registerUriHandler("/", &handler);
   * ~~~
   *
   * This function is thread-safe.
   */
  static PiiHttpServer* addServer(const QString& serverName, const QString& address);

  /**
   * @overload
   *
   * Same as addServer("", address).
   */
  static PiiHttpServer* addServer(const QString& address) { return addServer("", address); }
  
  /**
   * Creates a new instance of PiiHttpServer at the given address. See
   * [addServer()] for valid addresses. This function does not add the
   * new server to the server list, and the caller is responsible for
   * deleting the server.
   *
   * This function is thread-safe.
   */
  static PiiHttpServer* createServer(const QString& address);
  
  /**
   * Returns the server called `serverName`. The server must have
   * been previously added with [addServer()]. If `serverName` is
   * empty, the default server will be returned.
   *
   * This function is thread-safe.
   */
  static PiiHttpServer* server(const QString& serverName = "");

  /**
   * Removes the server called `serverName` from the list of HTTP
   * servers.
   *
   * This function is thread-safe.
   */
  static void removeServer(const QString& serverName);

private:
  class ServerMap : public QMap<QString, PiiHttpServer*>
  {
  public:
    ~ServerMap()
    {
      for (iterator i = begin(); i != end(); ++i)
        delete i.value();
    }
  };

  PiiHttpServer(PiiNetworkServer* server, PiiHttpProtocol* protocol);

  /// @internal
  class Data
  {
  public:
    Data(PiiNetworkServer* server, PiiHttpProtocol* protocol);
    virtual ~Data();
    PiiNetworkServer* pServer;
    PiiHttpProtocol* pProtocol;
  } *d;

  static void deleteServer(const QString& serverName);
  
  static ServerMap* serverMap();
  static QMutex _mapLock;
  static PiiHttpServer* _pDefaultServer;
};

#endif //_PIIHTTPSERVER_H
