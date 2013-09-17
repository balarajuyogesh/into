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

#ifndef _PIIMULTIPARTSTREAMBUFFER_H
#define _PIIMULTIPARTSTREAMBUFFER_H

#include "PiiStreamBuffer.h"

/**
 * An output filter that buffers data into itself until [flush()] is
 * called. It sends a MIME header before the actual data.
 *
 * ~~~
 * void MyHandler::handleRequest(const QString& uri,
 *                               PiiHttpDevice* h,
 *                               PiiProgressController* controller)
 * {
 *   QString strBoundary("243F6A8885A308D31319");
 *   h->setHeader("Content-Type", "multipart/mixed; boundary=\"" + strBoundary + "\"");
 *
 *   QStringList lstResponses = QStringList() << "Response1" << "Response2";
 *   for (int i=0; i<lstResponses.size(); ++i)
 *     {
 *        PiiMultipartStreamBuffer* bfr = new PiiMultipartStreamBuffer(strBoundary);
 *        bfr->setHeader("Content-Type", "text/plain");
 *        h->startOutputFiltering(bfr);
 *        h->print(lstResponses[i]);
 *        h->endOutputFiltering();
 *     }
 *   h->print("\r\n--" + strBoundary + "--\r\n");
 * }
 * ~~~
 *
 */
class PII_NETWORK_EXPORT PiiMultipartStreamBuffer : public PiiStreamBuffer
{
public:
  /**
   * Create a new multipart stream buffer.
   */
  PiiMultipartStreamBuffer(const QString& boundary);

  ~PiiMultipartStreamBuffer();
  
  /**
   * Sends all buffered data to the output device.
   */
  qint64 flushFilter();

  /**
   * Set a header field.
   */
  void setHeader(const QString& name, const QString& value);
  
  /**
   * Returns the number of bytes currently in the buffer, including
   * header information.
   */
  qint64 bufferedSize() const;

private:
  /// @internal
  class Data : public PiiStreamBuffer::Data
  {
  public:
    Data(PiiMultipartStreamBuffer* owner, const QString& boundary);
    QString strHeader, strContentType, strBoundary;
    bool bFlushed;
  };
  inline Data* _d() { return static_cast<Data*>(PiiStreamFilter::d); }
  inline const Data* _d() const { return static_cast<const Data*>(PiiStreamFilter::d); }
};


#endif //_PIIMULTIPARTSTREAMBUFFER_H
