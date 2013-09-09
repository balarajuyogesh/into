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

#include "PiiMimeTypeMap.h"

#include <QFile>
#include <QRegExp>

PiiMimeTypeMap::PiiMimeTypeMap() :
  d(new Data)
{
  d->mapTypes.insert("html", "text/html");
  d->mapTypes.insert("css", "text/css");
  d->mapTypes.insert("js", "application/javascript");
  d->mapTypes.insert("txt", "text/plain");
  d->mapTypes.insert("png", "image/png");
  d->mapTypes.insert("jpg", "image/jpeg");
  d->mapTypes.insert("pdf", "application/pdf");
}

PiiMimeTypeMap::PiiMimeTypeMap(const QString& mimeTypeFile) :
  d(new Data)
{
  readMimeTypes(mimeTypeFile);
}

PiiMimeTypeMap::~PiiMimeTypeMap()
{
  delete d;
}


void PiiMimeTypeMap::readMimeTypes(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return;
  char buffer[1024];
  QRegExp reSpace("[\t \n]+");
  while (file.readLine(buffer, 1024) > 0)
    {
      if (buffer[0] == '#') continue;
      QStringList lstParts(QString(buffer).split(reSpace, QString::SkipEmptyParts));
      if (lstParts.size() < 2) continue;
      for (int i=1; i<lstParts.size(); ++i)
        d->mapTypes.insert(lstParts[i], lstParts[0]);
    }
}

QString PiiMimeTypeMap::typeForExtension(const QString& extension) const
{
  return d->mapTypes[extension];
}

QStringList PiiMimeTypeMap::extensionsForType(const QString& mimeType) const
{
  QStringList lstResult;
  for (QMap<QString,QString>::const_iterator i=d->mapTypes.begin();
       i != d->mapTypes.end(); ++i)
    if (i.value() == mimeType)
      lstResult << i.key();
  return lstResult;
}
