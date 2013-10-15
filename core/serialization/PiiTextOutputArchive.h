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

#ifndef _PIITEXTOUTPUTARCHIVE_H
#define _PIITEXTOUTPUTARCHIVE_H

#include <QTextStream>
#include <cstring>
#include "PiiArchive.h"
#include "PiiOutputArchive.h"  
#include "PiiArchiveMacros.h"
#include "PiiTextArchive.h"

/**
 * Text output archive stores data in a space-separated textual
 * format. The archive uses UTF-8 to encode non-ASCII characters.
 *
 */
class PII_SERIALIZATION_EXPORT PiiTextOutputArchive :
  public PiiOutputArchive<PiiTextOutputArchive>,
  public PiiArchive,
  private QTextStream
{
public:
  /**
   * Construct a new text output archive that writes data to the given
   * I/O device. The device must be open.
   *
   * @exception PiiSerializationException& if the stream is not open
   * or cannot be written to.
   */
  PiiTextOutputArchive(QIODevice* d);
  
  /**
   * Writes raw binary data to the text archive. The data is base64
   * encoded before writing.
   */
  void writeRawData(const void* ptr, unsigned int size);

  PiiTextOutputArchive& operator<< (const QString& value);  
  PiiTextOutputArchive& operator<< (const char* value);

  PiiTextOutputArchive& operator<< (char value) { return operator<<((short)value); }
  PiiTextOutputArchive& operator<< (unsigned char value) { return operator<<((unsigned short)value); }
  PII_PRIMITIVE_OUTPUT_OPERATORS(PiiTextOutputArchive, QTextStream)
  PII_DEFAULT_OUTPUT_OPERATORS(PiiTextOutputArchive)

protected:
  // Text archive separates each value by a single space.
  void startDelim() { QTextStream::operator<< (' '); }
  void endDelim() {}
};

PII_DECLARE_SERIALIZER(PiiTextOutputArchive);
PII_DECLARE_FACTORY_MAP(PiiTextOutputArchive);

#endif //_PIITEXTOUTPUTARCHIVE_H
