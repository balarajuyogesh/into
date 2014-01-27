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

#include "PiiExecutionException.h"
#include <PiiOperation.h>

#include "PiiSerializableExport.h"
PII_SERIALIZABLE_EXPORT(PiiExecutionException);
PII_SERIALIZABLE_EXPORT(PiiCompoundExecutionException);

PiiExecutionException::Data::Data(const QString& message, const QString& location, Code c) :
  PiiException::Data(message, location),
  code(c)
{}

PiiExecutionException::Data::Data(const QString& message, const QString& location) :
  PiiException::Data(message, location),
  code(Error)
{}

PiiExecutionException::Data::Data(const Data& other) :
  PiiException::Data(other.strMessage, other.strLocation),
  code(other.code)
{}

PiiExecutionException::PiiExecutionException(const QString& message, const QString& location) :
  PiiException(new Data(message, location))
{}

PiiExecutionException::PiiExecutionException(Code code) :
  PiiException(new Data("", "", code))
{}

PiiExecutionException::PiiExecutionException(Code code, const QString& message, const QString& location) :
  PiiException(new Data(message, location, code))
{}

PiiExecutionException::PiiExecutionException(const PiiExecutionException& other) :
  PiiException(new Data(other.message(), other.location(), other.code()))
{}

PiiExecutionException::PiiExecutionException(Data* data) :
  PiiException(data)
{}

PiiExecutionException::~PiiExecutionException()
{}

PiiExecutionException::Code PiiExecutionException::code() const { return _d()->code; }
void PiiExecutionException::setCode(Code code) { _d()->code = code; }

const char* PiiExecutionException::errorName(PiiExecutionException::Code code)
{
  static const char* errorNames[] = { "Error", "Finished", "Interrupted", "Paused" };
  return errorNames[code];
}

bool PiiExecutionException::isCompound() const { return false; }

void PiiExecutionException::throwThis() { throw *this; }



PiiCompoundExecutionException::Data::Data(const QString& message, const QString& location) :
  PiiExecutionException::Data(message, location)
{}

PiiCompoundExecutionException::Data::Data(const Data& other) :
  PiiExecutionException::Data(other)
{
  for (int i=0; i<other.lstExceptions.size(); ++i)
    addException(other.lstExceptions[i].first,
                 other.lstExceptions[i].second->isCompound() ?
                 new PiiCompoundExecutionException(*static_cast<PiiCompoundExecutionException*>(other.lstExceptions[i].second)) :
                 new PiiExecutionException(*other.lstExceptions[i].second));
}

void PiiCompoundExecutionException::Data::addException(const QString& childName, PiiExecutionException* ex)
{
  lstExceptions << qMakePair(childName, ex);
}

PiiCompoundExecutionException::PiiCompoundExecutionException(const QString& message, const QString& location) :
  PiiExecutionException(new Data(message, location))
{}

PiiCompoundExecutionException::PiiCompoundExecutionException(const PiiCompoundExecutionException& other) :
  PiiExecutionException(new Data(*other._d()))
{}

PiiCompoundExecutionException::~PiiCompoundExecutionException()
{
  PII_D;
  for (int i=0; i<d->lstExceptions.size(); ++i)
    delete d->lstExceptions[i].second;
}

void PiiCompoundExecutionException::addException(const QString& childName,
                                                 const PiiExecutionException& ex)
{
  PII_D;
  d->addException(childName,
                  ex.isCompound() ?
                  new PiiCompoundExecutionException(static_cast<const PiiCompoundExecutionException&>(ex)) :
                  new PiiExecutionException(ex));
  if (!d->strMessage.isEmpty())
    d->strMessage.append('\n');
  d->strMessage.append(ex.message());
}

PiiCompoundExecutionException::ExceptionList PiiCompoundExecutionException::exceptions() const
{
  return _d()->lstExceptions;
}

bool PiiCompoundExecutionException::isCompound() const { return true; }

void PiiCompoundExecutionException::throwThis() { throw *this; }
