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

#ifndef _PIIOPERATION_H
#define _PIIOPERATION_H

#include <PiiSerializationUtil.h>
#include <PiiMatrixSerialization.h>
#include <PiiConfigurable.h>
#include "PiiInputSocket.h"
#include "PiiOutputSocket.h"
#include "PiiExecutionException.h"
#include "PiiYdin.h"

#include <QMutex>

/// @file

/**
 * Declares a virtual piiMetaObject() function and implements a
 * serialization function that serializes the properties of the class.
 */
#define PII_OPERATION_SERIALIZATION_FUNCTION \
  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION; \
  PII_PROPERTY_SERIALIZATION_FUNCTION(PiiOperation)

/**
 * A superclass for operations that can be run by Ydin. Operations can
 * be roughly divided into produces, consumers, and transformations. 
 * An image source, such as a camera, is a producer (an input), image
 * processing operations are transforms, and a connection to a process
 * control system is a consumer (an output). There are however
 * operations that can work in many roles simultaneously.
 *
 * Each operation can be seen as a process that receives some input
 * data and produces some results after processing. An operation may
 * also have no inputs or no outputs, in which case it is considered a
 * producer or a consumer, respectively.
 *
 * Operations can work either synchronously or asynchronously. For
 * example, an image source can emit images in its own internal
 * frequency, irrespective of any consumers of the image data
 * (asynchronous operation). The receivers process the data if they
 * aren't too busy doing other things. In most cases it is however
 * necessary to process all images captured. In this case the image
 * source halts until all processing has been done before it sends the
 * next image.
 *
 * Each operation can have any number (0-N) of inputs and outputs
 * called sockets (see PiiSocket). Each socket has a data type or
 * multiple data types associated with it. That is, each socket
 * handles certain types of objects. Input and output sockets with
 * matching types can be connected to each other, and an output can be
 * connected to multiple inputs.
 *
 * Sockets are automatically destroyed with their parent operation. 
 * Furthermore, all connections between deleted sockets are
 * automatically deleted. Thus, one doesn't need to care about
 * deleting anything but the operation itself.
 *
 */
class PII_YDIN_EXPORT PiiOperation : public QObject, public PiiConfigurable
{
  Q_OBJECT
  Q_INTERFACES(PiiConfigurable)

  /**
   * The name of the property set currently being cached. See
   * [startPropertySet()]. If no property set is being cached, the name
   * is an empty string. Note that an empty string is also the default
   * name for a cached property set.
   */
  Q_PROPERTY(QString propertySetName READ propertySetName);

  /**
   * This flag is `true` if the operation is currently caching
   * properties (see [startPropertySet()]) and `false` otherwise.
   */
  Q_PROPERTY(bool cachingProperties READ isCachingProperties);
    
  Q_ENUMS(State);

  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION;
  PII_DEFAULT_SERIALIZATION_FUNCTION(QObject);

public:

  ~PiiOperation();
  
  /**
   * The state of an operation can assume six different values:
   *
   * - `Stopped` - the operation is not running.
   *
   * - `Starting` - the operation has received a [start()] signal, but
   * it is not running yet.
   *
   * - `Running` - the operation is running.
   *
   * - `Pausing` - the operation has received a [pause()] signal, but
   * it hasn't finished execution yet.
   *
   * - `Paused` - the operation has finished execution due to a
   * [pause()] command.
   *
   * - `Stopping` - the operation has received a [stop()] signal, but
   * it hasn't stopped yet.
   *
   * - `Interrupted` - the operation has received an [interrupt()]
   * signal, but it hasn't stopped yet.
   */
  enum State
    {
      Stopped,
      Starting,
      Running,
      Pausing,
      Paused,
      Stopping,
      Interrupted
    };

  /**
   * Protection levels for setting properties.
   *
   * - `WriteAlways` - setting the value of a property is always
   * allowed. This is the default value for all properties.
   *
   * - `WriteWhenStoppedOrPaused` - setting the value of a property
   * is allowed only if the state of the operation is either 
   * `Stopped` or `Paused`.
   *
   * - `WriteWhenStopped` - setting the value of a property is
   * allowed only if the state of the operation is `Stopped`.
   *
   * - `WriteNotAllowed` - setting the value of a property is not
   * allowed at all. This value makes it possible for subclasses to
   * make the properties of a superclass read-only even if they had a
   * setter function.
   */
  enum ProtectionLevel
    {
      WriteAlways,
      WriteWhenStoppedOrPaused,
      WriteWhenStopped,
      WriteNotAllowed
    };

  /**
   * Returns a string presentation of the given state.
   */
  static const char* stateName(State state);
  
  /**
   * Checks that the necessary preconditions for processing are met. 
   * This function is called by before processing is started. It is
   * followed by a [start()] call, provided that all operations in an
   * engine have been successfully initialized. The function should
   * check, for example, that all required inputs are connected.
   *
   * This function is called by PiiEngine once for all operations
   * before any of them is started. This ensures there will be no
   * input into any operation before each of them have been checked.
   *
   * @param reset if `true`, reset the operation to its initial
   * state. This flag is true whenever the operation was interrupted
   * and started again. If the operation was just paused, the reset
   * flag is set to `false`, and the operation should continue where
   * ever it was left.
   *
   * @exception PiiExecutionException& if the operation cannot be run.
   */
  virtual void check(bool reset) = 0;

  /**
   * Starts the operation and executes it until interrupted or paused. 
   * The operation should prepare itself for work and change state to
   * `Starting` or `Running`.
   *
   * This function will be called not only on start-up but also before
   * restarting after a [pause()] call.
   *
   * Note that PiiOperationCompound (and PiiEngine) commands child
   * operations in an arbitrary order. Therefore, objects may appear
   * in the inputs of a operation before %start() is invoked. Any
   * resetting action should take place in [check()].
   */
  virtual void start() = 0;

  /**
   * Pauses the operation. The operation should change its state to 
   * `Pausing`. Pausing has direct effect on producer operations only. 
   * If a producer is in `Pausing` state, it'll finish its current
   * processing round, turn to `Paused` state and inform all
   * connected operations that it is now safe to pause. This will
   * finally turn all operations in a pipeline to `Paused` state. 
   * Before the next [start()] call, [check()] will be called with the
   * `reset` parameter set to `false`.
   *
   * Note that pause() has no effect if the operation is not in 
   * `Running` state.
   */
  virtual void pause() = 0;

  /**
   * Stops the operation. The operation should change its state to 
   * `Stopping`. Stopping and pausing work similarly in most cases. 
   * Usually, the only difference is in the final state of the
   * operation. Overriding this function makes it possible for an
   * operation to prepare for stopping. The difference between 
   * `stop`() and [interrupt()] is in that the former performs a "clean"
   * stop, which won't leave any object currently being processed in
   * the pipeline.
   *
   * Note that stop() has no effect if the operation is not in 
   * `Running` state.
   */
  virtual void stop() = 0;
  
  /**
   * Interrupts the execution. Calling this function should stop the
   * execution of this operation as soon as possible, even in the
   * middle of a processing round. After an interrupt, the next
   * [start()] call will be preceded by a [check()] call with the 
   * `reset` parameter set to `true`.
   */
  virtual void interrupt() = 0;

  /**
   * Returns the current state of the operation.
   */
  Q_INVOKABLE virtual State state() const = 0;

  /**
   * Waits for this operation to stop running. The calling function
   * blocks until the operation stops running or `time` milliseconds
   * has elapsed. By default, the call will never time out. Note that
   * the state of the operation may not have changed yet when this
   * function returns.
   *
   * @return `true` if the operation exited within `time`
   * milliseconds, `false` if the call timed out.
   */
  virtual bool wait(unsigned long time = ULONG_MAX) = 0;

  /**
   * Returns the number of input sockets. The default implementation
   * returns inputs().size().
   */
  Q_INVOKABLE virtual int inputCount() const;
  
  /**
   * Returns a pointer to the input associated with *name*.
   *
   * @return a pointer to the input socket or 0 if no socket matches
   * *name*
   */
  Q_INVOKABLE virtual PiiAbstractInputSocket* input(const QString& name) const = 0;

  /**
   * Returns the number of output sockets. The default implementation
   * returns outputs().size().
   */
  Q_INVOKABLE virtual int outputCount() const;

  /**
   * Returns a pointer to the output associated with *name*.
   *
   * @return a pointer to the output socket or 0 if no socket matches
   * *name*
   */
  Q_INVOKABLE virtual PiiAbstractOutputSocket* output(const QString& name) const = 0;

  /**
   * Returns a list of all input sockets connected to this operation. 
   * The order in which the sockets are returned should be
   * "intuitive", but no strict restrictions are imposed.
   */
  Q_INVOKABLE virtual QList<PiiAbstractInputSocket*> inputs() const = 0;

  /**
   * Returns the names of all inputs.
   */
  Q_INVOKABLE QStringList inputNames() const;

  /**
   * Returns a list of all output sockets connected to this operation. 
   * Analogous to inputs().
   */
  Q_INVOKABLE virtual QList<PiiAbstractOutputSocket*> outputs() const = 0;

  /**
   * Returns the names of all outputs.
   */
  Q_INVOKABLE QStringList outputNames() const;

  /**
   * Returns the name of a socket in the context of this operation. 
   * The name of a socket may change based on nesting level. The same
   * socket may be accessed with multiple names, if it is exported to
   * the interface of an [operation compound](PiiOperationCompound).
   *
   * Operations are free to implement any scheme for naming their
   * sockets. For example, PiiBasicOperation uses the `objectName`
   * property of the socket, and PiiOperationCompound keeps an
   * internal map of socket aliases. Since there is no default naming
   * scheme, the default implementation returns an empty string.
   *
   * This is a convenience function that calls socketProperty(
   * *socket*, "name").
   *
   * @param socket the socket whose name is to be found
   *
   * @return the name of the socket or an empty string if the socket
   * is not owned by this operation.
   */
  Q_INVOKABLE QString socketName(PiiAbstractSocket* socket) const;

  /**
   * Returns meta information associated with *socket*. This function
   * can be used to query named properties of input and output
   * sockets. Operations are required to provide at least the `name`
   * property. Other properties can be used depending on application. 
   * Below is a short list of commonly used properies:
   *
   * - `name` - the name of the socket in the context of this
   * operation. See socketName().
   *
   * - `min` - the minimum possible scalar value a socket can
   * send/receive
   *
   * - `max` - the maximum possible scalar value a socket can
   * send/receive
   *
   * - `resolution` - the resolution of the value. Integers have a
   * resolution of 1. If the value can take values only in known
   * steps, the `resolution` property specifies the step size.
   *
   * - `displayName` - a user-displayable name of the socket. May be
   * translated.
   *
   * @param socket the socket whose properties are queried
   *
   * @param the name of the property to query
   *
   * @return the value of the property, or an invalid QVariant if the
   * socket is not owned by this operation or the named property does
   * not exist.
   *
   * @see PiiYdin::isNameProperty()
   */
  virtual QVariant socketProperty(PiiAbstractSocket* socket, const char* name) const;

  Q_INVOKABLE QVariant socketProperty(PiiAbstractSocket* socket, const QString& name) const;
  
  /**
   * A convenience function for connecting a named output socket to a
   * named input socket in another operation. This is (almost)
   * analogous to:
   *
   * ~~~
   * output("output")->connectInput(other->input(input));
   * ~~~
   *
   * The difference is in that this function returns `false` in case
   * of a failure.
   *
   * @param output the name of the output socket
   *
   * @param other the operation that contains the input we need to
   * connect to
   *
   * @param input the name of the input socket in `other`
   *
   * @return `true` if both sockets were found, `false` otherwise
   */
  Q_INVOKABLE bool connectOutput(const QString& output, PiiOperation* other, const QString& input);

  Q_INVOKABLE bool connectOutput(const QString& output, PiiAbstractInputSocket* input);

  /**
   * Starts storing a named property set. Between startPropertySet()
   * and [endPropertySet()] calls all [setProperty()] calls will be
   * cached. The property values won't be changed until
   * [applyPropertySet()] is called.
   *
   * While this mechanism may seem unnecessary and even cumbersome on
   * surface, it makes it possible to change the properties of a
   * running configuration in a deterministic manner. PiiOperation
   * subclasses ensure that applyPropertySet() is executed so that all
   * modified properties will be in effect simultaneously with respect
   * to the data being processed. The processing pipeline can continue
   * without delays, and properties will be changed as data passes
   * through. The beginning of a processing pipeline may be processing
   * new data with new properties while the end of the pipeline still
   * handles old data with old property values.
   *
   * @param name a unique identifier for the property set.
   */
  Q_INVOKABLE virtual void startPropertySet(const QString& name = QString());

  bool isCachingProperties() const;
  QString propertySetName() const;
  
  /**
   * Ends storing a named property set. After calling this function
   * calls to [setProperty()] will change property values directly.
   */
  Q_INVOKABLE virtual void endPropertySet();

  /**
   * Removes the properties cached in the set identified by *name*.
   */
  Q_INVOKABLE virtual void removePropertySet(const QString& name = QString());

  /**
   * Synchronously reconfigures an operation with the properties
   * cached in the set identified by *propertySetName*. This function
   * may not change the values immediately. The properties will be
   * changed as data passes through the processing pipeline.
   *
   * ! Reconfiguration may fail if operations are interrupted
   * (either explicitly or due to a processing error) during
   * reconfiguration. In such a case only a subset of operations may
   * have applied the new properties.
   */
  Q_INVOKABLE virtual void reconfigure(const QString& propertySetName = QString()) = 0;
  
  /**
   * Virtual version of QObject::setProperty(). Making a non-virtual
   * function virtual in a subclass is *baad*. But we need to be able
   * to override this function to support nested property names,
   * mutual exclusion, and cached properties.
   *
   * @see PiiOperationCompound::setProperty()
   * @see PiiDefaultOperation::setProperty()
   */
  virtual bool setProperty(const char* name, const QVariant& value);

  /**
   * Returns the value of the property identified by *name*.
   */
  virtual QVariant property(const char* name) const;

  /**
   * A convenience function that automatically creates a QVariant out
   * of a PiiVariant.
   *
   * ~~~
   * PiiOperation* op = ...;
   * op->setProperty("property", Pii::createVariant(PiiMatrix<int>(4,4)));
   * ~~~
   */
  bool setProperty(const char* name, const PiiVariant& value);

  /**
   * Creates a clone of this operation. The default implementation
   * uses the serialization library to find a factory object for the
   * class. Once an instance is created it iterates over all
   * properties and copies their values to the new operation instance.
   *
   * @return a deep copy of the operation, or 0 if no factory was
   * found.
   */
  Q_INVOKABLE virtual PiiOperation* clone() const;

  /**
   * Disconnects all inputs.
   */
  Q_INVOKABLE void disconnectAllInputs();

  /**
   * Disconnects all outputs.
   */
  Q_INVOKABLE void disconnectAllOutputs();
  
  /**
   * Returns the protection level of *property*.
   */
  ProtectionLevel protectionLevel(const char* property) const;

  /// @internal
  bool isCompound() const;
  
signals:
  /**
   * Signals an error. The *message* should be a user-friendly
   * explanation of the cause of the error. *sender* is the original
   * source of the message.
   */
  void errorOccured(PiiOperation* sender, const QString& message);

  /**
   * Indicates that the state of this operation has changed. If you
   * connect to this signal from outside, make sure you either run an
   * event loop in the receiving thread or create a direct connection. 
   * This is needed because the signal will most likely be emitted
   * from another thread. If you create a queued connection and don't
   * run an event loop in the receiving thread, the signal will be
   * lost. If you create a direct connection, you must explicitly
   * implement a mutual exclusion mechanism in the receiving slot.
   *
   * @param state the new state of the operation. The type of this
   * value is actually PiiOperation::State, but `int` is used to
   * avoid registering a new meta type.
   */
  void stateChanged(int state);
  
protected:
  /// @cond null
  typedef QList<QPair<const char*, ProtectionLevel> > ProtectionList;
  typedef QList<QPair<QString,QVariant> > PropertyList;

  class PII_YDIN_EXPORT Data
  {
  public:
    Data();
    virtual ~Data();

    virtual bool isCompound() const { return false; }
    ProtectionList lstProtectionLevels;
    QMutex stateMutex;
    bool bCachingProperties, bApplyingPropertySet;
    QString strPropertySetName;
    QMap<QString,PropertyList> mapCachedProperties;
  } *d;
  
  PiiOperation(Data* d);
  /// @endcond

  /**
   * Constructs a new PiiOperation.
   */
  PiiOperation();

  /**
   * Sets the protection level of *property* to *level*. This
   * function is a generic way of controlling write access to
   * properties. By default, all properties writable independent of
   * the state of the operation. Some properties do however affect the
   * internal structure of an operation in a manner that cannot be
   * handled at run time. For example, the number of sockets cannot be
   * changed on the fly without careful handling of the internal
   * synchronization mechanism.
   *
   * ~~~
   * MyOperation::MyOperation()
   * {
   *   // Disallow changing of the processing mode
   *   setWritePermission("processingMode", WriteNever);
   * }
   * ~~~
   *
   * ! Protection is only effective if properties are set through
   * setProperty(). Calling property setters directly bypasses the
   * protection mechanism.
   */
  void setProtectionLevel(const char* property, ProtectionLevel level);

  /**
   * Copies cached properties from the set identified by *name* to
   * the operation's properties. If you don't call endPropertySet()
   * before this function, the properties will be cached in the set
   * that was last started with [startPropertySet()]. This makes it
   * possible to easily copy property sets to each other.
   *
   * Applying a set of cached properties this way is equivalent to
   * setting the properties while the operation is paused. If the
   * protection level allows setting the property while the operation
   * is paused, run-time reconfigurations through this function will
   * also be allowed. You can override this function to perform
   * additional checks and/or initialization if needed.
   */
  virtual void applyPropertySet(const QString& name);

  /**
   * Returns a pointer to the mutex that prevents concurrent access to
   * the state of this operation.
   *
   * ~~~
   * void MyOperation::stop()
   * {
   *   synchronized (stateLock())
   *   {
   *     if (state() == Running)
   *       setState(Stopping);
   *   }
   * }
   * ~~~
   */
  QMutex* stateLock();

private:
  int indexOf(const char* property) const;
  void addPropertyToList(PropertyList& properties,
                         const QString& name,
                         const QVariant& value);
  PII_DISABLE_COPY(PiiOperation);
};

typedef PiiSharedPtr<PiiOperation> PiiOperationPtr;
typedef QList<PiiAbstractInputSocket*> PiiInputSocketList;
typedef QList<PiiAbstractOutputSocket*> PiiOutputSocketList;

Q_DECLARE_METATYPE(PiiOperation*);
Q_DECLARE_METATYPE(PiiOperationPtr);
Q_DECLARE_METATYPE(QList<PiiAbstractInputSocket*>);
Q_DECLARE_METATYPE(QList<PiiAbstractOutputSocket*>);

#define PII_SERIALIZABLE_CLASS PiiOperation
#define PII_SERIALIZABLE_IS_ABSTRACT
#define PII_BUILDING_LIBRARY PII_BUILDING_YDIN
#include <PiiSerializableRegistration.h>

#include <PiiQVariantWrapper.h>
#define PII_SERIALIZABLE_CLASS PiiQVariantWrapper::Template<PiiOperation*>
#define PII_SERIALIZABLE_CLASS_NAME "PiiQVariantWrapper<PiiOperation*>"
#define PII_BUILDING_LIBRARY PII_BUILDING_YDIN
#include <PiiSerializableRegistration.h>

#define PII_SERIALIZABLE_CLASS PiiQVariantWrapper::Template<PiiOperationPtr>
#define PII_SERIALIZABLE_CLASS_NAME "PiiQVariantWrapper<PiiOperationPtr>"
#define PII_BUILDING_LIBRARY PII_BUILDING_YDIN
#include <PiiSerializableRegistration.h>

#endif //_PIIOPERATION_H
