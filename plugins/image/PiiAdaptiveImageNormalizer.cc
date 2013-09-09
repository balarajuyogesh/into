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

#include "PiiAdaptiveImageNormalizer.h"

#include <PiiYdinTypes.h>

#include "PiiThresholding.h"

PiiAdaptiveImageNormalizer::Data::Data() :
  windowSize(64,64),
  dTargetMean(NAN)
{
}

PiiAdaptiveImageNormalizer::PiiAdaptiveImageNormalizer() : PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("image"));
}

void PiiAdaptiveImageNormalizer::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_GRAY_IMAGE_CASES(normalizeGray, obj);
      PII_COLOR_IMAGE_CASES(normalizeColor, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> struct PiiAdaptiveImageNormalizer::Normalizer : Pii::BinaryFunction<T,float,T>
{
  Normalizer(float target) : fTarget(target) {}
  T operator() (T pixel, float mean) const { return mean != 0 ? T(fTarget * pixel / mean) : 0; }
  float fTarget;
};

template <> struct PiiAdaptiveImageNormalizer::Normalizer<uchar> : Pii::BinaryFunction<uchar,float,uchar>
{
  Normalizer(float target) : fTarget(target) {}
  uchar operator() (uchar pixel, float mean) const { return mean != 0 ? uchar(qMin(255.0f, fTarget * pixel / mean)) : 0; }
  float fTarget;
};

template <class T> void PiiAdaptiveImageNormalizer::normalizeGray(const PiiVariant& obj)
{
  emitObject(normalize(obj.valueAs<PiiMatrix<T> >()));
}

template <class T> void PiiAdaptiveImageNormalizer::normalizeColor(const PiiVariant& obj)
{
  PiiMatrix<T> image(obj.valueAs<PiiMatrix<T> >());
  PiiMatrix<T> matResult(PiiMatrix<T>::uninitialized(image.rows(), image.columns()));
  for (int i=0; i<T::ChannelCount; ++i)
    PiiImage::setColorChannel(matResult, i, normalize(PiiImage::colorChannel(image, i)));
  emitObject(matResult);
}

template <class T> PiiMatrix<T> PiiAdaptiveImageNormalizer::normalize(const PiiMatrix<T>& image)
{
  PII_D;
  double dTarget = d->dTargetMean;
  if (Pii::isNan(dTarget))
    dTarget = PiiImage::Traits<T>::max() / 2;
  return PiiImage::adaptiveThreshold(image, Normalizer<T>(dTarget), d->windowSize.width(), d->windowSize.height());
}

void PiiAdaptiveImageNormalizer::setWindowSize(const QSize& windowSize) { _d()->windowSize = windowSize; }
QSize PiiAdaptiveImageNormalizer::windowSize() const { return _d()->windowSize; }

void PiiAdaptiveImageNormalizer::setTargetMean(double targetMean) { _d()->dTargetMean = targetMean; }
double PiiAdaptiveImageNormalizer::targetMean() const { return _d()->dTargetMean; }
