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

#include "PiiHttpRequestHeader.h"

PiiHttpRequestHeader::Data::Data() :
  strMethod("GET"),
  strPath("/"),
  httpVersion(1,1),
  bValid(true)
{}

PiiHttpRequestHeader::Data::Data(const Data& other) :
  PiiMimeHeader::Data(other),
  strMethod(other.strMethod),
  strPath(other.strPath),
  httpVersion(other.httpVersion),
  bValid(other.bValid)
{}

PiiHttpRequestHeader::Data::~Data()
{}

PiiHttpRequestHeader::Data* PiiHttpRequestHeader::Data::clone() const
{
  return new Data(*this);
}

PiiHttpRequestHeader::PiiHttpRequestHeader() :
  PiiMimeHeader(new Data)
{}

PiiHttpRequestHeader::PiiHttpRequestHeader(const QByteArray& headerData) :
  PiiMimeHeader(new Data)
{
  PII_D;
  QRegExp requestRe("^(\\w+)\\s+(\\S+)\\s+HTTP/(\\d\\.\\d)\\r?$");
  int iLineBreakPos = headerData.indexOf('\n');
  if (iLineBreakPos != -1 &&
      requestRe.indexIn(QString(headerData.left(iLineBreakPos))) != -1)
    {
      d->strMethod = requestRe.cap(1);
      d->strPath = requestRe.cap(2);
      d->httpVersion = PiiVersionNumber(requestRe.cap(3));

      int iRemainingLength = headerData.size() - iLineBreakPos - 1;
      if (iRemainingLength > 0 &&
          !addValues(headerData.constData() + iLineBreakPos + 1, iRemainingLength))
        d->bValid = false;
    }
  else
    d->bValid = false;
}

PiiHttpRequestHeader::PiiHttpRequestHeader(const PiiHttpRequestHeader& other) :
  PiiMimeHeader(other)
{}

PiiHttpRequestHeader::~PiiHttpRequestHeader()
{}

PiiHttpRequestHeader& PiiHttpRequestHeader::operator= (const PiiHttpRequestHeader& other)
{
  PiiMimeHeader::operator= (other);
  return *this;
}

void PiiHttpRequestHeader::setMethod(const QString& method) { _d()->strMethod = method; }
QString PiiHttpRequestHeader::method() const { return _d()->strMethod; }
void PiiHttpRequestHeader::setPath(const QString& path) { _d()->strPath = path; }
QString PiiHttpRequestHeader::path() const { return _d()->strPath; }
void PiiHttpRequestHeader::setHttpVersion(const PiiVersionNumber& httpVersion) { _d()->httpVersion = httpVersion; }
PiiVersionNumber PiiHttpRequestHeader::httpVersion() const { return _d()->httpVersion; }

void PiiHttpRequestHeader::setRequest(const QString& method, const QString& path)
{
  PII_D;
  d->strMethod = method;
  d->strPath = path;
}

QByteArray PiiHttpRequestHeader::toByteArray() const
{
  const PII_D;
  QByteArray aResult;
  aResult.append(d->strMethod.toLatin1());
  aResult.append(' ');
  aResult.append(d->strPath.toLatin1());
  aResult.append(" HTTP/");
  aResult.append(d->httpVersion.toString().toLatin1());
  aResult.append("\r\n");
  aResult.append(PiiMimeHeader::toByteArray());
  return aResult;
}

bool PiiHttpRequestHeader::isValid() const
{
  return _d()->bValid;
}
