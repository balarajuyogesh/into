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

#include "PiiOneGroupFlowController.h"
#include "PiiOutputSocket.h"

PiiOneGroupFlowController::Data::Data(const QList<PiiInputSocket*>& inputs,
                                      const QList<PiiOutputSocket*>& outputs)
{
  // Store connected inputs
  for (int i=0; i<inputs.size(); ++i)
    if (inputs[i]->isConnected())
      vecInputs << inputs[i];

  // Store connected outputs
  for (int i=0; i<outputs.size(); ++i)
    if (outputs[i]->isConnected() && outputs[i]->groupId() == vecInputs[0]->groupId())
      vecOutputs << outputs[i];

  if (vecInputs.size() > 0)
    iActiveInputGroup = vecInputs[0]->groupId();
}

PiiOneGroupFlowController::PiiOneGroupFlowController(const QList<PiiInputSocket*>& inputs,
                                                     const QList<PiiOutputSocket*>& outputs) :
  PiiFlowController(new Data(inputs, outputs))
{
}

void PiiOneGroupFlowController::shiftInputs()
{
  PII_D;
  for (int i=0; i<d->vecInputs.size(); ++i)
    d->vecInputs[i]->shift();
}

PiiFlowController::FlowState PiiOneGroupFlowController::prepareProcess()
{
  PII_D;
  int typeMask = inputGroupTypeMask(d->vecInputs.begin(), d->vecInputs.end());

  switch (typeMask)
    {
    case NormalObject:
      // All objects are there -> shift sockets and process
      shiftInputs();
      return ProcessableState;

    case NoObject:
      // Not all objects are there
      return IncompleteState;

    case EndTag:
    case StartTag:
      {
        // Pass the tag
        const PiiVariant& obj = d->vecInputs[0]->queuedObject(0);
        for (int i=0; i<d->vecOutputs.size(); ++i)
          d->vecOutputs[i]->emitObject(obj);
        shiftInputs();
        return SynchronizedState;
      }

    case ReconfigurationTag:
      // Take the name of the property set from the first object
      // (assuming all are the same).
      setPropertySetName(d->vecInputs[0]->queuedObject(0).valueAs<QString>());
      shiftInputs();
      return ReconfigurableState;

    case StopTag:
      shiftInputs();
      return FinishedState;

    case PauseTag:
      shiftInputs();
      return PausedState;

    case ResumeTag:
      shiftInputs();
      return ResumedState;

    default:
      // Oops
      PII_THROW(PiiExecutionException,
                tr("Synchronization error: inputs are in inconsistent state (type mask 0x%2).\n").arg(typeMask, 0, 16) +
                dumpInputObjects(d->vecInputs.begin(), d->vecInputs.end()));
    }
}
