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

#include "PiiMaskGenerator.h"
#include "PiiMorphology.h"
#include <PiiYdinTypes.h>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QtDebug>

PiiMaskGenerator::Data::Data() :
  maskType(PiiImage::EllipticalMask)
{
}

PiiMaskGenerator::PiiMaskGenerator() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("trigger"));
  addSocket(new PiiInputSocket("size"));
  addSocket(new PiiOutputSocket("mask"));

  inputAt(0)->setOptional(true);
  inputAt(1)->setOptional(true);
}

void PiiMaskGenerator::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);

  if (!(inputAt(0)->isConnected() || inputAt(1)->isConnected()))
    PII_THROW(PiiExecutionException, tr("Either %1 or %2 needs to be connected.").arg("trigger").arg("size"));

  // If size input is not connected, initialize a static mask
  if (!inputAt(1)->isConnected())
    d->maskMatrix = createMask(d->maskSize.height(), d->maskSize.width());
  // Otherwise reset the mask
  else
    d->maskMatrix = PiiMatrix<unsigned char>();
}

void PiiMaskGenerator::process()
{
  PII_D;
  if (!inputAt(1)->isConnected())
    emitObject(d->maskMatrix);
  else
    {
      PiiVariant obj = inputAt(1)->firstObject();
      if (obj.type() != PiiYdin::IntMatrixType)
        PII_THROW_UNKNOWN_TYPE(inputAt(1));
      const PiiMatrix<int> size = obj.valueAs<PiiMatrix<int> >();
      int lastCol = size.columns()-1;
      if (size.rows() < 1 || lastCol < 1)
        PII_THROW_WRONG_SIZE(inputAt(1), size, "1-N", "2|4");

      // Always take the last two entries on the first row
      emitObject(createMask(size(0,lastCol), size(0,lastCol-1)));
    }
}

PiiMatrix<unsigned char> PiiMaskGenerator::createMask(int rows, int cols)
{
  if (rows <= 0 || cols <= 0)
    PII_THROW(PiiExecutionException, tr("Cannot create a %1-by-%2 mask matrix.").arg(rows).arg(cols));
  return PiiImage::createMask(_d()->maskType, rows, cols);
}

void PiiMaskGenerator::setMaskType(PiiImage::MaskType maskType) { _d()->maskType = maskType; }
PiiImage::MaskType PiiMaskGenerator::maskType() const { return _d()->maskType; }
void PiiMaskGenerator::setMaskSize(QSize maskSize) { _d()->maskSize = maskSize; }
QSize PiiMaskGenerator::maskSize() const { return _d()->maskSize; }
