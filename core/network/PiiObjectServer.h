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

#ifndef _PIIOBJECTSERVER_H
#define _PIIOBJECTSERVER_H

#include "PiiHttpProtocol.h"
#include <PiiUniversalSlot.h>
#include <PiiGenericFunction.h>
#include <PiiTimer.h>
#include <PiiException.h>

#include <QMetaMethod>
#include <QVariant>
#include <QWaitCondition>
#include <QQueue>
#include <QStringList>
#include <QHash>

/**
 * A URI handler for PiiHttpProtocol that maps HTTP requests to member
 * function calls on any object instance. 
 *
 * Usage example
 * -------------
 *
 * In the example below, a GET request to
 * http://localhost:3142/myClass/functions/plus?1&2 would return 3. 
 * http://localhost:3142/myClass/functions/hello would return "Hello,
 * Finland!".
 *
 * ~~~(c++)
 * class MyClass
 * {
 * public:
 *   QString hello() { return "Hello, Finland!"; }
 *   int plus(int a, int b) { return a + b; }
 *   void voidFunc() {}
 * };
 *
 * PiiHttpServer* pHttpServer = PiiHttpServer::addServer("My server", "tcp://0.0.0.0:3142");
 * MyClass* pMyClass = new MyClass;
 * PiiObjectServer* pObjectServer = new PiiObjectServer;
 *
 * pObjectServer->addFunction("hello", pMyClass, &MyClass::hello);
 * pObjectServer->addFunction("plus", pMyClass, &MyClass::plus);
 * pObjectServer->addFunction("voidFunc", pMyClass, &MyClass::voidFunc);
 * pObjectServer->addCallback("callback(QString)");
 * 
 * pHttpServer->protocol()->registerUriHandler("/myClass/", pObjectServer);
 * pHttpServer->start();
 *
 * // ...
 * // Invoke a call-back function
 *
 * pObjectServer->callBack("callback", "Callback invoked.");
 * ~~~
 *
 * Mapping HTTP request to function calls
 * --------------------------------------
 *
 * PiiObjectServer provides three functions at the root of the remote
 * object: functions, channels, callbacks, and "ping". A request to
 * "/", "/functions/", "/channels/", or "/callbacks/" returns a list
 * of sub-URIs available at the requested location.
 *
 * ~~~
 * GET /myClass/ HTTP/1.1
 * ~~~
 *
 * Response:
 *
 * ~~~
 * HTTP/1.1 200 OK
 * Content-Type: text/plain
 * Content-Length: 25
 * 
 * functions/
 * channels/
 * ping
 * ~~~
 *
 *
 * A GET request to "/functions/" lists all callable functions:
 *
 * ~~~
 * GET /myClass/functions/ HTTP/1.1
 * ~~~
 *
 * Response:
 *
 * ~~~
 * HTTP/1.1 200 OK
 * Content-Type: text/plain
 * Content-Length: 44
 * 
 * QString hello()
 * int plus(int,int)
 * voidFunc()
 * ~~~
 *
 * A call to a function with no parameters is a simple GET request:
 *
 * ~~~
 * GET /myClass/functions/voidFunc HTTP/1.1
 * ~~~
 *
 * Note that the Host header is not necessary, although HTTP 1.1
 * dictates one. The response is equally simple (no return value):
 *
 * ~~~
 * HTTP/1.1 200 OK
 * Content-Length: 0
 * ~~~
 *
 * If the function has parameters, they can be specified in the URI:
 *
 * ~~~
 * GET /myClass/functions/plus?a=1&b=2 HTTP/1.1
 * ~~~
 *
 * The server requires that parameters are given in the order they are
 * declared in the function signature. It ignores the parameter name,
 * which can therefore be omitted. Therefore, the example above is
 * equivalent to:
 *
 * ~~~
 * GET /myClass/functions/plus?1&2 HTTP/1.1
 * ~~~
 *
 * Parameters are automatically decoded using @ref
 * PiiHttpDevice::decodeVariant(). If the function has a return value,
 * the same encoding will be used in the return message body.
 *
 * Channels and call-back functions
 * --------------------------------
 *
 * Channels can be used to implement callback functions from the
 * remote object. Creating a call-back requires a persistent return
 * channel the server can use to push data to the client. The
 * "/channels" URI contains functions related to channel management.
 *
 * - /channels/
 * - /channels/new
 * - /channels/channel-id
 * - /channels/channel-id/connect
 * - /channels/channel-id/disconnect
 * - /channels/channel-id/sources
 * - /channels/channel-id/delete
 *
 * A new channel is created by requesting /channels/new. The server
 * responds by sending the ID of a newly created channel as the first
 * row of the return body (the preamble of a multipart message). The
 * requesting socket will be left open, and the server pushes new data
 * to it when needed.
 *
 * ~~~
 * GET /myClass/channels/new HTTP/1.1
 * ~~~
 *
 * Response:
 *
 * ~~~
 * HTTP/1.1 200 OK
 * Content-Type: multipart/mixed-replace; boundary="ural"
 * 
 * 4A40938-2229-9F31-D008-2EFA98EC4E6C
 * --ural
 * X-ID: callbacks/callback(QString)
 * Content-Length: 17
 * 
 * Callback invoked.
 * --ural
 * ...
 * ~~~
 *
 * Now that the channel has been set up, one can add pushable sources
 * to the channel. To request the server to push a source called
 * "callback" to the channel, the following request must be made:
 *
 * ~~~
 * GET /channels/4A40938-2229-9F31-D008-2EFA98EC4E6C/connect?callbacks/callback(QString) HTTP/1.1
 * ~~~
 *
 * The request must have one paratemer that identifies the pushable
 * source. Usually, a URI relative to the server's root is used. A
 * pushable source can be disconnected by requesting
 * /channels/channel-id/disconnect in a similar manner. All connected
 * sources are listed at /channels/channel-id/sources/.
 *
 * Whenever new registered data becomes available it will be written
 * to the channel. The ID of the source will be sent in a non-standard
 * "X-ID" MIME header. From the point of view of PiiObjectServer, the
 * data pushed to the channel is just a QByteArray. Encoding and
 * decoding must be implemented case-by-case.
 *
 * A client can reconnect to a disconnected channel by requesting
 * /channels/channel-id. The server keeps unclosed channels in memory
 * for a while allowing clients to recover from network failures. If
 * the channel is still alive, this call will re-establish it as if
 * /channels/new was requested. The only differences are that all
 * previously added pushable sources are still in effect and that
 * the channel ID will not be returned.
 *
 * A channel can be explicitly destroyed by requesting
 * /channels/channel-id/delete.
 *
 * Ping
 * ----
 *
 * PiiObjectServer provides "ping" functionality, which is mainly
 * provides a a non-intrusive availability check. Requesting "/ping"
 * only returns a "200 OK" status code. If PiiObjectServer is used
 * with PiiInstantiableObjectServer, pinging can be used to indicate
 * the server that the client is still alive.
 *
 */
class PII_NETWORK_EXPORT PiiObjectServer :
  public QObject,
  public PiiHttpProtocol::UriHandler
{
  Q_OBJECT
  Q_INTERFACES(PiiHttpProtocol::UriHandler)  
  Q_ENUMS(ThreadSafetyLevel)
public:
  /**
   * Thread safety levels.
   *
   * - `AccessFromMainThread` - the object/function can only be
   * accessed from a single, fixed thread - the main thread of the
   * server application. This is a strict requirement and may become a
   * performance bottleneck. It may however be necessary for certain
   * GUI-related tasks.
   *
   * - `AccessFromAnyThread` - the object/function can be accessed
   * from any thread, but synchronized wrt each other.
   *
   * - `AccessConcurrently` - the object/function can be accessed
   * from any thread, even simultaneously. The object will take care
   * of mutual exclusion in its internal logic.
   */
  enum ThreadSafetyLevel
  {
    AccessFromMainThread,
    AccessFromAnyThread,
    AccessConcurrently
  };
  
  PiiObjectServer();
  ~PiiObjectServer();

  /**
   * Sets the default safety level for the served object. If no
   * function-specific levels are set, this level will be used for all
   * calls. The default value is `AccessFromAnyThread`.
   *
   * ! If a QObjectServer is not created in the main thread (e.g. 
   * via PiiInstantiableObjectServer), it must be moved to the main
   * thread if the access mode is `AccessFromMainThread`. Otherwise,
   * calls to the object would never be delivered. The same applies to
   * the object being served. Therefore, setting the safety level to
   * `AccessFromMainThread` automatically moves this object to the
   * main thread. This move cannot be undone. Thus, once the safety
   * level is set to `AccessFromMainThread`, it can no longer be
   * changed.
   */
  void setSafetyLevel(ThreadSafetyLevel safetyLevel);
  ThreadSafetyLevel safetyLevel() const;

  /**
   * Sets the safety level of an individual function, identified by 
   * *functionSignature*. Getting the signature right is important
   * because different overloads of the same function may have
   * different safety levels.
   *
   * ~~~(c++)
   * pObjectServer->setFunctionSafetyLevel("plus(int,int)", PiiObjectServer::AccessFromMainThread);
   * ~~~
   */
  void setFunctionSafetyLevel(const QString& functionSignature, ThreadSafetyLevel safetyLevel);
  /**
   * Returns the safety level of the given function. If no
   * function-specific safety level is set, returns the default safety
   * level.
   */
  ThreadSafetyLevel functionSafetyLevel(const QString& functionSignature) const;
  /**
   * Removes a previously set function-specific safety level.
   */
  void removeFunctionSafetyLevel(const QString& functionSignature);

  void handleRequest(const QString& uri, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller);
  
  /**
   * Adds a new callable function to the server's interface. The
   * signature of the function will be added to the function list
   * under "functions/". Returns the signature of the function.
   *
   * @param name the name of the function. A function may have many
   * overloads that will be resolved using parameter types.
   *
   * @param obj a pointer to an object whose function is to be called.
   *
   * @param func the function to be called. Passed as a function
   * pointer.
   *
   * ~~~(c++)
   * pObjectServer->addFunction("func", &myClass, &MyClass::func);
   * ~~~
   */
  template <class Object, class Function>
  inline QString addFunction(const QString& name, Object* obj, Function func)
  {
    return addFunction(name, Pii::genericFunction(obj, func));
  }

  /**
   * Adds *function* to the server's interface as *name*. 
   * PiiObjectServer takes the ownership of the pointer.
   */
  QString addFunction(const QString& name, PiiGenericFunction* function);

  /**
   * Removes the function with the given signature from the server's
   * interface.
   */
  void removeFunction(const QString& signature);

  /**
   * Removes all functions from the server's interface.
   */
  void removeFunctions();

  /**
   * Returns the signatures of all functions added with
   * [addFunction()]. Each signature contains a return type (if there
   * is one), function name, and a list of parameter types. For
   * example "start(int)" is a valid signature.
   */
  virtual QStringList functionSignatures() const;

  /**
   * Returns a "directory list" of the root folder. This function
   * makes it possible for subclasses to add functionality to the
   * default URI tree. If you override this function, add your new
   * entries to the list returned by the default implementation. It is
   * also possible to remove entries from the list.
   *
   * @return a list of sub-URIs. URIs that may contain sub-URIs should
   * end with a slash (/).
   */
  virtual QStringList listRoot() const;

  /**
   * Sets the number of millisecond a channel will be kept alive after
   * a client breaks connection to it without explicitly closing the
   * channel. The default is 10000.
   */
  void setChannelTimeout(int channelTimeout);
  
  /**
   * Returns the current channel time-out.
   */
  int channelTimeout() const;

  /**
   * Registers a call-back function with the given *signature*. The
   * signature will be listed under "/callbacks/" and clients will be
   * allowed to add "callbacks/signature" as a pushable source to
   * their return channels.
   *
   * @param signature the signature of a call-back function. It is
   * important to get the signature right; it is used by clients to
   * determine the parameter types of a call-back function. The
   * signature consists of a function name followed by a
   * comma-separated list of parameter names in braces. 
   * "valueChanged(int)" is an example of a valid signature. If
   * unsure, use QMetaObject::normalizedSignature().
   *
   * @return `true` if successful, `false` if *signature* is
   * already registered or ill-formatted.
   */
  bool addCallback(const QString& signature);
  /**
   * Removes a call-back with the given *signature*.
   */
  void removeCallback(const QString& signature);
  /**
   * Removes all call-back functions.
   */
  void removeCallbacks();

  /**
   * @fn void callBack<R>(const QString& function, ...)
   *
   * Invokes the call-back function called *function* with the given
   * parameters and return type R (which may be void). The parameters
   * will be checked against all registered call-back functions.
   *
   * @exception PiiInvalidArgumentException& if no acceptable overload
   * for the named function is found.
   *
   * @exception PiiNetworkException& or PiiHttpException& if the
   * remote function call failed.
   *
   * @exception PiiSerializationException& if function parameters
   * could not be encoded.
   */
  
  /// @hide
  template <class R> R callBack(const QString& function)
  {
    QVariantList lstParams;
    return PiiNetwork::returnValue<R>(callBackList(function, lstParams));
  }
  PII_CREATE_REMOTE_CALLBACK(1, (P1))
  PII_CREATE_REMOTE_CALLBACK(2, (P1,P2))
  PII_CREATE_REMOTE_CALLBACK(3, (P1,P2,P3))
  PII_CREATE_REMOTE_CALLBACK(4, (P1,P2,P3,P4))
  PII_CREATE_REMOTE_CALLBACK(5, (P1,P2,P3,P4,P5))
  PII_CREATE_REMOTE_CALLBACK(6, (P1,P2,P3,P4,P5,P6))
  PII_CREATE_REMOTE_CALLBACK(7, (P1,P2,P3,P4,P5,P6,P7))
  PII_CREATE_REMOTE_CALLBACK(8, (P1,P2,P3,P4,P5,P6,P7,P8))
  /// @endhide

  QVariant callBackList(const QString& function, QVariantList& params);

protected:
  /**
   * A return channel used to push data from pushable sources to the
   * client. Return channels are created upon client requests and
   * managed by the server. The [connectToChannel()],
   * [disconnectFromChannel()] and [channelDeleted()] functions are used
   * to inform subclasses about events related to the channel.
   */
  class PII_NETWORK_EXPORT Channel
  {
  public:
    virtual ~Channel();
    /**
     * Puts *data* from the source identified by *sourceId* to the
     * output queue. Returns `true` if successful, `false` if the
     * queue cannot take more right now.
     */
    bool enqueuePushData(const QString& sourceId, const QByteArray& data);

  protected:
    /// @hide
    mutable QMutex _queueMutex;
    QWaitCondition _queueCondition, _pushEndCondition;
    QQueue<QPair<QString,QByteArray> > _dataQueue;
    /// @endhide
  };

  /// @internal
  class PII_NETWORK_EXPORT ChannelImpl : public Channel
  {
  public:
    ChannelImpl();
    
    void push(PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller, QMutexLocker* lock);
    void removeObjectsQueuedTo(const QString& uri);
    bool isAlive(int timeout) const;
    void quit();
    void wait();

    QStringList lstSources;

  private:
    bool _bPushing, _bKilled;
    PiiTimer _idleTimer;
  };

  /// @internal
  class CallbackFunction : public PiiGenericFunction
  {
  public:
    CallbackFunction(const QString& name, const QList<int>& types)
    {
      d->lstParamTypes = types;
      _strSourceId = "callbacks/" + signature(name);
    }
    void call(void** args);

    QList<Channel*> lstChannels;
  private:
    QString _strSourceId;
  };

  /// @internal (name, function) pairs
  typedef QList<QPair<QString,PiiGenericFunction*> > FunctionList;

  /// @internal
  typedef QMap<QString,ThreadSafetyLevel> SafetyLevelMap;
  
  /// @internal
  class PII_NETWORK_EXPORT Data
  {
  public:
    Data();
    virtual ~Data();
    FunctionList lstFunctions, lstCallbacks;
    QMutex channelMutex;
    QHash<QString,ChannelImpl*> hashChannelsById;
    int iChannelTimeout;
    ThreadSafetyLevel safetyLevel;
    SafetyLevelMap mapFunctionSafetyLevels;
    mutable QMutex accessMutex;
  } *d;

  /// @internal
  PiiObjectServer(Data*);

  /**
   * Invokes *function* with *params* and returns its return value. 
   * This function is called by PiiObjectServer when a client
   * requests /functions/function. The parameters to the call are
   * automatically decoded and passed to this function. The return
   * value will be written back to the client.
   *
   * The default implementation looks for the function name in the
   * list of functions added with [addFunction()] and resolves their
   * overloaded versions. If there is a function that accepts the
   * passed argument types directly or if the arguments can be
   * converted to the required types, the function will be invoked. 
   * You can override this function to implement functions in a
   * different way.
   *
   * @param function function name without parentheses or parameters,
   * e.g "start".
   *
   * @param params function parameters, may be modified by the call
   *
   * @return the return value of the function, or an invalid variant
   * if the function has no return value.
   *
   * @exception PiiHttpException& if something goes wrong. The default
   * implementation throws `NotFoundStatus` if the function does not
   * exist, and `BadRequestStatus` if an overload matching *params*
   * cannot be found.
   *
   * @exception Any other exception if the call itself fails for some
   * other reason.
   */
  virtual QVariant call(const QString& function, QVariantList& params);

  /**
   * Connects the pushable the source identified by *sourceId* to 
   * *channel*. The default implementation assumes that *sourceId* is
   * "signals/signature", where signature is the function signature of
   * a signal in the QObject being served, and connects it to the
   * channel.
   *
   * Subclasses can override this function to add custom pushable
   * sources to channels. Whenever new pushable data from 
   * *sourceUri* is available, it should be pushed to all channels to
   * which the source is connected.
   *
   * @exception PiiHttpException& if something goes wrong. The default
   * implementation throws an exception with `NotFoundStatus` if 
   * *sourceUri* isn't a valid URI for a signal.
   */
  virtual void connectToChannel(Channel* channel, const QString& sourceId);
  
  /**
   * Disconnects the pushable the source identified by *sourceId*
   * from *channel*. The default implementation assumes that 
   * *sourceId* is "signals/signature", where signature is the function
   * signature of a signal in the QObject being served, and
   * disconnects it from the channel.
   *
   * Subclasses can override this function to remove custom pushable
   * sources from channels. Once this function has been called, no
   * more data from *sourceId* must be pushed to the channel.
   *
   * @exception PiiHttpException& if something goes wrong. The default
   * implementation throws an exception with `NotFoundStatus` if 
   * *sourceUri* isn't a valid URI for a signal.
   */
  virtual void disconnectFromChannel(Channel* channel, const QString& sourceId);

  /**
   * Informs subclasses that *channel* is about to be deleted. The
   * channel is no longer valid, and sources must stop sending data to
   * it. The default implementation detaches the channel from all
   * call-back functions.
   */
  virtual void channelDeleted(Channel* channel);

  /// @internal
  virtual ChannelImpl* createChannel() const;

  /// @internal
  virtual void moveToMainThread();
  
private slots:
  void collectGarbage();
  QVariant callFromMainThread(void* function, void* params);

private:
  void init();
  void createExceptionResponse(PiiHttpDevice* dev, const PiiException& ex);
  QString createNewChannel();
  inline ChannelImpl* channelById(const QString& id);
  void killChannels();
  void disconnectChannel(Channel* channel);
  void handleChannelCommand(const QString& uri, PiiHttpDevice* dev,
                            PiiHttpProtocol::TimeLimiter* controller,
                            QMutexLocker* lock);
  QVariantList paramList(PiiHttpDevice* dev, const QStringList& names, const QVariantMap& map);
  int indexOf(const QString& signature, const FunctionList& lst) const;
  void removeFunction(const QString& signature, FunctionList& lst);
  void removeFunctions(FunctionList& lst);

  PII_DISABLE_COPY(PiiObjectServer);
};


#endif //_PIIOBJECTSERVER_H
