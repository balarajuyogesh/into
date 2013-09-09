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

#include "PiiConstantOperation.h"

#include <PiiYdinTypes.h>

PiiConstantOperation::Data::Data() :
  value(0)
{}

PiiConstantOperation::PiiConstantOperation() : PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("trigger"));
  addSocket(new PiiOutputSocket("value"));
}

void PiiConstantOperation::process()
{
  emitObject(_d()->value);
}

void PiiConstantOperation::setValue(const PiiVariant& value) { _d()->value = value; }
PiiVariant PiiConstantOperation::value() const { return _d()->value; }
