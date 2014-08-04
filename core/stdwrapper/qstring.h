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

#include <PiiGlobal.h>
#include <string>
#include <sstream>

class QStringList;

class PII_CORE_EXPORT QString : public std::string
{
public:
  typedef std::string SuperType;
  enum SplitBehavior { KeepEmptyParts, SkipEmptyParts };

  QString() {}
  QString(const SuperType::value_type* str) : SuperType(str) {}
  QString(const SuperType& other) : SuperType(other) {}

  QString(const QString& other) : SuperType(other) {}
  ~QString() {}
#ifdef PII_CXX11
  QString(SuperType&& other) : SuperType(other) {}
#endif
  using SuperType::operator=;

  QString& append(const SuperType& str)
  {
    SuperType::append(str);
    return *this;
  }

  QString& append(char chr)
  {
    push_back(chr);
    return *this;
  }

  int indexOf(char chr, int from = 0) const
  {
    size_type i = find(chr, from);
    return i != npos ? i : -1;
  }

  bool isEmpty() const { return size() == 0; }
  int size() const { return int(SuperType::size()); }
  int length() const { return int(SuperType::size()); }

  QString mid(int pos, int n = -1) const
  {
    if (n < 0) n = size() + n + 1 - pos;
    return substr(pos, n);
  }

  template <class T>
  static QString number(T n)
  {
    QString strResult;
    std::basic_ostringstream<SuperType::value_type> ss(strResult);
    ss << n;
    return ss.str();
  }

  template <class T>
  bool to(T& value) const
  {
    std::basic_istringstream<SuperType::value_type> ss(*this);
    return ss >> value;
  }

  template <class T>
  T to(bool* ok = 0) const
  {
    T result = T();
    bool bOk = to(result);
    if (ok) *ok = bOk;
    return result;
  }

  int toInt(bool* bOk = 0) const { return to<int>(bOk); }

  using SuperType::operator+=;

  // An incorrect but sufficient imitation of QString::arg().
  template <class T> QString arg(const T& value) const
  {
    size_type iFirst = find('%');
    while (iFirst != npos)
      {
        size_type iLast = iFirst + 1;
        if (iLast == SuperType::size())
          break;
        if (at(iLast) == '%')
          {
            iFirst = find('%', iLast + 1);
            continue;
          }
        while (iLast < SuperType::size() && at(iLast) >= '0' && at(iLast) <= '9') ++iLast;
        if (iLast != iFirst + 1)
          {
            std::basic_ostringstream<SuperType::value_type> stream;
            stream << substr(0, iFirst) << value << substr(iLast);
            return stream.str();
          }
        iFirst = find('%', iFirst + 1);
      }
    return *this;
  }

  QStringList split(char sep, SplitBehavior behavior = KeepEmptyParts) const;

  QString toLower() const;
  QString toUpper() const;
};

namespace Pii
{
  template <class T> inline T stringTo(const QString& number, bool* ok = 0) { return number.to<T>(ok); }
}

#endif //_QSTRING_H
