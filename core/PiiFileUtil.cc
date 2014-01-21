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

#include "PiiFileUtil.h"

#include <QFileInfo>
#include <QCoreApplication>

namespace Pii
{
  QStringList fileList(const QString& pattern, QDir::Filters filters, QDir::SortFlags sort)
  {
    QStringList result;

    QDir directory = QFileInfo(pattern).dir();
    QString glob = QFileInfo(pattern).fileName();

    // Take the names of matching file names relative to "directory"
    QStringList names = directory.entryList(QStringList() << glob, //take all matching entries
                                            filters,
                                            sort); //sort by name
    // Add path to each name
    foreach (QString name, names)
      result << directory.path() + "/" + name;
    return result;
  }


  QString applicationBasePath()
  {
    QString strPath = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    strPath.remove(QRegExp("(/(debug|release)/[^/]+.app/Contents)?/MacOS$"));
#else
    if (strPath.endsWith("/debug"))
      strPath.chop(6);
    else if (strPath.endsWith("/release"))
      strPath.chop(8);
#endif
    return strPath;
  }

  bool deleteDirectory(const QString& dirName)
  {
    QDir dir(dirName);
    if (!dir.exists())
      return false;
    QFileInfoList lstInfos(dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot));
    for (int i=0; i<lstInfos.size(); ++i)
      {
        if (lstInfos[i].isFile() || lstInfos[i].isSymLink())
          QFile(lstInfos[i].filePath()).remove();
        else
          deleteDirectory(lstInfos[i].filePath());
      }
    QString strThisDir = dir.dirName();
    dir.cdUp();
    return dir.rmdir(strThisDir);
  }

  QString fixPath(const QString& path)
  {
    return path.endsWith("/") ? path : path + '/';
  }
}
