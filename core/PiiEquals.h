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
  /* The ptrEquals() function allows one to compare two objects, given
     their addresses. It uses the SFNIAE trick to implement priority
     order:

     1. If there is a function "equals()" that can be called with two
        objects of the type being compared, it will be called.

     2. If there is an operator==() overload that can be called with
        two objects of the type being compared, it will be selected next.

     3. If neither can be done, the objects are deemed to be equal if
        and only if they are in the same address.
   */

  void equals(void*, void*);

  // Used in a SFINAE
  template <std::size_t N>
  struct EqualsHelper
  {
    typedef int Type;
  };

  // Selected when there is an operator== () overload that can be
  // called with the input parameters.
  template <class T>
  bool ptrEquals2(const T* a, const T* b,
                  typename EqualsHelper<sizeof(std::declval<T>() ==
                                               std::declval<T>())>::Type = 0)
  {
    return *a == *b;
  }

  // Selected otherwise. This function will be called if and only if
  // there is no equals() or operator==() overload for the type T.
  template <class T>
  bool ptrEquals2(const void* a, const void* b)
  {
    return a == b;
  }

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
    // Repeat the trick to operator==
    return ptrEquals2<T>(reinterpret_cast<const T*>(a), reinterpret_cast<const T*>(b));
  }
}
/// @endhide

#endif //_PIIEQUALS_H
