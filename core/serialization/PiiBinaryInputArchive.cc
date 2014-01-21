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

#include "PiiBinaryInputArchive.h"

PII_DEFINE_SERIALIZER(PiiBinaryInputArchive);
PII_DEFINE_FACTORY_MAP(PiiBinaryInputArchive);

PiiBinaryInputArchive::PiiBinaryInputArchive(QIODevice* d) : QDataStream(d)
{
  if (!d->isOpen())
    PII_SERIALIZATION_ERROR(StreamNotOpen);

  // Read and verify ID
  char id[PII_BINARY_ARCHIVE_ID_LEN];
  if (d->read(id, PII_BINARY_ARCHIVE_ID_LEN) != PII_BINARY_ARCHIVE_ID_LEN)
    PII_SERIALIZATION_ERROR(StreamError);
  if (std::strncmp(id, PII_BINARY_ARCHIVE_ID, PII_BINARY_ARCHIVE_ID_LEN))
    PII_SERIALIZATION_ERROR(UnrecognizedArchiveFormat);

  // Read and verify major and minor version
  int iVersion;
  *this >> iVersion;
  if (iVersion > PII_ARCHIVE_VERSION)
    PII_SERIALIZATION_ERROR(ArchiveVersionMismatch);
  setMajorVersion(iVersion);
  *this >> iVersion;
  if (iVersion > PII_BINARY_ARCHIVE_VERSION)
    PII_SERIALIZATION_ERROR(ArchiveVersionMismatch);
  setMinorVersion(iVersion);
}

void PiiBinaryInputArchive::readRawData(void* ptr, unsigned int size)
{
  if (QDataStream::readRawData(static_cast<char*>(ptr), size) != int(size))
    PII_SERIALIZATION_ERROR(StreamError);
}

PiiBinaryInputArchive& PiiBinaryInputArchive::operator>> (QString& value)
{
  // Read the raw bytes
  unsigned int len;
  char* ptr;
  this->readArray(ptr, len);
  if (ptr != 0)
    value = QString::fromUtf8(ptr, len);
  else
    value.clear();
  delete[] ptr;
  return *this;
}

PiiBinaryInputArchive& PiiBinaryInputArchive::operator>> (char*& value)
{
  unsigned int len;
  this->readArray(value, len);
  if (value != 0 && len != 0)
    // Should already be zero, but it costs next to nothing to ensure.
    value[len-1] = '\0';
  return *this;
}
