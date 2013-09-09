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

#include "PiiTextOutputArchive.h"

PII_DEFINE_SERIALIZER(PiiTextOutputArchive);
PII_DEFINE_FACTORY_MAP(PiiTextOutputArchive);

PiiTextOutputArchive::PiiTextOutputArchive(QIODevice* d) : QTextStream(d)
{
  if (!d->isOpen())
    PII_SERIALIZATION_ERROR(StreamNotOpen);

  setCodec("UTF-8");

  // Store archive ID
  if (d->write(PII_TEXT_ARCHIVE_ID, PII_TEXT_ARCHIVE_ID_LEN) != PII_TEXT_ARCHIVE_ID_LEN)
    PII_SERIALIZATION_ERROR(StreamError);

  // Store major archive version number
  *this << PII_ARCHIVE_VERSION;
  // Store text archive version
  *this << PII_TEXT_ARCHIVE_VERSION;
  setMinorVersion(PII_TEXT_ARCHIVE_VERSION);
}

void PiiTextOutputArchive::writeRawData(const void* ptr, unsigned int size)
{
  startDelim();
  QByteArray encoded(QByteArray::fromRawData(static_cast<const char*>(ptr), size).toBase64());
  QTextStream::operator<< ((const char*)encoded.data());
}

PiiTextOutputArchive& PiiTextOutputArchive::operator<< (const QString& value)
{
  int len = value.size();
  *this << len;
  if (len > 0)
    {
      startDelim();
      QTextStream::operator<<(value);
    }
  return *this;
}
  
PiiTextOutputArchive& PiiTextOutputArchive::operator<< (const char* value)
{
  int len = std::strlen(value);
  *this << len;
  if (len > 0)
    {
      // Separate length and data
      startDelim();
      QTextStream::operator<< (value);
    }
  return *this;
}
