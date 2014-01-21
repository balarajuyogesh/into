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

#include "PiiImageReaderOperation.h"
#include <PiiYdinTypes.h>
#include <PiiQImage.h>
#include <PiiColor.h>

using namespace PiiYdin;
using namespace Pii;

PiiImageReaderOperation::Data::Data() :
  imageType(Original), iMaxImages(-1), iCurrentIndex(0)
{
}

PiiImageReaderOperation::PiiImageReaderOperation(Data* dat) :
  PiiDefaultOperation(dat)
{
  setThreadCount(1);
  PII_D;
  d->pTriggerInput = new PiiInputSocket("trigger");
  d->pTriggerInput->setOptional(true);
  addSocket(d->pTriggerInput);

  d->pImageOutput = new PiiOutputSocket("image");
  addSocket(d->pImageOutput);
}

void PiiImageReaderOperation::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);
  if (reset)
    d->iCurrentIndex = 0;
}

int PiiImageReaderOperation::maxImages() const { return _d()->iMaxImages; }
void PiiImageReaderOperation::setMaxImages(int cnt) { _d()->iMaxImages = cnt; }
PiiImageReaderOperation::ImageType PiiImageReaderOperation::imageType() const { return _d()->imageType; }
int PiiImageReaderOperation::currentImageIndex() const { return _d()->iCurrentIndex; }

void PiiImageReaderOperation::emitGrayImage(QImage& img)
{
  Pii::convertToGray(img);
  _d()->pImageOutput->emitObject(PiiGrayQImage::create(img)->toMatrix());
}

void PiiImageReaderOperation::emitColorImage(QImage& img)
{
  convertToRgba(img);
  _d()->pImageOutput->emitObject(PiiColorQImage::create(img)->toMatrix());
}

void PiiImageReaderOperation::emitImage(QImage& img)
{
  if (img.depth() == 32)
    emitColorImage(img);
  else
    emitGrayImage(img);
}

void PiiImageReaderOperation::setImageType(ImageType type) { _d()->imageType = type; }
