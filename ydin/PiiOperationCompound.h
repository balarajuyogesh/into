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

#ifndef _PIIOPERATIONCOMPOUND_H
#define _PIIOPERATIONCOMPOUND_H

#include "PiiOperation.h"
#include "PiiProxySocket.h"

#include <QMap>


/**
 * Declares a virtual piiMetaObject() function and implements a
 * serialization function that serializes the child operations and the
 * properties of the class.
 */
#define PII_COMPOUND_SERIALIZATION_FUNCTION \
  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION; \
  PII_PROPERTY_SERIALIZATION_FUNCTION(PiiOperationCompound)

/**
 * PiiOperationCompound is a class that controls a set of operations. 
 * It can contain any number of other operations, which may also be
 * compounds themselves.
 *
 * Execution
 * ---------
 *
 * PiiOperationCompound does actually nothing when it is started,
 * paused, or interrupted. It merely delegates the commands to its
 * direct ancestors. The state of a compound changes when the state of
 * all its children has changed. For example, when compound is
 * started, it changes state to `Starting` immediately after any of
 * its children changes state to `Starting` or `Running`. The state
 * of the compound will turn to `Running` when all child operations
 * have changed their state to `Running`.
 *
 * If the execution of any of a compound's child operations is
 * terminated before all of its siblings have changed their state to
 * `Running`, the compound itself may never change its state to 
 * `Running`. This can happen, for example, if a data source quickly
 * runs out of data. In such a case the state turns first to 
 * `Starting` and then to `Stopping` and finally to `Stopped`,
 * omitting `Running`, although the operations themself were
 * successfully started.
 *
 * It is important to note that the start(), pause() and interrupt()
 * functions may not immediately change the state of
 * PiiOperationCompound to `Started`, `Paused`, or `Stopped` but to
 * the corresponding intermediate states `Starting`, `Pausing` and
 * `Interrupted`. The state will change to the final state only after
 * all child operations have changed their state. The thread that
 * creates a PiiOperationCompound must run Qt's event handler every
 * now and then. Otherwise, state change signals from child operations
 * won't get delivered, and the compound itself will never change its
 * state. Typically, one does not need to do anything to accomplish
 * this as operations are created in the main thread, which calls
 * QApplication::exec() at some point. If a PiiOperationCompound is
 * created in another thread, one must either run a thread-specific
 * event loop (QThread::exec()) or call
 * QCoreApplication::processEvents() directly.
 *
 *
 * Naming
 * ------
 *
 * The inner operations of the compound can be
 * accessed with a naming scheme in which sub-operations are denoted
 * by their `objectName`, and name parts are separated by dots.
 *
 * Assume the compound contains a PiiImageViewer sub-operation whose
 * `objectName` is "imageviewer". The `image` input of this object
 * can be read as follows:
 *
 * ~~~(c++)
 * // PiiOperationCompound *compound;
 * PiiInputSocket* input = compound->input("imageviewer.image");
 * ~~~
 *
 * The parsing recurses to sub-operations. Thus, if one knows the
 * internals of compound operations, any input can be found (e.g. 
 * "subop.subop.image"). `parent` denotes the parent operation.
 * This makes it possible to connect to a socket that is within
 * another compound.
 *
 * In the example below, `Op0` contains `Op1`. In the context of
 * the root operation that contains both `Op0` and `Op2` (not
 * drawn), the sockets of the operations are denoted by
 * "Op0.Op1.in1", "Op0.Op1.out", and "Op2.in2". In the context of 
 * `Op0`, `in2` is "parent.Op2.in2".
 *
 * ![Compound example](operationcompound.png)
 *
 * Inputs and Outputs
 * ------------------
 *
 * Inputs and outputs for an operation compound can be created in two
 * alternative ways: aliasing and proxying. When a socket is aliased,
 * it just appears in the interface of the compound, but is still
 * directly connected to the sub-operation it came from. Aliasing
 * works for all output sockets and for inputs that are connected to
 * just one input within the compound, provided that the name of the
 * socket doesn't need to be changed. Since the connection through an
 * alias is actually made directly to the lower-level socket, there is
 * no overhead in passing objects.
 *
 * Let us assume we have a pointer to `Op0` in the figure above. We
 * can create an alias, i.e. *expose* any socket within the compound
 * as follows:
 *
 * ~~~(c++)
 * //PiiOperation* op0, *op1;
 * op0->exposeInput("Op1.in1");
 * // Analogously:
 * op0->exposeInput(op1.input("in1"));
 * // Map the output
 * op0->exposeOutput("Op1.out");
 * ~~~
 *
 * The result is that `Op0` appears to have an input and an output
 * that actually are redirected to/from `Op1`.
 *
 * Proxying is useful if an input needs to be redirected to many
 * operations. For this, PiiOperationCompound uses the PiiProxySocket
 * class. Consider the illustration below. The compound operation
 * `Masker` creates a binary mask for an input image, and multiplies
 * the input by the mask. For this, the `image` input must be
 * connected to the operation that creates the mask and to the
 * operation that multiplies the mask and the input. What we need to
 * do is to create a proxy input that is connected to two inputs
 * within the compound. From the outside, the input appears as one:
 *
 * ![Proxy socket example](operationcompoundproxy.png)
 *
 * ~~~(c++)
 * // We assume that the objectName of PiiThresholdingOperation is
 * // "threshold" and that of PiiArithmeticOperation is "multiplier".
 * masker->createInputProxy("image",
 *                          QStringList() << "threshold.image" << "multiplier.input0")
 *
 * // Proxy the output as "image"
 * masker->createOutputProxy("image", "multiplier.output");
 * ~~~
 *
 * Serialization
 * -------------
 *
 * Operation compounds usually need to work differently when
 * constructed directly by user than when deserialized. Since the
 * child operations are automatically deserialized, it is not needed
 * to create them in the constructor. Therefore, two constructors are
 * usually provided, and the serialization system is told to use a
 * special "Void" function when deserializing.
 *
 * ~~~(c++)
 * class MyOperation : public PiiOperationCompound
 * {
 *   // Called by the serialization library. Doesn't create child operations.
 *   MyOperation(PiiSerialization::Void);
 *
 *   // This ensures that properties and child operations are serialized
 *   PII_COMPOUND_SERIALIZATION_FUNCTION
 * public:
 *   // Called by everybody else. Creates child operations.
 *   MyOperation();
 * };
 * ~~~
 *
 * When registering an operation compound to a plug-in, use the 
 * [PII_REGISTER_COMPOUND] macro instead of [PII_REGISTER_OPERATION].
 *
 */
class PII_YDIN_EXPORT PiiOperationCompound : public PiiOperation
{
  Q_OBJECT

  Q_ENUMS(ConnectionType);
  
  friend struct PiiSerialization::Accessor;
  PII_SEPARATE_SAVE_LOAD_MEMBERS
  PII_DECLARE_SAVE_LOAD_MEMBERS
  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION;

public:
  /// Constructs a new operation compound.
  PiiOperationCompound();
  /// Destroys the compound.
  ~PiiOperationCompound();
  
  /**
   * Checks all child operations.
   *
   * ! This function must always be called before start().
   * Otherwise, start() will do nothing.
   */
  void check(bool reset);

  /**
   * Starts all child operations. The compound immediately changes its
   * state to `Starting` and turns `Running` once all children have
   * turned `Running`. The call has no effect if the compound is not
   * `Stopped` or `Paused`.
   */
  void start();

  /**
   * Pauses all child operations. The compound immediately changes its
   * state to `Pausing` and turns `Paused` once all children have
   * turned `Paused`. The call has no effect if the compound is not
   * `Running`.
   */
  Q_INVOKABLE void pause();

  /**
   * Interrupts all child operations. The compound immediately changes
   * its state to `Stopping` and turns `Stopped` once all children
   * have turned `Stopped`. The call has no effect if the compound is
   * `Stopped`.
   *
   * ! Calling interrupt terminates all child operations as soon
   * as possible, independent of their current processing state. If
   * your configuration has many parallel processing pipelines, they
   * may exit at different phases. For example, if the configuration
   * has two parallel PiiImageFileWriter operations, the image
   * currently being written may be finished by only one of them.
   *
   * @see stop()
   */
  Q_INVOKABLE void interrupt();

  /**
   * Stops all child operations. The compound immediately changes its
   * state to `Stopping` and turns `Stopped` once all children have
   * turned `Stopped`. The call has no effect if the compound is not
   * `Running`.
   */
  Q_INVOKABLE void stop();

  /**
   * Waits for all child operations to terminate their execution.
   *
   * @param time the maximum time to wait (in milliseconds). 
   * `ULONG_MAX` causes the function not to time out.
   *
   * @return `true` if all child operations exited before the call
   * timed out, `false` otherwise.
   *
   * ! All children may not have signalled their state change to
   * `Stopped` yet even if this method returns `true`. They have,
   * however, finished execution. It is **unsafe** to delete the
   * compound immediately. Use [QObject::deleteLater()]
   * "deleteLater()" or wait until the state has changed.
   */
  bool wait(unsigned long time = ULONG_MAX);

  /**
   * Waits for the compound to change state to *state*.
   *
   * @param state the target state
   *
   * @param time the maximum time to wait (in milliseconds). 
   * `ULONG_MAX` causes the method not to time out.
   *
   * @return `true` if the state was reached before the call timed
   * out, `false` otherwise.
   *
   * ~~~(c++)
   * PiiEngine engine;
   * engine.interrupt();
   * engine.wait(PiiOperation::Stopped);
   * ~~~
   *
   * It is in general unsafe not to specify a timeout value. In some
   * cases it may happen that the compound never turns into the state
   * you expect. For example, if an error occurs in one of a
   * compound's sub-operations while start-up, the compound turns into
   * `Stopped` state. Another unexpected situation may arise if a
   * producer operation quickly runs out of data and spontaneously
   * stops while other operations are still `Pausing` or `Starting`. 
   * In this case the compound may will turn into `Running` state and
   * immediately to `Stopping`. Waiting for `Stopped` after stop()
   * or interrupt() and waiting for `Paused` after pause() is safe
   * with well-behaved operations, but a better convention is to
   * always specify a reasonable timeout and inspect the return value.
   *
   * ~~~(c++)
   * PiiEngine engine;
   * engine.pause();
   * // Wait for 2 seconds
   * if (!engine.wait(PiiOperation::Paused, 2000))
   *   {
   *     engine.interrupt();
   *     // Handle error here
   *   }
   * ~~~
   *
   * ! This function executes the event loop of the active thread
   * in order to deliver the state change signals of the operations
   * and to keep the application responsive. As a result, any of your
   * application events, including timer events, may get processed
   * during a the wait() call. This may result in unexpected behavior
   * especially if you alter the state of the compound in the slots
   * that will be called.
   */
  Q_INVOKABLE bool wait(State state, unsigned long time = ULONG_MAX);

  /**
   * Adds an operation to the compound's internal operation list.
   *
   * The new operation is placed into the QObject's child list of the
   * PiiOperationCompound object. Therefore, the memory taken up by
   * the operations is automatically released upon the destruction of
   * the compound object.
   *
   * If the compound is not stopped, this function has no effect.
   *
   * PiiOperationCompound does its best to ensure that its children do
   * not have duplicate object names. If you don't set the object
   * names manually, addOperation() creates a unique name by
   * concatenating the object's class name and an arbitrary number. If
   * the operation already has a name, it will not be modified.
   *
   * @param op the operation
   */
  Q_INVOKABLE virtual void addOperation(PiiOperation* op);

  /**
   * Removes an operation from the compound's internal operation list. 
   * The operation no longer belongs to the children of the compound
   * object.
   *
   * If the compound is neither stopped nor paused, this function has
   * no effect.
   *
   * @param op the operation to remove. All incoming and outgoing
   * connections will be disconnected.
   */
  Q_INVOKABLE virtual void removeOperation(PiiOperation* op);

  Q_INVOKABLE PiiOperation* removeOperation(const QString& name);

  /**
   * Returns all direct child operations of this compound as a list.
   */
  Q_INVOKABLE QList<PiiOperation*> childOperations() const;

  /**
   * Returns the number of child operations. Equal to but faster than
   * childOperations.size().
   */
  Q_INVOKABLE int childCount() const;
  
  /**
   * Returns a pointer to the child operation identified by *name*. 
   * This function support tree look-ups. That is, a "sub.child" as
   * the name will first look up a child with "sub" as its name and
   * then recurse to find its "child".
   */
  Q_INVOKABLE PiiOperation* childOperation(const QString& name) const;

  /**
   * Returns the names of child operations.
   */
  Q_INVOKABLE QStringList childNames() const;

  /**
   * Returns the child operation at *index*, or 0 if there is no such
   * operation.
   */
  Q_INVOKABLE PiiOperation* childAt(int index) const;

  /**
   * Replaces *oldOp* with *newOp*. If the compound is neither
   * stopped nor paused, this function has no effect. If *oldOp* has
   * connected inputs and/or outputs, they will be disconnected and
   * reconnected to *newOp* to the corresponding sockets.
   *
   * @param oldOp the operation to remove
   *
   * @param newOp the operation to add in stead. If *newOp* is zero,
   * the function behaves like removeOperation(`oldOp`).
   *
   * @return `true` if the operation was successsfully
   * replaced/removed, `false` otherwise.
   */
  Q_INVOKABLE bool replaceOperation(PiiOperation *oldOp, PiiOperation* newOp);
  
  /**
   * Deletes all child operations and exposed sockets.
   */
  Q_INVOKABLE void clear();
  
  /**
   * Returns the named input socket. The name may refer to an exposed
   * socket, to a proxy or to a socket in an internal operation. For
   * example, `input("sub.socket")` returns the input called "socket"
   * in a child operation called "sub".
   *
   * ~~~(c++)
   * pMyOperation->setObjectName("sub");
   * pCompound->addOperation(pMyOperation);
   * pCompound->input("sub.input"); // returns "input" in pMyOperation
   * pCompound->exposeInput("sub.input");
   * pCompound->input("input"); // returns "input" in pMyOperation
   * pCompound->createOutputProxy("out");
   * pCompound->input(".out"); // returns the input side of the proxy
   * ~~~
   */
  PiiAbstractInputSocket* input(const QString& name) const;

  /**
   * Returns the named output socket. See [input()] for details.
   */
  PiiAbstractOutputSocket* output(const QString& name) const;

  /**
   * Returns exposed input sockets.
   */
  QList<PiiAbstractInputSocket*> inputs() const;

  /**
   * Returns exposed output sockets.
   */
  QList<PiiAbstractOutputSocket*> outputs() const;

  /**
   * Exposes an input socket to this compound's interface. After
   * exposing, the socket is available with its name on the public
   * interface of this compound.
   *
   * @param socket the socket to be exposed. Must be owned by an
   * operation that is a child of this compound.
   *
   * @return `true` if successful, `false` otherwise (*socket* is not
   * a child of this compound or there is a name conflict).
   * 
   */
  Q_INVOKABLE bool exposeInput(PiiAbstractInputSocket* socket);
  Q_INVOKABLE bool exposeOutput(PiiAbstractOutputSocket* socket);

  /**
   * A convenience function that allows one to expose an input socket
   * with the dot syntax.
   */
  Q_INVOKABLE bool exposeInput(const QString& fullName);
  
  /**
   * A convenience function that allows one to expose an output socket
   * with the dot syntax (explained above).
   */
  Q_INVOKABLE bool exposeOutput(const QString& fullName);

  /**
   * Removes *socket* from the public interface. Returns `true` if the
   * input was successfully removed, and `false` if the socket is not
   * currently exposed.
   */
  Q_INVOKABLE bool removeInput(PiiAbstractInputSocket* socket);
  /**
   * Removes an input socket from the public interface. Returns `true`
   * if the input was successfully removed, and `false` if no socket
   * with the given name is exposed.
   */
  Q_INVOKABLE bool removeInput(const QString& name);
  /**
   * Removes *socket* from the public interface. Returns `true` if the
   * output was successfully removed, and `false` if the socket is not
   * currently exposed.
   */
  Q_INVOKABLE bool removeOutput(PiiAbstractOutputSocket* socket);
  /**
   * Removes an output socket from the public interface. Returns
   * `true` if the output was successfully removed, and `false` if no
   * socket with the given name is exposed.
   */
  Q_INVOKABLE bool removeOutput(const QString& name);

  /**
   * Removes all input and output sockets from the public interface.
   */
  Q_INVOKABLE void removeAllSockets();
  
  /**
   * Creates an unconnected proxy input. This function creates a new
   * PiiProxySocket and exposes its input socket as *name*. If a
   * socket is already exposed with the given name, the function does
   * nothing.
   */
  Q_INVOKABLE PiiProxySocket* createInputProxy(const QString& name);

  /**
   * Creates an input proxy with the given *name* and connects the
   * input listed in *inputNames* to it. Returns a pointer to the new
   * proxy or 0 if an exposed input or a proxy with the given name
   * already exists.
   */
  Q_INVOKABLE PiiProxySocket* createInputProxy(const QString& name, const QStringList& inputNames);

  /**
   * Returns the proxy whose input is exposed as *name*. If the input
   * does not exist or it is not a proxy input, 0 will be returned.
   */
  Q_INVOKABLE PiiProxySocket* inputProxy(const QString& name) const;

  /**
   * Creates an unconnected proxy output. This function creates a new
   * PiiProxySocket and exposes its output socket as *name*. If a
   * socket is already exposed with the given name, the function does
   * nothing.
   *
   * Note that output sockets seldom need to be proxied because any
   * output can directly be connected to many inputs. The need for a
   * proxy output arises if you want to connect an output both
   * internally and externally, or you want to preserve the output
   * even when internal operations are deleted.
   */
  Q_INVOKABLE PiiProxySocket* createOutputProxy(const QString& name);

  /**
   * Creates an output proxy with the given *name* and connects it to
   * the internal output identified by *outputName*. Returns a pointer
   * to the created proxy or 0 if *name* is already in use.
   */
  Q_INVOKABLE PiiProxySocket* createOutputProxy(const QString& name, const QString& outputName);

  /**
   * Returns the proxy whose output is exposed as *name*. If the
   * output does not exist or it is not a proxy output, 0 will be
   * returned.
   */
  Q_INVOKABLE PiiProxySocket* outputProxy(const QString& name) const;
  
  /**
   * Returns the number of output sockets. Equivalent to but faster
   * than outputs().size().
   */
  int outputCount() const;

  /**
   * Returns the number of input sockets. Equivalent to but faster
   * than inputs().size().
   */
  int inputCount() const;

  State state() const;

  /**
   * A convenience function that creates an instance of the named
   * class and adds it as a child to this compound. If the operation
   * cannot be created, 0 will be returned.
   *
   * @param className the name of the operation to create.
   *
   * @param objectName set the objectName property of the newly
   * created operation. This parameter is provided for convenience as
   * it often saves a separate setObjectName() call.
   *
   * @return a pointer to the operation or 0 if the class could not be
   * instantiated.
   */
  Q_INVOKABLE PiiOperation* createOperation(const QString& className, const QString& objectName = "");

  /**
   * Calls startPropertySet() on each child operation. Note that this
   * function has no effect on the compound itself. Compound
   * properties will never be cached because they cannot directly
   * affect processing. If setting a property in a compound's
   * interface affects properties in child operations, they will be
   * cached.
   */
  void startPropertySet(const QString& name = QString());

  /**
   * Calls endPropertySet() on each child operation.
   */
  void endPropertySet();

  /**
   * Calls removePropertySet() on each child operation.
   */
  void removePropertySet(const QString& name = QString());

  /**
   * Calls reconfigure() on each child operation.
   */
  void reconfigure(const QString& propertySetName = QString());

  /**
   * Sets a property in this compound. This function supports the "dot
   * syntax" for setting properties. If the compound has a child
   * operation called `child`, then the properties of the child can
   * be set with `setProperty("child.property", value)`.
   */
  bool setProperty(const char* name, const QVariant& value);

  /**
   * Returns a property using the "dot syntax".
   */
  QVariant property(const char * name) const;
  
  /**
   * Creates a clone of this compound. This function recursively
   * clones all of its child operations.
   *
   * @return a deep copy of the compound.
   */
  PiiOperationCompound* clone() const;

protected:
  /// @hide
  class PII_YDIN_EXPORT Data;
  PII_UNSAFE_D_FUNC;
  PiiOperationCompound(Data* data);

  struct Start { static void perform(PiiOperation* op) { op->start(); } };
  struct Pause { static void perform(PiiOperation* op) { op->pause(); } };
  struct Stop { static void perform(PiiOperation* op) { op->stop(); } };
  struct Interrupt { static void perform(PiiOperation* op) { op->interrupt(); } };

  struct StartPropertySet { static void perform(PiiOperation* op, const QString& n) { op->startPropertySet(n); } };
  struct RemovePropertySet { static void perform(PiiOperation* op, const QString& n) { op->removePropertySet(n); } };
  struct Reconfigure { static void perform(PiiOperation* op, const QString& n) { op->reconfigure(n); } };
  struct EndPropertySet { static void perform(PiiOperation* op) { op->endPropertySet(); } };
  
  /**
   * Sends a command to all child operations. Use the action structures
   * to specify the action to be taken. This function loops through
   * the list of child operation and applies the action to each.
   *
   * ~~~(c++)
   * commandChildren(Start());
   * ~~~
   */
  template <class Action> void commandChildren(const Action& action)
  {
    QList<PiiOperation*> lstOperations = childOperations();
    for (int i=0; i<lstOperations.size(); ++i)
      action.perform(lstOperations[i]);
  }
  template <class Action, class Param> void commandChildren(const Action& action, const Param& param)
  {
    QList<PiiOperation*> lstOperations = childOperations();
    for (int i=0; i<lstOperations.size(); ++i)
      action.perform(lstOperations[i], param);
  }
  /// @endhide

  /**
   * Sets the state of the operation. If the state changes, the
   * stateChanged() signal will be emitted.
   */
  void setState(State state);

  /**
   * Called by [setState()] just before the operation changes to a new
   * state. The function will be called independent of the cause of
   * the state change (internal or external). Derived classes may
   * implement this function to perform whatever is needed when a
   * state changes. The default implementation does nothing.
   */
  virtual void aboutToChangeState(State newState);

private slots:
  void childStateChanged(int state);
  void childDestroyed(QObject* op);
  void handleError(PiiOperation* sender, const QString& msg);
  void removeExposedInput(QObject* socket);
  void removeExposedOutput(QObject* socket);
  
private:
  // Recursive socket look-up
  struct InputFinder;
  struct OutputFinder;

  // Finders for properties
  struct SetPropertyFinder;
  struct GetPropertyFinder;
  // and sub-operations
  struct OperationFinder;
  
  // Generic find function for sockets and UI components
  template <class Finder> typename Finder::Type find(Finder f, const QString& path) const;

  // One-level child lookup
  PiiOperation* findChildOperation(const QString& childName) const;

  // State changing utilities
  bool checkSteadyStateChange(State newState, State intermediateState, State steadyState);
  bool checkChildStates(State state);

  // Serialization stuff
  typedef QPair<PiiOperation*, QString> EndPointType;
  typedef QList<EndPointType> EndPointListType;
  PiiOperation* findCommonParent(QObject* obj1, QObject* obj2, int& parentIndex) const;
  EndPointType locateSocket(PiiSocket* socket, const PiiOperationCompound *context = 0) const;
  EndPointListType buildEndPointList(PiiAbstractOutputSocket* socket, const PiiOperationCompound *context = 0) const;
  void connectAll(PiiAbstractOutputSocket* source, const EndPointListType& targets);
  QString proxyInputName(PiiAbstractInputSocket* input) const;


  template <class Socket> static void resetProxies(const QList<Socket*>& list);
  template <class Socket> static Socket* findSocket(const QString& name, const QList<Socket*>& list);
  template <class Socket> static void clearSocketList(QList<Socket*>& list);
  template <class Socket> static bool removeSocket(QObject* socket, QList<Socket*>& list);
  template <class Socket> static bool removeSocket(const QString& name, QList<Socket*>& list);

  QString fullName(QObject* operation);
};

/// @internal
class PiiOperationCompound::Data : public PiiOperation::Data
{
public:
  Data();
  virtual ~Data();

  bool isCompound() const { return true; }

  /**
   * The current state.
   */
  State state;
  
  /**
   * List of child operations.
   */
  QList<PiiOperation*> lstOperations;

private:
  friend class PiiOperationCompound;
  
  /**
   * Exposed input sockets.
   */
  QList<PiiAbstractInputSocket*> lstInputs;
  /**
   * Exposed output sockets.
   */
  QList<PiiAbstractOutputSocket*> lstOutputs;
    
  /**
   * States of child operations. In each pair, "first" is the state of
   * the operation and "second" a flag that tells if the operation has
   * been in Running state.
   */
  QVector<QPair<State,bool> > vecChildStates;

  bool bChecked, bWaiting;
};

Q_DECLARE_METATYPE(PiiOperationCompound*);
typedef QList<PiiOperation*> PiiOperationList;
Q_DECLARE_METATYPE(PiiOperationList);

#include "PiiOperationCompound-templates.h"

#define PII_SERIALIZABLE_CLASS PiiOperationCompound
#define PII_BUILDING_LIBRARY PII_BUILDING_YDIN

#include <PiiSerializableRegistration.h>

#endif //_PIIOPERATIONCOMPOUND_H
