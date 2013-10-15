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

#include "PiiBasicOperation.h"
#include "PiiYdinTypes.h"

PiiBasicOperation::Data::Data() :
  state(PiiOperation::Stopped)
{
}

PiiBasicOperation::Data::~Data()
{
}

PiiBasicOperation::PiiBasicOperation() :
  PiiOperation(new Data)
{
}

PiiBasicOperation::PiiBasicOperation(Data* data) :
  PiiOperation(data)
{
}

PiiBasicOperation::~PiiBasicOperation()
{
  if (_d()->state != Stopped)
    piiCritical("Deleting %s (objectName %s) while it is still running.",
                metaObject()->className(), piiPrintable(objectName()));
}

void PiiBasicOperation::check(bool reset)
{
  PII_D;

  if (parent() == 0)
    piiWarning(tr("%1 (objectName %2) has no parent.")
               .arg(metaObject()->className())
               .arg(objectName()));
  
  for (int i=d->lstInputs.size(); i--; )
    {
      if (!d->lstInputs[i]->isOptional() &&
          !d->lstInputs[i]->isConnected())
        PII_THROW(PiiExecutionException, tr("Input \"%1\" of %2 (objectName %3) is required but not connected.")
                  .arg(d->lstInputs[i]->objectName())
                  .arg(metaObject()->className())
                  .arg(objectName()));

      if (reset)
        d->lstInputs[i]->reset();
    }
  
  for (int i=d->lstOutputs.size(); i--; )
    d->lstOutputs[i]->reset();
}

void PiiBasicOperation::interruptOutputs()
{
  PII_D;
  // Interrupt all pending emissions
  for (int i=d->lstOutputs.size(); i--; )
    d->lstOutputs[i]->interrupt();
}

int PiiBasicOperation::inputCount() const
{
  return _d()->lstInputs.size();
}

QList<PiiAbstractInputSocket*> PiiBasicOperation::inputs() const
{
  const PII_D;
  QList<PiiAbstractInputSocket*> lstResult;
  for (int i=0; i<d->lstInputs.size(); ++i)
    lstResult << d->lstInputs[i];
  return lstResult;
}

QList<PiiInputSocket*> PiiBasicOperation::inputSockets() const
{
  return _d()->lstInputs;
}

int PiiBasicOperation::outputCount() const
{
  return _d()->lstOutputs.size();
}

QList<PiiAbstractOutputSocket*> PiiBasicOperation::outputs() const
{
  const PII_D;
  QList<PiiAbstractOutputSocket*> lstResult;
  for (int i=0; i<d->lstOutputs.size(); ++i)
    lstResult << d->lstOutputs[i];
  return lstResult;
}

bool PiiBasicOperation::wait(unsigned long) { return true; }

QList<PiiOutputSocket*> PiiBasicOperation::outputSockets() const
{
  return _d()->lstOutputs;
}

PiiInputSocket* PiiBasicOperation::input(const QString& name) const
{
  return findChild<PiiInputSocket*>(name);
}

PiiInputSocket* PiiBasicOperation::inputAt(int index) const
{
  return _d()->lstInputs[index];
}

PiiOutputSocket* PiiBasicOperation::output(const QString& name) const
{
  return findChild<PiiOutputSocket*>(name);
}

PiiOutputSocket* PiiBasicOperation::outputAt(int index) const
{
  return _d()->lstOutputs[index];
}

PiiVariant PiiBasicOperation::readInput(int index) const
{
  return _d()->lstInputs[index]->firstObject();
}

PiiVariant PiiBasicOperation::readInput() const
{
  // PENDING could save the first input into an extra member variable
  return _d()->lstInputs[0]->firstObject();
}

void PiiBasicOperation::insertInput(PiiInputSocket* socket, int index)
{
  PII_D;
  if (d->lstInputs.contains(socket))
    {
      piiWarning(tr("Trying to add an input socket twice."));
      return;
    }
  //qDebug("Adding %s input to %s", qPrintable(socket->objectName()), metaObject()->className());
  socket->setParent(this);
  if (index == -1)
    d->lstInputs.append(socket);
  else
    d->lstInputs.insert(index, socket);
}

void PiiBasicOperation::insertOutput(PiiOutputSocket* socket, int index)
{
  PII_D;
  if (d->lstOutputs.contains(socket))
    {
      piiWarning(tr("Trying to add an output socket twice."));
      return;
    }
  //qDebug("Adding %s output to %s", qPrintable(socket->objectName()), metaObject()->className());
  socket->setParent(this);
  if (index == -1)
    d->lstOutputs.append(socket);
  else
    d->lstOutputs.insert(index, socket);
}

void PiiBasicOperation::addSocket(PiiInputSocket* socket)
{
  insertInput(socket, -1);
}

void PiiBasicOperation::addSocket(PiiOutputSocket* socket)
{
  insertOutput(socket, -1);
}

void PiiBasicOperation::removeSocket(PiiInputSocket* socket)
{
  removeInput(_d()->lstInputs.indexOf(socket));
}

void PiiBasicOperation::removeSocket(PiiOutputSocket* socket)
{
  removeOutput(_d()->lstOutputs.indexOf(socket));
}

PiiInputSocket* PiiBasicOperation::removeInput(int index)
{
  PII_D;
  if (index >= 0 && index < d->lstInputs.size())
    {
      PiiInputSocket* socket = d->lstInputs.takeAt(index);
      socket->setParent(0);
      return socket;
    }
  return 0;
}

PiiOutputSocket* PiiBasicOperation::removeOutput(int index)
{
  PII_D;
  if (index >= 0 && index < d->lstOutputs.size())
    {
      PiiOutputSocket* socket = d->lstOutputs.takeAt(index);
      socket->setParent(0);
      return socket;
    }
  return 0;
}

void PiiBasicOperation::setState(State state)
{
  PII_D;
  if (d->state != state)
    {
      aboutToChangeState(state);
      d->state = state;
      emit stateChanged(state);
    }
}

void PiiBasicOperation::aboutToChangeState(State /*newState*/)
{}

PiiOperation::State PiiBasicOperation::state() const
{
  return _d()->state;
}

void PiiBasicOperation::operationStopped()
{
  sendTag(PiiYdin::createStopTag());
  throw PiiExecutionException(PiiExecutionException::Finished);
}

void PiiBasicOperation::operationPaused()
{
  sendTag(PiiYdin::createPauseTag());
  throw PiiExecutionException(PiiExecutionException::Paused);
}

void PiiBasicOperation::operationResumed()
{
  PII_D;
  // Send resume tag to all outputs
  for (int i=d->lstOutputs.size(); i--; )
    {
      int iMaxDelay = 0, iFlowLevel = 0;
      // Look for inputs with the same group id
      for (int j=d->lstInputs.size(); j--; )
        {
          // We found one. Great.
          if (d->lstInputs[j]->isConnected() &&
              d->lstInputs[j]->groupId() == d->lstOutputs[i]->groupId())
            {
              // It must contain a resume tag.
              PiiSocketState state = d->lstInputs[j]->firstObject().valueAs<PiiSocketState>();
              // Find maximum delay in this group of inputs.
              int iDelay = QATOMICINT_LOAD(state.delay);
              if (iDelay > iMaxDelay)
                iMaxDelay = iDelay;
              // The flow levels should all be the same...
              iFlowLevel = QATOMICINT_LOAD(state.flowLevel);
            }
        }

      // Now we know the state of the synchronized input group. Resume
      // the output.
      d->lstOutputs[i]->resume(PiiSocketState(iFlowLevel, iMaxDelay));
    }
}

void PiiBasicOperation::sendTag(const PiiVariant& tag)
{
  PII_D;
  for (int i=d->lstOutputs.size(); i--; )
    d->lstOutputs[i]->emitObject(tag);
}

void PiiBasicOperation::emitObject(const PiiVariant& obj, int index)
{
  _d()->lstOutputs[index]->emitObject(obj);
}

template <class T> void PiiBasicOperation::setNumberedSockets(QList<T*>& sockets,
                                                              int count,
                                                              int staticSockets,
                                                              const QString& prefix,
                                                              int firstIndex)
{
  if (count < 0) return;
  
  //Do we need to add new inputs?
  while (sockets.size() - staticSockets < count)
    {
      QString name = QString("%1%2").arg(prefix).arg(sockets.size() - staticSockets + firstIndex);
      addSocket(new T(name));
    }

  //Do we need to remove some inputs?
  while (sockets.size() - staticSockets > count)
    delete sockets.takeLast();
}

void PiiBasicOperation::setNumberedInputs(int count, int staticInputs, const QString& prefix, int firstIndex)
{
  setNumberedSockets(_d()->lstInputs, count, staticInputs, prefix, firstIndex);
}

void PiiBasicOperation::setNumberedOutputs(int count, int staticOutputs, const QString& prefix, int firstIndex)
{
  setNumberedSockets(_d()->lstOutputs, count, staticOutputs, prefix, firstIndex);
}

bool PiiBasicOperation::hasConnectedInputs() const
{
  const PII_D;
  for (int i=d->lstInputs.size(); i--; )
    if (d->lstInputs[i]->isConnected())
      return true;
  return false;
}

void PiiBasicOperation::startMany(int firstOutput, int lastOutput)
{
  PII_D;
  if (lastOutput < 0) lastOutput = d->lstOutputs.size() - 1;
  while (firstOutput <= lastOutput)
    d->lstOutputs[firstOutput++]->startMany();
}

void PiiBasicOperation::endMany(int firstOutput, int lastOutput)
{
  PII_D;
  if (lastOutput < 0) lastOutput = d->lstOutputs.size() - 1;
  while (firstOutput <= lastOutput)
    d->lstOutputs[firstOutput++]->endMany();
}

void PiiBasicOperation::applyPropertySet(const QString& name)
{
  PiiOperation::applyPropertySet(name);
  sendTag(PiiYdin::createReconfigurationTag(name));
}
