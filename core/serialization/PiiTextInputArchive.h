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

#ifndef _PIITEXTINPUTARCHIVE_H
#define _PIITEXTINPUTARCHIVE_H

#include <QTextStream>
#include "PiiArchive.h"
#include "PiiInputArchive.h"
#include "PiiArchiveMacros.h"
#include "PiiTextArchive.h"
#include <cstring>

/* PENDING This class and siblings should perhaps be separated into
 * two parts: PiiTextInputArchiveBase which does not derive
 * PiiInputArchive and the actual implementation -
 * PiiTextInputArchive - that derives PiiTextInputArchiveBase and
 * PiiInputArchive. Currently, the default operators (pointer
 * tracking stuff) are instantiated even with polymorphic archive
 * types, which implement them anyway. The polymorphic types could use
 * PiiTextInputArchiveBase as the template argument to avoid this.
 */


/**
 * TextInputArchive reads space-separated textual data. All non-ASCII
 * characters need to be UTF-8 encoded.
 *
 */
class PII_SERIALIZATION_EXPORT PiiTextInputArchive :
  public PiiInputArchive<PiiTextInputArchive>,
  public PiiArchive,
  private QTextStream
{
public:
  /**
   * Construct a new text input archive that reads the given I/O
   * device. The device must be open.
   *
   * @exception PiiSerializationException& if the device is not open,
   * or it cannot be read from, or the archive format is unknown
   */
  PiiTextInputArchive(QIODevice* d);

  /**
   * Read raw binary data from the text archive. The data is base64
   * decoded after reading.
   */
  void readRawData(void* ptr, unsigned int size);

  PiiTextInputArchive& operator>> (QString& value);

  PiiTextInputArchive& operator>> (char*& value);

  PiiTextInputArchive& operator>> (char& value)
  {
    short shrt;
    QTextStream::operator>>(shrt);
    value = (char)shrt;
    return *this;
  }
  PiiTextInputArchive& operator>> (unsigned char& value)
  {
    unsigned short shrt;
    QTextStream::operator>>(shrt);
    value = (unsigned char)shrt;
    return *this;
  }
  PiiTextInputArchive& operator>> (float& value)
  {
    return readFloat(value);
  }

  PiiTextInputArchive& operator>> (double& value)
  {
    return readFloat(value);
  }

  PII_DEFAULT_INPUT_OPERATORS(PiiTextInputArchive)
  PII_INTEGER_INPUT_OPERATORS(PiiTextInputArchive, QTextStream)

protected:
  // Text archive sucks all white space before reading the actual value.
  void startDelim() { QTextStream::skipWhiteSpace(); }
  void endDelim() {}

private:
  template <class T> PiiTextInputArchive& readFloat(T& value)
  {
    // Read the number as text to catch "nan", "inf", and "-inf"
    startDelim();
    QString valueTxt;
    QTextStream::operator>> (valueTxt);
    bool ok;
    value = (T)valueTxt.toDouble(&ok);
    if (!ok)
      PII_SERIALIZATION_ERROR(InvalidDataFormat);
    return *this;
  }
};

PII_DECLARE_SERIALIZER(PiiTextInputArchive);
PII_DECLARE_FACTORY_MAP(PiiTextInputArchive);

#endif //_PIITEXTINPUTARCHIVE_H
