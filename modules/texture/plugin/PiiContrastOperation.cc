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


#include "PiiContrastOperation.h"
#include <PiiYdinTypes.h>
#include <PiiMath.h>

PiiContrastOperation::Data::Data() :
  type(MaxDiff),
  iRadius(1)
{
}

PiiContrastOperation::PiiContrastOperation() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  PII_D;
  d->pImageInput = new PiiInputSocket("image");
  addSocket(d->pImageInput);

  d->pImageOutput = new PiiOutputSocket("image");
  addSocket(d->pImageOutput);

}

void PiiContrastOperation::process()
{
  PII_D;
  PiiVariant obj = d->pImageInput->firstObject();

  switch (obj.type())
    {
      PII_NUMERIC_MATRIX_CASES(contrast, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(d->pImageInput);
    }
}

template <class T> void PiiContrastOperation::contrast(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> image = obj.valueAs<PiiMatrix<T> >();
  int margin = d->iRadius, doubleMargin = margin << 1, windowSize = doubleMargin+1;

  if (image.rows() <= doubleMargin || image.columns() <= doubleMargin)
    PII_THROW(PiiExecutionException, tr("Input image is too small"));

  switch (d->type)
    {
    case MaxDiff:
      {
        PiiMatrix<T> matResult(PiiMatrix<T>::uninitialized(image.rows()-doubleMargin, image.columns()-doubleMargin));
        for (int r=margin; r<image.rows()-margin; r++)
          {
            const T* centerRow = image.row(r);
            T* resultRow = matResult.row(r-margin);
            for (int c=margin; c<image.columns()-margin; c++)
              {
                T center = centerRow[c];
                T maxDiff = 0;
                for (int neighborY=r-margin; neighborY<r+margin; neighborY++)
                  {
                    const T* neighborRow = image.row(neighborY);
                    for (int neighborX=c-margin; neighborX<c+margin; neighborX++)
                      {
                        T neighbor = neighborRow[neighborX];
                        T diff = Pii::abs(neighbor - center);
                        if (diff > maxDiff)
                          maxDiff = diff;
                      }
                  }
                resultRow[c-margin] = maxDiff;
              }
          }
        d->pImageOutput->emitObject(matResult);
      }
      break;
    case LocalVar:
      {
        PiiMatrix<float> matResult(PiiMatrix<float>::uninitialized(image.rows()-doubleMargin,
                                                                   image.columns()-doubleMargin));
        for (int r=matResult.rows(); r--; )
          {
            float *resultRow = matResult.row(r);
            for (int c=matResult.columns(); c--; )
              resultRow[c] = Pii::var<float,PiiMatrix<T> >(image(r,c, windowSize, windowSize));
          }
        d->pImageOutput->emitObject(matResult);
      }
      break;
    }
}

PiiContrastOperation::ContrastType PiiContrastOperation::type() const { return _d()->type; }
void PiiContrastOperation::setType(ContrastType type) { _d()->type = type; }
int PiiContrastOperation::radius() const { return _d()->iRadius; }
void PiiContrastOperation::setRadius(int radius) { _d()->iRadius = radius; }
