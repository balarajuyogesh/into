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

#include "PiiHttpResponseHeader.h"

PiiHttpResponseHeader::Data::Data() :
  httpVersion(1,1),
  iStatusCode(200),
  strReasonPhrase("OK"),
  bValid(true)
{}

PiiHttpResponseHeader::Data::Data(const Data& other) :
  PiiMimeHeader::Data(other),
  httpVersion(other.httpVersion),
  iStatusCode(other.iStatusCode),
  strReasonPhrase(other.strReasonPhrase),
  bValid(other.bValid)
{}

PiiHttpResponseHeader::Data::~Data()
{}

PiiHttpResponseHeader::Data* PiiHttpResponseHeader::Data::clone() const
{
  return new Data(*this);
}

PiiHttpResponseHeader::PiiHttpResponseHeader() :
  PiiMimeHeader(new Data)
{}

PiiHttpResponseHeader::PiiHttpResponseHeader(const QByteArray& headerData) :
  PiiMimeHeader(new Data)
{
  PII_D;
  QRegExp responseRe("^HTTP/(\\d\\.\\d)\\s+(\\d+)\\s+(\\S.*)\\r?$");
  responseRe.setMinimal(true);
  int iLineBreakPos = headerData.indexOf('\n');
  if (iLineBreakPos != -1 &&
      responseRe.indexIn(QString(headerData.left(iLineBreakPos))) != -1)
    {
      d->httpVersion = PiiVersionNumber(responseRe.cap(1));
      bool bStatusOk = false;
      d->iStatusCode = responseRe.cap(2).toInt(&bStatusOk);
      if (bStatusOk)
        {
          d->strReasonPhrase = responseRe.cap(3);
          int iRemainingLength = headerData.size() - iLineBreakPos - 1;
          if (iRemainingLength > 0 &&
              !addValues(headerData.constData() + iLineBreakPos + 1, iRemainingLength))
            d->bValid = false;
        }
      else
        d->bValid = false;
    }
  else
    d->bValid = false;
}

PiiHttpResponseHeader::PiiHttpResponseHeader(const PiiHttpResponseHeader& other) :
  PiiMimeHeader(other)
{}

PiiHttpResponseHeader::~PiiHttpResponseHeader()
{}

PiiHttpResponseHeader& PiiHttpResponseHeader::operator= (const PiiHttpResponseHeader& other)
{
  PiiMimeHeader::operator= (other);
  return *this;
}

void PiiHttpResponseHeader::setStatusCode(int statusCode) { _d()->iStatusCode = statusCode; }
int PiiHttpResponseHeader::statusCode() const { return _d()->iStatusCode; }
void PiiHttpResponseHeader::setReasonPhrase(const QString& reasonPhrase) { _d()->strReasonPhrase = reasonPhrase; }
QString PiiHttpResponseHeader::reasonPhrase() const { return _d()->strReasonPhrase; }
void PiiHttpResponseHeader::setHttpVersion(const PiiVersionNumber& httpVersion) { _d()->httpVersion = httpVersion; }
PiiVersionNumber PiiHttpResponseHeader::httpVersion() const { return _d()->httpVersion; }

void PiiHttpResponseHeader::setStatusLine(int statusCode, const QString& reasonPhrase)
{
  PII_D;
  d->iStatusCode = statusCode;
  d->strReasonPhrase = reasonPhrase;
}

QByteArray PiiHttpResponseHeader::toByteArray() const
{
  const PII_D;
  QByteArray aResult;
  aResult.append("HTTP/");
  aResult.append(d->httpVersion.toString().toLatin1());
  aResult.append(' ');
  aResult.append(QByteArray::number(d->iStatusCode));
  aResult.append(' ');
  aResult.append(d->strReasonPhrase.toLatin1());
  aResult.append("\r\n");
  aResult.append(PiiMimeHeader::toByteArray());
  return aResult;
}

bool PiiHttpResponseHeader::isValid() const
{
  return _d()->bValid;
}
