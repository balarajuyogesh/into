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

#ifndef _KEYVALUECONTAINER_H
#define _KEYVALUECONTAINER_H

#include <utility>

template <class SuperType> class KeyValueContainer : public SuperType
{
public:
  typedef typename SuperType::key_type key_type;
  typedef typename SuperType::mapped_type mapped_type;

  KeyValueContainer() {}
  KeyValueContainer(const KeyValueContainer& other) : SuperType(other) {}
#ifdef PII_CXX11
  KeyValueContainer(KeyValueContainer&& other) : SuperType(other) {}
#endif
  using SuperType::operator=;

  void insert(const key_type& key, const mapped_type& value)
  {
    SuperType::insert(std::make_pair(key, value));
  }

  mapped_type value(const key_type& key) const
  {
    typename SuperType::const_iterator it = this->find(key);
    if (it != this->end())
      return it->second;
    return mapped_type();
  }

  void remove(const key_type& key)
  {
    typename SuperType::iterator it = this->find(key);
    if (it != this->end())
      this->erase(it);
  }
};


#endif //_KEYVALUECONTAINER_H
