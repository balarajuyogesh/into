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

#include "PiiObjectRateChanger.h"

PiiObjectRateChanger::Data::Data() :
  iRateChange(0), iCounter(0)
{
}

PiiObjectRateChanger::PiiObjectRateChanger() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
}

void PiiObjectRateChanger::check(bool reset)
{
  PiiDefaultOperation::check(reset);
  if (reset)
    _d()->iCounter = 0;
}

void PiiObjectRateChanger::process()
{
  PII_D;
  PiiVariant obj = readInput();

  if (d->iRateChange >= 0)
    {
      for (int i=0; i<=d->iRateChange; ++i)
        emitObject(obj);
    }
  else
    {
      if (--d->iCounter < d->iRateChange)
        {
          emitObject(obj);
          d->iCounter = 0;
        }
    }
}

void PiiObjectRateChanger::setRateChange(int rateChange) { _d()->iRateChange = rateChange; }
int PiiObjectRateChanger::rateChange() const { return _d()->iRateChange; }
