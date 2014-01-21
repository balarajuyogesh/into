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

#ifndef _PIIHTTPRESPONSEHEADER_H
#define _PIIHTTPRESPONSEHEADER_H

#include "PiiMimeHeader.h"
#include <PiiVersionNumber.h>

class PII_NETWORK_EXPORT PiiHttpResponseHeader : public PiiMimeHeader
{
public:
  PiiHttpResponseHeader();
  PiiHttpResponseHeader(const PiiHttpResponseHeader& other);
  PiiHttpResponseHeader(const QByteArray& headerData);
  ~PiiHttpResponseHeader();
  PiiHttpResponseHeader& operator= (const PiiHttpResponseHeader& other);

  void setHttpVersion(const PiiVersionNumber& httpVersion);
  PiiVersionNumber httpVersion() const;

  void setStatusCode(int statusCode);
  int statusCode() const;

  void setReasonPhrase(const QString& reasonPhrase);
  QString reasonPhrase() const;

  void setStatusLine(int statusCode, const QString& reasonPhrase);

  bool isValid() const;

  QByteArray toByteArray() const;

protected:
  class Data : public PiiMimeHeader::Data
  {
  public:
    Data();
    Data(const Data& other);
    ~Data();
    Data* clone() const;

    PiiVersionNumber httpVersion;
    int iStatusCode;
    QString strReasonPhrase;
    bool bValid;
  };
  PII_SHARED_D_FUNC;
};

#endif //_PIIHTTPRESPONSEHEADER_H
