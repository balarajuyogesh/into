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

#include "PiiRgbNormalizer.h"

#include <PiiYdinTypes.h>
#include <PiiColor.h>
#include "PiiColors.h"

PiiRgbNormalizer::Data::Data() :
  dMaxValue(255), channels(RedGreen)
{
}

PiiRgbNormalizer::PiiRgbNormalizer() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("channel0"));
  addSocket(new PiiOutputSocket("channel1"));
}

void PiiRgbNormalizer::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_COLOR_IMAGE_CASES(normalizedRgb, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiRgbNormalizer::normalizedRgb(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> clrImg(obj.valueAs<PiiMatrix<T> >());
  // Create output matrices in heap (exception-safe)
  PiiMatrix<typename T::Type> ch1(PiiMatrix<typename T::Type>::uninitialized(clrImg.rows(), clrImg.columns()));
  PiiMatrix<typename T::Type> ch2(PiiMatrix<typename T::Type>::uninitialized(clrImg.rows(), clrImg.columns()));

  int iCh1 = 0, iCh2 = 1;
  if (d->channels == RedBlue)
    iCh2 = 2;
  else if (d->channels == GreenBlue)
    {
      iCh1 = 1;
      iCh2 = 2;
    }

  PiiColors::normalizedRgb(clrImg, ch1, ch2, (float)d->dMaxValue, iCh1, iCh2);

  emitObject(ch1, 0);
  emitObject(ch2, 1);
}

void PiiRgbNormalizer::setMaxValue(double maxValue) { _d()->dMaxValue = maxValue; }
double PiiRgbNormalizer::maxValue() const { return _d()->dMaxValue; }
void PiiRgbNormalizer::setChannels(const ChannelPair& channels) { _d()->channels = channels; }
PiiRgbNormalizer::ChannelPair PiiRgbNormalizer::channels() const { return _d()->channels; }
