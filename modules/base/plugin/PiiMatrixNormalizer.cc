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

#include "PiiMatrixNormalizer.h"
#include <PiiMath.h>
#include <PiiYdinTypes.h>

PiiMatrixNormalizer::Data::Data() :
  dMin(0.0), dMax(1.0), dMean(0.0), dVariance(0.0),
  iOutputType(PiiYdin::DoubleMatrixType)
{
}

PiiMatrixNormalizer::PiiMatrixNormalizer() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
}

void PiiMatrixNormalizer::check(bool reset)
{
  PiiDefaultOperation::check(reset);
  PII_D;
  if (d->iOutputType < PiiYdin::CharMatrixType ||
      d->iOutputType > PiiYdin::DoubleMatrixType)
    throwOutputTypeError();
}

void PiiMatrixNormalizer::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_NUMERIC_MATRIX_CASES(normalize, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiMatrixNormalizer::normalize(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> mat = obj.valueAs<PiiMatrix<T> >();

  double scale = 1.0, preShift = 0.0, postShift = 0.0;
  if (d->normalizationMode == NormalizeMinMax)
    {
      T minimum(0);
      T maximum(0);
      Pii::minMax(mat, &minimum, &maximum);

      preShift = -double(minimum);
      if (minimum != maximum)
        scale = (d->dMax - d->dMin)/(maximum-minimum);
      postShift = d->dMin;
    }
  else
    {
      if (d->dVariance != 0)
        {
          double mean;
          double variance = Pii::var<double>(mat, &mean);
          preShift = -mean;
          scale = d->dVariance/variance;
        }
      else
        preShift = -Pii::mean<double>(mat);

      postShift = d->dMean;
    }

  PiiMatrix<double> matNormalized(normalizeAs<double>(mat, preShift, scale, postShift));
  switch (d->iOutputType)
    {
      PII_NUMERIC_MATRIX_CASES(emitMatrix, matNormalized);
    default:
      throwOutputTypeError();
    }
}

template <class T> PiiMatrix<double> PiiMatrixNormalizer::normalizeAs(const PiiMatrix<T>& matrix,
                                                                      double preShift,
                                                                      double scale,
                                                                      double postShift)
{
  PiiMatrix<double> matResult(matrix);
  if (preShift != 0)
    matResult += preShift;
  if (scale != 1.0)
    matResult *= scale;
  if (postShift != 0)
    matResult += postShift;
  return matResult;
}

void PiiMatrixNormalizer::setMin(double min) { _d()->dMin = min; }
double PiiMatrixNormalizer::min() const { return _d()->dMin; }
void PiiMatrixNormalizer::setMax(double max) { _d()->dMax = max; }
double PiiMatrixNormalizer::max() const { return _d()->dMax; }
void PiiMatrixNormalizer::setOutputType(int outputType) { _d()->iOutputType = outputType; }
int PiiMatrixNormalizer::outputType() const { return _d()->iOutputType; }
void PiiMatrixNormalizer::setMean(double mean) { _d()->dMean = mean; }
double PiiMatrixNormalizer::mean() const { return _d()->dMean; }
void PiiMatrixNormalizer::setVariance(double variance) { _d()->dVariance = variance; }
double PiiMatrixNormalizer::variance() const { return _d()->dVariance; }
void PiiMatrixNormalizer::setNormalizationMode(NormalizationMode normalizationMode) { _d()->normalizationMode = normalizationMode; }
PiiMatrixNormalizer::NormalizationMode PiiMatrixNormalizer::normalizationMode() const { return _d()->normalizationMode; }
