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

#ifndef _PIIALGORITHM_H
#define _PIIALGORITHM_H

#include "PiiGlobal.h"
#include "PiiTypeTraits.h"
#include <cstdarg>
#include <cstdlib>

#define PII_DIFF_TYPE typename std::iterator_traits<Iterator>::difference_type

namespace Pii
{
  /**
   * Sets all values in the range [*begin*, *end*) to *value*.
   * Returns *end*.
   */
  template <class Iterator>
  Iterator fill(Iterator begin, Iterator end, typename std::iterator_traits<Iterator>::value_type value)
  {
    for (; begin != end; ++begin)
      *begin = value;
    return end;
  }

  /**
   * Sets all values in the range [*begin*, *begin* + *b*) to
   * *value*. Returns *begin* + *n*.
   */
  template <class Iterator>
  Iterator fillN(Iterator begin, size_t n, typename std::iterator_traits<Iterator>::value_type value)
  {
    for (; n > 0; --n, ++begin)
      *begin = value;
    return begin;
  }

  /**
   * Sets all values in the range [
   * *begin*, *end*) for which *predicate* returns a non-zero value to
   * `value`. Returns *end*.
   */
  template <class Iterator, class UnaryPredicate>
  Iterator fillIf(Iterator begin, Iterator end,
                  UnaryPredicate predicate,
                  typename std::iterator_traits<Iterator>::value_type value)
  {
    for (; begin != end; ++begin)
      if (predicate(*begin))
        *begin = value;
    return begin;
  }

  /**
   * Sets all values in the range [*begin*, *end*) to the value
   * returned by *generator*. Returns *end*.
   */
  template <class Iterator, class Generator>
  Iterator generate(Iterator begin, Iterator end, Generator generator)
  {
    for (; begin != end; ++begin)
      *begin = generator();
    return end;
  }

  /**
   * Sets all values in the range [*begin*, *begin* + *n*) to the
   * value returned by *generatore*. Returns *begin* + *n*.
   */
  template <class Iterator, class Generator>
  Iterator generateN(Iterator begin, size_t n, Generator generator)
  {
    for (; n > 0; --n, ++begin)
      *begin = generator();
    return begin;
  }

  /**
   * Assigns sequentially increasing or decreasing values to the range
   * [*begin*, *end*). The first value will be set to
   * *initialValue*, which will then increase *step* units on each
   * iteration. Returns *end*.
   */
  template <class Iterator>
  Iterator sequence(Iterator begin, Iterator end,
                    typename std::iterator_traits<Iterator>::value_type initialValue,
                    typename std::iterator_traits<Iterator>::value_type step)
  {
    for (; begin != end; ++begin, initialValue += step)
      *begin = initialValue;
    return begin;
  }

  /**
   * Assigns sequentially increasing values to the range [*begin*,
   * *end*). The first value will be set to *initialValue*, which will
   * then be incremented by one on each iteration. Returns *end*.
   */
  template <class Iterator>
  inline Iterator sequence(Iterator begin, Iterator end,
                           typename std::iterator_traits<Iterator>::value_type initialValue)
  {
    return sequence(begin, end, initialValue, 1);
  }

  /**
   * Assigns sequentially increasing or decreasing values to the range
   * [*begin*, *begin* + *n*). The first value will be set to
   * *initialValue*, which will then increase *step* units on each
   * iteration. Returns *begin* + *n*.
   */
  template <class Iterator>
  Iterator sequenceN(Iterator begin, size_t n,
                    typename std::iterator_traits<Iterator>::value_type initialValue,
                    typename std::iterator_traits<Iterator>::value_type step)
  {
    for (; n > 0; --n, ++begin, initialValue += step)
      *begin = initialValue;
    return begin;
  }

  /**
   * Assigns sequentially increasing values to the range [*begin*,
   * *end* + *n*). The first value will be set to *initialValue*,
   * which will then be incremented by one on each iteration. Returns
   * *begin* + *end*.
   */
  template <class Iterator>
  inline Iterator sequenceN(Iterator begin, size_t n,
                            typename std::iterator_traits<Iterator>::value_type initialValue)
  {
    return sequenceN(begin, n, initialValue, 1);
  }

  /**
   * Applies a unary function *func* to all elements in the range [
   * *begin*, *end*), and stores the result in place. The `map`
   * algorithm is equal to `std::transform` except that it always
   * modifies the elements in place. Returns *end*.
   *
   * ~~~(c++)
   * // Subtract one from all elements of a QVector
   * QVector<double> vec(6);
   * Pii::map(vec.begin(), vec.end(), std::bind2nd(std::minus<double>(), 1));
   * ~~~
   */
  template <class Iterator, class UnaryFunction>
  Iterator map(Iterator begin, Iterator end, UnaryFunction func)
  {
    for (; begin != end; ++begin)
      *begin = func(*begin);
    return end;
  }

  /**
   * Applies a unary function *func* to all elements in the range [
   * *begin*, *begin* + *n*), and stores the result in place. Returns
   * *begin* + *n*.
   */
  template <class Iterator, class UnaryFunction>
  Iterator mapN(Iterator begin, size_t n, UnaryFunction func)
  {
    for (; n > 0; --n, ++begin)
      *begin = func(*begin);
    return begin;
  }

  /**
   * Applies a unary function *func* to all elements in the range [
   * *begin*, *end*) for which *predicate* returns a non-zero value.
   * The returned value will be stored in place. Returns *end*.
   */
  template <class Iterator, class UnaryPredicate, class UnaryFunction>
  Iterator mapIf(Iterator begin, Iterator end,
                 UnaryPredicate predicate,
                 UnaryFunction func)
  {
    for (; begin != end; ++begin)
      if (predicate(*begin))
        *begin = func(*begin);
    return begin;
  }

  /**
   * Applies a binary function *func* to all elements in the range
   * [*begin1*, *end1*), and stores the result in place. The second
   * argument to the binary function is taken from *begin2*. The
   * `map` algorithm is equal to [transform()] except that it always
   * modifies the elements in place and works slightly faster.
   *
   * ~~~(c++)
   * // Add the elements of lst2 to lst1
   * QList<int> lst1, lst2;
   * lst1 << 1 << 2;
   * lst2 << -1 << -2;
   * Pii::map(lst1.begin(), lst2.end(), lst2.begin(), std::plus<int>());
   * // lst1 is now (0, 0)
   * ~~~
   */
  template <class Iterator1, class Iterator2, class BinaryFunction>
  void map(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, BinaryFunction func)
  {
    while (begin1 != end1)
      {
        *begin1 = func(*begin1, *begin2);
        ++begin1;
        ++begin2;
      }
  }

  /**
   * Applies a binary function *func* to all elements in the range
   * [*begin1*, *begin1* + *n*), and stores the result in place. The second
   * argument to the binary function is taken from *begin2*. The
   * `map` algorithm is equal to [transform()] except that it always
   * modifies the elements in place and works slightly faster.
   */
  template <class Iterator1, class Iterator2, class BinaryFunction>
  void mapN(Iterator1 begin1, size_t n, Iterator2 begin2, BinaryFunction func)
  {
    for (; n > 0; --n)
      {
        *begin1 = func(*begin1, *begin2);
        ++begin1;
        ++begin2;
      }
  }

  template <class Iterator1, class Iterator2>
  void swap(Iterator1 begin1, Iterator1 end1, Iterator2 begin2)
  {
    while (begin1 != end1)
      {
        qSwap(*begin1, *begin2);
        ++begin1;
        ++begin2;
      }
  }

  template <class Iterator1, class Iterator2>
  void swapN(Iterator1 begin1, size_t n, Iterator2 begin2)
  {
    for (; n > 0; --n)
      {
        qSwap(*begin1, *begin2);
        ++begin1;
        ++begin2;
      }
  }

  /**
   * Copies all elements in the range [*begin*, *end*) to *output*.
   * Returns *output* + (*end* - *begin*).
   */
  template <class InputIterator, class OutputIterator>
  OutputIterator copy(InputIterator begin, InputIterator end, OutputIterator output)
  {
    for (; begin != end; ++begin, ++output)
      *output = PII_MOVE(*begin);
    return output;
  }

  /**
   * Copies *n* elements starting at *begin* to *output*.
   * Returns *output* + *n*.
   */
  template <class InputIterator, class OutputIterator>
  OutputIterator copyN(InputIterator begin, size_t n, OutputIterator output)
  {
    for (; n > 0; --n, ++begin, ++output)
      *output = PII_MOVE(*begin);
    return output;
  }

  /**
   * Copies a variable argument list to a range of elements. This
   * function is for [copyVaArgs()] what vprintf() is for printf(). The
   * caller is responsible for invoking va_end() on *rest*.
   *
   * @see copyVaArgs
   */
  template <class Iterator> void copyVaList(Iterator begin, Iterator end,
                                            typename Pii::VaArg<typename std::iterator_traits<Iterator>::value_type>::Type first,
                                            va_list rest)
  {
    typedef typename std::iterator_traits<Iterator>::value_type T;
    typedef typename Pii::VaArg<T>::Type VaArgType;
    *begin = static_cast<T>(first);
    ++begin;
    while (begin != end)
      {
        *begin = static_cast<T>(va_arg(rest, VaArgType));
        ++begin;
      }
  }

  /**
   * Copies a variable number of arguments to a range of elements
   * bounded by the iterators *begin* and *end*.
   *
   * ~~~(c++)
   * QVector<int> vec(5);
   * // Set vector contents to [ 1, 2, 3, 4, 5 ]
   * Pii::copyVaArgs(vec.begin(), vec.end(), 1, 2, 3, 4, 5);
   * ~~~
   */
  template <class Iterator> void copyVaArgs(Iterator begin, Iterator end,
                                            typename Pii::VaArg<typename std::iterator_traits<Iterator>::value_type>::Type first,
                                            ...)
  {
    va_list rest;
    va_start(rest, first);
    copyVaList(begin, end, first, rest);
    va_end(rest);
  }

  /**
   * Returns the inner product of two vectors `v1` and `v2`.
   *
   * @param begin1 an interator the beginning of v1
   *
   * @param end1 an interator to the end of v1
   *
   * @param begin2 an interator the beginning of v2
   *
   * @param initialValue initial value for the inner product, usually
   * 0. Used to determine the return type.
   *
   * @return \(v1 \cdot v2\)
   *
   * ~~~(c++)
   * PiiMatrix<double> m(2, 1, 1.0, -1.0);
   * double dot = Pii::innerProduct(m.columnBegin(0),
   *                                m.columnEnd(0),
   *                                m.columnBegin(0),
   *                                0.0);
   * QCOMPARE(dot, 2.0);
   * ~~~
   */
  template <class InputIterator1, class InputIterator2, class T>
  T innerProduct(InputIterator1 begin1, InputIterator1 end1, InputIterator2 begin2, T initialValue)
  {
    for (; begin1 != end1; ++begin1, ++begin2)
      initialValue += T(*begin1) * (*begin2);
    return initialValue;
  }

  /**
   *
   * Returns the inner product of two *n*-dimensional vectors.
   */
  template <class InputIterator1, class InputIterator2, class T>
  T innerProductN(InputIterator1 begin1, size_t n, InputIterator2 begin2, T initialValue)
  {
    for (; n > 0; --n, ++begin1, ++begin2)
      initialValue += T(*begin1) * T(*begin2);
    return initialValue;
  }

  /**
   * Applies a unary function *func* to all elements in the range [
   * *begin*, *end*), and stores the returned value to *output*.
   * Returns *output* + (*end* - *begin*).
   */
  template <class InputIterator, class OutputIterator, class UnaryFunction>
  OutputIterator transform(InputIterator begin, InputIterator end,
                           OutputIterator output, UnaryFunction func)
  {
    for (; begin != end; ++begin, ++output)
      *output = func(*begin);
    return output;
  }

  /**
   * Applies a unary function *func* to all elements in the range [
   * *begin*, *begin* + *n*), and stores the returned value to
   * *output*. Returns *output* + *n*.
   */
  template <class InputIterator, class OutputIterator, class UnaryFunction>
  OutputIterator transformN(InputIterator begin, size_t n,
                            OutputIterator output, UnaryFunction func)
  {
    for (; n > 0; --n, ++begin, ++output)
      *output = func(*begin);
    return output;
  }


  /**
   * Applies a binary function *func* to all elements in the range
   * [*begin1*, *end1*), using the corresponding element in the
   * range starting at *begin2* as the second argument to the
   * function. The return value of the function will be stored to
   * *output*. Returns *output* + (*end1* - *begin1*).
   */
  template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryFunction>
  OutputIterator transform(InputIterator1 begin1, InputIterator1 end1,
                           InputIterator2 begin2, OutputIterator output,
                           BinaryFunction func)
  {
    for (; begin1 != end1; ++begin1, ++begin2, ++output)
      *output = func(*begin1, *begin2);
    return output;
  }

  /**
   * Applies a binary function *func* to all elements in the range
   * [*begin1*, *begin1* + *n*), using the corresponding element in
   * the range starting at *begin2* as the second argument to the
   * function. The return value of the function will be stored to
   * *output*. Returns *output* + *n*.
   */
  template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryFunction>
  OutputIterator transformN(InputIterator1 begin1, size_t n,
                            InputIterator2 begin2, OutputIterator output,
                            BinaryFunction func)
  {
    for (; n > 0; --n, ++begin1, ++begin2, ++output)
      *output = func(*begin1, *begin2);
    return output;
  }

  /**
   * Loops through all values in the range [*begin*, *end*) and
   * applies *func* to each element and the current value of
   * *initialValue*. Returns the final value of *initialValue*.
   *
   * ~~~(c++)
   * // Sum up all entries in a matrix
   * PiiMatrix<int> mat(1,2, -1, 2);
   * std::cout << Pii::accumulate(mat.begin(), mat.end(),
   *                              std::plus<int>(), 0);
   * // Outputs 1
   * ~~~
   */
  template <class InputIterator, class BinaryFunction, class T>
  T accumulate(InputIterator begin, InputIterator end,
               BinaryFunction func, T initialValue)
  {
    for (; begin != end; ++begin)
      initialValue = func(initialValue, *begin);
    return initialValue;
  }

  /**
   *
   * This fuction works in the range [*begin*, *begin* + *n*).
   */
  template <class InputIterator, class BinaryFunction, class T>
  T accumulateN(InputIterator begin, size_t n,
                BinaryFunction func, T initialValue)
  {
    for (; n > 0; --n, ++begin)
      initialValue = func(initialValue, *begin);
    return initialValue;
  }

  /**
   * Loops through all values in the range [*begin*, *end*) and
   * applies *func* to each element and the current value of
   * *initialValue*, if *predicate* returns a non-zero value for the
   * element. Returns the final value of *initialValue*.
   *
   * ~~~(c++)
   * // Sum up all positive entries in a matrix
   * PiiMatrix<int> mat(1,3, -1, 2);
   * std::cout << Pii::accumulate(mat.begin(), mat.end(),
   *                              std::bind2nd(std::greater<int>(), 0),
   *                              std::plus<int>(),
   *                              0);
   * // Outputs 3
   * ~~~
   */
  template <class InputIterator, class BinaryFunction, class UnaryPredicate, class T>
  T accumulateIf(InputIterator begin, InputIterator end,
                 UnaryPredicate predicate, BinaryFunction func,
                 T initialValue)
  {
    for (; begin != end; ++begin)
      if (predicate(*begin))
        initialValue = func(initialValue, *begin);
    return initialValue;
  }

  /**
   * Find a special value such as maximum or minimum in the range
   * specified by *begin* and *end*. The "speciality" is defined by
   * the given function objects. Specifically, the algorithm increases
   * *begin* until it equals *end*, and sets *x* to `*begin` whenever
   * `op(f(*begin), f(*x))` returns a non-zero value.  Returns *x* on
   * success, or *end* if the range is empty.
   *
   * ~~~(c++)
   * double* values[] = { 1, 2.0, -5.3, 4.2 };
   * // Find the maximum absolute value
   * using namespace Pii;
   * double dMaxAbs = abs(*findSpecialValue(values, values + 4, std::greater<double>(), Abs<double>()));
   * ~~~
   */
  template<class InputIterator, class UnaryFunction, class BinaryPredicate>
  InputIterator findSpecialValue(InputIterator begin,
                                 InputIterator end,
                                 BinaryPredicate op,
                                 UnaryFunction f)
  {
    typedef typename UnaryFunction::result_type R;
    if (begin == end) return end;

    InputIterator result(begin);
    R specialValue = f(*begin);
    ++begin;
    while (begin != end)
      {
        R val = f(*begin);
        if (op(val, specialValue))
          {
            specialValue = val;
            result = begin;
          }
        ++begin;
      }
    return result;
  }

  /**
   * Applies *func* to each element in the range [*begin*, *end*).
   * The return value of the unary function *func* (if any) is
   * ignored. Returns *func*.
   *
   * ~~~(c++)
   * // Calculate the mean of all entries
   * PiiMatrix<float> matrix(3,3);
   * std::cout << Pii::forEach(matrix.begin(), matrix.end(),
   *                           Pii::Mean<float>()).mean();
   * ~~~
   */
  template<class InputIterator, class UnaryFunction>
  UnaryFunction forEach(InputIterator begin, InputIterator end, UnaryFunction func)
  {
    for (; begin != end; ++begin) func(*begin);
    return func;
  }

  /**
   * Applies *func* to each element in the range [*begin*, *end*)
   * for which *predicate* returns a non-zero value. The return value
   * of the unary function *func* (if any) is ignored. Returns
   * *func*.
   *
   * ~~~(c++)
   * // Calculate the mean of non-zero entries
   * PiiMatrix<float> matrix(3,3);
   * std::cout << Pii::forEachIf(matrix.begin(), matrix.end(),
   *                             std::bind2nd(std::not_equal_to<float>(), 0.0),
   *                             Pii::Mean<float>()).mean();
   * ~~~
   */
  template<class InputIterator, class UnaryPredicate, class UnaryFunction>
  UnaryFunction forEachIf(InputIterator begin, InputIterator end, UnaryPredicate predicate, UnaryFunction func)
  {
    for (; begin != end; ++begin)
      if (predicate(*begin))
        func(*begin);
    return func;
  }

  /**
   * Randomize the order of elements in a sequence.
   *
   * @param begin an iterator to the beginning of the random access
   * sequence to shuffle
   *
   * @param size the number of elements in the array
   */
  template <class Iterator> void shuffleN(Iterator begin, size_t n)
  {
    for (size_t i=0; i<n; ++i)
      {
        // PENDING rand() limits iNewIndex to RAND_MAX
        // Randomize a new location for each element
        size_t iNewIndex = size_t(::rand()) % n;
        if (iNewIndex != i)
          qSwap(begin[i], begin[iNewIndex]);
      }
  }

  /**
   * Randomize the order of elements in [*begin*, *end*).
   */
  template <class Iterator> inline void shuffle(Iterator begin, Iterator end)
  {
    shuffleN(begin, end-begin);
  }
}

#endif //_PIIALGORITHM_H
