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

#include "PiiOneInputFlowController.h"
#include "PiiOutputSocket.h"

PiiOneInputFlowController::Data::Data(PiiInputSocket* input,
                                      const QList<PiiOutputSocket*>& outputs) :
  pInput(input), iOutputCount(0)
{
  // Store synchronized outputs
  for (int i=0; i<outputs.size(); ++i)
    if (outputs[i]->groupId() == pInput->groupId())
      vecOutputs << outputs[i];

  iActiveInputGroup = input->groupId();
  iOutputCount = vecOutputs.size();
}

PiiOneInputFlowController::PiiOneInputFlowController(PiiInputSocket* input,
                                                     const QList<PiiOutputSocket*>& outputs) :
  PiiFlowController(new Data(input, outputs))
{
}

PiiFlowController::FlowState PiiOneInputFlowController::prepareProcess()
{
  PII_D;
  using namespace PiiYdin;
  unsigned int uiType = d->pInput->queuedType(0);
  //qDebug("PiiOneInputFlowController::prepareProcess(): queue length = %d, type = 0x%x", d->pInput->queueLength(), uiType);
  if (uiType == PiiVariant::InvalidType)
    return IncompleteState;
  // If the incoming object is an ordinary one
  else if (isNonControlType(uiType))
    {
      // The input is now free and we are ready to process
      d->pInput->shift();
      return ProcessableState;
    }
  else
    {
      switch (uiType)
        {
        case SynchronizationTagType:
          {
            const PiiVariant& tag = d->pInput->queuedObject(0);
            // Pass sync tags to all synchronized outputs
            for (int i=0; i<d->iOutputCount; ++i)
              d->vecOutputs[i]->emitObject(tag);
            d->pInput->shift();
            return SynchronizedState;
          }

        case ReconfigurationTagType:
          setPropertySetName(d->pInput->queuedObject(0).valueAs<QString>());
          d->pInput->shift();
          return ReconfigurableState;

        case StopTagType:
          d->pInput->shift();
          return FinishedState;

        case PauseTagType:
          d->pInput->shift();
          return PausedState;

        case ResumeTagType:
          d->pInput->shift();
          return ResumedState;

        default:
          PII_THROW_UNKNOWN_TYPE(d->pInput);
        }
    }
}
