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

#include "PiiImageScaleOperation.h"
#include "PiiImage.h"
#include <PiiYdinTypes.h>

PiiImageScaleOperation::Data::Data() :
  scaleMode(ScaleAccordingToFactor),
  dScaleRatio(1.0),
  scaledSize(100,100),
  interpolation(LinearInterpolation)
{
}

PiiImageScaleOperation::PiiImageScaleOperation() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("image"));
}

void PiiImageScaleOperation::process()
{
  PiiVariant image = readInput();

  switch (image.type())
    {
      PII_GRAY_IMAGE_CASES(scaleImage, image);
      PII_COLOR_IMAGE_CASES(scaleImage, image);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiImageScaleOperation::scaleImage(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> image = obj.valueAs<PiiMatrix<T> >();

  int rows = image.rows(), cols = image.columns();
  switch (d->scaleMode)
    {
    case ScaleAccordingToFactor:
      rows = (int)(d->dScaleRatio * rows + 0.5);
      cols = (int)(d->dScaleRatio * cols + 0.5);
      break;
    case ScaleToSize:
      rows = d->scaledSize.height();
      cols = d->scaledSize.width();
      break;
    case ScaleToBox:
      {
        double dScaleRatio = qMin(d->scaledSize.width() / (double)cols, d->scaledSize.height() / (double)rows);
        rows = (int)(dScaleRatio * rows + 0.5);
        cols = (int)(dScaleRatio * cols + 0.5);
        break;
      }
    case ScaleToFillBox:
      {
        double dScaleRatio = qMax(d->scaledSize.width() / (double)cols, d->scaledSize.height() / (double)rows);
        rows = (int)(dScaleRatio * rows + 0.5);
        cols = (int)(dScaleRatio * cols + 0.5);
        break;
      }
    case ScaleToPixelCount:
      {
        double dScaleRatio = sqrt(d->scaledSize.width() * d->scaledSize.height() / double(rows * cols));
        rows = (int)(dScaleRatio * rows + 0.5);
        cols = (int)(dScaleRatio * cols + 0.5);
        break;
      }
    default:
      {
        double currentAspectRatio = (double)cols / rows;
        if (d->scaleMode == ScaleToAspectRatioX ||
            (d->scaleMode == ScaleDownToAspectRatio && d->dScaleRatio < currentAspectRatio) ||
            (d->scaleMode == ScaleUpToAspectRatio &&  d->dScaleRatio >= currentAspectRatio))
          cols = (int)(rows * d->dScaleRatio + 0.5);
        else
          rows = (int)(cols / d->dScaleRatio + 0.5);
      }
    }
  // Do we actually need to scale the image?
  if ((rows != image.rows() || cols != image.columns()) &&
      rows > 0 && cols > 0)
    emitObject(PiiImage::scale(image, rows, cols, (Pii::Interpolation)d->interpolation));
  else // Pass the image without modification
    emitObject(obj);
}

PiiImageScaleOperation::ScaleMode PiiImageScaleOperation::scaleMode() const { return _d()->scaleMode; }
void PiiImageScaleOperation::setScaleMode(ScaleMode scaleMode) { _d()->scaleMode = scaleMode; }
double PiiImageScaleOperation::scaleRatio() const { return _d()->dScaleRatio; }
void PiiImageScaleOperation::setScaleRatio(double scaleRatio) { if (scaleRatio > 0) _d()->dScaleRatio = scaleRatio; }
QSize PiiImageScaleOperation::scaledSize() const { return _d()->scaledSize; }
void PiiImageScaleOperation::setScaledSize(QSize scaledSize)
{
  if (scaledSize.width() < 1) scaledSize.setWidth(1);
  if (scaledSize.height() < 1) scaledSize.setHeight(1);
  _d()->scaledSize = scaledSize;
}
PiiImageScaleOperation::Interpolation PiiImageScaleOperation::interpolation() const { return _d()->interpolation; }
void PiiImageScaleOperation::setInterpolation(Interpolation interpolation) { _d()->interpolation = interpolation; }
