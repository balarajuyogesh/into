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

#ifndef _PIIFILEINFO_H
#define _PIIFILEINFO_H

#include "PiiNetwork.h"
#include <PiiSharedD.h>
#include <QDateTime>

class PII_NETWORK_EXPORT PiiFileInfo
{
public:
  PiiFileInfo();
  PiiFileInfo(const PiiFileInfo& other);
  PiiFileInfo(const QString& uri);
  ~PiiFileInfo();

  PiiFileInfo& operator= (const PiiFileInfo& other);

  bool exists() const;
  QDateTime lastModified() const;

  void refresh();

  void setUri(const QString& uri);

private:
  class Data : public PiiSharedD<Data>
  {
  public:
    Data();
    Data(const QString& uri);

    QString strUri;
    bool bExists;
    QDateTime lastModifiedTime;
  } *d;
};

#endif //_PIIFILEINFO_H
