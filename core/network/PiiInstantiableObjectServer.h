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

#ifndef _PIIINSTANTIABLEOBJECTSERVER_H
#define _PIIINSTANTIABLEOBJECTSERVER_H

#include "PiiHttpProtocol.h"

/**
 * A remote object server that creates new instances of remote objects
 * on client request and passes requests to the correct instances.
 *
 * PiiObjectServer provides a HTTP interface for a single instance of
 * a remote object. All clients will be connected to the same object,
 * and changes made by one will affect others.
 *
 * PiiInstantiableObjectServer makes it possible to create a new
 * instance for each client. It works as a primary server that either
 * creates new server instances or passes requests to existing ones. 
 * There will be no "default" server object, but each client must
 * create a instance by requesting /new. The server uses
 * #createServer() function to create a new secondary server and
 * assigns a unique ID to it. The ID is then returned to the client.
 *
@verbatim
GET /new HTTP/1.1
@endverbatim
 *
 * Response:
 *
@verbatim
HTTP/1.1 200 OK
Content-Length: 35

243F6A8-885A-308D-3131-98A2E0370734
@endverbatim
 *
 * The returned ID can now be used as the URI of the new instance:
 *
@verbatim
GET /243F6A8-885A-308D-3131-98A2E0370734/properties/ HTTP/1.1
@endverbatim
 *
 * Inactive object instances will be kept alive only for a limited
 * amount of time. If no accesses have been made to the object for a
 * specified maximum number of milliseconds (see #instanceTimeout()),
 * the object will be automatically deleted. If no other requests are
 * made, the instance can be kept alive by requesting /ping.
 *
@verbatim
GET /243F6A8-885A-308D-3131-98A2E0370734/ping HTTP/1.1
@endverbatim
 *
 * Response:
 *
@verbatim
HTTP/1.1 200 OK
Content-Length: 0
@endverbatim
 *
 * The remote object instance can be explicitly destroyed by
 * requesting /delete:
 *
@verbatim
GET /243F6A8-885A-308D-3131-98A2E0370734/delete HTTP/1.1
@endverbatim
 *
 * It is possible to pass parameters to the new object instance by
 * encoding them into the request (GET or POST). This makes it
 * possible to even create totally different remote objects based on
 * the parameters. Suppose a server that can create QObjects based on
 * class name. Then, the following request would create an instance of
 * a QTimer.
 *
@verbatim
GET /new?className=QTimer HTTP/1.1
@endverbatim
 *
 * @ingroup Network
 */   
class PII_NETWORK_EXPORT PiiInstantiableObjectServer:
  public QObject,
  public PiiHttpProtocol::UriHandler
{
  Q_OBJECT
  Q_INTERFACES(PiiHttpProtocol::UriHandler)
public:
  PiiInstantiableObjectServer();
  ~PiiInstantiableObjectServer();
  
  /**
   * Sets the number of millisecond an object instance will be kept
   * alive after a client breaks connection to it without explicitly
   * deleting the instance. The default is 10000.
   */
  void setInstanceTimeout(int instanceTimeout);
  /**
   * Returns the instance time-out.
   */
  int instanceTimeout() const;

  /**
   * Sets the maximum number of remote object instances the server
   * will manage concurrently. Once this limit is reached, the server
   * will refuse to create new object instances. The default value is
   * 100.
   */
  void setMaxInstances(int maxInstances);
  /**
   * Returns the maximum number of remote object instances.
   */
  int maxInstances() const;

  void handleRequest(const QString& uri, PiiHttpDevice* dev,
                     PiiHttpProtocol::TimeLimiter* controller);

protected:
  /// @internal
  struct Instance
  {
    Instance(PiiHttpProtocol::UriHandler* server) : pServer(server)
    {
      idleTime.start();
    }
    
    QTime idleTime;
    PiiHttpProtocol::UriHandler* pServer;
  };  

  /// @internal
  class PII_NETWORK_EXPORT Data
  {
  public:
    Data();
    virtual ~Data();
    QMutex instanceMutex;
    int iInstanceTimeout;
    int iMaxInstances, iInstanceCount;
    QHash<QString,Instance> hashInstances;
  } *d;
  
  /**
   * Returns a new server instance given the @a parameters passed in
   * the HTTP request. This function must be overridden to create a
   * new server object for each client.
   *
   * @note If the returned handler is a PiiObjectServer that has its
   * thread safety level set to AccessFromMainThread, the returned
   * PiiObjectServer and the object to be served must be moved to the
   * main thread before returning. Otherwise, calls to the object will
   * never be delivered.
   */
  virtual PiiHttpProtocol::UriHandler* createServer(const QVariantMap& parameters) = 0;

private slots:
  void collectGarbage();
  
private:
  QString createNewInstance(const QVariantMap& parameters);
  PiiHttpProtocol::UriHandler* findInstance(const QString& instanceId) const;
  void deleteServer(const QString& serverId);
  void deleteServers();  
};

#endif //_PIIINSTANTIABLEOBJECTSERVER_H
