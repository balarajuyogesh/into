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

#ifndef _PIIOUTPUTSOCKET_H
#define _PIIOUTPUTSOCKET_H

#include "PiiSocket.h"
#include "PiiAbstractOutputSocket.h"
#include "PiiSocketState.h"
#include "PiiExecutionException.h"
#include "PiiInputListener.h"

#include <PiiVariant.h>
#include <PiiMatrix.h>
#include <PiiWaitCondition.h>

#include <QLinkedList>
#include <QVarLengthArray>
#include <QPair>

class PiiAbstractInputSocket;
class PiiInputSocket;
class PiiInputController;

namespace PiiYdin
{ 
  /// @internal
  template <class Matrix, class T>
  inline PiiVariant createVariant(const PiiConceptualMatrix<Matrix>*,
                                  const T& mat)
  {
    // This should actually go into PiiYdinTypes.h, but that would
    // make inclusion order a huge trouble. Stupid C++.
    return PiiVariant(Pii::matrix(mat));
  }

  /// @internal
  template <class T> inline PiiVariant createVariant(const void*, const T& value)
  {
    return PiiVariant(value);
  }

  /**
   * Returns a variant that contains `value`. By default, this
   * function just returns `PiiVariant`(value). Using this function
   * as an indirection is however useful since it allows
   * specialized versions for different data types. For example, all
   * conceptual matrices are converted into concrete PiiMatrix
   * instances before creating a variant out of them.
   *
   * ~~~(c++)
   * PiiMatrix<int> a, b;
   * PiiVariant v1(a+b); //Won't work, because a+b is not a concrete matrix
   * PiiVariant v2(PiiYdin::createVariant(a+b)); // works
   * ~~~
   */
  template <class T> inline PiiVariant createVariant(const T& value)
  {
    // Hack resolution order for superclasses. Stupid C++.
    return createVariant(&value, value);
  }
}

/**
 * An output socket.
 *
 */
class PII_YDIN_EXPORT PiiOutputSocket : public PiiAbstractOutputSocket
{
  Q_OBJECT

public:
  /**
   * Construct a new output socket with the given name. This
   * constructor sets `name` as the `objectName` property of the
   * class.
   */
  PiiOutputSocket(const QString& name);

  /**
   * Destroy the socket. Automatically disconnects all input sockets.
   */
  ~PiiOutputSocket();

  /**
   * Set the group id of this socket. The group id of an output socket
   * is used to find synchronization pairs. The default flow control
   * implementation sends synchronization information from an input
   * socket to all output sockets matching the group id of the input.
   */
  void setGroupId(int groupId);

  /**
   * Get the group id of this socket.
   */
  Q_INVOKABLE int groupId() const;

  /**
   * Set this socket synchronized with respect to `input`. This is a
   * convenience function that sets the socket's group id to that of
   * `input`, which indicates that this output emits objects in
   * relation to the specified input. For example, the output may emit
   * an object each time a new object is received in `input`. Or it
   * may emit two objects for each input, or one for each three
   * inputs, or two for every other input and three for the others. In
   * principle, any variation is allowed. The main point is that the
   * output is caused by objects read from `input`. By default, all
   * inputs and outputs are in sync group 0. There is usually no need
   * to change this.
   */
  void synchronizeTo(PiiInputSocket* input);

  /**
   * Inform the synchronization system that the rate of the following
   * emissions differs from that of the synchronized input socket. 
   * This function must be called whenever you start sending more
   * objects than you receive (or no objects at all). To emit two
   * objects for each input, do something like this in the process()
   * function of your operation:
   *
   * ~~~(c++)
   * PiiVariant obj = readObject();
   * output->startMany();
   * output->emitObject(1);
   * output->emitObject(2);
   * output->endMany();
   * ~~~
   */
  void startMany();

  /**
   * End the flow rate change.
   *
   * @see startMany()
   */
  void endMany();

  /**
   * Inform the synchronization system that the output from this
   * socket will be delayed. There is seldom need to use this function
   * as a synchronized output usually emits an object immediately upon
   * processing an input object. In some cases, however, the output
   * will be available only after some additional data has been
   * received. Examples of such operations include PiiImagePieceJoiner
   * and PiiHistogramCollector.
   *
   * The following example assumes `MyOperation` has two inputs: one
   * for a large image and another for values calculated from image
   * pieces just like PiiImagePieceJoiner. `MyOperation` calculates a
   * value for each large image, but it needs both the large image and
   * the values from sub-images for it.
   *
   * ~~~(c++)
   * void MyOperation::process()
   * {
   *   if (activeInputGroup() == _pImageInput->groupId())
   *     {
   *       _pResultOutput->startDelay();
   *       storeImage(_pImageInput->firstObject());
   *     }
   *   else
   *     storeValue(_pValueInput->firstObject());
   * }
   *
   * void MyOperation::syncEvent(SyncEvent* event)
   * {
   *   if (event.type() == SyncEvent::EndInput)
   *     {
   *       _pResultOutput->emitObject(calculateValue());
   *       _pResultOutput->endDelay();
   *     }
   * }
   * ~~~
   *
   * It is possible to delay emissions more than one processing round,
   * but this function must be called once for each delayed object. 
   * Once the objects are finally sent, the same number of endDelay()
   * calls must be performed.
   *
   * One must ensure that the size of the input queue at a receiver is
   * larger enough. In the example above, if `_pResultOutput` is
   * connected to an operation that needs both the result and the
   * large image, the size of the input queue for the input that
   * receives the image must be at least three (one for the object
   * being processed, one for the buffered object, and one for the
   * next object. The default queue size is three. Thus, delaying one
   * object is always fine.
   *
   * @see endDelay()
   */
  void startDelay();

  /**
   * Informs the synchronization system that a delayed object has now
   * been sent.
   *
   * @see startDelay()
   */
  void endDelay();

  /**
   * Restores socket state after pause. This function will modify 
   * *inputState* according to the current state of this socket and pass
   * the information to all connected input sockets.
   *
   * @param inputState the state of a synchronized input socket group.
   */
  void resume(PiiSocketState inputState);

  /**
   * Sends an object through this output to all connected inputs. This
   * function blocks until all connected synchronous inputs are able
   * to receive the object.
   *
   * ! emitObject() assumes that this output socket is used as the
   * listener for all connected inputs. If you have set a custom
   * listener to the connected input sockets, this function may block
   * indefinitely. You may need to call the `inputReady()` function of
   * the original listener from your implementation.
   *
   * @exception PiiExecutionException& if the emission was interrupted
   * by an external signal.
   */
  void emitObject(const PiiVariant& obj);

  /**
   * Tries to sends an object through this output to all connected
   * inputs. If any of the inputs is unable to receive the object,
   * returns `false`. Otherwise returns `true`. Successive calls to
   * this function will retry the emission until all connected inputs
   * have accepted the object. If you use this function, you may also
   * need to set a custom listener to the connected input sockets to
   * avoid unnecessary retries.
   *
   * @exception PiiExecutionException& if the emission was interrupted
   * by an external signal.
   */
  bool tryEmit(const PiiVariant& object);

  /**
   * Creates a PiiVariant out of the given *value* and emits it. This
   * is a convenience function that frees you from manually creating
   * the variant. The function calls PiiYdin::createVariant(T). The
   * following code emits a PiiVariant whose type() function returns
   * PiiVariant::IntType.
   *
   * ~~~(c++)
   * output->emitObject(5);
   * ~~~
   */
  template <class T> inline void emitObject(const T& value)
  {
    emitObject(PiiYdin::createVariant(value));
  }

  /**
   * Returns `true` if the output is connected to at least one input
   * that is not a proxy whose output is not connected, and `false`
   * otherwise.
   *
   * @see PiiSocketProxy
   */
  Q_INVOKABLE bool isConnected() const;

  /**
   * Interrupts any ongoing object emission. This function is used
   * when the operation must be cancelled as soon as possible without
   * waiting that receivers are ready.
   */
  void interrupt();

  /**
   * Clears the socket. This function is called to prepare the socket
   * for a clean start.
   */
  void reset();

  /**
   * Puts *activeThreadId* to the emission order queue. This function
   * makes it possible to use the same output socket from different
   * threads. Before letting concurrent threads send objects to a
   * socket one can call this function for each of the threads in the
   * order they should emit the objects. The output socket will then
   * buffer the objects as necessary to ensure that everything will be
   * sent in correct order. If there are no threads in the emission
   * queue, objects will be passed without buffering.
   *
   * If an emitting thread is not listed in the emission queue,
   * emitted objects will be blocked indefinitely. One can make use of
   * this feature to buffer all objects emitted between startEmit()
   * and [endEmit()].
   *
   * ~~~(c++)
   * // Only thread id 0 is allowed to emit objects, others will be buffered.
   * // This effectively blocks all threads.
   * pOutput->startEmit(0);
   * pOutput->emitObject(PiiVariant(1));
   * pOutput->emitObject(PiiVariant(2));
   * // Remove the blocking thread from the queue.
   * pOutput->endEmit(0);
   * ~~~
   */
  void startEmit(Qt::HANDLE activeThreadId);

  /**
   * Removes *activeThreadId* from the emission order queue. If there
   * are buffered objects blocked by this thread's emission turn, they
   * will be flushed. If you use custom input listeners, same
   * precautions as with [emitObject()] apply.
   *
   * @exception PiiExecutionException& if the emission of buffered
   * objects was interrupted by an external signal.
   *
   * ! Flushing parallel outputs easily hangs your processing
   * pipeline. If you flush buffered outputs one at a time, the input
   * queue at the receiving end must be able to hold all buffered
   * objects. You may consider using tryEndEmit() instead.
   */
  void endEmit(Qt::HANDLE activeThreadId);

  /**
   * Tries to remove *activeThreadId* from the emission order queue
   * and flush all buffered objects blocked by this thread's emission
   * turn. Returns `true` if successful, `false` otherwise. See
   * [tryEmit()].
   */
  bool tryEndEmit(Qt::HANDLE activeThreadId);

  /**
   * Sets *listener* as the input listener for all connected inputs.
   * If *listener* is zero, uses a default listener.
   */
  void setInputListener(PiiInputListener* listener = 0);
  
protected:
  /// @hide
  struct ThreadInfo
  {
    ThreadInfo(Qt::HANDLE i=0, bool f = false) : id(i), bFinished(f) {}
    Qt::HANDLE id;
    bool bFinished;
  };

#if (QT_VERSION < 0x040800)
  typedef QVector<ThreadInfo> ThreadList;
#else
  typedef QVarLengthArray<ThreadInfo,16> ThreadList;
#endif
  typedef QLinkedList<QPair<Qt::HANDLE,PiiVariant> > OutputBuffer;

  class Data :
    public PiiAbstractOutputSocket::Data,
    public PiiInputListener
  {
  public:
    Data();
    ~Data();
    
    void inputReady(PiiAbstractInputSocket* input);
    bool setOutputConnected(bool connected);

    inline int queueIndex(Qt::HANDLE threadId) const;
    inline bool hasBufferedObjects(Qt::HANDLE activeThreadId) const;
    void inputConnected(PiiAbstractInputSocket* input);
    void inputDisconnected(PiiAbstractInputSocket* input);
    void inputUpdated(PiiAbstractInputSocket* input);
    void createFlagArray();
  
    int iGroupId;
    bool bConnected;
    // A wait condition that is used when some inputs aren't ready to
    // receive new objects.
    PiiWaitCondition freeInputCondition, *pFreeInputCondition;
    PiiAbstractInputSocket* pFirstInput;
    PiiInputController* pFirstController;
    bool bInterrupted;
    bool *pbInputCompleted;
    PiiSocketState state;
    Qt::HANDLE activeThreadId;
    OutputBuffer lstBuffer;
    ThreadList lstThreads;
    QMutex emitLock;
    QWaitCondition endEmitCondition;
  };
  PII_UNSAFE_D_FUNC;

  PiiOutputSocket(const QString& name, Data* data);
  /// @endhide

private:
  bool flushBuffer();
  void emitThreaded(const PiiVariant& object);
  void emitNonThreaded(const PiiVariant& object);
};

Q_DECLARE_METATYPE(PiiOutputSocket*);

#endif //_PIIOUTPUTSOCKET_H
