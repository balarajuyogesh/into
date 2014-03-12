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

#include "PiiDistributionNormalizer.h"
#include <PiiYdinTypes.h>
#include <PiiMath.h>

PiiDistributionNormalizer::Data::Data() :
  bDoubleMode(false)
{
}

PiiDistributionNormalizer::PiiDistributionNormalizer() :
  PiiDefaultOperation(new Data)
{
  PII_D;
  addSocket(d->pFeatureInput = new PiiInputSocket("features"));
  addSocket(d->pBoundaryInput = new PiiInputSocket("boundaries"));
  addSocket(d->pFeatureOutput = new PiiOutputSocket("features"));

  d->pBoundaryInput->setOptional(true);

}

void PiiDistributionNormalizer::process()
{
  PII_D;
  PiiVariant obj = d->pFeatureInput->firstObject();

  switch (obj.type())
    {
      PII_NUMERIC_MATRIX_CASES(normalize, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiDistributionNormalizer::normalize(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> features = obj.valueAs<PiiMatrix<T> >();

  if (!d->pBoundaryInput->isConnected())
    {
      // Normalize the whole vector at once
      if (d->bDoubleMode)
        {
          PiiMatrix<double> matResult(features);
          double sum = Pii::sum<double>(matResult);
          if (sum != 0)
            matResult /= sum;
          d->pFeatureOutput->emitObject(matResult);
        }
      else
        {
          PiiMatrix<float> matResult(features);
          float sum = Pii::sum<float>(matResult);
          if (sum != 0)
            matResult /= sum;
          d->pFeatureOutput->emitObject(matResult);
        }
    }
  else
    {
      PiiVariant obj = d->pBoundaryInput->firstObject();
      if (obj.type() == PiiYdin::IntMatrixType)
        {
          const PiiMatrix<int>& boundaries = obj.valueAs<PiiMatrix<int> >();
          if (d->bDoubleMode)
            normalizePieces<double>(features, boundaries);
          else
            normalizePieces<float>(features, boundaries);
        }
    }
}

template <class U, class T> void PiiDistributionNormalizer::normalizePieces(const PiiMatrix<T>& vector,
                                                                            const PiiMatrix<int>& boundaries)

{
  PII_D;
  PiiMatrix<U> matResult(vector);
  int start = 0;
  for (int i=0; i<boundaries.columns() && start < vector.columns(); i++)
    {
      int end = Pii::min(boundaries(i), vector.columns());
      U sum = Pii::sum<U>(vector(0, start, 1, end-start));
      if (sum != 0)
        matResult(0, start, 1, end-start) /= sum;
      start = end;
    }
  d->pFeatureOutput->emitObject(matResult);
}

bool PiiDistributionNormalizer::doubleMode() const { return _d()->bDoubleMode; }
void PiiDistributionNormalizer::setDoubleMode(bool mode) { _d()->bDoubleMode = mode; }
