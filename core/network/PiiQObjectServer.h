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

#ifndef _PIIQOBJECTSERVER_H
#define _PIIQOBJECTSERVER_H

#include "PiiObjectServer.h"

class PiiConfigurable;

/**
 * An object that maps QObject's functions and properties into the URI
 * space of a PiiHttpProtocol.
 *
 * Usage example
 * -------------
 *
 * If you just need to start quick, do this:
 *
 * ~~~(c++)
 * QTimer t; // ... or any other class derived from QObject
 * PiiHttpServer* pHttpServer = PiiHttpServer::addServer("My server", "tcp://0.0.0.0:3142");
 * PiiQObjectServer* pQObjectServer = new PiiQObjectServer(&t);
 * pHttpServer->protocol()->registerUriHandler("/timer/", pQObjectServer);
 * pHttpServer->start();
 * ~~~
 *
 * See PiiRemoteQObject for the client side. If you want the details,
 * read on.
 *
 * Slots and other functions
 * -------------------------
 *
 * PiiQObjectServer makes a QObject's properties, signals, slots, and
 * other invokable functions available to client software by mapping
 * the object to an URI in PiiHttpServer. By default, all properties
 * (including dynamic ones) and public invokable methods will be made
 * available as sub-URIs. For example, a QTimer registered at /timer/
 * would create the following URI structure:
 *
 * - /timer/
 * - /timer/functions/
 * - /timer/functions/start
 * - /timer/functions/stop
 * - /timer/signals/
 * - /timer/signals/timeout()
 * - /timer/properties/
 * - /timer/properties/active
 * - /timer/properties/interval
 * - /timer/properties/singleShot
 *
 * Note that the QTimer::start() function has two overloaded versions,
 * both of which can be accessed through the same URI. The server
 * inspects the parameter types to find out which function to call.
 *
 * A call to slot is as simple as a GET request:
 *
 * ~~~
 * GET /timer/functions/start HTTP/1.1
 * ~~~
 *
 * With parameters:
 *
 * ~~~
 * GET /timer/functions/start?1000 HTTP/1.1
 * ~~~
 *
 * Properties
 * ----------
 *
 * The properties of a QObject are provided under the "/properties/"
 * URI. Each property has a URI that corresponds to its name. By
 * default, both static and dynamic properties will be listed.
 * Property definitions can be queried with a GET request to
 * /properties/:
 *
 * ~~~
 * GET /timer/properties/ HTTP/1.1
 * ~~~
 *
 * Response:
 *
 * ~~~
 * HTTP/1.1 200 OK
 * Content-Type: text/plain
 * Content-Length: 41
 *
 * bool active
 * int interval
 * bool singleShot
 * ~~~
 *
 * The output format can be altered by adding a "format" parameter to
 * the request. If you are writing an Ajax application, you may want
 * to try this:
 *
 * ~~~
 * GET /timer/properties/?format=json&fields=type,name,value HTTP/1.1
 * ~~~
 *
 * Individual property values can be retrieved with a GET request to
 * the property URI:
 *
 * ~~~
 * GET /timer/properties/active HTTP/1.1
 * ~~~
 *
 * Response:
 *
 * ~~~
 * HTTP/1.1 200 OK
 * Content-Type: text/plain
 * Content-Length: 4
 *
 * true
 * ~~~
 *
 * Setting property values may or may not be allowed by the server.
 * The value of an individual property can be set either with a GET or
 * a POST request:
 *
 * ~~~
 * GET /timer/properties/interval?12345 HTTP/1.1
 *
 * POST /timer/properties/interval HTTP/1.1
 * Content-Type: application/x-www-form-urlencoded
 * Content-Length: 15
 *
 * interval=12345
 * ~~~
 *
 * Many properties can be set at once with either a GET request or a
 * POST (x-www-form-urlencoded) request.
 *
 * ~~~
 * GET /timer/properties?interval=1000&singleShot=false HTTP/1.1
 *
 * POST /timer/properties HTTP/1.1
 * Content-Type: application/x-www-form-urlencoded
 * Content-Length: 30
 *
 * interval=1000&singleShot=false
 * ~~~
 *
 * A property setter function doesn't necessary accept the value as
 * such. Therefore, PiiQObjectServer checks the value after a
 * setProperty() call. If the returned value differs from what was
 * intended, the actual value will be returned. For example, if you
 * try to assign a negative number to a property that only accepts
 * non-negative values:
 *
 * ~~~
 * GET /timer/properties/interval?-1 HTTP/1.1
 * ~~~
 *
 * The server could respond with this:
 *
 * ~~~
 * HTTP/1.1 200 OK
 * Content-Type: text/plain
 * Content-Length: 1
 *
 * 0
 * ~~~
 *
 * If the provided value cannot be converted to the target type (e.g.
 * when assigning a string to an integer property), the server
 * responds with a bad request (400) status code.
 *
 *
 * Signals
 * -------
 *
 * Signals are implemented using the channel mechanism of
 * PiiObjectServer. A return channel must be set up before a signal
 * can be connected to. Then, the return channel can be used to
 * deliver any number of signals and also other data.
 *
 * Signals are listed under the "/signals" URI and identified by their
 * function signature. Connecting the timeout signal to a newly
 * created channel goes like so:
 *
 * ~~~
 * GET /channels/4A40938-2229-9F31-D008-2EFA98EC4E6C/connect?signals/timeout() HTTP/1.1
 * ~~~
 *
 * When a signal is emitted, its function parameters will be encoded
 * as a QVariantList using Into's [Serialization] mechanism.
 *
 *
 * Property Change Notification
 * ----------------------------
 *
 * Qt uses notification signals as a way to inform interested parties
 * about changes to properties. This technique can be used to inspect
 * changes to properties over the network as well, provided that such
 * signals have been implemented. This is however not the case with
 * all properties.
 *
 * Since performance on client side relies on property caching,
 * PiiQObjectServer generates property change signals to all
 * properties that are set through the network interface. It doesn't
 * however keep an eye on locally changed properties. Such changes
 * must be informed using the normal notification mechanism.
 *
 * If a property has a native property change signal whose signature
 * follows the naming pattern "<propertyName>Changed(<propertyType>)",
 * implicit property change signals will not be generated. Instead, it
 * is assumed that the native implementation of the property setter
 * generates the signal when needed.
 *
 *
 * Thread-safety issues
 * --------------------
 *
 * Qt's threading architecture makes it impossible to do certain
 * things from many threads even if mutual exclusion was handled
 * properly. For example, it is not possible to set the parent of a
 * QObject to an object that was created in another thread.
 * PiiObjectServer and PiiQObjectServer work around this by allowing
 * one to set thread safety levels to functions and properties.
 *
 * If the safety level of an object or a property is set to
 * `AccessPropertyFromMainThread`, both reads and writes to the
 * property will go through the main event loop. To avoid making the
 * main thread a major bottleneck, the default behaviour is
 * `WritePropertyFromMainThread` that reads the property from multiple
 * threads, taking care of mutual exclusion.
 *
 * To avoid manually setting the safety levels, PiiQObjectServer
 * provides a way of marking unsafe properties and functions with
 * class info fields:
 *
 * ~~~(c++)
 * class MyClass : public QObject
 * {
 *   Q_OBJECT
 *   // WritePropertyFromMainThread for unsafeProperty1 and unsafeProperty2,
 *   // AccessPropertyFromMainThread for fragileProperty
 *   Q_CLASSINFO("propertySafetyLevel",
 *               "unsafeProperty1:1 unsafeProperty2:1 fragileProperty:0");
 *   // AccessFromMainThread
 *   Q_CLASSINFO("functionSafetyLevel", "unsafeSlot1(int):0 unsafeSlot2(QString):0");
 *
 *   Q_PROPERTY(int unsafeProperty1 READ unsafeProperty1 WRITE setUnsafeProperty1);
 *   Q_PROPERTY(bool unsafeProperty2 READ unsafeProperty2 WRITE setUnsafeProperty2);
 *   Q_PROPERTY(double fragileProperty READ fragileProperty WRITE setFragileProperty);
 *
 * public slots:
 *   int unsafeSlot1(int value);
 *   void unsafeSlot2(const QString& str);
 *
 *   // ...
 * };
 * ~~~
 *
 * Property names and function signatures are both separated with a
 * single space. Safety levels can be set using their integer values
 * or names. Use the normalized function signature for functions. The
 * default safety can also be set using Q_CLASSINFO:
 *
 * ~~~(c++)
 * class MyClass : public QObject
 * {
 *   Q_OBJECT
 *   Q_CLASSINFO("safetyLevel", "AccessFromMainThread");
 * };
 * ~~~
 *
 * **NOTE:** If the safety level of the whole object or any of its
 * members is set to AccessFromMainThread, the PiiQObjectServer
 * instance must be created in the main thread.
 */
class PII_NETWORK_EXPORT PiiQObjectServer :
  public PiiObjectServer
{
  Q_OBJECT
  Q_ENUMS(PropertySafetyLevel)
  Q_FLAGS(ExposedFeatures)
public:
  enum ExposedFeature
  {
    ExposeNothing = 0,
    ExposeSignals = 1,
    ExposeSlots = 2,
    ExposeMethods = 4,
    ExposeProperties = 8,
    ExposeDynamicProperties = 16,
    ExposeProtected = 32,
    ExposePrivate = 64,
    ExposeDefault = -1 & ~ExposeProtected & ~ExposePrivate,
    ExposeAll = -1
  };

  enum PropertySafetyLevel
  {
    AccessPropertyFromMainThread,
    WritePropertyFromMainThread,
    AccessPropertyFromAnyThread,
    AccessPropertyConcurrently
  };

  Q_DECLARE_FLAGS(ExposedFeatures, ExposedFeature);

  /**
   * Creates a new PiiRemoteObjectServer that maps HTTP request to
   * the given *object*. There will be only one instance of the
   * remote object, and all client requests will use it. The caller
   * owns *object*; PiiRemoteObjectServer will not delete it.
   *
   * @param object an object to which remote function calls and
   * properties are mapped.
   *
   * @param features a list of features exposed to the HTTP interface.
   *
   * ! If *object* is a QWidget or a QWindow, thread safety level
   * will be set to `AccessFromMainThread`.
   */
  PiiQObjectServer(QObject* object, ExposedFeatures features = ExposeDefault);

  ~PiiQObjectServer();

  void handleRequest(const QString& uri, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller);

  QObject* object() const;

  /**
   * Returns a list of accessible property names with their types.
   * Each entry in this list consists of a type and a property name,
   * e.g. "int value".
   */
  QStringList propertyDeclarations() const;

  /**
   * Returns the signatures of all signals.
   */
  QStringList signalSignatures() const;

  QStringList listRoot() const;

  /**
   * Sets the safety level of an individual property, identified by
   * *propertyName*.
   */
  void setPropertySafetyLevel(const QString& propertyName, PropertySafetyLevel safetyLevel);
  /**
   * Returns the safety level of the given property. If no
   * property-specific safety level is set, returns the default safety
   * level.
   */
  PropertySafetyLevel propertySafetyLevel(const QString& propertyName) const;
  /**
   * Removes a previously set property-specific safety level.
   */
  void removePropertySafetyLevel(const QString& propertyName);

  PropertySafetyLevel strictestPropertySafetyLevel() const
  {
    const PII_D;
    return qMin(PropertySafetyLevel(d->safetyLevel + 1),
                d->minPropertySafetyLevel);
  }

protected:
  void connectToChannel(Channel* channel, const QString& sourceId);
  void disconnectFromChannel(Channel* channel, const QString& sourceId);
  void channelDeleted(Channel* channel);

  /// @hide
  class MetaFunction : public PiiGenericFunction
  {
  public:
    inline MetaFunction(QObject* o, const QMetaMethod& m);

    void call(void* args[]);

  private:
    class Data : public PiiGenericFunction::Data
    {
    public:
      Data(QObject*, const QMetaMethod&);
      QObject* pObject;
      QMetaMethod method;
    };
    PII_D_FUNC;
  };

  class ChannelSlot :
    public PiiUniversalSlot
  {
  public:
    ChannelSlot(QMutex* mutex) : _pMutex(mutex) {}
    QByteArray signal() const { return signatureOf(0); }
    QList<Channel*> lstChannels;
  protected:
    bool invokeSlot(int id, void** args);
  private:
    QMutex* _pMutex;
  };

  typedef QList<ChannelSlot*> SlotList;
  typedef QMap<QString, QList<Channel*> > ImplicitSignalMap;

  class PII_NETWORK_EXPORT Data : public PiiObjectServer::Data
  {
  public:
    Data(QObject* object, ExposedFeatures features);
    ~Data();
    QObject *pObject;
    PiiConfigurable* pConfigurable;
    ExposedFeatures features;
    QStringList lstSignals;
    SlotList lstSlots;
    QStringList lstEnums;
    QHash<QString, QStringList> hashEnums;
    QHash<QString, int> hashEnumValues;
    PropertySafetyLevel minPropertySafetyLevel;
    QMap<QString, PropertySafetyLevel> mapPropertySafetyLevels;
    // Notifier signal name -> connected channels
    ImplicitSignalMap mapImplicitNotifiers;
    // Property name -> notifier signal name
    QMap<QString, QString> mapNotifiedProps;
  };
  PII_D_FUNC;

  PiiQObjectServer(Data*);

  void moveToMainThread();
  /// @endhide

private slots:
  bool setProperties(const QVariantMap& props, bool lock);
  QVariant setSingleProperty(const QString& name, const QVariant& value, bool lock);
  QVariant readProperty(const QString& name, bool lock);

private:
  void init();
  void listFunctions(QObject* object);
  void addToEnums(const QString& name, const QMetaEnum& enumerator);
  void jsonProperties(PiiHttpDevice* dev, const QStringList& fields) const;
  void listProperties(PiiHttpDevice* dev) const;
  template <class T> QList<T> properties() const;
  QVariant objectProperty(const QString& name);
  bool setObjectProperties(const QVariantMap& props);
  QVariant setObjectProperty(const QString& name, const QVariant& value);
  ChannelSlot* findSlot(const QString& signal) const;
  template <class Enum> Enum stringToEnum(const char* enumName, const QString& str) const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PiiQObjectServer::ExposedFeatures)

#endif //_PIIQOBJECTSERVER_H
