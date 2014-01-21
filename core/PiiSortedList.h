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

#ifndef _PIISORTEDLIST_H
#define _PIISORTEDLIST_H

#include <QList>

/**
 * A list that maintains its elements in sorted order. The interface
 * of this class is identical to QList except that there are no
 * functions that would allow inserting elements in arbitrary order.
 * One can insert elements using the [insert()] function or the += and
 * << operators.
 *
 * PiiSortedList uses binary search to quickly locate items in the
 * list. This guarantees O(log(N)) look-up complexity in functions
 * such as [indexOf()] and [lastIndexOf()]. Inserting elements to the
 * middle of the list is an O(N) operation.
 *
 * ! Be careful when modifying the elements of the list. Doing so may
 *   invalidate the sort order.
 */
template <class T, class Less = std::less<T>, class Equals = std::equal_to<T> >
class PiiSortedList : private QList<T>
{
public:
  typedef typename QList<T>::iterator iterator;
  typedef typename QList<T>::reference reference;
  typedef typename QList<T>::pointer pointer;
  typedef typename QList<T>::const_iterator const_iterator;
  typedef typename QList<T>::const_reference const_reference;
  typedef typename QList<T>::const_pointer const_pointer;
  typedef typename QList<T>::difference_type difference_type;
  typedef typename QList<T>::size_type size_type;
  typedef typename QList<T>::value_type value_type;

  PiiSortedList() {}
  PiiSortedList(Less less, Equals equals) : less(less), equals(equals) {}
  PiiSortedList(const PiiSortedList& other) : QList<T>(other) {}

  using QList<T>::at;
  using QList<T>::back;
  using QList<T>::begin;
  using QList<T>::end;
  using QList<T>::clear;
  using QList<T>::constBegin;
  using QList<T>::constEnd;
  using QList<T>::empty;
  using QList<T>::erase;
  using QList<T>::first;
  using QList<T>::front;
  using QList<T>::isEmpty;
  using QList<T>::last;
  using QList<T>::length;
  using QList<T>::removeAt;
  using QList<T>::removeFirst;
  using QList<T>::removeLast;
  using QList<T>::reserve;
  using QList<T>::size;
  using QList<T>::takeAt;
  using QList<T>::takeFirst;
  using QList<T>::takeLast;
  using QList<T>::toSet;
  using QList<T>::toStdList;
  using QList<T>::toVector;
  using QList<T>::value;
  using QList<T>::operator[];

  template <class U> bool endsWith(const U& value) const { return !isEmpty() && equals(last(), value); }
  template <class U> bool startsWith(const U& value) const { return !isEmpty() && equals(first(), value); }

  QList<T> toList() const { return QList<T>(*this); }
  PiiSortedList mid(int pos, int length=-1)
  {
    return PiiSortedList(QList<T>::mid(pos,length), less);
  }
  PiiSortedList& operator= (const PiiSortedList& other)
  {
    QList<T>::operator= (other);
    less = other.less;
    equals = other.equals;
    return *this;
  }
  template <class U> bool contains(const U& value) const
  {
    return qBinaryFind(begin(), end(), value, less) != end();
  }
  int count() const { return size(); }
  template <class U> int count(const U& value) const
  {
    const_iterator itStart = qBinaryFind(begin(), end(), value, less);
    int iCnt = 0;
    const_iterator it(itStart);
    while (it != end() && equals(*it, value)) ++it;
    return it - itStart;
  }
  template <class U> iterator find(const U& value) { return qBinaryFind(begin(), end(), value, less); }
  template <class U> const_iterator constFind(const U& value) const { return qBinaryFind(begin(), end(), value, less); }
  template <class U> const_iterator find(const U& value) const { return constFind(value); }
  template <class U> int indexOf(const U& value, int from = 0) const
  {
    const_iterator it = qBinaryFind(from == 0 ? begin() : begin() + from,
                                    end(),
                                    value,
                                    less);
    return it != end() ? it - begin() : -1;
  }
  void insert(const T& value)
  {
    QList<T>::insert(qUpperBound(begin(), end(), value, less), value);
  }
  void insert(const QList<T>& other)
  {
    for (typename QList<T>::const_iterator it = other.begin(); it != other.end(); ++it)
      insert(*it);
  }
  void insert(const PiiSortedList<T>& other)
  {
    insert(static_cast<QList<T>&>(other));
  }

  template <class U> int lastIndexOf(const U& value, int from = -1) const
  {
    if (isEmpty()) return -1;
    const_iterator it = qUpperBound(begin(),
                                    from != -1 ? end() : begin() + from + 1,
                                    value,
                                    less);
    return equals(*(it-1), value) ? it - begin() - 1 : -1;
  }

  template <class U> void removeOne(const U& value)
  {
    iterator it = qBinaryFind(begin(), end(), value, less);
    if (it != end())
      QList<T>::erase(it);
  }
  template <class U> void removeAll(const U& value)
  {
    iterator itStart = qBinaryFind(begin(), end(), value, less);
    if (itStart != end())
      {
        iterator itEnd = itStart;
        while (++itEnd != end() && equals(*itEnd, value)) ;
        QList<T>::erase(itStart, itEnd);
      }
  }
  PiiSortedList& operator+= (const T& value) { insert(value); return *this; }
  PiiSortedList& operator<< (const T& value) { insert(value); return *this; }
  PiiSortedList& operator+= (const QList<T>& other) { insert(other); return *this; }
  PiiSortedList& operator+= (const PiiSortedList<T>& other) { insert(other); return *this; }
  PiiSortedList& operator<< (const QList<T>& other) { insert(other); return *this; }
  PiiSortedList& operator<< (const PiiSortedList<T>& other) { insert(other); return *this; }
  PiiSortedList operator+ (const QList<T>& other) const
  {
    PiiSortedList lstResult(*this);
    return lstResult.insertList(other);
  }
  PiiSortedList operator+ (const PiiSortedList<T>& other) const
  {
    PiiSortedList lstResult(*this);
    return lstResult.insertList(other);
  }
  bool operator== (const QList<T>& other) const
  {
    if (size() != other.size())
      return false;
    return std::equal(other.begin(), other.end(), begin(), equals);
  }
  bool operator!= (const QList<T>& other) const { return !*this == other; }
  bool operator== (const PiiSortedList<T>& other) const {  return other == static_cast<const QList<T>&>(this); }
  bool operator!= (const PiiSortedList<T>& other) const {  return other != static_cast<const QList<T>&>(this); }

private:
  PiiSortedList(const QList<T>& other, Less less, Equals equals) :
    QList<T>(other),
    less(less),
    equals(equals)
  {}

  Less less;
  Equals equals;
};

#endif //_PIISORTEDLIST_H
