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

#include "PiiBinaryOutputArchive.h"

PII_DEFINE_SERIALIZER(PiiBinaryOutputArchive);
PII_DEFINE_FACTORY_MAP(PiiBinaryOutputArchive);

PiiBinaryOutputArchive::PiiBinaryOutputArchive(QIODevice* d) : QDataStream(d)
{
  if (!d->isOpen())
    PII_SERIALIZATION_ERROR(StreamNotOpen);

  // Store archive ID
  if (d->write(PII_BINARY_ARCHIVE_ID, PII_BINARY_ARCHIVE_ID_LEN) != PII_BINARY_ARCHIVE_ID_LEN)
    PII_SERIALIZATION_ERROR(StreamError);

  // Store archive version
  *this << PII_ARCHIVE_VERSION;
  *this << PII_BINARY_ARCHIVE_VERSION;
}

void PiiBinaryOutputArchive::writeRawData(const void* ptr, unsigned int size)
{
  // Store bytes
  if (QDataStream::writeRawData(reinterpret_cast<const char*>(ptr), size) != int(size))
    PII_SERIALIZATION_ERROR(StreamError);
}

PiiBinaryOutputArchive& PiiBinaryOutputArchive::operator<< (const QString& value)
{
  QByteArray utf8Data = value.toUtf8();
  writeArray(utf8Data.constData(), utf8Data.size());
  return *this;
}

PiiBinaryOutputArchive& PiiBinaryOutputArchive::operator<< (const char* value)
{
  // Write also the null byte at the end
  writeArray(value, strlen(value)+1);
  return *this;
}
