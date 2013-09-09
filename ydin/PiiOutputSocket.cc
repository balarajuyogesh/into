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

#include "PiiOutputSocket.h"
#include "PiiInputSocket.h"
#include "PiiYdinTypes.h"
#include "PiiOperation.h"

#include <PiiUtil.h>
#include <PiiSerializableExport.h> // MSVC

#include <QThread>

using namespace PiiYdin;

PiiOutputSocket::Data::Data(PiiOutputSocket* owner) :
  PiiAbstractOutputSocket::Data(owner),
  iGroupId(0),
  freeInputCondition(PiiWaitCondition::NoQueue),
  pFirstInput(0),
  pFirstController(0),
  bInterrupted(false),
  pbInputCompleted(0),
  activeThreadId(0)
{
}

bool PiiOutputSocket::Data::setOutputConnected(bool connected)
{
  return bConnected = PiiAbstractOutputSocket::Data::setOutputConnected(connected);
}

PiiAbstractOutputSocket* PiiOutputSocket::Data::rootOutput() const
{
  return q;
}

PiiOutputSocket::PiiOutputSocket(const QString& name) :
  PiiAbstractOutputSocket(new Data(this))
{
  setObjectName(name);
}

PiiOutputSocket::PiiOutputSocket(Data* data, const QString& name) :
  PiiAbstractOutputSocket(data)
{
  setObjectName(name);
}

PiiOutputSocket::~PiiOutputSocket()
{
  delete[] _d()->pbInputCompleted;
}

PiiSocket::Type PiiOutputSocket::type() const { return Output; }

void PiiOutputSocket::setGroupId(int id) { _d()->iGroupId = id; }
int PiiOutputSocket::groupId() const { return _d()->iGroupId; }

bool PiiOutputSocket::isConnected() const
{
  return _d()->bConnected;
}

void PiiOutputSocket::interrupt()
{
  PII_D;
  d->bInterrupted = true;
  // Bypass any forthcoming wait() call.
  d->freeInputCondition.wakeOne();
  d->state = PiiSocketState();
}

void PiiOutputSocket::reset()
{
  PII_D;
  d->bInterrupted = false;
  d->freeInputCondition.wakeAll();
  d->lstBuffer.clear();
  d->activeThreadId = 0;
}

bool PiiOutputSocket::flushBuffer()
{
  PII_D;

  while (!d->lstThreads.isEmpty())
    {
      // Flush every object belonging to the thread that currently has
      // emission turn.
      Qt::HANDLE activeThreadId = d->lstThreads[0].id;
      for (OutputBuffer::iterator i=d->lstBuffer.begin(); i!=d->lstBuffer.end(); )
        {
          if (i->first == activeThreadId)
            {
              if (!tryEmit(i->second))
                return false;
              i = d->lstBuffer.erase(i);
            }
          else
            ++i;
        }
      // If this thread is already done, we can safely remove it.
      if (d->lstThreads[0].bFinished)
        {
          d->lstThreads.remove(0);
          d->endEmitCondition.wakeAll();
        }
      // Otherwise stop flushing.
      else
        return true;
    }
  // If the last thread was removed from emission queue and there are
  // still buffered objects, flush everything.
  if (d->lstThreads.isEmpty() && !d->lstBuffer.isEmpty())
    {
      for (OutputBuffer::iterator i=d->lstBuffer.begin(); i!=d->lstBuffer.end(); )
        {
          if (!tryEmit(i->second))
            return false;
          i = d->lstBuffer.erase(i);
        }
    }
  return true;
}

void PiiOutputSocket::emitThreaded(const PiiVariant& object)
{
  PII_D;
  QMutexLocker lock(&d->emitLock);
  d->lstBuffer.append(qMakePair(QThread::currentThreadId(), object));
}

int PiiOutputSocket::Data::queueIndex(Qt::HANDLE threadId) const
{
  for (int i=0; i<lstThreads.size(); ++i)
    if (lstThreads[i].id == threadId)
      return i;
  return -1;
}

bool PiiOutputSocket::Data::hasBufferedObjects(Qt::HANDLE activeThreadId) const
{
  // See if there are any buffered objects for this thread
  for (OutputBuffer::const_iterator i=lstBuffer.begin(); i!=lstBuffer.end(); ++i)
    if (i->first == activeThreadId)
      return true;
  return false;
}

void PiiOutputSocket::startEmit(Qt::HANDLE activeThreadId)
{
  // Put the thread to the tail of the emission queue.
  PII_D;
  QMutexLocker lock(&d->emitLock);
  //piiDebug("Start %p (%d)", (void*)activeThreadId, d->lstThreads.size());
  // Must check that the thread isn't already in queue. If it is, wait
  // until it is finished.
  while (d->queueIndex(activeThreadId) != -1 && !d->bInterrupted)
    d->endEmitCondition.wait(&d->emitLock, 100);

  d->lstThreads.append(ThreadInfo(activeThreadId));
}

void PiiOutputSocket::endEmit(Qt::HANDLE activeThreadId)
{
  PII_D;
  do
    {
      if (tryEndEmit(activeThreadId))
        return;
      d->freeInputCondition.wait();
    }
  while (!d->bInterrupted);
  throw PiiExecutionException(PiiExecutionException::Interrupted);
}

bool PiiOutputSocket::tryEndEmit(Qt::HANDLE activeThreadId)
{
  PII_D;
  QMutexLocker lock(&d->emitLock);

  int iQueueIndex = d->queueIndex(activeThreadId);

  //piiDebug("End %p (%d)", (void*)activeThreadId, iQueueIndex);

  switch (iQueueIndex)
    {
    case 0:
      // This is the blocking thread
      d->lstThreads[iQueueIndex].bFinished = true;
    case -1:
      // The thread is no longer in queue -> last call ended with an
      // incomplete flush.
      return flushBuffer(); // may throw
    default:
      // If there are buffered objects, cannot yet remove the thread from queue.
      if (d->hasBufferedObjects(activeThreadId))
        d->lstThreads[iQueueIndex].bFinished = true;
      else // This thread wasn't blocking -> no signal needed, just remove.
        d->lstThreads.remove(iQueueIndex);
      break;
    }
  return true;
}

void PiiOutputSocket::emitObject(const PiiVariant& object)
{
  if (_d()->lstThreads.isEmpty())
    emitNonThreaded(object);
  else
    emitThreaded(object);
}

bool PiiOutputSocket::tryEmit(const PiiVariant& object)
{
  if (!object.isValid())
    PII_THROW(PiiExecutionException, tr("Trying to send an invalid object."));

  PII_D;
  const int iCnt = d->lstInputs.size();

  // Optimized emission for a single connected input
  if (iCnt == 1)
    return d->pFirstController->tryToReceive(d->pFirstInput, object);
  else if (iCnt == 0)
    return true;
  
  bool bAllCompleted = true;
  for (int i=0; i<iCnt; ++i)
    {
      if (!d->pbInputCompleted[i])
        bAllCompleted &= d->pbInputCompleted[i] =
          d->lstInputs.controllerAt(i)->tryToReceive(d->lstInputs.inputAt(i), object);
    }
  if (bAllCompleted)
    {
      Pii::fillN(d->pbInputCompleted, d->lstInputs.size(), false);
      d->freeInputCondition.wakeAll();
    }
  
  return bAllCompleted;
}
  
void PiiOutputSocket::emitNonThreaded(const PiiVariant& object)
{
  PII_D;
  // Try to send until the object is successfully received.
  do
    {
      if (tryEmit(object))
        return;
      d->freeInputCondition.wait();
    }
  while (!d->bInterrupted);
  throw PiiExecutionException(PiiExecutionException::Interrupted);
}

void PiiOutputSocket::inputReady(PiiAbstractInputSocket* /*input*/)
{
  _d()->freeInputCondition.wakeOne();
}

PiiOutputSocket* PiiOutputSocket::socket() { return this; }
PiiAbstractInputSocket* PiiOutputSocket::asInput() { return 0; }
PiiAbstractOutputSocket* PiiOutputSocket::asOutput() { return this; }

void PiiOutputSocket::synchronizeTo(PiiInputSocket* input)
{
  setGroupId(input->groupId());
}

void PiiOutputSocket::inputConnected(PiiAbstractInputSocket* input)
{
  PII_D;
  // Run-time optimization
  if (d->lstInputs.size() == 1)
    {
      d->pFirstInput = input;
      d->pFirstController = input->controller();
    }
  createFlagArray();
}

void PiiOutputSocket::inputDisconnected(PiiAbstractInputSocket*)
{
  PII_D;
  if (d->lstInputs.size() > 0)
    {
      d->pFirstInput = d->lstInputs.inputAt(0);
      d->pFirstController = d->lstInputs.controllerAt(0);
    }
  else
    {
      d->pFirstInput = 0;
      d->pFirstController = 0;
    }
  createFlagArray();
}

void PiiOutputSocket::inputUpdated(PiiAbstractInputSocket*)
{
  PII_D;
  if (d->lstInputs.size() > 0)
    d->pFirstController = d->lstInputs.controllerAt(0);
}

void PiiOutputSocket::createFlagArray()
{
  PII_D;
  //qDebug("PiiOutputSocket: creating flag array for %d connections.", d->lstInputs.size());
  delete[] d->pbInputCompleted;
  if (d->lstInputs.size() > 0)
    {
      d->pbInputCompleted = new bool[d->lstInputs.size()];
      Pii::fillN(d->pbInputCompleted, d->lstInputs.size(), false);
    }
  else
    d->pbInputCompleted = 0;
}

void PiiOutputSocket::resume(PiiSocketState state)
{
  PII_D;
  /*piiDebug("Resuming %s::%s. (%d, %d) + (%d, %d)", parent() ? parent()->metaObject()->className() : "" ,
           qPrintable(objectName()),
           (int)state.flowLevel, (int)state.delay,
           (int)d->state.flowLevel, (int)d->state.delay);
  */
  emitObject(PiiSocketState(QATOMICINT_LOAD(state.flowLevel) + QATOMICINT_LOAD(d->state.flowLevel),
                            QATOMICINT_LOAD(state.delay) + QATOMICINT_LOAD(d->state.delay)));
}

void PiiOutputSocket::startDelay()
{
  _d()->state.delay.ref();
}

void PiiOutputSocket::endDelay()
{
  _d()->state.delay.deref();
}

void PiiOutputSocket::startMany()
{
  emitObject(PiiVariant(1, SynchronizationTagType));
  _d()->state.flowLevel.ref();
}

void PiiOutputSocket::endMany()
{
  emitObject(PiiVariant(-1, SynchronizationTagType));
  _d()->state.flowLevel.deref();
}

void PiiOutputSocket::setInputListener(PiiInputListener* listener)
{
  if (listener == 0) listener = this;
  PII_D;
  for (int i=0; i<d->lstInputs.size(); ++i)
    d->lstInputs.inputAt(i)->setListener(listener);
}
