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

#ifndef _PIIITERATOR_H
#define _PIIITERATOR_H

#include "PiiTypeTraits.h"
#include <iterator>

/**
 * An iterator that transforms another iterator using a unary
 * function. Given a function `func` and an iterator `it`, this
 * iterator returns `func(*it)`.
 *
 * ~~~
 * // Add two to all elements of vec1 and store the result to vec2
 * QVector<int> vec1(5), vec2(5);
 * Pii::copyN(Pii::unaryFunctionIterator(vec1.begin(), std::bind2nd(std::plus<int>(), 2)),
 *            vec1.size(), vec2.begin());
 * ~~~
 *
 */
template <class Iterator, class UnaryFunction> class PiiUnaryFunctionIterator
{
public:
  typedef typename std::iterator_traits<Iterator>::iterator_category iterator_category;
  typedef typename std::iterator_traits<Iterator>::difference_type difference_type;
  typedef typename UnaryFunction::result_type value_type;
  typedef value_type* pointer;
  typedef value_type& reference;

  /**
   * Constructs an iterator that transforms *iterator* with *func*. 
   * This constructor is seldom used explicitly; use the @ref
   * Pii::unaryFunctionIterator() function instead.
   */
  PiiUnaryFunctionIterator(Iterator iterator, const UnaryFunction& func) :
    _iterator(iterator), _func(func)
  {}
  
  /**
   * Constructs a copy of *other*.
   */
  PiiUnaryFunctionIterator(const PiiUnaryFunctionIterator& other) :
    _iterator(other._iterator),
    _func(other._func)
  {}
  /**
   * Copies *other* and returns a reference to `this`.
   */
  PiiUnaryFunctionIterator& operator= (const PiiUnaryFunctionIterator& other)
  {
    _iterator = other._iterator;
    const_cast<UnaryFunction&>(_func) = other._func;
    return *this;
  }

  /**
   * Returns `func(*iterator)`.
   */
  value_type operator* () const { return _func(*_iterator); }

  /**
   * Returns `true` if the `iterator` members of `this` and 
   * *other* are equal, and `false` otherwise.
   */
  bool operator== (const PiiUnaryFunctionIterator& other) const { return _iterator == other._iterator; }
  /**
   * Returns `false` if the `iterator` members of `this` and 
   * *other* are equal, and `true` otherwise.
   */
  bool operator!= (const PiiUnaryFunctionIterator& other) const { return _iterator != other._iterator; }

  bool operator< (const PiiUnaryFunctionIterator& other) const { return _iterator < other._iterator; }
  bool operator<= (const PiiUnaryFunctionIterator& other) const { return _iterator <= other._iterator; }
  bool operator> (const PiiUnaryFunctionIterator& other) const { return _iterator > other._iterator; }
  bool operator>= (const PiiUnaryFunctionIterator& other) const { return _iterator >= other._iterator; }

  /**
   * Increments `iterator` and returns a reference to `this`.
   */
  PiiUnaryFunctionIterator& operator++() { ++_iterator; return *this; }
  /**
   * Decrements `iterator` and returns a reference to `this`.
   */
  PiiUnaryFunctionIterator& operator--() { --_iterator; return *this; }
  /**
   * Increments `iterator` and returns a copy of `this`.
   */
  PiiUnaryFunctionIterator operator++(int)
  {
    PiiUnaryFunctionIterator tmp(*this);
    operator++();
    return tmp;
  }
  /**
   * Decrements `iterator` and returns a copy of `this`.
   */
  PiiUnaryFunctionIterator operator--(int)
  {
    PiiUnaryFunctionIterator tmp(*this);
    operator--();
    return tmp;
  }
  /**
   * Returns the value of the iterator at current position + *index*.
   */
  value_type operator[] (difference_type index) const
  {
    return _func(_iterator[index]);
  }
  /**
   * Moves `iterator` forwards *i* elements. Returns a reference to
   * `this`.
   */
  PiiUnaryFunctionIterator& operator+= (difference_type i) { _iterator += i; return *this; }
  /**
   * Moves `iterator` backwards *i* elements. Returns a reference to
   * `this`.
   */
  PiiUnaryFunctionIterator& operator-= (difference_type i) { _iterator -= i; return *this; }

  /**
   * Returns the difference between `iterator` members of `this` and
   * *other*.
   */
  difference_type operator- (const PiiUnaryFunctionIterator& other) const { return _iterator - other._iterator; }

private:
  Iterator _iterator;
  UnaryFunction _func;
};

/**
 * An iterator that uses two iterators in parallel and transforms
 * their values using a binary function. Given two iterators `it1`
 * and `it2`, and a function `func`, this iterator returns
 * `func(*it1, *it2)`.
 *
 * ~~~
 * // Store the difference of vec1 and vec2 to vec1
 * QVector<int> vec1(5), vec2(5), vec3(5);
 * Pii::copyN(Pii::binaryFunctionIterator(vec1.begin(), vec2.begin(), std::minus<int>()),
 *            vec1.size(), vec1.begin());
 * ~~~
 *
 */
template <class Iterator1, class Iterator2, class BinaryFunction> class PiiBinaryFunctionIterator
{
public:
  typedef typename std::iterator_traits<Iterator1>::iterator_category iterator_category;
  typedef typename std::iterator_traits<Iterator1>::difference_type difference_type;
  typedef typename BinaryFunction::result_type value_type;
  typedef value_type* pointer;
  typedef value_type& reference;

  /**
   * Constructs an iterator that transforms *iterator1* and 
   * *iterator2* with *func*. This constructor is seldom used
   * explicitly; use the [Pii::binaryFunctionIterator()] function
   * instead.
   */
  PiiBinaryFunctionIterator(Iterator1 iterator1, Iterator2 iterator2, const BinaryFunction& func) :
    _iterator1(iterator1), _iterator2(iterator2), _func(func)
  {}

  /**
   * Constructs a copy of *other*.
   */
  PiiBinaryFunctionIterator(const PiiBinaryFunctionIterator& other) :
    _iterator1(other._iterator1), _iterator2(other._iterator2), _func(other._func)
  {}

  /**
   * Copies *other* and returns a reference to `this`.
   */
  PiiBinaryFunctionIterator& operator= (const PiiBinaryFunctionIterator& other)
  {
    _iterator1 = other._iterator1;
    _iterator2 = other._iterator2;
    const_cast<BinaryFunction&>(_func) = other._func;
    return *this;
  }

  /**
   * Returns `func(*iterator1, *iterator2)`.
   */
  value_type operator* () const { return _func(*_iterator1, *_iterator2); }

  /**
   * Returns `true` if the `iterator1` members of `this` and 
   * *other* are equal, and `false` otherwise.
   */
  bool operator== (const PiiBinaryFunctionIterator& other) const { return _iterator1 == other._iterator1; }
  /**
   * Returns `false` if the `iterator1` members of `this` and 
   * *other* are equal, and `true` otherwise.
   */
  bool operator!= (const PiiBinaryFunctionIterator& other) const { return _iterator1 != other._iterator1; }

  bool operator< (const PiiBinaryFunctionIterator& other) const { return _iterator1 < other._iterator1; }
  bool operator<= (const PiiBinaryFunctionIterator& other) const { return _iterator1 <= other._iterator1; }
  bool operator> (const PiiBinaryFunctionIterator& other) const { return _iterator1 > other._iterator1; }
  bool operator>= (const PiiBinaryFunctionIterator& other) const { return _iterator1 >= other._iterator1; }

  /**
   * Increments both `iterator1` and `iterator2`. Returns a
   * reference to `this`.
   */
  PiiBinaryFunctionIterator& operator++ () { ++_iterator1; ++_iterator2; return *this; }
  /**
   * Decrements both `iterator1` and `iterator2`. Returns a
   * reference to `this`.
   */
  PiiBinaryFunctionIterator& operator-- () { --_iterator1; --_iterator2; return *this; }
  /**
   * Increments both `iterator1` and `iterator2`. Returns a
   * copy of `this`.
   */
  PiiBinaryFunctionIterator operator++ (int)
  {
    PiiBinaryFunctionIterator tmp(*this);
    operator++();
    return tmp;
  }
  /**
   * Decrements both `iterator1` and `iterator2`. Returns a copy of
   * `this`.
   */
  PiiBinaryFunctionIterator operator-- (int)
  {
    PiiBinaryFunctionIterator tmp(*this);
    operator--();
    return tmp;
  }
  /**
   * Returns a new PiiBinaryFunctionIterator that is initialized with
   * `iterator1`[*index*] and `iterator2`[*index*].
   */
  value_type operator[] (difference_type index) const
  {
    return _func(_iterator1[index], _iterator2[index]);
  }
  /**
   * Moves both iterators forwards *i* elements. Returns a reference
   * to `this`.
   */
  PiiBinaryFunctionIterator& operator+= (difference_type i) {_iterator1 += i; _iterator2 += i; return *this; }
  /**
   * Moves both iterators backwards *i* elements. Returns a reference
   * to `this`.
   */
  PiiBinaryFunctionIterator& operator-= (difference_type i) {_iterator1 -= i; _iterator2 -= i; return *this; }

  /**
   * Returns the difference between `iterator1` members of `this`
   * and *other*.
   */
  difference_type operator- (const PiiBinaryFunctionIterator& other) const { return _iterator1 - other._iterator1; }

private:
  Iterator1 _iterator1;
  Iterator2 _iterator2;
  BinaryFunction _func;
};

/**
 * An iterator that picks selected elements out of another
 * iterator. PiiFilteredIterator is initialized with two ranges of
 * equal length: the range to be filtered an the filter. When
 * advanced, PiiFilteredIterator steps over items whose corresponding
 * element in the filter range is zero.
 *
 */
template <class Iterator, class FilterIterator, bool bConst = false> class PiiFilteredIterator
{
public:
  typedef typename std::iterator_traits<Iterator>::iterator_category iterator_category;
  typedef typename std::iterator_traits<Iterator>::difference_type difference_type;
  typedef typename std::iterator_traits<Iterator>::value_type value_type;
  typedef typename Pii::If<bConst, const value_type*, value_type*>::Type pointer;
  typedef typename Pii::If<bConst, const value_type&, value_type&>::Type reference;

  /**
   * Constructs a new filtered iterator that works in the range [
   * *begin*, *end*). The start of the filter range is *filter*. The
   * current item is set to the first item in the range whose
   * corresponding element in *filter* is non-zero.
   */
  PiiFilteredIterator(const Iterator& begin, const Iterator& end,
                      const FilterIterator& filter) :
    _begin(begin), _end(end),
    _current(begin),
    _filter(filter)
  {
    // Move begin and current to the the first hit.
    if (!*filter)
      {
        ++(*this);
        _begin = _current;
      }
  }

  /**
   * Constructs a new filtered iterator that works in the range [
   * *begin*, *end*). The start of the filter range is *filter*. The
   * current item is placed at *current*, which must be within the
   * range.
   */
  PiiFilteredIterator(const Iterator& begin, const Iterator& end,
                      const Iterator& current,
                      const FilterIterator& filter) :
    _begin(begin), _end(end),
    _current(current),
    _filter(filter)
  {}

  /**
   * Constructs a copy of *other*.
   */
  PiiFilteredIterator(const PiiFilteredIterator& other) :
    _begin(other._begin), _end(other._end), _current(other._current), _filter(other._filter)
  {}

  /**
   * Copies *other* and returns a reference to `this`.
   */
  PiiFilteredIterator& operator= (const PiiFilteredIterator& other)
  {
    _begin = other._begin;
    _end = other._end;
    _current = other._current;
    _filter = other._filter;
    return *this;
  }

  /**
   * Returns the current value of `iterator`.
   */
  value_type operator* () const { return *_current; }
  /// @overload
  reference operator* () { return *_current; }

  /**
   * Returns `true` if the `iterator` members of `this` and 
   * *other* are equal, and `false` otherwise.
   */
  bool operator== (const PiiFilteredIterator& other) const { return _current == other._current; }
  /**
   * Returns `false` if the `iterator` members of `this` and 
   * *other* are equal, and `true` otherwise.
   */
  bool operator!= (const PiiFilteredIterator& other) const { return _current != other._current; }

  bool operator< (const PiiFilteredIterator& other) const { return _current < other._current; }
  bool operator<= (const PiiFilteredIterator& other) const { return _current <= other._current; }
  bool operator> (const PiiFilteredIterator& other) const { return _current > other._current; }
  bool operator>= (const PiiFilteredIterator& other) const { return _current >= other._current; }

  /**
   * Increments `iterator` and `filter` until @p *filter is
   * non-zero. Returns a reference to `this`.
   */
  PiiFilteredIterator& operator++ ()
  {
    while (_current != _end)
      {
        ++_current; ++_filter;
        if (*_filter) break;
      }
    return *this;
  }
  /**
   * Decrements `iterator` and `filter` until @p *filter is
   * non-zero. Returns a reference to `this`.
   */
  PiiFilteredIterator& operator-- ()
  {
    while (_current != _begin)
      {
        --_current; --_filter;
        if (*_filter) break;
      }
    return *this;
  }
  /**
   * Increments both `iterator` and `filter`. Returns a copy of 
   * `this`.
   */
  PiiFilteredIterator operator++ (int)
  {
    PiiFilteredIterator tmp(*this);
    operator++();
    return tmp;
  }
  /**
   * Decrements both `iterator` and `filter`. Returns a copy of 
   * `this`.
   */
  PiiFilteredIterator operator-- (int)
  {
    PiiFilteredIterator tmp(*this);
    operator--();
    return tmp;
  }
  /**
   * Returns a new PiiFilteredIterator that is initialized with
   * `iterator`[*index*] and `filter`[*index*].
   */
  PiiFilteredIterator operator[] (difference_type index) const
  {
    PiiFilteredIterator result(*this);
    for (; index; --index, ++result) ;
    return result;
  }
  /**
   * Moves both iterators forwards *i* elements. Returns a reference
   * to `this`.
   */
  PiiFilteredIterator& operator+= (difference_type i)
  {
    for (; i; --i, ++(*this)) ;
    return *this;
  }
  /**
   * Moves both iterators backwards *i* elements. Returns a reference
   * to `this`.
   */
  PiiFilteredIterator& operator-= (difference_type i)
  {
    for (; i; --i, --(*this)) ;
    return *this;
  }

  Iterator begin() const { return _begin; }
  Iterator end() const { return _end; }
  Iterator current() const { return _current; }
  FilterIterator filter() const { return _filter; }
  
private:
  Iterator _begin, _end, _current;
  FilterIterator _filter;
};

namespace Pii
{
  /**
   * Create an iterator that transforms *iterator* with *func*.
   *
   * @relates PiiUnaryFunctionIterator
   */
  template <class Iterator, class UnaryFunction>
  PiiUnaryFunctionIterator<Iterator, UnaryFunction> unaryFunctionIterator(Iterator iterator, const UnaryFunction& func)
  {
    return PiiUnaryFunctionIterator<Iterator, UnaryFunction>(iterator, func);
  }

  /**
   * Create an iterator that transforms *iterator1* and *iterator2*
   * with *func*.
   *
   * @relates PiiBinaryFunctionIterator
   */
  template <class Iterator1, class Iterator2, class BinaryFunction>
  PiiBinaryFunctionIterator<Iterator1, Iterator2, BinaryFunction> binaryFunctionIterator(Iterator1 iterator1,
                                                                                         Iterator2 iterator2,
                                                                                         const BinaryFunction& func)
  {
    return PiiBinaryFunctionIterator<Iterator1, Iterator2, BinaryFunction>(iterator1, iterator2, func);
  }

  /**
   * Create an iterator that accesses the elements in the range [
   * *begin*, *end*) that match *filter*. The iterator will be
   * initially moved to the first element matching the filter.
   *
   * @relates PiiFilteredIterator
   */
  template <class Iterator, class FilterIterator>
  PiiFilteredIterator<Iterator, FilterIterator> filteredIterator(Iterator begin, Iterator end,
                                                                 FilterIterator filter)
  {
    return PiiFilteredIterator<Iterator, FilterIterator>(begin, end, filter);
  }

  /**
   * Create an iterator that accesses the elements in the range [
   * *begin*, *end*) that match *filter*. The iterator will be
   * initially placed at *current*.
   *
   * @relates PiiFilteredIterator
   */
  template <class Iterator, class FilterIterator>
  PiiFilteredIterator<Iterator, FilterIterator> filteredIterator(Iterator begin, Iterator end,
                                                                 Iterator current,
                                                                 FilterIterator filter)
  {
    return PiiFilteredIterator<Iterator, FilterIterator>(begin, end, current, filter);
  }
}

#endif //_PIIITERATOR_H
