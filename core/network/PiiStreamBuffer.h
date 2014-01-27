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

#ifndef _PIISTREAMBUFFER_H
#define _PIISTREAMBUFFER_H

#include "PiiStreamFilter.h"
#include <QBuffer>

/**
 * An output filter that buffers data into itself until [flushFilter()]
 * is called.
 *
 */
class PII_NETWORK_EXPORT PiiStreamBuffer : public QByteArray, public PiiDefaultStreamFilter
{
public:
  /**
   * Creates a new output buffer.
   */
  PiiStreamBuffer();

  ~PiiStreamBuffer();

  /**
   * Writes `maxSize` bytes of `data` to the buffer.
   */
  qint64 filterData(const char* data, qint64 maxSize);

  /**
   * Sends all buffered data to the output device.
   */
  qint64 flushFilter();

  /**
   * Returns the number of bytes currently in the buffer.
   */
  qint64 bufferedSize() const;

protected:
  /// @internal
  class Data : public PiiDefaultStreamFilter::Data
  {
  public:
    Data(PiiStreamBuffer* owner);
    QBuffer* pBuffer;
  };
  inline Data* _d() { return static_cast<Data*>(PiiStreamFilter::d); }
  inline const Data* _d() const { return static_cast<const Data*>(PiiStreamFilter::d); }

  /// @internal
  PiiStreamBuffer(Data* d);
};

#endif //_PIISTREAMBUFFER_H
