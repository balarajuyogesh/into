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

#include "PiiObjectCounter.h"

#include <PiiYdinTypes.h>

PiiObjectCounter::Data::Data() :
  iCount(0), bAutoReset(true)
{
}

PiiObjectCounter::PiiObjectCounter() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("count"));
}

void PiiObjectCounter::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);
  if (reset && d->bAutoReset)
    d->iCount = 0;
}

void PiiObjectCounter::process()
{
  PII_D;
  d->lstOutputs[0]->emitObject(++d->iCount);
}


void PiiObjectCounter::setCount(int count) { _d()->iCount = count; }
int PiiObjectCounter::count() const { return _d()->iCount; }
void PiiObjectCounter::setAutoReset(bool autoReset) { _d()->bAutoReset = autoReset; }
bool PiiObjectCounter::autoReset() const { return _d()->bAutoReset; }
