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

#ifndef _PIIARITHMETICBASE_H
#define _PIIARITHMETICBASE_H

#include <PiiFunctional.h>
#include <functional>

template <class T> struct PiiArithmeticTraits;

/**
 * A template that adds support for arithmetic operators for any data
 * structure that can be iterated over. This class is intended to be
 * used with the CRTP (curiously recurring template pattern). The
 * derived class must have have stl-style `begin`() and `end`()
 * functions for iterators. Information about the element type itself
 * is provided through the helper type `Traits`. The required type
 * definitions are shown in the example below.
 *
 * ~~~(c++)
 * // Forward declaration for the rebinder in MyTraits.
 * template <class T> class MyClass;
 *
 * template <class T> struct PiiArithmeticTraits<MyClass<T> >
 * {
 *   // The element type
 *   typedef T Type;
 *   // Iterator type
 *   typedef T* Iterator;
 *   // Const iterator type
 *   typedef const T* ConstIterator;
 *   // Rebinder creates a MyClass types with varying content types.
 *   template <class U> struct Rebind { typedef MyClass<U> Type; };
 * };
 *
 * template <class T> class MyClass : public PiiArithmeticBase<MyClass<T> >
 * {
 * public:
 *   typedef PiiArithmeticBase<MyClass<T> > BaseType;
 *
 *   // Required iterator functions
 *   typename BaseType::ConstIterator begin() const { return _pData; }   // const iterator to the beginning
 *   typename BaseType::Iterator begin() { return _pData; }              // iterator to the beginning
 *   typename BaseType::ConstIterator end() const { return _pData + 5; } // const iterator to the end
 *   typename BaseType::Iterator end() { return _pData + 5; }            // const iterator to the end
 *
 *   // The following aren't necessarily required if your class is a POD type
 *
 *   // Standard onstructors (implementations may vary)
 *   MyClass() { BaseType::operator=(0); }
 *   MyClass(const MyClass& other) { *this = other; }
 *
 *   // Copy assignment isn't inherited...
 *   MyClass& operator= (const MyClass& other) { return BaseType::operator=(other); }
 *
 * private:
 *   T _pData[5];
 * };
 *
 * // Now, you can do this:
 * MyClass a, b;
 * MyClass c = a + b;
 * ~~~
 *
 * ! The length of the vector/array is assumed to be fixed. That
 * is, all instances of the derived class should hold the same amount
 * of elements. If this is not the case, your program will (in the
 * best case) crash.
 *
 */
template <class Derived> class PiiArithmeticBase
{
public:
  /**
   * A typedef for the traits template parameter. Can be used to
   * access the rebinder and other parts of the traits directly.
   *
   * ~~~(c++)
   * PiiColor<> clr;
   * PiiColor<double> clr2 = PiiColor<>::TraitsType::Rebind<double>::Type();
   * ~~~
   */
  typedef PiiArithmeticTraits<Derived> TraitsType;
  /**
   * A shorthand for the content type of the derived class.
   */
  typedef typename TraitsType::Type T;
  typedef typename TraitsType::Type Type;
  typedef typename TraitsType::Type value_type;
  /**
   * Qt-style iterator.
   */
  typedef typename TraitsType::Iterator Iterator;
  /**
   * Qt-style const iterator.
   */
  typedef typename TraitsType::ConstIterator ConstIterator;
  /**
   * Stl-style iterator.
   */
  typedef typename TraitsType::Iterator iterator;
  /**
   * Stl-style const iterator.
   */
  typedef typename TraitsType::ConstIterator const_iterator;

  /**
   * Returns an stl-style const iterator to the beginning of data.
   */
  inline const_iterator constBegin() const { return self()->begin(); }
  /**
   * Returns an stl-style const iterator to the end of data.
   */
  inline const_iterator constEnd() const { return self()->end(); }

  /**
   * Sums corresponding elements.
   */
  Derived operator+ (const Derived& other) const { return mapped(std::plus<T>(), other); }
  /**
   * Subtracts corresponding elements from each other.
   */
  Derived operator- (const Derived& other) const { return mapped(std::minus<T>(), other); }
  /**
   * Divides corresponding elements by each other.
   */
  Derived operator/ (const Derived& other) const { return mapped(std::divides<T>(), other); }
  /**
   * Multiplies corresponding elements by each other.
   */
  Derived operator* (const Derived& other) const { return mapped(std::multiplies<T>(), other); }
  /**
   * Adds corresponding elements in `other` to this.
   */
  Derived& operator+= (const Derived& other) { return map(std::plus<T>(), other); }
  /**
   * Subtracts corresponding elements in `other` from this.
   */
  Derived& operator-= (const Derived& other) { return map(std::minus<T>(), other); }
  /**
   * Divides corresponding elements in this by `other`.
   */
  Derived& operator/= (const Derived& other) { return map(std::divides<T>(), other); }
  /**
   * Multiplies corresponding elements in this by `other`.
   */
  Derived& operator*= (const Derived& other) { return map(std::multiplies<T>(), other); }
  /**
   * Adds `value` to all elements.
   */
  Derived operator+ (T value) const { return mapped(std::plus<T>(), value); }
  /**
   * Subtracts `value` from all elements.
   */
  Derived operator- (T value) const { return mapped(std::minus<T>(), value); }
  /**
   * Divides all elements by `value`.
   */
  Derived operator/ (T value) const { return mapped(std::divides<T>(), value); }
  /**
   * Multiplies all elements by `value`.
   */
  Derived operator* (T value) const { return mapped(std::multiplies<T>(), value); }
  /**
   * Adds `value` to all elements.
   */
  Derived& operator+= (T value) { return map(std::plus<T>(), value); }
  /**
   * Subtracts `value` from all elements.
   */
  Derived& operator-= (T value) { return map(std::minus<T>(), value); }
  /**
   * Divides all elements by `value`.
   */
  Derived& operator/= (T value) { return map(std::divides<T>(), value); }
  /**
   * Multiplies all elements by `value`.
   */
  Derived& operator*= (T value) { return map(std::multiplies<T>(), value); }
  /**
   * Creates a negation of all elements.
   */
  Derived operator- () const { return mapped(std::negate<T>()); }
  /**
   * Assigns the values in `other` to this.
   */
  Derived& operator= (const Derived& other) { return map(Pii::SelectSecond<typename TraitsType::Type>(), other); }
  /**
   * Sets all elements to `value`.
   */
  Derived& operator= (T value) { return map(Pii::SelectSecond<typename TraitsType::Type>(), value); }

  /**
   * Returns `true` if all elements in `this` and `other` are
   * equal, `false` otherwise.
   */
  bool operator== (const Derived& other) const
  {
    typename TraitsType::ConstIterator myCurrent = self()->begin();
    typename TraitsType::ConstIterator otherCurrent = other.begin();
    while (myCurrent != self()->end())
      if (*(myCurrent++) != *(otherCurrent++))
        return false;
    return true;
  }
  /**
   * Returns `false` if all elements in `this` and `other` are
   * equal, `true` otherwise.
   */
  bool operator!= (const Derived& other) const
  {
    typename TraitsType::ConstIterator myCurrent = self()->begin();
    typename TraitsType::ConstIterator otherCurrent = other.begin();
    while (myCurrent != self()->end())
      if (*(myCurrent++) != *(otherCurrent++))
        return true;
    return false;
  }

  /**
   * Applies a unary function to all elements. Returns a new object.
   */
  template <class Operation>
  typename TraitsType::template Rebind<typename Operation::result_type>::Type mapped(Operation op) const
  {
    typedef typename TraitsType::template Rebind<typename Operation::result_type>::Type ResultType;
    ResultType result;

    // Take iterators to the beginning of both elements
    typename TraitsType::ConstIterator myCurrent = self()->begin();
    typename ResultType::Iterator resultCurrent = result.begin();
    while (myCurrent != self()->end())
      {
        *resultCurrent = op(*myCurrent);
        ++myCurrent;
        ++resultCurrent;
      }
    return result;
  }

  /**
   * Applies a unary function to all elements. Modifies the elements in
   * place.
   */
  template <class Operation>
  Derived& map(Operation op)
  {
    // Take iterators to the beginning my elements
    typename TraitsType::Iterator myCurrent = self()->begin();
    // Applies unary function to all
    while (myCurrent != self()->end())
      {
        *myCurrent = op(*myCurrent);
        ++myCurrent;
      }
    return selfRef();
  }

  /**
   * Applies a binary function to all elements using the corresponding
   * elements in `this` and `other` as function parameters. Returns a
   * new object.
   */
  template <class Operation>
  typename TraitsType::template Rebind<typename Operation::result_type>::Type
  mapped(Operation op,
         const typename TraitsType::template Rebind<typename Operation::second_argument_type>::Type& other) const
  {
    typedef typename TraitsType::template Rebind<typename Operation::result_type>::Type ResultType;
    typedef typename TraitsType::template Rebind<typename Operation::second_argument_type>::Type SecondType;
    ResultType result;
    // Take iterators to the beginning of all elements
    typename TraitsType::ConstIterator myCurrent = self()->begin();
    typename SecondType::ConstIterator otherCurrent = other.begin();
    typename ResultType::Iterator resultCurrent = result.begin();
    while (myCurrent != self()->end())
      {
        *resultCurrent = op(*myCurrent, *otherCurrent);
        ++myCurrent;
        ++otherCurrent;
        ++resultCurrent;
      }
    return result;
  }

  /**
   * Applies a binary function to all elements using the corresponding
   * elements in `this` and `other` as function parameters. Modifies
   * elements in place.
   */
  template <class Operation>
  Derived& map(Operation op,
               const typename TraitsType::template Rebind<typename Operation::second_argument_type>::Type& other)

  {
    typedef typename TraitsType::template Rebind<typename Operation::second_argument_type>::Type SecondType;

    // Take iterators to the beginning of both elements
    typename TraitsType::Iterator myCurrent = self()->begin();
    typename SecondType::ConstIterator otherCurrent = other.begin();
    while (myCurrent != self()->end())
      {
        *myCurrent = op(*myCurrent, *otherCurrent);
        ++myCurrent;
        ++otherCurrent;
      }
    return selfRef();
  }

  /**
   * Applies a binary function to all elements using `value` as the
   * second function parameter. Returns a new object.
   */
  template <class Operation> typename TraitsType::template Rebind<typename Operation::result_type>::Type
  mapped(Operation op, typename Operation::second_argument_type value) const
  {
    typedef typename TraitsType::template Rebind<typename Operation::result_type>::Type ResultType;
    ResultType result(*self());
    result.map(op, value);
    return result;
  }

  /**
   * Applies a binary function to all elements using `value` as the
   * second function parameter. Modifies elements in place.
   */
  template <class Operation>
  Derived& map(Operation op, typename Operation::second_argument_type value)
  {
    // Take iterator to the beginning of my elements
    typename TraitsType::Iterator myCurrent = self()->begin();
    while (myCurrent != self()->end())
      {
        *myCurrent = op(*myCurrent, value);
        ++myCurrent;
      }
    return selfRef();
  }

private:
  Derived* self() { return static_cast<Derived*>(this); }
  const Derived* self() const { return static_cast<const Derived*>(this); }
  Derived& selfRef() { return *self(); }
  const Derived& selfRef() const { return *self(); }
};

template <class Derived>
Derived operator+ (typename PiiArithmeticBase<Derived>::value_type x,
                   const PiiArithmeticBase<Derived>& other)
{
  return other.mapped(std::bind1st(std::plus<typename Derived::value_type>(), x));
}

template <class Derived>
Derived operator* (typename PiiArithmeticBase<Derived>::value_type x,
                   const PiiArithmeticBase<Derived>& other)
{
  return other.mapped(std::bind1st(std::multiplies<typename Derived::value_type>(), x));
}

template <class Derived>
Derived operator- (typename PiiArithmeticBase<Derived>::value_type x,
                   const PiiArithmeticBase<Derived>& other)
{
  return other.mapped(std::bind1st(std::minus<typename Derived::value_type>(), x));
}

template <class Derived>
Derived operator/ (typename PiiArithmeticBase<Derived>::value_type x,
                   const PiiArithmeticBase<Derived>& other)
{
  return other.mapped(std::bind1st(std::divides<typename Derived::value_type>(), x));
}

#endif //_PIIARITHMETICBASE_H
