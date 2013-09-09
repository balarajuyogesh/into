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

#include "PiiLabelingOperation.h"
#include <PiiMatrix.h>
#include "PiiLabeling.h"
#include "PiiImageTraits.h"
#include <PiiYdinTypes.h>

PiiLabelingOperation::Data::Data() :
  connectivity(PiiImage::Connect4),
  dThreshold(0),
  dHysteresis(0),
  bInverse(false)
{
}

PiiLabelingOperation::PiiLabelingOperation() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  PII_D;
  d->pBinaryImageInput = new PiiInputSocket("image");
  d->pLabeledImageOutput = new PiiOutputSocket("image");
  d->pLabelsOutput = new PiiOutputSocket("labels");
  
  addSocket(d->pBinaryImageInput);
  addSocket(d->pLabeledImageOutput);
  addSocket(d->pLabelsOutput);
}
  

void PiiLabelingOperation::process()
{
  PII_D;
  
  PiiVariant obj = d->pBinaryImageInput->firstObject();

  switch (obj.type())
    {
      PII_GRAY_IMAGE_CASES(operate, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(d->pBinaryImageInput);
    }  
}


template <class T> void PiiLabelingOperation::operate(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> image = obj.valueAs<PiiMatrix<T> >();
  int iLabels = 0;
  if (d->connectivity == PiiImage::Connect4 && d->dHysteresis == 0)
    {
      if (!d->bInverse)
        d->pLabeledImageOutput->emitObject(PiiImage::labelImage(image,
                                                                std::bind2nd(std::greater<T>(),
                                                                             T(d->dThreshold)),
                                                                PiiImage::DefaultLabelingLimiter(),
                                                                &iLabels));
      else
        d->pLabeledImageOutput->emitObject(PiiImage::labelImage(image,
                                                                std::bind2nd(std::less<T>(),
                                                                             T(d->dThreshold)),
                                                                PiiImage::DefaultLabelingLimiter(),
                                                                &iLabels));
    }
  else if (!d->bInverse)
    d->pLabeledImageOutput->emitObject(PiiImage::labelImage(image,
                                                            std::bind2nd(std::greater<T>(),
                                                                         T(qMax(0.0,
                                                                                d->dThreshold - d->dHysteresis))),
                                                            std::bind2nd(std::greater<T>(),
                                                                         T(d->dThreshold)),
                                                            d->connectivity,
                                                            1,
                                                            &iLabels));
  else
    d->pLabeledImageOutput->emitObject(PiiImage::labelImage(image,
                                                            std::bind2nd(std::less<T>(),
                                                                         T(qMin(double(PiiImage::Traits<T>::max()),
                                                                                d->dThreshold + d->dHysteresis))),
                                                            std::bind2nd(std::less<T>(),
                                                                         T(d->dThreshold)),
                                                            d->connectivity,
                                                            1,
                                                            &iLabels));
  d->pLabelsOutput->emitObject(iLabels);
}

void PiiLabelingOperation::setConnectivity(PiiImage::Connectivity connectivity) { _d()->connectivity = connectivity; }
PiiImage::Connectivity PiiLabelingOperation::connectivity() const { return _d()->connectivity; }

void PiiLabelingOperation::setThreshold(double threshold) { _d()->dThreshold = threshold; }
double PiiLabelingOperation::threshold() const { return _d()->dThreshold; }
void PiiLabelingOperation::setHysteresis(double hysteresis) { _d()->dHysteresis = hysteresis; }
double PiiLabelingOperation::hysteresis() const { return _d()->dHysteresis; }
void PiiLabelingOperation::setInverse(bool inverse) { _d()->bInverse = inverse; }
bool PiiLabelingOperation::inverse() const { return _d()->bInverse; }
