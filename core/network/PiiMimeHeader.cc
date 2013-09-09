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

#include "PiiMimeHeader.h"
#include "PiiMimeException.h"
#include <PiiUtil.h>

PiiMimeHeader::Data::Data()
{}

PiiMimeHeader::Data::Data(const Data& other) :
  lstHeaders(other.lstHeaders),
  aPreamble(other.aPreamble)
{}

PiiMimeHeader::Data::~Data()
{}

PiiMimeHeader::Data* PiiMimeHeader::Data::clone() const
{
  return new Data(*this);
}

PiiMimeHeader::PiiMimeHeader() : d(new Data)
{}

PiiMimeHeader::PiiMimeHeader(const PiiMimeHeader& other) :
  d(other.d->reserved())
{}

PiiMimeHeader::PiiMimeHeader(const QByteArray& headerData) :
  d(new Data)
{
  if (!addValues(headerData))
    d->lstHeaders.clear();
}

PiiMimeHeader::PiiMimeHeader(Data* d) : d(d) {}

PiiMimeHeader::~PiiMimeHeader()
{
  d->release();
}

void PiiMimeHeader::setPreamble(const QByteArray& preamble)
{
  _d()->aPreamble = preamble;
}
  
QByteArray PiiMimeHeader::preamble() const
{
  return _d()->aPreamble;
}

void PiiMimeHeader::addValue(const QString& key, const QString& value)
{
  _d()->lstHeaders << qMakePair(key, value);
}

int PiiMimeHeader::indexOf(const QString& key) const
{
  const PII_D;
  QString strLowerKey = key.toLower();
  for (int i=0; i<d->lstHeaders.size(); ++i)
    if (d->lstHeaders[i].first.toLower() == strLowerKey)
      return i;
  return -1;
}

bool PiiMimeHeader::hasKey(const QString& key) const
{
  return indexOf(key) != -1;
}

bool PiiMimeHeader::isEmpty() const
{
  return _d()->lstHeaders.isEmpty();
}

bool PiiMimeHeader::isValid() const
{
  return !_d()->lstHeaders.isEmpty();
}


void PiiMimeHeader::setValue(const QString& key, const QString& value)
{
  int iKeyIndex = indexOf(key);
  if (iKeyIndex == -1)
    _d()->lstHeaders << qMakePair(key, value);
  else
    _d()->lstHeaders[iKeyIndex].second = value;
}

QString PiiMimeHeader::value(const QString& key) const
{
  int iKeyIndex = indexOf(key);
  if (iKeyIndex != -1)
    return _d()->lstHeaders[iKeyIndex].second;
  return QString();
}

void PiiMimeHeader::removeValue(const QString& key)
{
  int iKeyIndex = indexOf(key);
  if (iKeyIndex != -1)
    _d()->lstHeaders.removeAt(iKeyIndex);
}

void PiiMimeHeader::removeAllValues(const QString& key)
{
  PII_D;
  QString strLowerKey = key.toLower();
  for (int i=d->lstHeaders.size(); i--; )
    if (d->lstHeaders[i].first.toLower() == strLowerKey)
      d->lstHeaders.removeAt(i);
}

void PiiMimeHeader::clear()
{
  _d()->lstHeaders.clear();
}

QList<QPair<QString,QString> > PiiMimeHeader::values() const
{
  return _d()->lstHeaders;
}

bool PiiMimeHeader::addLine(const char* ptr, int length)
{
  const char* pValueStart = (const char*)(memchr(ptr, ':', length));
  if (pValueStart == 0)
    return false;
  const char* pValueEnd = ptr + length;
  int iKeyLen = pValueStart - ptr;
  while (++pValueStart < pValueEnd && *pValueStart member_of (' ', '\t')) ;
  // PENDING decoding of value (varying character sets)
  // Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?=
  // should be interpreted as "Subject: ¡Hola, señor!".
  addValue(QString(QByteArray(ptr,iKeyLen)), QString(QByteArray(pValueStart, pValueEnd - pValueStart)));
  return true;
}

bool PiiMimeHeader::addValues(const QByteArray& headerData)
{
  return addValues(headerData.constData(), headerData.size());
}


bool PiiMimeHeader::addValues(const char* headerData, int length)
{
  int iLoc = 0, iPreviousStart = 0;
  while (iLoc < length)
    {
      if (headerData[iLoc] == '\n')
        {
          if (iLoc > 0 && headerData[iLoc-1] == '\r')
            {
              if (!addLine(headerData + iPreviousStart, iLoc-iPreviousStart-1))
                return false;
            }
          else
            {
              if (!addLine(headerData + iPreviousStart, iLoc-iPreviousStart))
                return false;
            }
          iPreviousStart = iLoc+1;
        }
      ++iLoc;
    }
  return true;
}

QString PiiMimeHeader::contentDisposition() const
{
  QString strDisposition = value("Content-Disposition");
  int iSemicolonIndex = strDisposition.indexOf(';');
  if (iSemicolonIndex == -1)
    return strDisposition;
  return strDisposition.left(iSemicolonIndex).trimmed();
}

QString PiiMimeHeader::dispositionParam(const QString& param) const
{
  QString strDisposition = value("Content-Disposition");
  int iSemicolonIndex = strDisposition.indexOf(';');
  if (iSemicolonIndex != -1)
    {
      QVariantMap mapParams = Pii::decodeProperties(strDisposition.mid(iSemicolonIndex + 1),
                                                    ';', '=', '\\',
                                                    Pii::TrimPropertyName |
                                                    Pii::TrimPropertyValue |
                                                    Pii::RemoveQuotes |
                                                    Pii::DowncasePropertyName);
      return mapParams[param.toLower()].toString();
    }
  return QString();
}

QString PiiMimeHeader::contentType() const
{
  return value("Content-Type");
}

uint PiiMimeHeader::contentLength() const
{
  return value("Content-Length").toUInt();
}

void PiiMimeHeader::setContentLength(uint length)
{
  setValue("Content-Length", QString::number(length));
}

bool PiiMimeHeader::hasContentLength() const
{
  return hasKey("Content-Length");
}

bool PiiMimeHeader::isMultipart() const
{
  return contentType().startsWith("multipart/");
}

bool PiiMimeHeader::isUploadedFile() const
{
  return contentDisposition() == "form-data" &&
    !dispositionParam("filename").isEmpty();
}

QString PiiMimeHeader::controlName() const
{
  return dispositionParam("name");
}

QString PiiMimeHeader::fileName() const
{
  return dispositionParam("filename");
}

QByteArray PiiMimeHeader::boundary() const
{
  QRegExp reBoundary("boundary=(\".[^\"]+\"|[^\r\n]+)", Qt::CaseInsensitive);
  QString strBoundary;
  if (reBoundary.indexIn(contentType()) != -1)
    {
      // The first subexpression
      strBoundary = reBoundary.cap(1);
      // Get rid of quotation marks
      if (strBoundary[0] == '"' && strBoundary[strBoundary.size()-1] == '"') // cap(1) always has at least one character
        strBoundary = strBoundary.mid(1, strBoundary.size()-2);
    }
  return strBoundary.toLatin1();
}

PiiMimeHeader& PiiMimeHeader::operator= (const PiiMimeHeader& other)
{
  other.d->assignTo(d);
  return *this;
}

QByteArray PiiMimeHeader::toByteArray() const
{
  const PII_D;
  QByteArray aResult;
  for (int i=0; i<d->lstHeaders.size(); ++i)
    {
      aResult.append(d->lstHeaders[i].first.toLatin1());
      aResult.append(": ");
      // PENDING encoding
      aResult.append(d->lstHeaders[i].second.toLatin1());
      aResult.append("\r\n");
    }
  aResult.append("\r\n");
  return aResult;
}

QByteArray PiiMimeHeader::readHeaderData(QIODevice* device, qint64 maxLength, qint64* bytesRead)
{
  static const qint64 iBufferSize = 4096;
  char lineBuffer[iBufferSize];
  qint64 iHeaderSize = 0;
  QByteArray aHeader;
  bool bEolRead = true;

  for (;;)
    {
      qint64 iCurrentBlockSize = qMin(maxLength, iBufferSize);
      // If buffered data is available, empty buffer first. The buffer
      // can contain a newline and we don't even need to consult the
      // low-level device.
      if (device->bytesAvailable() > 0)
        // Need +1 because readline reserves one byte for a linefeed character.
        iCurrentBlockSize = qMin(iCurrentBlockSize, device->bytesAvailable() + 1);
      //qDebug("readHeaderData(): Bytes available: %d. Block size: %d", int(device->bytesAvailable()), int(iCurrentBlockSize));
      // Read one line of HTTP header
      qint64 iBytes = device->readLine(lineBuffer, iCurrentBlockSize);
      //qDebug("readHeaderData(): got %d bytes", (int)iBytes);

      // EOD or error in reading
      if (iBytes <= 0)
        break;

      iHeaderSize += iBytes;

      // Too many bytes in header
      if (iHeaderSize > maxLength)
        PII_THROW_MIME(HeaderTooLarge);

      // Empty line -> end of header
      if (bEolRead && *lineBuffer member_of ('\r', '\n'))
        break;

      aHeader.append(lineBuffer, iBytes);

      // Check if we got a full line.
      bEolRead = (lineBuffer[iBytes-1] == '\n');
    }

  if (bytesRead != 0)
    *bytesRead = iHeaderSize;

  return aHeader;
}
