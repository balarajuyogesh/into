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

#ifndef _PIIIMAGETRAITS_H
#define _PIIIMAGETRAITS_H

namespace PiiImage
{
  /**
   * Traits for integer-valued images.
   */
  template <class T> struct IntegerTraits
  {
    /**
     * Returns the maximum value of a color channel. The maximum
     * channel value is 255 for all integer types and 1.0 for
     * floating-point types.
     */
    static T max() { return 255; }
    template <class U> static U toFloat(T value) { return U(value)/max(); }
    template <class U> static U toInt(T value) { return U(value); }
    template <class U> static T fromFloat(U value) { return T(value * max()); }
    template <class U> static T fromInt(U value) { return T(value); }
  };
  
  /**
   * Traits for floating-point images.
   */
  template <class T> struct FloatTraits
  {
    static T max() { return 1.0; }
    template <class U> static U toFloat(T value) { return U(value); }
    template <class U> static U toInt(T value) { return U(value * IntegerTraits<U>::max()); }
    template <class U> static T fromFloat(U value) { return T(value); }
    template <class U> static T fromInt(U value) { return T(value) / IntegerTraits<U>::max(); }
  };
  
  /**
   * The Traits structure specifies default properties of different
   * image types.
   */
  template <class T> struct Traits : Pii::IfClass<Pii::IsInteger<T>, IntegerTraits<T>, FloatTraits<T> >::Type
  {
  };

  /**
   * Specialization for three-channel colors. Just maps the color type
   * to the channel type.
   */
  template <class T> struct Traits<PiiColor<T> > : Traits<T> {};
  /**
   * Specialization for four-channel colors. Just maps the color type
   * to the channel type.
   */
  template <class T> struct Traits<PiiColor4<T> > : Traits<T> {};
};

#endif //_PIIIMAGETRAITS_H
