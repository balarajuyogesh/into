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

#ifndef _LINEARCONTAINER_H
#define _LINEARCONTAINER_H

#include <algorithm>

template <class Derived, class T> class LinearContainer
{
public:
  int indexOf(const T& elem) const
  {
    typename Derived::const_iterator it = std::find(self()->begin(), self()->end(), elem);
    if (it != self()->end())
      return int(it - self()->begin());
    return -1;
  }

  T& last() { return self()->back(); }
  const T& last() const { return self()->back(); }

  T& first() { return self()->front(); }
  const T& first() const { return self()->front(); }

  bool isEmpty() const { return self()->size() == 0; }

  void removeAt(int i) { self()->erase(self()->begin() + i); }
  void removeLast() { self()->pop_back(); }
  void append(const T& value) { self()->push_back(value); }
  void append(const Derived& values)
  {
    for (int i=0; i<values.size(); ++i)
      self()->push_back(values.at(i));
  }

  Derived& operator<< (const T& value)
  {
    append(value);
    return *self();
  }

private:
  Derived* self() { return static_cast<Derived*>(this); }
  const Derived* self() const { return static_cast<const Derived*>(this); }
};

#endif //_LINEARCONTAINER_H
