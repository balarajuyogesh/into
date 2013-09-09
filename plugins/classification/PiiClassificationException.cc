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

#include "PiiClassificationException.h"


const char* PiiClassificationException::messageForCode(Code code)
{
  static const char* messages[] =
    {
      QT_TRANSLATE_NOOP("PiiClassificationException", "Unknown error"),
      QT_TRANSLATE_NOOP("PiiClassificationException", "Learning was interrupted."),
    };
  return messages[code];
}

PiiClassificationException::Data::Data(Code c, const QString& location) :
  PiiException::Data(messageForCode(c), location),
  code(c)
{}

PiiClassificationException::Data::Data(const QString& message, const QString& location) :
  PiiException::Data(message, location),
  code(Unknown)
{}

PiiClassificationException::PiiClassificationException(Code code, const QString& location) :
  PiiException(new Data(code, location))
{}

PiiClassificationException::PiiClassificationException(const QString& message, const QString& location) :
  PiiException(new Data(message, location))
{}
  
PiiClassificationException::Code PiiClassificationException::code() const { return _d()->code; }
