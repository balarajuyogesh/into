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

#ifndef _PIIBINARYINPUTARCHIVE_H
#define _PIIBINARYINPUTARCHIVE_H

#include <QDataStream>
#include <cstring>
#include "PiiArchive.h"
#include "PiiInputArchive.h"
#include "PiiArchiveMacros.h"
#include "PiiBinaryArchive.h"

/**
 * PiiBinaryInputArchive reads raw binary data. The binary format is
 * platform-dependent.
 *
 */
class PII_SERIALIZATION_EXPORT PiiBinaryInputArchive :
  public PiiInputArchive<PiiBinaryInputArchive>,
  public PiiArchive,
  private QDataStream
{
public:
  /**
   * Construct a new binary input archive that reads the given I/O
   * device. The device must be open.
   *
   * @exception PiiSerializationException& if the device is not open,
   * or it cannot be read from, or the archive format is unknown
   */
  PiiBinaryInputArchive(QIODevice* d);

  void readRawData(void* ptr, unsigned int size);

  PiiBinaryInputArchive& operator>> (QString& value);

  PiiBinaryInputArchive& operator>> (char*& value);

  PiiBinaryInputArchive& operator>> (char& value) { return operator>> ((signed char&)value); }
  PiiBinaryInputArchive& operator>> (unsigned char& value) { QDataStream::operator>>(value); return *this; }

  PII_PRIMITIVE_INPUT_OPERATORS(PiiBinaryInputArchive, QDataStream)
  PII_PRIMITIVE_OPERATOR(PiiBinaryInputArchive, signed char, QDataStream, >>, &)
  PII_DEFAULT_INPUT_OPERATORS(PiiBinaryInputArchive)

protected:
  void startDelim() {}
  void endDelim() {}
};

PII_DECLARE_SERIALIZER(PiiBinaryInputArchive);
PII_DECLARE_FACTORY_MAP(PiiBinaryInputArchive);

#endif //_PIIBINARYINPUTARCHIVE_H
