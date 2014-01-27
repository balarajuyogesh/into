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

#include "PiiMultipartStreamBuffer.h"

PiiMultipartStreamBuffer::Data::Data(PiiMultipartStreamBuffer* owner, const QString& boundary) :
  PiiStreamBuffer::Data(owner),
  strHeader(QString("\r\n"
                    "--%1\r\n"
                    "Content-Length: %2\r\n").arg(boundary)),
  bFlushed(false)
{
}

PiiMultipartStreamBuffer::PiiMultipartStreamBuffer(const QString& boundary) :
  PiiStreamBuffer(new Data(this, boundary))
{
}

PiiMultipartStreamBuffer::~PiiMultipartStreamBuffer()
{
}

void PiiMultipartStreamBuffer::setHeader(const QString& name, const QString& value)
{
  _d()->strHeader += name + ": " + value + "\r\n";
}

qint64 PiiMultipartStreamBuffer::flushFilter()
{
  PII_D;
  if (d->pOutputFilter)
    // Send MIME header and buffered body
    return d->pOutputFilter->filterData((d->strHeader.arg(PiiStreamBuffer::bufferedSize()) + "\r\n").toLatin1()) +
      PiiStreamBuffer::flushFilter();
  d->bFlushed = true;
  return 0;
}

qint64 PiiMultipartStreamBuffer::bufferedSize() const
{
  const PII_D;
  if (d->bFlushed) return 0;

  qint64 iBodyLength = PiiStreamBuffer::bufferedSize();
  QString strBodyLength = QString::number(iBodyLength);
  // In d->strHeader, %2 needs to be taken away, but the final \r\n
  // added in flushFilter() cancels the effect.
  return iBodyLength + d->strHeader.size() + strBodyLength.size();
}
