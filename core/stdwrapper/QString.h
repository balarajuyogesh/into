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

#ifndef _QSTRING_H
#define _QSTRING_H

#include <string>
#include <sstream>

class PII_CORE_EXPORT QString : public std::string
{
public:
  typedef std::string Super;
  
  QString() {}
  QString(const Super::value_type* str) : Super(str) {}
  QString(const Super& other) : Super(other) {}
#ifdef PII_CXX11
  QString(Super&& other) : Super(other) {}
  using Super::operator=;
#else
  QString& operator= (const Super& other) { Super::operator= (other); return *this; }
#endif

  using Super::operator+=;

  // An incorrect but sufficient imitation of QString::arg().
  template <class T> QString arg(const T& value) const
  {
    size_type iFirst = find('%');
    while (iFirst != -1)
      {
        size_type iLast = iFirst + 1;
        if (iLast == size())
          break;
        if (at(iLast) == '%')
          {
            iFirst = find('%', iLast + 1);
            continue;
          }
        while (iLast < size() && at(iLast) >= '0' && at(iLast) <= '9') ++iLast;
        if (iLast != iFirst + 1)
          {
            std::basic_ostringstream<Super::value_type> stream;
            stream << substr(0, iFirst) << value << substr(iLast);
            return stream.str();
          }
        iFirst = find('%', iFirst + 1);
      }
    return *this;
  }
};

#endif //_QSTRING_H
