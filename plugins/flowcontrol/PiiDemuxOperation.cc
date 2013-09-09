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

#include "PiiDemuxOperation.h"
#include <PiiYdinTypes.h>

PiiDemuxOperation::PiiDemuxOperation() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("control"));
  addSocket(new PiiInputSocket("input"));
  setDynamicOutputCount(2);
}

void PiiDemuxOperation::setDynamicOutputCount(int cnt)
{
  if (cnt < 1) return;
  setNumberedOutputs(cnt);
}

int PiiDemuxOperation::dynamicOutputCount() const { return outputCount(); }

void PiiDemuxOperation::process()
{
  int iCtrl = PiiYdin::primitiveAs<int>(input(0));
  if (iCtrl < 0 || iCtrl >= outputCount())
    PII_THROW(PiiExecutionException, tr("Control input value (%1) is out of range (0-%2).").arg(iCtrl).arg(outputCount()-1));

  // Pass the incoming data to the output selected by the control input
  emitObject(readInput(1), iCtrl);
}
