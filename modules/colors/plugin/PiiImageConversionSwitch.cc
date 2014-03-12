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

#include "PiiImageConversionSwitch.h"

#include <PiiYdinTypes.h>
#include <PiiMatrix.h>
#include <PiiColor.h>

PiiImageConversionSwitch::Data::Data() :
  bGrayImageOutputConnected(false),
  bColorImageOutputConnected(false),
  colorConversion(RgbToGrayMean)
{
}

PiiImageConversionSwitch::PiiImageConversionSwitch() :
  PiiDefaultOperation(new Data)
{
  PII_D;
  addSocket(d->pImageInput = new PiiInputSocket("image"));
  addSocket(d->pGrayImageOutput = new PiiOutputSocket("gray image"));
  addSocket(d->pColorImageOutput = new PiiOutputSocket("color image"));
}

void PiiImageConversionSwitch::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);

  d->bGrayImageOutputConnected = d->pGrayImageOutput->isConnected();
  d->bColorImageOutputConnected = d->pColorImageOutput->isConnected();
}

void PiiImageConversionSwitch::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
    case PiiYdin::UnsignedCharMatrixType:
      operateGrayImage(obj);
      break;
    case PiiYdin::UnsignedCharColor4MatrixType:
      operateColorImage(obj);
      break;
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}


void PiiImageConversionSwitch::operateGrayImage(const PiiVariant& obj)
{
  PII_D;
  if (d->bGrayImageOutputConnected)
    d->pGrayImageOutput->emitObject(obj);

  //convert to color image and emit it
  if (d->bColorImageOutputConnected)
    {
      const PiiMatrix<unsigned char> image = obj.valueAs<PiiMatrix<unsigned char> >();
      d->pColorImageOutput->emitObject(PiiMatrix<PiiColor4<unsigned char> >(image));
    }
}

void PiiImageConversionSwitch::operateColorImage(const PiiVariant& obj)
{
  PII_D;
  if (d->bGrayImageOutputConnected)
    {
      const PiiMatrix<PiiColor4<unsigned char> > image = obj.valueAs<PiiMatrix<PiiColor4<unsigned char> > >();

      switch(d->colorConversion)
        {
        case RgbToGrayMean:
          d->pGrayImageOutput->emitObject(PiiMatrix<unsigned char>(image));
          break;
        }
    }

  if (d->bColorImageOutputConnected)
    d->pColorImageOutput->emitObject(obj);
}

void PiiImageConversionSwitch::setColorConversion(ColorConversion colorConversion) { _d()->colorConversion = colorConversion; }
PiiImageConversionSwitch::ColorConversion PiiImageConversionSwitch::colorConversion() const { return _d()->colorConversion; }
