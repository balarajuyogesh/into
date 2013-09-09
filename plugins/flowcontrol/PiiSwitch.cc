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

#include "PiiSwitch.h"
#include <PiiYdinTypes.h>

PiiSwitch::Data::Data() :
  operationMode(SynchronousMode),
  iTriggerCount(0),
  bPassThrough(false)
{
}

PiiSwitch::PiiSwitch() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("trigger"));

  _d()->iStaticInputCount = inputCount();
  setDynamicInputCount(1);

  setProtectionLevel("dynamicInputCount", WriteWhenStoppedOrPaused);
  setProtectionLevel("operationMode", WriteWhenStoppedOrPaused);
}

PiiInputSocket* PiiSwitch::input(const QString &name) const
{
  if (name == "input")
    return PiiDefaultOperation::input("input0");
  return PiiDefaultOperation::input(name);
}

PiiOutputSocket* PiiSwitch::output(const QString &name) const
{
  if (name == "output")
    return PiiDefaultOperation::output("output0");
  return PiiDefaultOperation::output(name);
}

void PiiSwitch::setDynamicInputCount(int inputCount)
{
  setNumberedInputs(inputCount,1);
  setNumberedOutputs(inputCount);
}

void PiiSwitch::check(bool reset)
{
  PII_D;

  // In AsynchronousMode, trigger input is not synchronized to the
  // others.
  int iGroupId = d->operationMode == SynchronousMode ? 0 : -1;
  inputAt(0)->setGroupId(iGroupId);
  for (int i=0; i<outputCount(); ++i)
    outputAt(i)->setGroupId(iGroupId);

  inputAt(0)->setOptional(d->operationMode == AsynchronousMode);

  if (reset)
    {
      d->iTriggerCount = d->bPassThrough ? 1 : 0;
      d->lstObjects.clear();
    }

  PiiDefaultOperation::check(reset);
}

void PiiSwitch::aboutToChangeState(State state)
{
  if (state == Stopped)
    _d()->lstObjects.clear();
}

void PiiSwitch::emitInputObjects()
{
  PII_D;
  for (int i=d->iStaticInputCount; i<inputCount(); ++i)
    // Pass the incoming object to the peer output
    emitObject(readInput(i), i - d->iStaticInputCount);
}

void PiiSwitch::emitObjectList()
{
  PII_D;
  for (int i=0; i<d->lstObjects.size(); ++i)
    emitObject(d->lstObjects[i], i);
}

void PiiSwitch::storeInputObjects()
{
  PII_D;
  d->lstObjects.clear();
  for (int i=d->iStaticInputCount; i<inputCount(); ++i)
    d->lstObjects << readInput(i);
}

void PiiSwitch::process()
{
  PII_D;
  if (d->operationMode == SynchronousMode)
    {
      if (PiiYdin::primitiveAs<int>(inputAt(0)))
        emitInputObjects();
    }
  else
    {
      if (activeInputGroup() == -1) // trigger input
        {
          //if (!d->bPassThrough)
          //  {
              if (d->lstObjects.size() > 0)
                emitObjectList();
              else
                ++d->iTriggerCount;
              //  }
        }
      else // dynamic inputs
        {
          storeInputObjects();
          if (d->iTriggerCount > 0)
            {
              for (; d->iTriggerCount > 0; --d->iTriggerCount)
                emitObjectList();
              if (d->bPassThrough)
                d->iTriggerCount = 1;
            }
        }
    }
}

void PiiSwitch::setOperationMode(OperationMode operationMode) { _d()->operationMode = operationMode; }
PiiSwitch::OperationMode PiiSwitch::operationMode() const { return _d()->operationMode; }
int PiiSwitch::dynamicInputCount() const { return _d()->lstInputs.size() - _d()->iStaticInputCount; }
void PiiSwitch::setPassThrough(bool passThrough)
{
  PII_D;
  d->bPassThrough = passThrough;
  if (passThrough)
    d->iTriggerCount = 1;
}
bool PiiSwitch::passThrough() const { return _d()->bPassThrough; }
