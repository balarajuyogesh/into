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

#ifndef _QHASH_H
#define _QHASH_H

#ifdef PII_CXX11 // C++11 provides a hash table!
#  include <unordered_map>
#  include "keyvaluecontainer.h"

template <class Key, class Value> class QHash :
  public KeyValueContainer<std::unordered_map<Key,Value> >
{
public:
  typedef KeyValueContainer<std::unordered_map<Key,Value> > SuperType;
  QHash() {}
  QHash(const QHash& other) : SuperType(other) {}
#ifdef PII_CXX11
  QHash(QHash&& other) : SuperType(other) {}
  QHash& operator= (QHash&& other)
  {
    return static_cast<QHash&>(std::move(other));
  }
#endif
  QHash& operator= (const QHash& other)
  {
    return static_cast<QHash&>(other);
  }
};

#else // Suboptimal, but should work
#  include "qmap.h"
#  define QHash QMap
#endif

#endif //_QHASH_H
