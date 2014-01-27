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

#include "PiiMimeException.h"

const char* PiiMimeException::messageForCode(Code code)
{
  static const char* messages[] =
    {
      QT_TR_NOOP("Header too large"),
      QT_TR_NOOP("Invalid message format")
    };
  return messages[code];
}

#include "PiiSerializableExport.h"
PII_SERIALIZABLE_EXPORT(PiiMimeException);

PiiMimeException::Data::Data(Code c, const QString& location) :
  PiiException::Data(messageForCode(c), location),
  code(c)
{}

PiiMimeException::Data::Data(const Data& other) :
  PiiException::Data(other),
  code(other.code)
{}

PiiMimeException::PiiMimeException(Code code, const QString& location) :
  PiiException(new Data(code, location))
{}

PiiMimeException::PiiMimeException(const PiiMimeException& other) :
  PiiException(new Data(*other._d()))
{}

PiiMimeException::~PiiMimeException()
{}

PiiMimeException& PiiMimeException::operator= (const PiiMimeException& other)
{
  PiiException::operator= (other);
  _d()->code = other._d()->code;
  return *this;
}

PiiMimeException::Code PiiMimeException::code() const { return _d()->code; }

void PiiMimeException::throwThis() { throw *this; }
