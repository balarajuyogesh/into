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

#ifndef _PIIBINARYOUTPUTARCHIVE_H
#define _PIIBINARYOUTPUTARCHIVE_H

#include <QDataStream>
#include <cstring>
#include "PiiArchive.h"
#include "PiiOutputArchive.h"
#include "PiiArchiveMacros.h"
#include "PiiBinaryArchive.h"

/**
 * Binary output archive stores data in a raw binary format. The
 * binary format is platform-dependent.
 *
 */
class PII_SERIALIZATION_EXPORT PiiBinaryOutputArchive :
  public PiiOutputArchive<PiiBinaryOutputArchive>,
  public PiiArchive,
  private QDataStream
{
public:
  /**
   * Construct a new binary output archive that writes data to the
   * given I/O device. The device must be open.
   *
   * @exception PiiSerializationException& if the stream is not open
   * or cannot be written to.
   */
  PiiBinaryOutputArchive(QIODevice* d);

  void writeRawData(const void* ptr, unsigned int size);

  PiiBinaryOutputArchive& operator<< (const QString& value);
  PiiBinaryOutputArchive& operator<< (const char* value);

  PII_PRIMITIVE_OUTPUT_OPERATORS(PiiBinaryOutputArchive, QDataStream)
  PII_PRIMITIVE_OPERATOR(PiiBinaryOutputArchive, char, QDataStream, <<, )
  PII_PRIMITIVE_OPERATOR(PiiBinaryOutputArchive, unsigned char, QDataStream, <<, )
  PII_DEFAULT_OUTPUT_OPERATORS(PiiBinaryOutputArchive)

protected:
  void startDelim() {}
  void endDelim() {}
};

PII_DECLARE_SERIALIZER(PiiBinaryOutputArchive);
PII_DECLARE_FACTORY_MAP(PiiBinaryOutputArchive);

#endif //_PIIBINARYOUTPUTARCHIVE_H
