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

#ifndef _PIIHTTPREQUESTHEADER_H
#define _PIIHTTPREQUESTHEADER_H

#include "PiiMimeHeader.h"
#include <PiiVersionNumber.h>

class PII_NETWORK_EXPORT PiiHttpRequestHeader : public PiiMimeHeader
{
public:
  PiiHttpRequestHeader();
  PiiHttpRequestHeader(const PiiHttpRequestHeader& other);
  PiiHttpRequestHeader(const QByteArray& headerData);
  ~PiiHttpRequestHeader();
  PiiHttpRequestHeader& operator= (const PiiHttpRequestHeader& other);

  void setMethod(const QString& method);
  QString method() const;

  void setPath(const QString& path);
  QString path() const;

  void setRequest(const QString& method, const QString& path);

  void setHttpVersion(const PiiVersionNumber& httpVersion);
  PiiVersionNumber httpVersion() const;

  bool isValid() const;

  QByteArray toByteArray() const;

protected:
  /// @internal
  class Data : public PiiMimeHeader::Data
  {
  public:
    Data();
    Data(const Data& other);
    ~Data();
    Data* clone() const;

    QString strMethod;
    QString strPath;
    PiiVersionNumber httpVersion;
    bool bValid;
  };
  PII_SHARED_D_FUNC;
};

#endif //_PIIHTTPREQUESTHEADER_H
