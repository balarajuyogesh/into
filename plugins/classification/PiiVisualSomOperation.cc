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

#include "PiiVisualSomOperation.h"
#include <PiiQImageUtil.h>

PiiVisualSomOperation::Data::Data() :
  dClassification(NAN),
  iFakedClassification(0)
{
}

PiiVisualSomOperation::PiiVisualSomOperation() :
  PiiSomOperation::Template<PiiMatrix<double> >(new Data)
{
  PII_D;
  d->pImageInput = new PiiInputSocket("image");
  addSocket(d->pImageInput);

  d->limiter.setMaxFrequency(10);
}

PiiVisualSomOperation::~PiiVisualSomOperation()
{
}

double PiiVisualSomOperation::classify()
{
  classificationOutput()->emitObject(_d()->dClassification);
  return _d()->dClassification;
}

void PiiVisualSomOperation::collectSample(double label, double weight)
{
  PII_D;

  if (d->limiter.check())
    {
      PiiVariant image = d->pImageInput->firstObject();
      d->dClassification = SuperType::classify();

      int iClassification;
      if (Pii::isNan(d->dClassification))
        {
          iClassification = d->iFakedClassification;
          d->iFakedClassification++;
          d->iFakedClassification %= (width() * height());
        }
      else
        iClassification = (int)d->dClassification;

      d->storeMutex.lock();
      SuperType::collectSample(label, weight);
      storeImage(image, iClassification);
      d->storeMutex.unlock();
    }
}

bool PiiVisualSomOperation::learnBatch()
{
  PII_D;

  d->storeMutex.lock();
  d->lstNewImages = d->lstImages;
  bool bSuccess = SuperType::learnBatch();
  d->storeMutex.unlock();

  if (!bSuccess)
    d->lstNewImages.clear();

  return bSuccess;
}

void PiiVisualSomOperation::replaceClassifier()
{
  PII_D;
  PiiMatrix<double>* pmatFeatures = d->collector.samples();
  // Classify all collected features and send images/classifications
  for (int i=0; i<pmatFeatures->rows(); i++)
    emit addSubImage(d->lstNewImages.at(i), d->pClassifier->classify(pmatFeatures->row(i)));
  d->lstNewImages.clear();
}

void PiiVisualSomOperation::storeImage(const PiiVariant& image, int classification)
{
  PII_D;

  QImagePtr pImage(0);
  switch (image.type())
    {
      PII_ALL_IMAGE_CASES(pImage = PiiYdin::createQImage, image);
    default:
      PII_THROW_UNKNOWN_TYPE(d->pImageInput);
    }

  d->lstImages << pImage;
  
  emit addSubImage(pImage, classification);
}

void PiiVisualSomOperation::removeImage(QImagePtr image)
{
  PII_D;
  QMutexLocker lock(&d->storeMutex);

  int index = d->lstImages.indexOf(image);
  if (index > -1)
    {
      d->lstImages.removeAt(index);
      d->collector.samples()->removeRow(index);
    }
}

void PiiVisualSomOperation::setMaxFrequency(double maxFrequency) { _d()->limiter.setMaxFrequency(maxFrequency); }
double PiiVisualSomOperation::maxFrequency() { return _d()->limiter.maxFrequency(); }
