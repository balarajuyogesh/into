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

#ifndef _QSTACK_H
#define _QSTACK_H

#include "qvector.h"

template <class T>
class QStack : public QVector<T>
{
public:
  typedef QVector<T> SuperType;

  QStack() {}
  QStack(const QStack& other) : SuperType(other) {}
#ifdef PII_CXX11
  QStack(QStack&& other) : SuperType(other) {}
  QStack& operator= (QStack&& other)
  {
    return static_cast<QStack&>(SuperType::operator= (std::move(other)));
  }
#endif
  QStack& operator= (const QStack& other)
  {
    return static_cast<QStack&>(SuperType::operator= (other));
  }

  T pop()
  {
    T val(this->back());
    this->pop_back();
    return val;
  }

  void push(const T& val) { this->push_back(val); }
  T& top() { return this->back(); }
  const T& top() const { return this->back(); }
};

#endif //_QSTACK_H
