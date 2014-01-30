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

#ifndef _PIICONCEPTUALMATRIX_H
#  error "Never use <PiiMatrixIterator.h> directly; include <PiiConceptualMatrix.h> instead."
#endif

#include <PiiTypeTraits.h>
#include <PiiMetaTemplate.h>

// Used in the usual case when stride is counted in columns.
template <class T> struct PiiStrideHandler
{
  static inline T add(T it, size_t offset) { return it + offset; }

  static inline int rowDiff(T it1, T it2, size_t stride)
  {
    return (it1 - it2) / ptrdiff_t(stride);
  }
};

// Used when the iterator type T is a pointer. In such case, stride is
// counted in bytes and may not be a multiple of sizeof(T). T* can be
// a const pointer.
template <class T> struct PiiStrideHandler<T*>
{
  static inline T* add(T* ptr, size_t offset)
  {
    // C-style casts to avoid const handling
    return (T*)((char*)(ptr) + offset);
  }

  static inline int rowDiff(T* ptr1, T* ptr2, size_t stride)
  {
    return ((char*)(ptr1) - (char*)(ptr2)) / ptrdiff_t(stride);
  }
};

template <class T>
class PiiMatrixIterator
{
public:
  typedef PiiStrideHandler<T> Stride;
  typedef typename std::iterator_traits<T>::iterator_category iterator_category;
  typedef typename std::iterator_traits<T>::difference_type difference_type;
  typedef typename std::iterator_traits<T>::value_type value_type;
  typedef typename std::iterator_traits<T>::pointer pointer;
  typedef typename std::iterator_traits<T>::reference reference;

  PiiMatrixIterator(T firstRow, int columns, size_t stride) :
    _firstRow(firstRow), _row(firstRow), _iColumn(0), _iColumns(columns), _sStride(stride)
  {}

  PiiMatrixIterator(T firstRow, T row, int columns, size_t stride) :
    _firstRow(firstRow), _row(row), _iColumn(0), _iColumns(columns), _sStride(stride)
  {}

  PiiMatrixIterator(const PiiMatrixIterator& other) :
    _firstRow(other._firstRow),
    _row(other._row),
    _iColumn(other._iColumn),
    _iColumns(other._iColumns),
    _sStride(other._sStride)
  {}

  reference operator* () const { return _row[_iColumn]; }
  // TODO: this doesn't work with non-pointer iterators
  pointer operator-> () const { return _row + _iColumn; }

  PiiMatrixIterator& operator= (const PiiMatrixIterator& other)
  {
    _firstRow = other._firstRow;
    _row = other._row;
    _iColumn = other._iColumn;
    _iColumns = other._iColumns;
    _sStride = other._sStride;
    return *this;
  }
  inline T addPtr(size_t offset) const { return Stride::add(_row, offset); }
  reference operator[] (int i) const { return *(*this + i); }
  bool operator== (const PiiMatrixIterator& other) const { return (_row + _iColumn) == (other._row + other._iColumn); }
  bool operator!= (const PiiMatrixIterator& other) const { return  (_row + _iColumn) != (other._row + other._iColumn); }
  bool operator< (const PiiMatrixIterator& other) const { return  (_row + _iColumn) < (other._row + other._iColumn); }
  bool operator<= (const PiiMatrixIterator& other) const { return  (_row + _iColumn) <= (other._row + other._iColumn); }
  bool operator> (const PiiMatrixIterator& other) const { return  (_row + _iColumn) > (other._row + other._iColumn); }
  bool operator>= (const PiiMatrixIterator& other) const { return  (_row + _iColumn) >= (other._row + other._iColumn); }

  PiiMatrixIterator& operator++ ()
  {
    if (++_iColumn >= _iColumns)
      {
        _iColumn = 0;
        _row = addPtr(_sStride);
      }
    return *this;
  }
  PiiMatrixIterator operator++(int)
  {
    PiiMatrixIterator tmp(*this);
    operator++();
    return tmp;
  }
  PiiMatrixIterator& operator--()
  {
    if (--_iColumn < 0)
      {
        _iColumn = _iColumns-1;
        _row = addPtr(-_sStride);
      }
    return *this;
  }
  PiiMatrixIterator operator--(int)
  {
    PiiMatrixIterator tmp(*this);
    operator--();
    return tmp;
  }
  PiiMatrixIterator& operator+= (int i)
  {
    if (_iColumns == 0) return *this;
    int iNewColumn = _iColumn + i;
    if (iNewColumn >= 0)
      {
        _row = addPtr(iNewColumn/_iColumns * _sStride);
        _iColumn = iNewColumn % _iColumns;
      }
    else
      {
        ++iNewColumn;
        _row = addPtr((iNewColumn/_iColumns - 1) * _sStride);
        _iColumn = _iColumns - 1 + iNewColumn % _iColumns;
      }
    return *this;
  }
  PiiMatrixIterator& operator-= (int i) { return operator+= (-i); }
  PiiMatrixIterator operator+ (int i) const { PiiMatrixIterator tmp(*this); return tmp += i; }
  PiiMatrixIterator operator- (int i) const { PiiMatrixIterator tmp(*this); return tmp -= i; }
  difference_type operator- (const PiiMatrixIterator& other) const
  {
    return _sStride != 0 ?
      Stride::rowDiff(_row, other._row, _sStride) * _iColumns + _iColumn - other._iColumn :
      0;
  }

  int column() const { return _iColumn; }
  int row() const
  {
    return _sStride != 0 ?
      Stride::rowDiff(_row, _firstRow, _sStride) :
      0;
  }

private:
  T _firstRow, _row;
  int _iColumn, _iColumns;
  size_t _sStride;
};
