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

#ifndef _QVECTOR_H
#define _QVECTOR_H

#include <vector>
#include "linearcontainer.h"

struct QVectorBoolWrapper
{
  QVectorBoolWrapper(bool b = false) : b(b) {}
  operator bool() const { return b; }
  bool b;
};

// HACK std::vector<bool> is actually a bitset, which is probably the
// stupidest thing the standards commitee did. (Hey, I know what we
// can use this fancy new feature called "specialization" for!) We
// need to use a wrapper for booleans to make the interface sane.
template <class T> struct QVectorType { typedef T Type; };
template <> struct QVectorType<bool> { typedef QVectorBoolWrapper Type; };

template <class T>
class QVector :
  public std::vector<typename QVectorType<T>::Type >,
  public LinearContainer<QVector<T>, T>
{
public:
  typedef std::vector<typename QVectorType<T>::Type> SuperType;
  QVector() {}
  QVector(const QVector& other) : SuperType(other) {}
  QVector(int size, const T& value) : SuperType(size, value) {}
  QVector(int size) : SuperType(size, T()) {}
#ifdef PII_CXX11
  QVector(QVector&& other) : SuperType(other) {}
  QVector& operator= (QVector&& other)
  {
    return static_cast<QVector&>(SuperType::operator= (std::move(other)));
  }
#endif
  QVector& operator= (const QVector& other)
  {
    return static_cast<QVector&>(SuperType::operator= (other));
  }

  int size() const { return int(SuperType::size()); }

  T* data() { return &(*this->begin()); }
  const T* data() const { return &(*this->begin()); }
  const T* constData() const { return data(); }

  void fill(const T& value, int size = -1)
  {
    if (size != -1)
      {
        while (this->size() > size)
          this->pop_back();
        setAll(value);
        while (this->size() < size)
          this->push_back(value);
      }
    else
      setAll(value);
  }

private:
  void setAll(const T& value)
  {
    for (int i=0; i<this->size(); ++i)
      this->at(i) = value;
  }
};

#endif //_QVECTOR_H
