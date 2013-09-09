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

#include "PiiLogOperation.h"

#include <PiiMath.h>
#include <PiiYdinTypes.h>

PiiLogOperation::Data::Data() :
  dBase(M_E), dOnePerLogBase(1.0)
{
}

PiiLogOperation::PiiLogOperation() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
}

void PiiLogOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_NUMERIC_CASES(logarithm, obj);
      PII_NUMERIC_MATRIX_CASES(matrixLogarithm, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

void PiiLogOperation::setBase(double base)
{
  PII_D;
  if (base > 0)
    {
      d->dBase = base;
      d->dOnePerLogBase = 1.0/std::log(base);
    }
}

template <class T> void PiiLogOperation::logarithm(const PiiVariant& obj)
{
  PII_D;
  if (d->dBase == M_E)
    emitObject(Pii::log(obj.valueAs<T>()));
  else
    emitObject(Pii::log(obj.valueAs<T>())*d->dOnePerLogBase);
}

template <class T> void PiiLogOperation::matrixLogarithm(const PiiVariant& obj)
{
  PII_D;
  if (d->dBase == M_E)
    emitObject(Pii::log(obj.valueAs<PiiMatrix<T> >()));
  else
    {
      PiiMatrix<double> mat(Pii::log(obj.valueAs<PiiMatrix<T> >()));
      mat *= d->dOnePerLogBase;
      emitObject(mat);
    }
}

double PiiLogOperation::base() const { return _d()->dBase; }
