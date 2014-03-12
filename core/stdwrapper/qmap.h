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

#ifndef _QMAP_H
#define _QMAP_H

#include <map>
#include "keyvaluecontainer.h"

template <class Key, class Value> class QMap :
  public KeyValueContainer<std::map<Key,Value> >
{
public:
  typedef KeyValueContainer<std::map<Key,Value> > SuperType;
  QMap() {}
  QMap(const QMap& other) : SuperType(other) {}
#ifdef PII_CXX11
  QMap(QMap&& other) : SuperType(other) {}
  QMap& operator= (QMap&& other)
  {
    return static_cast<QMap&>(std::move(other));
  }
#endif
  QMap& operator= (const QMap& other)
  {
    return static_cast<QMap&>(other);
  }
};

#endif //_QMAP_H
