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

#ifndef _PIIFILEUTIL_H
#define _PIIFILEUTIL_H

#include "PiiGlobal.h"

#include <QStringList>
#include <QDir>

namespace Pii
{
  /**
   * Returns a list of files matching a wildcard.
   *
   * @param pattern a wildcard pattern, e.g. "/usr/share/icons/img*.xpm"
   *
   * @param filters a filter for file attributes
   *
   * @param sort sort order
   *
   * @return all matching file names with full paths, sorted according
   * to `sort`.
   */
  PII_CORE_EXPORT QStringList fileList(const QString& pattern,
                                       QDir::Filters filters = QDir::Files | QDir::Readable | QDir::CaseSensitive,
                                       QDir::SortFlags sort = QDir::Name);
  /**
   * Returns the "base" path of an application without the trailing
   * slash. If the application binary is under a folder called "debug"
   * or "release", the last path component will be removed. On Mac,
   * all path components following "debug" or "release" will be
   * removed.
   */
  PII_CORE_EXPORT QString applicationBasePath();

  /**
   * Adds a slash to the end of path, if needed.
   */
  PII_CORE_EXPORT QString fixPath(const QString& path);
  
  /**
   * Deletes all files in the given directory and its sub-directories. 
   * Returns `true` on success and `false` on failure. If the
   * directory doesn't exists, return `false`. The function will try
   * to delete everything even when some files cannot be removed.
   */
  PII_CORE_EXPORT bool deleteDirectory(const QString& dirName);
}

#endif //_PIIFILEUTIL_H
