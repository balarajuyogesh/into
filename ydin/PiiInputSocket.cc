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

#include "PiiInputSocket.h"
#include "PiiOutputSocket.h"
#include "PiiYdinTypes.h"

#include <QStringList>
#include <QThread>

using namespace PiiYdin;

PiiInputSocket::Data::Data() :
  iGroupId(0),
  bConnected(false),
  bOptional(false),
  pController(0),
  iQueueStart(0),
  iQueueLength(0)
{
}

bool PiiInputSocket::Data::setInputConnected(bool connected)
{
  return bConnected = connected;
}

PiiInputSocket::PiiInputSocket(const QString& name) :
  PiiAbstractInputSocket(new Data)
{
  setObjectName(name);
  setQueueCapacity(2);
}

PiiInputSocket::PiiInputSocket(Data* data, const QString& name) :
  PiiAbstractInputSocket(data)
{
  setObjectName(name);
  setQueueCapacity(2);
}

PiiInputSocket::~PiiInputSocket()
{
}

PiiSocket::Type PiiInputSocket::type() const { return Input; }

bool PiiInputSocket::isConnected() const
{
  return _d()->bConnected;
}

void PiiInputSocket::setGroupId(int id) { _d()->iGroupId = id; }
int PiiInputSocket::groupId() const { return _d()->iGroupId; }

void PiiInputSocket::setQueueCapacity(int queueCapacity)
{
  PII_D;
  if (queueCapacity < 1) return;
  d->lstQueue.resize(queueCapacity);
  reset();
}

void PiiInputSocket::receive(const PiiVariant& obj)
{
  PII_D;
  d->lstQueue[queueIndex(d->iQueueLength)] = obj;
  ++d->iQueueLength;
}

void PiiInputSocket::shift()
{
  PII_D;
  Q_ASSERT(d->iQueueLength > 0);

  // Move queue head to the outgoing slot.
  d->varProcessableObject = d->lstQueue[d->iQueueStart];
  // Destroy the old head.
  d->lstQueue[d->iQueueStart] = PiiVariant();
  // Rotate the queue
  d->iQueueStart = (d->iQueueStart+1) % d->lstQueue.size();
  --d->iQueueLength;
  // Signal the sender if the queue was full (there may be a thread
  // waiting).
  if (d->iQueueLength == d->lstQueue.size()-1 && d->pListener != 0)
    d->pListener->inputReady(this);
}

void PiiInputSocket::assignFirstObject(Qt::HANDLE activeThreadId)
{
  PII_D;
  QMutexLocker lock(&d->firstObjectMutex);
  // Try to find an empty slot from the list of processable objects.
  for (int i=0; i<d->lstProcessableObjects.size(); ++i)
    if (d->lstProcessableObjects[i].first == 0)
      {
        d->lstProcessableObjects[i] = qMakePair(activeThreadId, d->varProcessableObject);
        d->varProcessableObject = PiiVariant();
        return;
      }
  // No empty slots found -> add a new one
  d->lstProcessableObjects.append(qMakePair(activeThreadId, d->varProcessableObject));
  d->varProcessableObject = PiiVariant();
}

void PiiInputSocket::unassignFirstObject(Qt::HANDLE activeThreadId)
{
  PII_D;
  QMutexLocker lock(&d->firstObjectMutex);
  // Find the slot based on thread id and clear it.
  for (int i=0; i<d->lstProcessableObjects.size(); ++i)
    if (d->lstProcessableObjects[i].first == activeThreadId)
      {
        d->lstProcessableObjects[i] = qMakePair(Qt::HANDLE(0), PiiVariant());
        return;
      }
}

void PiiInputSocket::jump(int oldIndex, int newIndex)
{
  PII_D;
  PiiVariant tmpObj = queuedObject(oldIndex);
  for (int i=oldIndex-1; i>=newIndex; --i)
    d->lstQueue[queueIndex(i+1)] = d->lstQueue[queueIndex(i)];
  d->lstQueue[queueIndex(newIndex)] = tmpObj;
}

int PiiInputSocket::indexOf(unsigned int type, int startIndex) const
{
  const PII_D;
  for (int i=startIndex; i<d->iQueueLength; ++i)
    {
      int iQueueIndex = queueIndex(i);
      if (d->lstQueue[iQueueIndex].type() == type)
        return i;
    }
  return -1;
}

void PiiInputSocket::reset()
{
  PII_D;
  for (int i=0; i<d->lstQueue.size(); ++i)
    d->lstQueue[i] = PiiVariant();
  d->varProcessableObject = PiiVariant();
  d->lstProcessableObjects.clear();
  d->iQueueLength = 0;
  d->iQueueStart = 0;
}

void PiiInputSocket::setController(PiiInputController* controller)
{
  PII_D;
  d->pController = controller;
  if (d->pConnectedOutput != 0)
    d->pConnectedOutput->updateInput(this);
}

PiiVariant PiiInputSocket::firstObject() const
{
  const PII_D;
  QMutexLocker lock(&d->firstObjectMutex);
  // If objects are not reserved to specific threads, return the
  // single processable object.
  if (d->lstProcessableObjects.isEmpty())
    return d->varProcessableObject;

  // Otherwise look for the thread id
  Qt::HANDLE currentThreadId = QThread::currentThreadId();
  for (int i=0; i<d->lstProcessableObjects.size(); ++i)
    if (d->lstProcessableObjects[i].first == currentThreadId)
      return d->lstProcessableObjects[i].second;

  // The calling thread is not assigned an object.
  return d->varProcessableObject;
}


PiiInputController* PiiInputSocket::controller() const { return _d()->pController; }
PiiInputSocket* PiiInputSocket::socket() { return this; }
PiiAbstractInputSocket* PiiInputSocket::asInput() { return this; }
PiiAbstractOutputSocket* PiiInputSocket::asOutput() { return 0; }
PiiVariant PiiInputSocket::queuedObject(int index) const { return _d()->lstQueue[queueIndex(index)]; }
unsigned int PiiInputSocket::queuedType(int index) const { return _d()->lstQueue[queueIndex(index)].type(); }
int PiiInputSocket::queueLength() const { return _d()->iQueueLength; }
int PiiInputSocket::queueCapacity() const { return _d()->lstQueue.size(); }
bool PiiInputSocket::canReceive() const { return _d()->lstQueue.size() > _d()->iQueueLength; }
void PiiInputSocket::setOptional(bool optional) { _d()->bOptional = optional; }
bool PiiInputSocket::isOptional() const { return _d()->bOptional; }


namespace PiiYdin
{
  QString unknownTypeErrorMessage(PiiInputSocket* input)
  {
    return QCoreApplication::translate("PiiInputSocket", "An object of an unknown type (0x%1) was received in \"%2\" input.")
      .arg(input->firstObject().type(), 0, 16)
      .arg(input->objectName());
  }

  QString wrongSizeErrorMessage(PiiInputSocket* input, const QString& received, const QString& assumed)
  {
    return QCoreApplication::translate("PiiInputSocket", "The matrix received in \"%1\" is %2, but %3 was assumed.")
      .arg(input->objectName()).arg(received).arg(assumed);
  }
}
