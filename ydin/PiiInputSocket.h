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

#ifndef _PIIINPUTSOCKET_H
#define _PIIINPUTSOCKET_H

#include "PiiSocket.h"
#include "PiiAbstractInputSocket.h"
#include "PiiInputController.h"

#include <QVarLengthArray>
#include <QPair>

class PiiOutputSocket;


/**
 * A macro that throws a PiiExecutionException. The error message
 * states that an object of an unknown type was received in `input`.
 *
 * @param input a pointer to the input that received an unrecognized
 * object.
 */
#define PII_THROW_UNKNOWN_TYPE(input) \
  PII_THROW(PiiExecutionException, PiiYdin::unknownTypeErrorMessage(input))

/**
 * A macro that throws a PiiExecutionException. The error message
 * states that the `matrix` received in `input` has a wrong size,
 * and it should actually have the specified number of `rows` and
 * `columns`.
 *
 * @param INPUT a pointer to the input that reveived the matrix.
 *
 * @param MATRIX a matrix (or a reference)
 *
 * @param ROWS the correct number of rows, e.g. 1 or "N"
 *
 * @param COLUMNS the correct number of columns, e.g. 4 or "M"
 */
#define PII_THROW_WRONG_SIZE(INPUT, MATRIX, ROWS, COLUMNS) \
  PII_THROW(PiiExecutionException, \
            PiiYdin::wrongSizeErrorMessage(INPUT, \
                                           QString("%1 x %2").arg((MATRIX).rows()).arg((MATRIX).columns()), \
                                           QString("%1 x %2").arg(ROWS).arg(COLUMNS)))

/**
 * Default implementation of an input socket. This class maintains an
 * input queue whose capacity can be controlled with the
 * [queueCapacity] property. A producer operation can send objects to
 * the input queue and continue working until the input queue is full.
 * Once this happens, the sending thread must wait until the receiver
 * has processed one or more objects.
 *
 * At the start of each processing round, the parent operation takes
 * one object from the queue to processing by calling [shift()]. This
 * moves the head of the input queue so that the processable object
 * can be retrieved with [firstObject()]. New objects may then appear
 * at any time until the queue is full again.
 *
 */
class PII_YDIN_EXPORT PiiInputSocket : public PiiAbstractInputSocket
{
  Q_OBJECT

  /**
   * The capacity of the input queue. The default value is 2. The
   * minimum value for queue capacity is 1. Note that queue capacity
   * can safely be changed only if the parent operation is stopped.
   * Changing the capacity will destroy all objects currently in the
   * queue.
   */
  Q_PROPERTY(int queueCapacity READ queueCapacity WRITE setQueueCapacity);

public:
  /**
   * Constructs a new input socket with the given name.
   */
  PiiInputSocket(const QString& name);

  /// Destroys the socket.
  ~PiiInputSocket();

  /**
   * Sets the group id. Input sockets are organized to groups
   * identified by numeric ids. Sockets with the same group id are
   * assumed to work in parallel. The default synchronization
   * mechanism in Ydin ensures that all synchronous inputs with the
   * same group id contain a readable object when an operation is
   * processed. This makes it possible to read in objects with
   * different flow rates. The default group id is zero, which causes
   * all synchronous inputs to work in parallel.
   */
  void setGroupId(int groupId);

  /**
   * Returns the group id of this socket.
   */
  Q_INVOKABLE int groupId() const;

  /**
   * Returns `true` if the input is connected to a PiiOutputSocket
   * either directly or through a PiiProxySocket, and `false`
   * otherwise.
   */
  Q_INVOKABLE bool isConnected() const;

  /**
   * Checks if this input socket can be left unconnected in processing.
   * If an optional input socket is left unconnected, the receiving
   * operation ignores it in processing.
   *
   * @return `true` if the socket can be left unconnected, `false`
   * otherwise
   */
  Q_INVOKABLE bool isOptional() const;

  /**
   * Sets the `optional` flag.
   */
  void setOptional(bool optional);

  /**
   * Moves the queue one step forwards. The current head of the queue
   * will be moved out of the queue so that it can be retrieved with
   * [firstObject()]. Once the queue is successfully shifted, the
   * socket signals inputReady().
   */
  void shift();

  /**
   * Assigns the first object in the input queue to the specified
   * thread. If the input object is assigned to a thread,
   * [firstObject()] will return it to the specified thread only. There
   * may be any number of threads with their own assigned objects.
   */
  void assignFirstObject(Qt::HANDLE activeThreadId);
  /**
   * Destroys the object currently assigned to the specified thread.
   */
  void unassignFirstObject(Qt::HANDLE activeThreadId);

  /**
   * Jumps the queue. This function moves the object at `oldIndex` to
   * a new place, identified by `newIndex`, shifting intermediate
   * objects backwards.
   */
  void jump(int oldIndex, int newIndex);

  /**
   * Searches the queue for an object whose type ID matches `type`.
   *
   * @param type the type ID to look for
   *
   * @param startIndex start search at this index
   *
   * @return the index of the first object with the given type ID, or
   * -1 if no such object is found.
   */
  int indexOf(unsigned int type, int startIndex = 0) const;

  /**
   * Resets the socket. This clears the object queue.
   */
  void reset();

  /**
   * Puts `obj` into the incoming queue.
   */
  void receive(const PiiVariant& obj);

  /**
   * Checks if the input queue in this socket still has room for a new
   * object. This function is a shorthand for queueCapacity() >
   * queueLength().
   */
  bool canReceive() const;

  /**
   * Sets the input queue capacity.
   */
  void setQueueCapacity(int queueCapacity);
  /**
   * Returns the capacity of the input queue.
   */
  int queueCapacity() const;

  /**
   * Returns the number of objects currently in the input queue.
   */
  int queueLength() const;

  /**
   * Returns the object at `index` in the input queue. If there is no
   * such object, an invalid variant will be returned.
   */
  PiiVariant queuedObject(int index) const;

  /**
   * Returns the type ID of the object at `index` in the input queue.
   */
  unsigned int queuedType(int index) const;

  /**
   * Returns the object that was last shifted from the input queue. If
   * no objects have been shifted, an invalid variant will be
   * returned. If [assignFirstObject()] has been called, a different
   * object will be returned for each calling thread.
   *
   * ~~~(c++)
   * PiiVariant obj(pInput->firstObject());
   * if (obj.isValid())
   *   cout << "We just received an object!" << endl;
   * ~~~
   */
  PiiVariant firstObject() const;

  /**
   * Sets the input controller. The controller must be set before the
   * input can receive objects. This is done automatically in
   * PiiDefaultOperation::check().
   */
  void setController(PiiInputController* controller);

  PiiInputController* controller() const;

protected:
  /// @internal
  class Data : public PiiAbstractInputSocket::Data
  {
  public:
    Data();

    bool setInputConnected(bool connected);

    int iGroupId;
    bool bConnected;
    bool bOptional;
    PiiInputController* pController;
    QVarLengthArray<PiiVariant, 4> lstQueue;
    PiiVariant varProcessableObject;
    QVarLengthArray<QPair<Qt::HANDLE, PiiVariant> > lstProcessableObjects;
    int iQueueStart, iQueueLength;
    mutable QMutex firstObjectMutex;
  };
  PII_D_FUNC;

  /// @internal
  PiiInputSocket(const QString& name, Data* data);

private:
  inline int queueIndex(int index) const { return (_d()->iQueueStart+index) % _d()->lstQueue.size(); }
};

Q_DECLARE_METATYPE(PiiInputSocket*);

namespace PiiYdin
{
  /// @internal
  PII_YDIN_EXPORT QString unknownTypeErrorMessage(PiiInputSocket* input);

  /// @internal
  PII_YDIN_EXPORT QString wrongSizeErrorMessage(PiiInputSocket* input, const QString& received, const QString& assumed);
}

#endif //_PIIINPUTSOCKET_H
