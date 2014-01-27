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

#include <PiiAbsoluteOperation.h>
#include <PiiYdinTypes.h>
#include <PiiMath.h>

using namespace Pii;
using namespace PiiYdin;

PiiAbsoluteOperation::PiiAbsoluteOperation() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
}

void PiiAbsoluteOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_NUMERIC_CASES(operatePrimitive, obj);
      PII_COMPLEX_CASES(operatePrimitive, obj);
      PII_NUMERIC_MATRIX_CASES(operateMatrix, obj);
      PII_COMPLEX_MATRIX_CASES(operateMatrix, obj);
    default:
      emitObject(obj);
    }
}

template <class T> void PiiAbsoluteOperation::operatePrimitive( const PiiVariant& obj )
{
  emitObject(Pii::abs(obj.valueAs<T>()));
}


template <class T> void PiiAbsoluteOperation::operateMatrix( const PiiVariant& obj )
{
  emitObject(Pii::abs(obj.valueAs<PiiMatrix<T> >()));
}
