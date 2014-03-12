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

#ifndef _QLIST_H
#define _QLIST_H

#include <deque>
#include "linearcontainer.h"

template <class T>
class QList :
  public std::deque<T>,
  public LinearContainer<QList<T>, T>
{
public:
  typedef std::deque<T> SuperType;

  QList() {}
  QList(const QList& other) : SuperType(other) {}
#ifdef PII_CXX11
  QList(QList&& other) : SuperType(other) {}
  QList& operator= (QList&& other)
  {
    return static_cast<QList&>(SuperType::operator= (std::move(other)));
  }
#endif
  QList& operator= (const QList& other)
  {
    return static_cast<QList&>(SuperType::operator= (other));
  }

  int size() const { return int(SuperType::size()); }
};

#endif //_QLIST_H
