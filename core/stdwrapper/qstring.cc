/* This file is part of Into.
 * Copyright (C) Intopii 2014.
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

#include "qstring.h"
#include "qstringlist.h"
#include <cctype>

QString QString::toLower() const
{
  std::string strResult(size(), ' ');
  for (int i=0; i<size(); ++i)
    strResult.at(i) = std::tolower(at(i));
  return strResult;
}

QString QString::toUpper() const
{
  std::string strResult(size(), ' ');
  for (int i=0; i<size(); ++i)
    strResult.at(i) = std::toupper(at(i));
  return strResult;
}

QStringList QString::split(char sep, SplitBehavior behavior) const
{
  QStringList lstResult;
  std::basic_istringstream<SuperType::value_type> ss(*this);
  std::string item;
  while (std::getline(ss, item, sep))
    if (behavior == KeepEmptyParts || item.size())
      lstResult.push_back(item);
  return lstResult;
}
