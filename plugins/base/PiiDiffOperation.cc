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

#include "PiiDiffOperation.h"
#include <PiiYdinTypes.h>
#include <complex>

PiiDiffOperation::Data::Data()
{
}

PiiDiffOperation::PiiDiffOperation() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("difference"));
}

void PiiDiffOperation::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);
  if (reset)
    d->lastObject = PiiVariant();
}

void PiiDiffOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_NUMERIC_CASES(diff, obj);
      PII_COMPLEX_CASES(diff, obj);
      PII_NUMERIC_MATRIX_CASES(matrixDiff, obj);
      PII_COMPLEX_MATRIX_CASES(matrixDiff, obj);

    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiDiffOperation::matrixDiff(const PiiVariant& obj)
{
  diff<PiiMatrix<T> >(obj);
}

template <class T> void PiiDiffOperation::diff(const PiiVariant& obj)
{
  PII_D;
  // If there is a previous object, emit the difference between this and it
  if (d->lastObject.isValid())
    {
      if (obj.type() != d->lastObject.type())
        PII_THROW(PiiExecutionException, tr("Cannot calculate the difference between objects of different type."));

      try
        {
          emitObject(obj.valueAs<T>() - d->lastObject.valueAs<T>());
        }
      catch (PiiMathException&)
        {
          PII_THROW(PiiExecutionException, tr("Cannot calculate the difference between matrices of different size."));
        }
    }
  // There is no previous object -> emit zero (subtract from itself)
  else
    emitObject(obj.valueAs<T>() - obj.valueAs<T>());

  d->lastObject = obj;
}
