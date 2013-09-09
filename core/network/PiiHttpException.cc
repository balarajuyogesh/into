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

#include "PiiHttpException.h"
#include "PiiHttpProtocol.h"

#include "PiiSerializableExport.h"
PII_SERIALIZABLE_EXPORT(PiiHttpException);

PiiHttpException::Data::Data(int statusCode, const QString& location) :
  PiiException::Data(PiiHttpProtocol::statusMessage(statusCode), location),
  iStatusCode(statusCode)
{}

PiiHttpException::Data::Data(int statusCode, const QString& message, const QString& location) :
  PiiException::Data(message.isEmpty() ? PiiHttpProtocol::statusMessage(statusCode) : message, location),
  iStatusCode(statusCode)
{}

PiiHttpException::PiiHttpException(int statusCode, const QString& location) :
  PiiNetworkException(new Data(statusCode, location))
{}

PiiHttpException::PiiHttpException(int statusCode, const QString& message, const QString& location) :
  PiiNetworkException(new Data(statusCode, message, location))
{}

PiiHttpException::PiiHttpException(const PiiHttpException& other) :
  PiiNetworkException(new Data(other.statusCode(), other.message(), other.location()))
{}

PiiHttpException::~PiiHttpException()
{}

PiiHttpException& PiiHttpException::operator= (const PiiHttpException& other)
{
  PiiNetworkException::operator= (other);
  _d()->iStatusCode = other._d()->iStatusCode;
  return *this;
}

int PiiHttpException::statusCode() const { return _d()->iStatusCode; }

void PiiHttpException::throwThis() { throw *this; }
