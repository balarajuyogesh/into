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

#ifndef _PIIVALUESET_H
#define _PIIVALUESET_H

#include <cstdarg>
#include <cstring>
#include <iterator>
#include "PiiTypeTraits.h"


/**
 * A macro that allows one to use a convenient syntax for checking if
 * a value is a member of a set. With compiler optimizations turned
 * on, this technique is equally fast as direct comparison.
 *
 * ~~~(c++)
 * if (rand() % 5 member_of (1, 2, 3))
 *   cout << "You had luck.\n";
 * const char* fruit = "Apple";
 * if (fruit member_of ("Apple", "Orange", "Pear"))
 *   cout << "Yes, this works too.\n";
 * ~~~
 *
 * @see not_member_of
 */
#define member_of == Pii::valueSet

/**
 * A macro that allows one to use a convenient syntax for checking if
 * a value is *not* a member of a set. With compiler optimizations
 * turned on, this technique is equally fast as direct comparison.
 *
 * ~~~(c++)
 * if (1 not_member_of (1, 2, 3))
 *   cout << "Now, this is weird.\n";
 * const char* fruit = "Apple";
 * if (fruit not_member_of ("Banana", "Orange", "Pear"))
 *   cout << "Comparing apples to oranges is not fair.\n";
 * ~~~
 *
 * @see member_of
 */
#define not_member_of != Pii::valueSet

/// @hide

#ifndef PII_NO_QT
namespace std
{
  template <class T> struct iterator_traits<QList<T> >
  {
    typedef typename std::random_access_iterator_tag iterator_category;
    typedef typename QList<T>::value_type            value_type;
    typedef typename QList<T>::difference_type       difference_type;
    typedef typename QList<T>::pointer               pointer;
    typedef typename QList<T>::reference             reference;
  };
}
#endif

namespace Pii
{
  template <class T> inline bool valueSetEqual(T v1, T v2) { return v1 == v2; }
  inline bool valueSetEqual(const char* v1, const char* v2) { return !strcmp(v1, v2); }
}

template <class T, int N> struct PiiValueSet
{
  typedef typename Pii::VaArg<T>::Type VaArgType;
  
  PiiValueSet(VaArgType firstValue, ...)
  {
    values[0] = firstValue;
    va_list argp;
    // initalize var ptr
    va_start(argp, firstValue);
    // repeat for each arg
    for (int i=1; i<N; ++i)
      values[i] = static_cast<T>(va_arg(argp, VaArgType));

    // done with args
    va_end(argp);
  }
  T values[N];
};
template <class T, int N> bool operator== (T v, const PiiValueSet<T,N>& set)
{
  for (int i=0; i<N; ++i)
    if (Pii::valueSetEqual(v, set.values[i]))
      return true;
  return false;
}
template <class T, int N> bool operator!= (T v, const PiiValueSet<T,N>& set)
{
  for (int i=0; i<N; ++i)
    if (Pii::valueSetEqual(v, set.values[i]))
      return false;
  return true;
}

template <class T> struct PiiValueSet<T,2>
{
  PiiValueSet(T val1, T val2) : v1(val1), v2(val2) {}
  T v1, v2;
};
template <class T> bool operator== (T v, const PiiValueSet<T,2>& set)
{
  return Pii::valueSetEqual(v, set.v1) || Pii::valueSetEqual(v, set.v2);
}
template <class T> bool operator!= (T v, const PiiValueSet<T,2>& set)
{
  return !Pii::valueSetEqual(v, set.v1) && !Pii::valueSetEqual(v, set.v2);
}

template <class T> struct PiiValueSet<T,3>
{
  PiiValueSet(T val1, T val2, T val3) : v1(val1), v2(val2), v3(val3) {}
  T v1, v2, v3;
};
template <class T> bool operator== (T v, const PiiValueSet<T,3>& set)
{
  return Pii::valueSetEqual(v, set.v1) || Pii::valueSetEqual(v, set.v2) || Pii::valueSetEqual(v, set.v3);
}
template <class T> bool operator!= (T v, const PiiValueSet<T,3>& set)
{
  return !Pii::valueSetEqual(v, set.v1) && !Pii::valueSetEqual(v, set.v2) && !Pii::valueSetEqual(v, set.v3);
}

template <class T> struct PiiValueSet<T,4>
{
  PiiValueSet(T val1, T val2,
              T val3, T val4) :
    v1(val1), v2(val2), v3(val3), v4(val4) {}
  T v1, v2, v3, v4;
};
template <class T> bool operator== (T v, const PiiValueSet<T,4>& set)
{
  return Pii::valueSetEqual(v, set.v1) || Pii::valueSetEqual(v, set.v2) ||
    Pii::valueSetEqual(v, set.v3) || Pii::valueSetEqual(v, set.v4);
}
template <class T> bool operator!= (T v, const PiiValueSet<T,4>& set)
{
  return !Pii::valueSetEqual(v, set.v1) && !Pii::valueSetEqual(v, set.v2) &&
    !Pii::valueSetEqual(v, set.v3) && !Pii::valueSetEqual(v, set.v4);
}

template <class T> struct PiiValueSet<T,5>
{
  PiiValueSet(T val1, T val2,
              T val3, T val4,
              T val5) :
    v1(val1), v2(val2), v3(val3), v4(val4), v5(val5) {}
  T v1, v2, v3, v4, v5;
};
template <class T> bool operator== (T v, const PiiValueSet<T,5>& set)
{
  return Pii::valueSetEqual(v, set.v1) || Pii::valueSetEqual(v, set.v2) ||
    Pii::valueSetEqual(v, set.v3) || Pii::valueSetEqual(v, set.v4) ||
    Pii::valueSetEqual(v, set.v5);
}
template <class T> bool operator!= (T v, const PiiValueSet<T,5>& set)
{
  return !Pii::valueSetEqual(v, set.v1) && !Pii::valueSetEqual(v, set.v2) &&
    !Pii::valueSetEqual(v, set.v3) && !Pii::valueSetEqual(v, set.v4) &&
    !Pii::valueSetEqual(v, set.v5);
}

template <class Iterable> struct PiiIterableValueSet
{
public:
  PiiIterableValueSet(const Iterable& iterable) : iterable(iterable) {}
  const Iterable& iterable;
};

template <class Iterable> bool operator== (typename std::iterator_traits<Iterable>::value_type v,
                                           const PiiIterableValueSet<Iterable>& set)
{
  for (typename Iterable::const_iterator it=set.iterable.begin(); it != set.iterable.end(); ++it)
    if (*it == v)
      return true;
  return false;
}
template <class Iterable> bool operator!= (typename std::iterator_traits<Iterable>::value_type v,
                                           const PiiIterableValueSet<Iterable>& set)
{
  for (typename Iterable::const_iterator it=set.iterable.begin(); it != set.iterable.end(); ++it)
    if (*it != v)
      return false;
  return true;
}

namespace Pii
{
  template <class Iterable> PiiIterableValueSet<Iterable> inline valueSet(const Iterable& iterable)
  { return PiiIterableValueSet<Iterable>(iterable); }
  template <class T> PiiValueSet<T,2> inline valueSet(T v1, T v2)
  { return PiiValueSet<T,2>(v1, v2); }
  template <class T> PiiValueSet<T,3> inline valueSet(T v1, T v2, T v3)
  { return PiiValueSet<T,3>(v1, v2, v3); }
  template <class T> PiiValueSet<T,4> inline valueSet(T v1, T v2, T v3, T v4)
  { return PiiValueSet<T,4>(v1, v2, v3, v4); }
  template <class T> PiiValueSet<T,5> inline valueSet(T v1, T v2, T v3, T v4, T v5)
  { return PiiValueSet<T,5>(v1, v2, v3, v4, v5); }
  template <class T> PiiValueSet<T,6> inline valueSet(T v1, T v2, T v3, T v4, T v5, T v6)
  { return PiiValueSet<T,6>(v1, v2, v3, v4, v5, v6); }
  template <class T> PiiValueSet<T,7> inline valueSet(T v1, T v2, T v3, T v4, T v5, T v6, T v7)
  { return PiiValueSet<T,7>(v1, v2, v3, v4, v5, v6, v7); }
  template <class T> PiiValueSet<T,8> inline valueSet(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8)
  { return PiiValueSet<T,8>(v1, v2, v3, v4, v5, v6, v7, v8); }
  template <class T> PiiValueSet<T,9> inline valueSet(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9)
  { return PiiValueSet<T,9>(v1, v2, v3, v4, v5, v6, v7, v8, v9); }
  template <class T> PiiValueSet<T,10> inline valueSet(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9, T v10)
  { return PiiValueSet<T,10>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10); }
}

/// @endhide

#endif //_PIIVALUESET_H
