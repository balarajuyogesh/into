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

#include "PiiFftOperation.h"

PiiFftOperation::Data::Data() :
  direction(Forward), bShift(false), bSubtractMean(false)
{
}

PiiFftOperation::PiiFftOperation(Data* data) :
  PiiDefaultOperation(data)
{
  setThreadCount(1);
  init();
}

PiiFftOperation::PiiFftOperation() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  init();
}

void PiiFftOperation::init()
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
  addSocket(new PiiOutputSocket("power spectrum"));
}


PiiFftOperation::FftDirection PiiFftOperation::direction() const { return _d()->direction; }
void PiiFftOperation::setDirection(FftDirection direction) { _d()->direction = direction; }
bool PiiFftOperation::shift() const { return _d()->bShift; }
void PiiFftOperation::setShift(bool shift) { _d()->bShift = shift; }

void PiiFftOperation::setSubtractMean(bool subtractMean) { _d()->bSubtractMean = subtractMean; }
bool PiiFftOperation::subtractMean() const { return _d()->bSubtractMean; }
