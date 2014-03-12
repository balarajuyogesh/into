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

#include "PiiException.h"

#ifndef PII_NO_QT
PII_DEFINE_VIRTUAL_METAOBJECT_FUNCTION(PiiException);
#  include "PiiSerializableExport.h"
PII_SERIALIZABLE_EXPORT(PiiException);
#endif

PiiException::Data::Data()
{}

PiiException::Data::Data(const QString& message, const QString& location) :
  strMessage(message), strLocation(location)
{}

PiiException::Data::Data(const Data& other) :
  strMessage(other.strMessage),
  strLocation(other.strLocation)
{}

PiiException::Data::~Data()
{}

PiiException::PiiException() :
  d(new Data)
{}

PiiException::PiiException(const QString& message) :
  d(new Data(message, ""))
{}

PiiException::PiiException(const QString& message, const QString& location) :
  d(new Data(message, location))
{}


PiiException::PiiException(const PiiException& other) :
  d(new Data(other.d->strMessage, other.d->strLocation))
{}

PiiException::PiiException(Data* data) :
  d(data)
{
}

PiiException& PiiException::operator= (const PiiException& other)
{
  d->strMessage = other.d->strMessage;
  d->strLocation = other.d->strLocation;
  return *this;
}

PiiException::~PiiException()
{
  delete d;
}

QString PiiException::message() const { return d->strMessage; }
void PiiException::setMessage(const QString& message) { d->strMessage = message; }
QString PiiException::location() const { return d->strLocation; }
QString PiiException::location(const QString& prefix, const QString& suffix) const
{
  if (d->strLocation.isEmpty())
    return QString();
  return prefix + d->strLocation + suffix;
}
void PiiException::setLocation(const QString& location) { d->strLocation = location; }

QString PiiException::toString() const
{
  QString str;
  if (!d->strLocation.isEmpty())
    str += d->strLocation + ": ";
#ifndef PII_NO_QT
  str += piiMetaObject()->className();
#endif
  str += '(';
  str += d->strMessage;
  str += ')';
  return str;
}

void PiiException::throwIt()
{
  try { throwThis(); }
  catch (...)
    {
      delete this;
      throw;
    }
}

void PiiException::throwThis() { throw *this; }
