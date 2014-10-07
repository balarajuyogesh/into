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

#ifndef _PIIEQUALS_H
#define _PIIEQUALS_H

/// @hide
namespace Pii
{
  void equals(void*, void*);

  // Used in a SFINAE
  template <std::size_t N>
  struct EqualsHelper
  {
    typedef int Type;
  };

  // Selected when there is an equals() overload that can be called
  // with the input parameters.
  template <class T>
  bool ptrEquals(const T* a, const T* b,
                 typename EqualsHelper<sizeof(equals(std::declval<T>(),
                                                     std::declval<T>()))>::Type = 0)
  {
    return Pii::equals(*a, *b);
  }

  // Selected otherwise.
  template <class T>
  bool ptrEquals(const void* a, const void* b)
  {
    return *reinterpret_cast<const T*>(a) == *reinterpret_cast<const T*>(b);
  }
}
  /// @endhide

#endif //_PIIEQUALS_H
