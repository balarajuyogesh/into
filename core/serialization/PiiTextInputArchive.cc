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

#include "PiiTextInputArchive.h"

PII_DEFINE_SERIALIZER(PiiTextInputArchive);
PII_DEFINE_FACTORY_MAP(PiiTextInputArchive);

PiiTextInputArchive::PiiTextInputArchive(QIODevice* d) : QTextStream(d)
{
  if (!d->isOpen())
    PII_SERIALIZATION_ERROR(StreamNotOpen);

  setCodec("UTF-8");

  // Read and verify ID
  char id[PII_TEXT_ARCHIVE_ID_LEN];
  if (d->read(id, PII_TEXT_ARCHIVE_ID_LEN) != PII_TEXT_ARCHIVE_ID_LEN)
    PII_SERIALIZATION_ERROR(StreamError);
  if (std::strncmp(id, PII_TEXT_ARCHIVE_ID, PII_TEXT_ARCHIVE_ID_LEN))
    PII_SERIALIZATION_ERROR(UnrecognizedArchiveFormat);

  // Read and verify version
  int iVersion;
  *this >> iVersion;
  // Version 0 had no major version number stored.
  if (iVersion > 0)
    {
      if (iVersion > PII_ARCHIVE_VERSION)
        PII_SERIALIZATION_ERROR(ArchiveVersionMismatch);

      setMajorVersion(iVersion);
      // Read text archive version
      *this >> iVersion;
    }
  else
    setMajorVersion(0);
  if (iVersion > PII_TEXT_ARCHIVE_VERSION)
    PII_SERIALIZATION_ERROR(ArchiveVersionMismatch);

  setMinorVersion(iVersion);
}

void PiiTextInputArchive::readRawData(void* ptr, unsigned int size)
{
  startDelim();
  // Base64 encoded data has no spaces.
  QString strEncoded;
  QTextStream::operator>>(strEncoded);
  QByteArray decoded(QByteArray::fromBase64(strEncoded.toLatin1()));
  if (int(size) != decoded.size())
    PII_SERIALIZATION_ERROR(InvalidDataFormat);
  memcpy(ptr, decoded.constData(), size);
}

PiiTextInputArchive& PiiTextInputArchive::operator>> (QString& value)
{
  startDelim();
  // Read the raw bytes
  int len;
  *this >> len;
  if (len > 0)
    {
      // Read separator.
      char ws;
      QTextStream::operator>>(ws);
      // Read all characters
      value = QTextStream::read(len);
      if (value.size() != len)
        PII_SERIALIZATION_ERROR(InvalidDataFormat);
    }
  return *this;
}

PiiTextInputArchive& PiiTextInputArchive::operator>> (char*& value)
{
  unsigned len;
  *this >> len;
  // PENDING limit for len
  value = new char[len+1];
  if (len > 0)
    {
      startDelim();
      for (unsigned i=0; i<len; i++)
        QTextStream::operator>>(value[i]);
    }
  // Terminate the string with null
  value[len] = '\0';
  return *this;
}
