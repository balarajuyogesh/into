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

#ifndef _PIIREMOTEOBJECT_H
#define _PIIREMOTEOBJECT_H

#include "PiiNetwork.h"
#include <PiiPreprocessor.h>
#include <PiiLockedPtr.h>
#include <PiiAtomicInt.h>

#include <QVector>
#include <QVariant>
#include <QCoreApplication>
#include <QMutex>
#include <QWaitCondition>

#include <PiiProgressController.h>
#include <PiiGenericFunction.h>

#include "PiiNetworkClient.h"
#include "PiiHttpDevice.h"

/**
 * PiiRemoteObject is a client for PiiObjectServer. It is used to call
 * functions transparently over network. PiiRemoteObject also supports
 * call-back functions invoked by the server object.
 *
 * The remote object is identified by a URI that speficies the
 * protocol, address, and relative path of the server. The server at
 * the specified URI must implement the interface speficied by
 * PiiObjectServer.
 *
 * ~~~(c++)
 * struct MyHandler
 * {
 *   // Invoked from the server
 *   void callback(const QString& str) { piiDebug(str); }
 * };
 *
 * PiiRemoteObject obj("tcp://localhost:3142/myClass/");
 * Handler h;
 * int iSum = obj.call<int>("plus", 1, 2);
 * QString strHello = obj.call<QString>("hello");
 * obj.call<void>("voidFunc");
 * obj.addCallback("callback", &h, &MyHandler::callback);
 * ~~~
 *
 */
class PII_NETWORK_EXPORT PiiRemoteObject :
  private PiiProgressController
{
public:
  /**
   * Creates a new PiiRemoteObject. The remote object becomes
   * functional only after [setServerUri()] has been called.
   */
  PiiRemoteObject();

  /**
   * Creates a new PiiRemoteObject and sets the server URI at
   * the same time. See [setServerUri()].
   *
   * @exception PiiNetworkException& if the server cannot be connected
   * @exception PiiInvalidArgumentException& if *serverUri* is
   * incorrecly formatted
   */
  PiiRemoteObject(const QString& serverUri);

  ~PiiRemoteObject();
  
  /**
   * Sets the server URI. The URI must contain a protocol, a host
   * name, and a path (a single slash at a minimum). Other components
   * (username, password and port) are optional. This function will
   * open a connection to the server and update the local meta object
   * based on the server's response.
   *
   * ~~~(c++)
   * PiiRemoteObject client;
   * client.setServerUri("tcp://intopii.com:3142/valuesetter/");
   * ~~~
   *
   * ! Reassigning signals and slots makes all previously
   * requested meta objects invalid. You can no longer call the
   * functions or slots that were previously available, and no old
   * properties can be read or written to. If you had connected a
   * sender to a dynamic slot and the meta information changed,
   * invoking the slot later would crash your application. Therefore,
   * changing the server URI will not recreate the meta-object. It is
   * assumed that the same object is present in the new URI. This
   * makes it possible to move remote objects.
   *
   * @exception PiiNetworkException& if the server cannot be connected
   * @exception PiiInvalidArgumentException& if *uri* is incorrecly formatted
   */
  void setServerUri(const QString& uri);

  /**
   * Returns the URI of the server object.
   */
  QString serverUri() const;

  /**
   * Sets the number of times the class will try to connect to a
   * server if the first attempt fails. The default is 2. The maximum
   * is 5 to avoid hangs.
   */
  void setRetryCount(int retryCount);
  int retryCount() const;

  /**
   * Sets the number of milliseconds between reconnection attempts. 
   * The default is 1000. The maximum is 2000 to avoid hangs.
   */
  void setRetryDelay(int retryDelay);
  int retryDelay() const;
  
  /**
   * Requests *path* relative to the server's root and returns the
   * reply as a list of strings split at each newline character.
   *
   * @param path relative path of the directory (no leading slash)
   */
  QList<QByteArray> readDirectoryList(const QString& path);

  template <class Object, class Function>
  inline QString addCallback(const QString& name, Object* obj, Function func)
  {
    return addCallback(name, Pii::genericFunction(obj, func));
  }

  /**
   * Adds *function* to the list of call-back functions. The given
   * function will be invoked when the call-back function identified
   * by *name* is called from the server.
   *
   * PENDING ownership
   */
  QString addCallback(const QString& name, PiiGenericFunction* function);

  void removeCallback(const QString& signature);

  void removeCallbacks();

  /**
   * @decl template <class R> R call(const QString& function, ...)
   *
   * Calls the remote *function* with a variable number of parameters
   * and returns its return value as an object of type `R`. The type
   * of the return value is given as a template parameter, and may be
   * `void`.
   *
   * @exception PiiNetworkException& or PiiHttpException& if the
   * remote function call failed.
   *
   * @exception PiiSerializationException& if function parameters
   * could not be encoded or if the server's response could not be
   * decoded.
   *
   * @exception PiiException& if the remote function throws one. If
   * the type of the exception is known by both the client and the
   * server, the type information will be preserved. That is, if the
   * server throws a PiiMathException, you can catch it as
   * a PiiMathException on the client side.
   */

  /// @hide
  template <class R> R call(const QString& function)
  {
    return PiiNetwork::returnValue<R>(callList(function, QVariantList()));
  }
  PII_CREATE_REMOTE_CALL(1, (P1))
  PII_CREATE_REMOTE_CALL(2, (P1,P2))
  PII_CREATE_REMOTE_CALL(3, (P1,P2,P3))
  PII_CREATE_REMOTE_CALL(4, (P1,P2,P3,P4))
  PII_CREATE_REMOTE_CALL(5, (P1,P2,P3,P4,P5))
  PII_CREATE_REMOTE_CALL(6, (P1,P2,P3,P4,P5,P6))
  PII_CREATE_REMOTE_CALL(7, (P1,P2,P3,P4,P5,P6,P7))
  PII_CREATE_REMOTE_CALL(8, (P1,P2,P3,P4,P5,P6,P7,P8))
  /// @endhide

  QVariant callList(const QString& function, const QVariantList& params);

  /**
   * Returns the number of failures in remote calls since construction
   * or last reset. The count is incremented each time a remote
   * function call fails.
   */
  int failureCount() const;
  /**
   * Resets the failure count to zero.
   */
  void resetFailureCount();

  /**
   * Sets the maximum number of failures accepted. Once 
   * *maxFailureCount* errors have occured, every remote call will fail
   * immediately with a PiiNetworkException. This value can be used to
   * speed up recovery. The default value is -1, which disables this
   * feature.
   */
  void setMaxFailureCount(int maxFailureCount);
  int maxFailureCount() const;  

protected:
  /// @internal
  class PII_NETWORK_EXPORT Data
  {
  public:
    Data();

    PiiNetworkClient networkClient;
    QMutex deviceMutex; // Must be held when accessing pHttpDevice
    PiiHttpDevice* pHttpDevice;
    QString strPath;
    QList<QPair<QString,PiiGenericFunction*> > lstCallbacks;

    QString strChannelId;
    QThread* pChannelThread;
    QMutex channelMutex;
    QWaitCondition channelUpCondition;
    volatile bool bChannelRunning;
    int iRetryCount;
    int iRetryDelay;
    PiiAtomicInt iFailureCount;
    QStringList lstConnectedSources;
    int iMaxFailureCount;
  } *d;
  /// @internal
  PiiRemoteObject(Data*);
  /// @internal
  PiiRemoteObject(Data*, const QString&);

  typedef PiiLockedPtr<PiiHttpDevice> HttpDevicePtr;
  
  /**
   * Adds the given *sourceId* to the resources pushed from the
   * server.
   *
   * @exception PiiNetworkException& or PiiHttpException& if the
   * remote function call failed.
   */
  void connectToChannel(const QString& sourceId);
  
  /**
   * Removes the given *sourceId* from the resources pushed from the
   * server.
   *
   * @exception PiiNetworkException& or PiiHttpException& if the
   * remote function call failed.
   */
  void disconnectFromChannel(const QString& sourceId);

  /**
   * Decodes a message received from a return channel. The default
   * implementation recognizes call-back functions and delivers them
   * to registered call-backs. Override this function to handle other
   * types of pushed data.
   *
   * @param sourceId the identifier of the source that sent the data
   *
   * @param data encoded data
   *
   * @exception PiiException& if *data* could not be decoded or the
   * action taken due to it failed.
   *
   * @see PiiObjectServer::enqueuePushData()
   */
  virtual void decodePushedData(const QString& sourceId, const QByteArray& data);
  
  /**
   * Tries [retryCount()] times to open a connection to [serverUri()]. 
   * Returns an exclusive pointer to a connected HTTP device. 
   * PiiRemoteObject owns the returned pointer.
   *
   * @exception PiiNetworkException& if the connection cannot be
   * established.
   */
  HttpDevicePtr openConnection();
  /**
   * Closes the connection. There is usually no need to call this
   * function in normal operation, but it may be useful in recovering
   * from errors.
   */
  void closeConnection();

  void addFailure();

  virtual void serverUriChanged(const QString& strNewUri);

private:
  inline static QString tr(const char* s) { return QCoreApplication::translate("PiiRemoteObject", s); }
  
  void manageChannel(const QString& operation, const QString& sourceId);
  void openChannel();
  void closeChannel();
  void readChannel();
  bool requestNewChannel(PiiHttpDevice& dev);
  bool reconnectSources();
  void stopChannelThread();
  bool checkChannelResponse(PiiHttpDevice& dev);
  bool canContinue(double progressPercentage) const;
  void setServerUriImpl(const QString& uri);
  void handleException(PiiHttpDevice* dev);
};


#endif //_PIIREMOTEOBJECT_H
