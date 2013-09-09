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

#ifndef _PIIMIMETYPEMAP_H
#define _PIIMIMETYPEMAP_H

#include "PiiNetwork.h"
#include <QMap>
#include <QStringList>

class PII_NETWORK_EXPORT PiiMimeTypeMap
{
public:
  PiiMimeTypeMap();
  PiiMimeTypeMap(const QString& mimeTypeFile);
  ~PiiMimeTypeMap();

  QString typeForExtension(const QString& extension) const;
  QStringList extensionsForType(const QString& mimeType) const;

private:
  class Data
  {
  public:
    QMap<QString,QString> mapTypes;
  } *d;

  void readMimeTypes(const QString& fileName);
};

#endif //_PIIMIMETYPEMAP_H
