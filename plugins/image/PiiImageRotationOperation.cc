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

#include "PiiImageRotationOperation.h"
#include "PiiImage.h"
#include <PiiYdinTypes.h>

PiiImageRotationOperation::Data::Data() :
  dAngle(0.0),
  transformedSize(PiiImage::ExpandAsNecessary)
{
}

PiiImageRotationOperation::PiiImageRotationOperation() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiInputSocket("angle"));
  inputAt(1)->setOptional(true);
  addSocket(new PiiOutputSocket("image"));
}

void PiiImageRotationOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_GRAY_IMAGE_CASES(rotate, obj);
      PII_COLOR_IMAGE_CASES(rotate, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> struct PiiImageRotationOperation::Background
{
  static inline T get(const QColor& clr) { return T(PiiImage::Traits<T>::fromInt(clr.value())); }
};

template <class T> struct PiiImageRotationOperation::Background<PiiColor<T> >
{
  static inline PiiColor<T> get(const QColor& clr)
  {
    return PiiColor<T>(PiiImage::Traits<T>::fromInt(clr.red()),
                       PiiImage::Traits<T>::fromInt(clr.green()),
                       PiiImage::Traits<T>::fromInt(clr.blue()));
  }
};

template <class T> struct PiiImageRotationOperation::Background<PiiColor4<T> >
{
  static inline PiiColor4<T> get(const QColor& clr)
  {
    return PiiColor4<T>(PiiImage::Traits<T>::fromInt(clr.red()),
                        PiiImage::Traits<T>::fromInt(clr.green()),
                        PiiImage::Traits<T>::fromInt(clr.blue()),
                        PiiImage::Traits<T>::fromInt(clr.alpha()));
  }
};

template <class T> void PiiImageRotationOperation::rotate(const PiiVariant& obj)
{
  PII_D;
  double angle = d->dAngle;

  // If the angle input is connected, read it to get the angle
  if (inputAt(1)->isConnected())
    {
      // We accept all primitive values
      PiiVariant angleObj = inputAt(1)->firstObject();
      switch (angleObj.type())
        {
          PII_PRIMITIVE_CASES(angle = (double)PiiYdin::primitiveAs, angleObj);
        default:
          PII_THROW_UNKNOWN_TYPE(inputAt(1));
        }
    }

  //qDebug("Rotating image %d degrees.", int(angle / M_PI * 180));
  // Rotate if needed
  if (angle == 0.0 || obj.valueAs<PiiMatrix<T> >().isEmpty())
    emitObject(obj);
  else
    emitObject(PiiImage::rotate(obj.valueAs<PiiMatrix<T> >(),
                                angle,
                                d->transformedSize,
                                Background<T>::get(d->backgroundColor)));
}

void PiiImageRotationOperation::setAngle(double angle) { _d()->dAngle = angle; }
double PiiImageRotationOperation::angle() const { return _d()->dAngle; }
void PiiImageRotationOperation::setAngleDeg(double angleDeg) { _d()->dAngle = angleDeg / 180.0 * M_PI; }
double PiiImageRotationOperation::angleDeg() const { return _d()->dAngle / M_PI * 180.0; }
void PiiImageRotationOperation::setTransformedSize(PiiImage::TransformedSize transformedSize) { _d()->transformedSize = transformedSize; }
PiiImage::TransformedSize PiiImageRotationOperation::transformedSize() const { return _d()->transformedSize; }
void PiiImageRotationOperation::setBackgroundColor(const QColor& backgroundColor) { _d()->backgroundColor = backgroundColor; }
QColor PiiImageRotationOperation::backgroundColor() const { return _d()->backgroundColor; }
