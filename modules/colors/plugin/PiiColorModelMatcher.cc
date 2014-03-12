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

#include "PiiColorModelMatcher.h"

#include <PiiYdinTypes.h>
#include <PiiThresholding.h>
#include "PiiColors.h"

PiiColorModelMatcher::Data::Data() :
  matBaseVectors(3,3), matCenter(1,3), dMatchingThreshold(0)
{
}

PiiColorModelMatcher::PiiColorModelMatcher() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiInputSocket("model"));
  addSocket(new PiiOutputSocket("image"));
}

void PiiColorModelMatcher::process()
{
  PiiVariant modelObj = inputAt(1)->firstObject();
  switch (modelObj.type())
    {
      PII_COLOR_IMAGE_CASES(calculateModel, modelObj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(1));
    }

  PiiVariant imageObj = readInput();

  switch (imageObj.type())
    {
      PII_COLOR_IMAGE_CASES(matchImageToModel, imageObj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiColorModelMatcher::calculateModel(const PiiVariant& obj)
{
  PII_D;
  PiiColors::measureColorDistribution(obj.valueAs<PiiMatrix<T> >(),
                                      d->matBaseVectors,
                                      d->matCenter);
}

template <class T> void PiiColorModelMatcher::matchImageToModel(const PiiVariant& obj)
{
  PII_D;
  if (d->dMatchingThreshold > 0)
    emitObject(PiiColors::matchColors(obj.valueAs<PiiMatrix<T> >(),
                                                      d->matBaseVectors,
                                                      d->matCenter,
                                                      std::bind2nd(PiiImage::InverseThresholdFunction<float,unsigned char>(),
                                                                   d->dMatchingThreshold)));
  else
    emitObject(PiiColors::matchColors(obj.valueAs<PiiMatrix<T> >(),
                                                      d->matBaseVectors,
                                                      d->matCenter,
                                                      PiiColors::LikelihoodFunction()));
}

void PiiColorModelMatcher::setMatchingThreshold(double matchingThreshold) { _d()->dMatchingThreshold = matchingThreshold; }
double PiiColorModelMatcher::matchingThreshold() const { return _d()->dMatchingThreshold; }
