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

#include "PiiStringConverter.h"

#include <PiiYdinTypes.h>

PiiStringConverter::Data::Data() :
  conversionMode(ConvertToInt)
{
}

PiiStringConverter::PiiStringConverter() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
}

void PiiStringConverter::process()
{
  PiiVariant obj = readInput();
  if (obj.type() != PiiYdin::QStringType)
    PII_THROW_UNKNOWN_TYPE(inputAt(0));
  if (_d()->conversionMode == ConvertToInt)
    emitObject(Pii::stringTo<int>(obj.valueAs<QString>()));
  else
    emitObject(Pii::stringTo<double>(obj.valueAs<QString>()));
}

void PiiStringConverter::setConversionMode(ConversionMode conversionMode) { _d()->conversionMode = conversionMode; }
PiiStringConverter::ConversionMode PiiStringConverter::conversionMode() const { return _d()->conversionMode; }
