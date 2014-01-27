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

#include "PiiSequenceGenerator.h"

#include <PiiYdinTypes.h>

PiiSequenceGenerator::Data::Data() :
  dSequenceStart(0), dSequenceEnd(1), dStep(1),
  sequenceMode(Repeat), iRepeatCount(0), bIntegerOutput(false)
{
}

PiiSequenceGenerator::PiiSequenceGenerator() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("trigger"));
  inputAt(0)->setOptional(true);
  addSocket(new PiiOutputSocket("output"));
}

void PiiSequenceGenerator::check(bool reset)
{
  setThreadCount(inputAt(0)->isConnected() ? 0 : 1);
  PiiDefaultOperation::check(reset);

  if (reset)
    {
      PII_D;
      if (d->dSequenceStart > d->dSequenceEnd)
        PII_THROW(PiiExecutionException, tr("Sequence start (%1) is greater than sequence end (%2).").arg(d->dSequenceStart).arg(d->dSequenceEnd));
      if (d->dStep > d->dSequenceEnd - d->dSequenceStart)
        PII_THROW(PiiExecutionException, tr("Sequence step (%1) is larger than sequence length (%2).").arg(d->dStep).arg(d->dSequenceEnd-d->dSequenceStart));

      d->dCurrentValue = d->dStep < 0 ? d->dSequenceEnd : d->dSequenceStart;
      d->dCurrentStep = d->dStep;
      d->iCurrentRepeatCount = 0;
      d->bAutoExit = !inputAt(0)->isConnected() && d->iRepeatCount > 0;
    }
}

void PiiSequenceGenerator::process()
{
  PII_D;
  //qDebug("%s emitting %lf", qPrintable(objectName()), d->dCurrentValue);
  if (d->bIntegerOutput)
    emitObject(int(d->dCurrentValue));
  else
    emitObject(d->dCurrentValue);

  d->dCurrentValue += d->dCurrentStep;

  // We are going down and the bottom was hit OR
  // We are going up and the ceiling was hit
  if ((d->dCurrentStep < 0 && d->dCurrentValue < d->dSequenceStart) ||
      (d->dCurrentStep > 0 && d->dCurrentValue > d->dSequenceEnd))
    {
      // Do we need to bail out?
      if (d->bAutoExit &&
          ++d->iCurrentRepeatCount >= d->iRepeatCount)
        operationStopped();

      if (d->sequenceMode == Repeat) // go back to the other end
        d->dCurrentValue = d->dCurrentStep > 0 ? d->dSequenceStart : d->dSequenceEnd;
      else // reverse direction
        {
          d->dCurrentStep = -d->dCurrentStep;
          d->dCurrentValue += d->dCurrentStep;
        }
    }
}


void PiiSequenceGenerator::setSequenceStart(double sequenceStart) { _d()->dSequenceStart = sequenceStart; }
double PiiSequenceGenerator::sequenceStart() const { return _d()->dSequenceStart; }
void PiiSequenceGenerator::setSequenceEnd(double sequenceEnd) { _d()->dSequenceEnd = sequenceEnd; }
double PiiSequenceGenerator::sequenceEnd() const { return _d()->dSequenceEnd; }
void PiiSequenceGenerator::setStep(double step) { _d()->dStep = step; }
double PiiSequenceGenerator::step() const { return _d()->dStep; }
void PiiSequenceGenerator::setSequenceMode(const SequenceMode& sequenceMode) { _d()->sequenceMode = sequenceMode; }
PiiSequenceGenerator::SequenceMode PiiSequenceGenerator::sequenceMode() const { return _d()->sequenceMode; }
void PiiSequenceGenerator::setRepeatCount(int repeatCount) { _d()->iRepeatCount = repeatCount; }
int PiiSequenceGenerator::repeatCount() const { return _d()->iRepeatCount; }
void PiiSequenceGenerator::setIntegerOutput(bool integerOutput) { _d()->bIntegerOutput = integerOutput; }
bool PiiSequenceGenerator::integerOutput() const { return _d()->bIntegerOutput; }
