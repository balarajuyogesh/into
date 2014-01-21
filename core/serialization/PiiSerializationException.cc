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

#include "PiiSerializationException.h"

const char* PiiSerializationException::messageForCode(Code code)
{
  static const char* messages[] =
    {
      QT_TR_NOOP("Unknown error"),
      QT_TR_NOOP("Invalid data format"),
      QT_TR_NOOP("Unregistered class"),
      QT_TR_NOOP("Could not find a serializer"),
      QT_TR_NOOP("Object to be read is newer than our implementation"),
      QT_TR_NOOP("Stream input/output error"),
      QT_TR_NOOP("Stream must be open before constructing an archive"),
      QT_TR_NOOP("Unrecognized archive format"),
      QT_TR_NOOP("Input archive is newer than our implementation")
    };
  return messages[(int)code];
}


PiiSerializationException::Data::Data(Code c, const QString& location) :
  PiiException::Data(messageForCode(c), location),
  code(c)
{}

PiiSerializationException::Data::Data(const QString& message, const QString& location) :
  PiiException::Data(message, location),
  code(Unknown)
{}

PiiSerializationException::Data::Data(Code c, const QString& info, const QString& location) :
  PiiException::Data(messageForCode(c), location),
  code(c),
  strInfo(info)
{}

PiiSerializationException::PiiSerializationException(Code code, const QString& location) :
  PiiException(new Data(code, location))
{}

PiiSerializationException::PiiSerializationException(const QString& message, const QString& location) :
  PiiException(new Data(message, location))
{}

PiiSerializationException::PiiSerializationException(Code code, const QString& info, const QString& location) :
  PiiException(new Data(code, info, location))
{}

PiiSerializationException::PiiSerializationException(const PiiSerializationException& other) :
  PiiException(new Data(other.code(), other.info(), other.location()))
{}

PiiSerializationException::~PiiSerializationException()
{}

PiiSerializationException& PiiSerializationException::operator= (const PiiSerializationException& other)
{
  PiiException::operator= (other);
  _d()->code = other._d()->code;
  return *this;
}

QString PiiSerializationException::message() const
{
  const PII_D;
  if (d->strInfo.isEmpty())
    return PiiException::message();
  return QString("%1 (%2)").arg(PiiException::message()).arg(d->strInfo);
}

PiiSerializationException::Code PiiSerializationException::code() const { return _d()->code; }
QString PiiSerializationException::info() const { return _d()->strInfo; }

void PiiSerializationException::throwThis() { throw *this; }
