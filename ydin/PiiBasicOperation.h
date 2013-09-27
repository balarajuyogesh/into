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

#ifndef _PIIBASICOPERATION_H
#define _PIIBASICOPERATION_H

#include "PiiOperation.h"

/**
 * A bare bones implementation of the PiiOperation interface. This
 * class provides facilities for adding and removing sockets. It
 * changes its state immediately when start(), pause(), stop(), or
 * interrupt() is called. PiiBasicOperation provides no means of
 * handling incoming objects in input sockets.
 *
 */
class PII_YDIN_EXPORT PiiBasicOperation : public PiiOperation
{
  Q_OBJECT
  
public:
  ~PiiBasicOperation();
  
  /**
   * Check the operation for execution. If any non-optional sockets is
   * not connected, an exception is thrown.
   *
   * @param reset if `true`, all sockets are cleared.
   *
   * ! If you override this function, remember to call the
   * parent's check() function in your own implementation.
   */
  void check(bool reset);

  QList<PiiAbstractInputSocket*> inputs() const;

  /**
   * Returns attached inputs as PiiInputSockets.
   */
  QList<PiiInputSocket*> inputSockets() const;

  QList<PiiAbstractOutputSocket*> outputs() const;

  /**
   * Returns attached outputs as PiiOutputSockets.
   */
  QList<PiiOutputSocket*> outputSockets() const;

  /**
   * Returns the value of a property associated with *socket*. If 
   * *name* is "name", returns the `objectName` property of *socket*. 
   * Otherwise returns the named property. If *socket* is not
   * attached to this operation, returns an invalid QVariant.
   */
  QVariant socketProperty(PiiAbstractSocket* socket, const char* name) const;

  /**
   * Returns the current state.
   */
  State state() const;

  /// Returns `true`.
  bool wait(unsigned long time);

  /**
   * Check if the operation has connected inputs.
   *
   * @param return `true` if the operation has input sockets and at
   * least of them is connected.
   */
  bool hasConnectedInputs() const;

  int inputCount() const;
  
  PiiInputSocket* input(const QString& name) const;
  
  /**
   * Returns the input a `index`. No overflow checking will be performed.
   */
  PiiInputSocket* inputAt(int index = 0) const;

  int outputCount() const;
  
  PiiOutputSocket* output(const QString& name) const;

  /**
   * Returns the output a `index`. No overflow checking will be performed.
   */
  PiiOutputSocket* outputAt(int index = 0) const;

protected:
  /// @internal
  class PII_YDIN_EXPORT Data : public PiiOperation::Data
  {
  public:
    Data();
    virtual ~Data();
    
    /**
     * The current state.
     */
    State state;
    
    /**
     * Pointers to input sockets.
     */
    QList<PiiInputSocket*> lstInputs;
    
    /**
     * Pointers to output sockets.
     */
    QList<PiiOutputSocket*> lstOutputs;
  };
  PII_D_FUNC;

  PiiBasicOperation();

  /// @internal
  PiiBasicOperation(Data* data);

  /**
   * Adds a new input socket. Sockets are returned by inputs() in
   * the order they were added.
   *
   * @param socket a pointer to an input socket
   *
   * @param index insert the socket at this position. -1 means last.
   */
  void insertInput(PiiInputSocket* socket, int index);

  /**
   * Adds a new output socket. Analogous to insertInput().
   */
  void insertOutput(PiiOutputSocket* socket, int index);

  /**
   * Adds an input to the end of the input list. This is a convenience
   * function that calls insertInput(socket, -1).
   *
   * @param socket a pointer to the socket to add to this operation's
   * list of inputs
   */
  void addSocket(PiiInputSocket* socket);

  /**
   * Adds an output to the end of the output list. This is a
   * convenience function that calls insertOutput(socket, -1).
   *
   * @param socket a pointer to the socket to add to this operation's
   * list of output
   */
  void addSocket(PiiOutputSocket* socket);

  /**
   * Removes the given socket. This function calls removeInput(int).
   */
  void removeSocket(PiiInputSocket* socket);

  /**
   * Removes the given socket. This function calls removeOutput(int).
   */
  void removeSocket(PiiOutputSocket* socket);

  /**
   * Removes the input at `index`. The socket will be removed from the
   * socket list and returned. To actually break the connection to
   * this socket, one needs to explicitly disconnect or delete the
   * socket:
   *
   * ~~~(c++)
   * delete operation.removeInput(0);
   * ~~~
   *
   * @return a pointer to the removed socket, or 0 if the index is out
   * of bounds.
   */
  PiiInputSocket* removeInput(int index);

  /**
   * Removes the input at `index`. The socket will be removed from the
   * socket list and returned. To actually break the connections from
   * this socket, one needs to explicitly disconnect or delete the
   * socket:
   *
   * ~~~(c++)
   * delete operation.removeOutput(0);
   * ~~~
   *
   * @return a pointer to the removed socket, or 0 if the index is out
   * of bounds.
   */
  PiiOutputSocket* removeOutput(int index);

  /**
   * A convenience function that reads the first object in the input
   * queue of input at `index`.
   */
  PiiVariant readInput(int index) const;

  /**
   * A convenience function that reads the first object in the input
   * queue of the first input.
   */
  PiiVariant readInput() const;

  /**
   * Sends `obj` to the output at `index`. A shorthand for
   * output(index)->emitObject(obj).
   *
   * @see PiiOutputSocket::emitObject().
   */
  void emitObject(const PiiVariant& obj, int index = 0);

  /**
   * Sends `value` to the output at `index`. A shorthand for
   * output(index)->emitObject(value).
   *
   * @see PiiOutputSocket::emitObject().
   */
  template <class T> void emitObject(const T& value, int index = 0)
  {
    emitObject(PiiYdin::createVariant(value), index);
  }
  
  /**
   * Sets the state to `state`. If the current state is equal to 
   * `state`, does nothing. Otherwise, the current state will be set to
   * `state` and a [stateChanged](PiiOperation::stateChanged())
   * signal will be emitted.
   *
   * ! This function is a primitive utility function that is not
   * protected with the [state lock](PiiOperation::stateLock()).
   * Subclasses must acquire the lock where needed.
   */
  void setState(State state);

  /**
   * Interrupts pending emissions in all output sockets. This is a
   * utility function that can be called from sub-classes'
   * interrupt().
   */
  void interruptOutputs();
  
  /**
   * Signals that this operation has finished execution for example
   * due to end of data. This function informs all connected
   * operations that the operation has finished by sending a stop tag
   * to all outputs. It finally throws an exception
   * (PiiExecutionException::Finished).
   *
   * One may override this function to perform cleanup before
   * stopping. The difference in this and the [aboutToChangeState()]
   * function is that this function will be called before the
   * operation passes stop tags to output sockets. It is therefore
   * possible to send objects to outputs, if needed. If the default
   * implementation is not called, the overridden function must throw
   * a `Finished` exception.
   */
  virtual void operationStopped();
  
  /**
   * Signals that this operation has paused because it has received a
   * pause tag from a previous operation in the processing pipeline. 
   * This function informs all connected operations that the operation
   * has paused by sending a pause tag to all outputs. It finally
   * throws an exception (PiiExecutionException::Paused).
   *
   * One may override this function to prepare for pausing. The
   * difference in this and the [aboutToChangeState()] function is that
   * this function will becalled before the operation passes pause
   * tags to output sockets. If the default implementation is not
   * called, the overridden function must throw a `Paused` exception.
   */
  virtual void operationPaused();

  /**
   * Signals that this operation has resumed from `Paused` state
   * because it has received a resume tag to all connected inputs. 
   * This function resolves the state of all input groups and resumes
   * the corresponding synchronized outputs by calling
   * PiiOutputSocket::resume().
   *
   * One may override this function to restore from pause. The
   * difference in this and the [aboutToChangeState()] function is that
   * this function will becalled before the operation passes resume
   * tags to output sockets.
   */
  virtual void operationResumed();

  /**
   * Calls PiiOperation::applyPropertySet() and sends a
   * reconfiguration tag to all outputs. You may override this
   * function to capture synchronous reconfiguration events.
   */
  void applyPropertySet(const QString& name);
  
  /**
   * Sends a control tag to all outputs.
   *
   * @param ptr the control object
   */
  void sendTag(const PiiVariant& ptr);
  
  /**
   * Maintains a set of numbered inputs at the end of the input list.
   *
   * @param count the number of inputs, numbered sequentially from 
   * *firstIndex* to *firstIndex* + *count* - 1.
   *
   * @param staticInputs retain this many inputs at the beginning.
   *
   * @param prefix a common prefix for input names.
   *
   * @param firstIndex the index of the first numbered input
   *
   * ~~~(c++)
   * // Add one static socket in the beginning of the input list
   * addSocket(new PiiInputSocket("first"));
   * // Add three inputs (input0, input1, input2) after the first one.
   * setNumberedInputs(3, 1);
   * // Remove the last two inputs
   * setNumberedInputs(1, 1);
   * ~~~
   */
  void setNumberedInputs(int count, int staticInputs = 0, const QString& prefix = "input", int firstIndex = 0);

  /**
   * Maintains a set of numbered inputs at the end of the output list.
   * This function works analogously to [setNumberedInputs()].
   */
  void setNumberedOutputs(int count, int staticOutputs = 0, const QString& prefix = "output", int firstIndex = 0);

  /**
   * Sends a start tag to outputs between (and including) 
   * *firstOutput* and *lastOutput*.
   */
  void startMany(int firstOutput = 0, int lastOutput = -1);
  /**
   * Sends an end tag to outputs between (and including) 
   * *firstOutput* and *lastOutput*.
   */
  void endMany(int firstOutput = 0, int lastOutput = -1);
  
  /**
   * Called by [setState()] just before the operation changes to a new
   * state. The function will be called independent of the cause of
   * the state change (internal or external). Derived classes may
   * implement this function to perform whatever functionality is
   * needed when a state changes. The default implementation does
   * nothing.
   */
  virtual void aboutToChangeState(State newState);

private:
  template <class T> void setNumberedSockets(QList<T*>& sockets,
                                             int count,
                                             int staticSockets,
                                             const QString& prefix,
                                             int firstIndex);
};

#endif //_PIIBASICOPERATION_H
