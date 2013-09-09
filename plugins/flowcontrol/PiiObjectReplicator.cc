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

#include "PiiObjectReplicator.h"
#include <PiiDefaultFlowController.h>

PiiObjectReplicator::PiiObjectReplicator() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("data"));
  addSocket(new PiiInputSocket("trigger"));
  inputAt(1)->setGroupId(1);
  addSocket(new PiiOutputSocket("output"));
  outputAt(0)->setGroupId(1);
}

PiiFlowController* PiiObjectReplicator::createFlowController()
{
  return new PiiDefaultFlowController(inputSockets(),
                                      outputSockets(),
                                      PiiDefaultFlowController::RelationList() <<
                                      PiiDefaultFlowController::strictRelation(0, 1));
}

void PiiObjectReplicator::process()
{
  PII_D;
  // If data input has an object, store it
  if (activeInputGroup() == 0)
    d->pData = readInput();
  // Otherwise it must be the trigger that was received
  else
    emitObject(d->pData);
}
