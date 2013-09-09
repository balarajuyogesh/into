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

#include "PiiColorCorrelogramOperation.h"

#include <PiiYdinTypes.h>
#include <PiiColors.h>
#include <PiiUtil.h>

PiiColorCorrelogramOperation::Data::Data() :
  iLevels(4),
  bQuantize(true)
{
  lstDistances << 1 << 3 << 5 << 7;
}


PiiColorCorrelogramOperation::PiiColorCorrelogramOperation() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  PII_D;
  
  addSocket(d->pInput = new PiiInputSocket("image"));
  addSocket(d->pOutput = new PiiOutputSocket("correlogram"));
}

PiiColorCorrelogramOperation::~PiiColorCorrelogramOperation()
{
}

void PiiColorCorrelogramOperation::check(bool reset)
{
  PII_D;
  
  if (d->iLevels < 2)
    PII_THROW(PiiExecutionException, tr("The number of quantization levels must be at least two."));
  PiiDefaultOperation::check(reset);
}

void PiiColorCorrelogramOperation::setLevels(int levels) { _d()->iLevels = levels; }
int PiiColorCorrelogramOperation::levels() const { return _d()->iLevels; }
void PiiColorCorrelogramOperation::setDistances(const QVariantList& distances) { _d()->lstDistances = Pii::variantsToList<int>(distances); }
QVariantList PiiColorCorrelogramOperation::distances() const { return Pii::listToVariants(_d()->lstDistances); }
void PiiColorCorrelogramOperation::setQuantize(bool quantize) { _d()->bQuantize = quantize; }
bool PiiColorCorrelogramOperation::quantize() const { return _d()->bQuantize; }


void PiiColorCorrelogramOperation::process()
{
  PII_D;
  
  PiiVariant obj = d->pInput->firstObject();

  switch (obj.type())
    {
      PII_INT_COLOR_IMAGE_CASES(processColor, obj);
      PII_INT_GRAY_IMAGE_CASES(processGray, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class Clr> void PiiColorCorrelogramOperation::processColor(const PiiVariant& obj)
{
  PII_D;
  
  const PiiMatrix<Clr> img = obj.valueAs<PiiMatrix<Clr> >();
  if (d->bQuantize)
    {
      d->pOutput->emitObject(PiiColors::autocorrelogram(PiiColors::toIndexed(img, d->iLevels),
                                                        d->lstDistances,
                                                        d->iLevels*d->iLevels*d->iLevels));
    }
  else
    {
      PiiMatrix<int> matIndexed(PiiMatrix<int>::uninitialized(img.rows(), img.columns()));
      int iSquare = d->iLevels * d->iLevels;
      for (int r=0; r<img.rows(); ++r)
        {
          const Clr* pSource = img[r];
          int* pTarget = matIndexed[r];
          for (int c=0; c<img.columns(); ++c)
            pTarget[c] =
              iSquare * pSource[c].rgbR +
              d->iLevels * pSource[c].rgbG +
              pSource[c].rgbB;
        }
      d->pOutput->emitObject(PiiColors::autocorrelogram(img,
                                                        d->lstDistances,
                                                        d->iLevels*d->iLevels*d->iLevels));
    }
}

template <class T> void PiiColorCorrelogramOperation::processGray(const PiiVariant& obj)
{
  PII_D;
  
  const PiiMatrix<T> img = obj.valueAs<PiiMatrix<T> >();
  if (d->bQuantize)
    {
      double dScale = double(d->iLevels) / PiiImage::Traits<T>::max();
      d->pOutput->emitObject(PiiColors::autocorrelogram(img.mapped(Pii::unaryCompose(Pii::Round<T>(),
                                                                                     std::bind2nd(std::multiplies<double>(), dScale))),
                                                        d->lstDistances,
                                                        d->iLevels));
    }
  else
    {
      d->pOutput->emitObject(PiiColors::autocorrelogram(img,
                                                        d->lstDistances,
                                                        d->iLevels));
    }
}
