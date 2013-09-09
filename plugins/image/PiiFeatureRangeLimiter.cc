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

#include "PiiFeatureRangeLimiter.h"
#include <PiiYdinTypes.h>

PiiFeatureRangeLimiter::Data::Data() :
  dLowerLimit(-1.0),
  dUpperLimit(-1.0),
  isLowerSelected(false),
  isUpperSelected(false),
  iRows(0)
{
}

PiiFeatureRangeLimiter::PiiFeatureRangeLimiter() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiOutputSocket("count"));
  setDynamicInputCount(1);
}

void PiiFeatureRangeLimiter::setDynamicInputCount(int cnt)
{
  setNumberedInputs(cnt, 0,"feature");
  setNumberedOutputs(cnt, 1, "feature");
}

void PiiFeatureRangeLimiter::process()
{
  PiiVariant obj = readInput();

  switch(obj.type())
    {
      PII_NUMERIC_MATRIX_CASES(calculateLimiterMatrix, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }

  for (int i=0; i<inputCount(); i++)
    {
      if (outputAt(i+1)->isConnected())
        {
          PiiVariant obj = inputAt(i)->firstObject();
          switch(obj.type())
            {
              PII_NUMERIC_MATRIX_CASES_M(operateMatrix, (obj, i+1));
            default:
              PII_THROW_UNKNOWN_TYPE(inputAt(i));
            }
        }
    }

}

template <class T> void PiiFeatureRangeLimiter::calculateLimiterMatrix(const PiiVariant& obj)
{
  PII_D;
  PiiMatrix<T> mat = obj.valueAs<PiiMatrix<T> >();
  d->lstRows.clear();
  d->iRows = mat.rows();
  
  for (int i=0; i<d->iRows; ++i)
    if ((!d->isLowerSelected || mat(i,0) >= d->dLowerLimit) &&
        (!d->isUpperSelected || mat(i,0) <= d->dUpperLimit))
      d->lstRows.append(i);
  
  emitObject(d->lstRows.count());
}

template <class T> void PiiFeatureRangeLimiter::operateMatrix(const PiiVariant& obj, int outputIndex)
{
  PII_D;
  const PiiMatrix<T>& mat = obj.valueAs<PiiMatrix<T> >();
  
  if (mat.rows() != d->iRows)
    PII_THROW_WRONG_SIZE(inputAt(outputIndex-1), mat, d->lstRows.size(), mat.columns());
              
  PiiMatrix<T> matResult(d->lstRows.size(), mat.columns());
  for (int r=0; r<d->lstRows.size(); r++)
    for (int c=0; c<mat.columns(); c++)
      matResult(r,c) = mat(d->lstRows.at(r),c);
  
  outputAt(outputIndex)->emitObject(matResult);
}
        

double PiiFeatureRangeLimiter::lowerLimit() const { return _d()->dLowerLimit; }
void PiiFeatureRangeLimiter::setLowerLimit(double lowerLimit) { _d()->isLowerSelected = true; _d()->dLowerLimit = lowerLimit; }
double PiiFeatureRangeLimiter::upperLimit() const { return _d()->dUpperLimit; }
void PiiFeatureRangeLimiter::setUpperLimit(double upperLimit) { _d()->isUpperSelected = true; _d()->dUpperLimit = upperLimit; }
int PiiFeatureRangeLimiter::dynamicInputCount() const { return PiiDefaultOperation::inputCount(); }
