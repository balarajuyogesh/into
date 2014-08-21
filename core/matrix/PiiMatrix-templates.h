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

#ifndef _PIIMATRIX_H
# error "Never use <PiiMatrix-templates.h> directly; include <PiiMatrix.h> instead."
#endif

#include <cstddef>

/**** Iterators ****/

template <class T> class PiiMatrixColumnIterator
{
public:
  // stl compatibility typedefs
  typedef std::random_access_iterator_tag iterator_category;
  typedef std::ptrdiff_t difference_type;
  typedef typename Pii::ToNonConst<T>::Type value_type;
  typedef typename Pii::IfClass<Pii::IsConst<T>,
                                const PiiMatrix<value_type>&,
                                PiiMatrix<value_type>&>::Type MatrixRef;
  typedef T* pointer;
  typedef T& reference;

  PiiMatrixColumnIterator(pointer ptr, std::size_t stride) :
    _ptr(ptr), _iStride(stride)
  {}
  PiiMatrixColumnIterator(const PiiMatrixColumnIterator &other) :
    _ptr(other._ptr), _iStride(other._iStride)
  {}
  PiiMatrixColumnIterator(MatrixRef mat, int column) :
    _ptr(mat[0]+column), _iStride(mat.stride())
  {}
  PiiMatrixColumnIterator(MatrixRef mat, int row, int column) :
    _ptr(mat[row]+column), _iStride(mat.stride())
  {}

  reference operator* () const { return *_ptr; }

  PiiMatrixColumnIterator& operator= (const PiiMatrixColumnIterator& other)
  {
    _ptr = other._ptr;
    _iStride = other._iStride;
    return *this;
  }
  reference operator[] (int i) const { return *reinterpret_cast<pointer>((char*)_ptr + i * std::ptrdiff_t(_iStride)); }
  bool operator== (const PiiMatrixColumnIterator& other) const { return _ptr == other._ptr; }
  bool operator!= (const PiiMatrixColumnIterator& other) const { return  _ptr != other._ptr; }
  bool operator< (const PiiMatrixColumnIterator& other) const { return _ptr < other._ptr; }
  bool operator<= (const PiiMatrixColumnIterator& other) const { return _ptr <= other._ptr; }
  bool operator> (const PiiMatrixColumnIterator& other) const { return _ptr > other._ptr; }
  bool operator>= (const PiiMatrixColumnIterator& other) const { return _ptr >= other._ptr; }
  PiiMatrixColumnIterator& operator++ ()
  {
    _ptr = reinterpret_cast<pointer>((char*)_ptr + _iStride);
    return *this;
  }
  PiiMatrixColumnIterator operator++ (int)
  {
    PiiMatrixColumnIterator tmp(*this);
    operator++();
    return tmp;
  }
  PiiMatrixColumnIterator& operator-- ()
  {
    _ptr = reinterpret_cast<pointer>((char*)_ptr - _iStride);
    return *this;
  }
  PiiMatrixColumnIterator operator-- (int)
  {
    PiiMatrixColumnIterator tmp(*this);
    operator--();
    return tmp;
  }
  PiiMatrixColumnIterator& operator+= (int i)
  {
    _ptr = &operator[](i);
    return *this;
  }
  PiiMatrixColumnIterator& operator-= (int i)
  {
    _ptr = &operator[](-i);
    return *this;
  }
  PiiMatrixColumnIterator operator+ (int i) const { PiiMatrixColumnIterator tmp(*this); return tmp += i; }
  PiiMatrixColumnIterator operator- (int i) const { PiiMatrixColumnIterator tmp(*this); return tmp -= i; }
  difference_type operator- (const PiiMatrixColumnIterator& other) const
  {
    return _iStride != 0 ?
      ((char*)(_ptr) - (char*)(other._ptr)) / std::ptrdiff_t(_iStride) :
      0;
  }

private:
  pointer _ptr;
  std::size_t _iStride;
};

/***** Submatrix specialization *****/

template <class T>
struct PiiMatrixTraits<PiiSubmatrix<PiiMatrix<T> > >
{
  enum { staticRows = -1, staticColumns = -1 };

  typedef PiiMatrix<T> Matrix;
  typedef typename Matrix::value_type value_type;
  typedef typename Matrix::reference reference;
  typedef PiiMatrixIterator<const T*> const_iterator;
  typedef PiiMatrixIterator<T*> iterator;

  typedef typename Matrix::const_row_iterator const_row_iterator;
  typedef typename Matrix::const_column_iterator const_column_iterator;
  typedef typename Matrix::row_iterator row_iterator;
  typedef typename Matrix::column_iterator column_iterator;
};

template <class T>
class PiiSubmatrix<PiiMatrix<T> > :
  public PiiSubmatrixBase<PiiSubmatrix<PiiMatrix<T> >, PiiMatrix<T> >
{
public:
  typedef PiiSubmatrixBase<PiiSubmatrix, PiiMatrix<T> > SuperType;
  typedef PiiMatrixTraits<PiiSubmatrix> Traits;

  PiiSubmatrix(PiiMatrix<T>& mat, int r, int c, int rows, int columns) :
    SuperType(mat, r, c, rows, columns)
  {}

  typename Traits::const_iterator begin() const
  {
    return typename Traits::const_iterator(this->_mat.row(this->_iRow) + this->_iColumn,
                                           this->_iColumns,
                                           this->_mat.stride());
  }
  typename Traits::const_iterator end() const
  {
    return typename Traits::const_iterator(this->_mat.row(this->_iRow) + this->_iColumn,
                                           this->_mat.row(this->_iRow + this->_iRows) + this->_iColumn,
                                           this->_iColumns,
                                           this->_mat.stride());
  }
  typename Traits::iterator begin()
  {
    return typename Traits::iterator(this->_mat.row(this->_iRow) + this->_iColumn,
                                     this->_iColumns,
                                     this->_mat.stride());
  }
  typename Traits::iterator end()
  {
    return typename Traits::iterator(this->_mat.row(this->_iRow) + this->_iColumn,
                                     this->_mat.row(this->_iRow + this->_iRows) + this->_iColumn,
                                     this->_iColumns,
                                     this->_mat.stride());
  }

  using PiiConceptualMatrix<PiiSubmatrix>::operator=;
};



/**** Matrix functions ****/

template <class T> PiiMatrix<T,-1,-1>::PiiMatrix(int rows, int columns, VaArgType firstElement, ...) :
  PiiTypelessMatrix(PiiMatrixData::createUninitializedData(rows, columns, columns * sizeof(T)))
{
  va_list argp;
  // initalize var ptr
  va_start(argp, firstElement);
  Pii::copyVaList(begin(), end(), firstElement, argp);
  // done with args
  va_end(argp);
}

template <class T>
template <class Matrix>
PiiMatrix<T>& PiiMatrix<T,-1,-1>::operator= (const PiiConceptualMatrix<Matrix>& other)
{
  // If sizes are different, need to create a copy
  if (other.self()->rows() != rows() || other.self()->columns() != columns())
    {
      PiiMatrix matCopy(PiiMatrixData::createUninitializedData(other.self()->rows(), other.self()->columns(),
                                                               other.self()->columns() * sizeof(T)));
      Pii::transform(other.self()->begin(), other.self()->end(), matCopy.begin(),
                     Pii::Cast<typename Matrix::value_type,T>());
      *this = matCopy;
    }
  else
    {
      Pii::transform(other.self()->begin(), other.self()->end(), begin(),
                     Pii::Cast<typename Matrix::value_type,T>());
    }
  return *this;
}

template <class T>
template <class Matrix> PiiFilteredMatrix<const PiiMatrix<T>, Matrix>
PiiMatrix<T,-1,-1>::operator() (const PiiConceptualMatrix<Matrix>& mask) const
{
  PII_MATRIX_CHECK_EQUAL_SIZE(*this, mask);
  return Pii::filteredMatrix(*this, mask.selfRef());
}

template <class T>
template <class Matrix> PiiFilteredMatrix<PiiMatrix<T>, Matrix>
PiiMatrix<T,-1,-1>::operator() (const PiiConceptualMatrix<Matrix>& mask)
{
  PII_MATRIX_CHECK_EQUAL_SIZE(*this, mask);
  return Pii::filteredMatrix(*this, mask.selfRef());
}

template <class T>
template <class Iterator>
typename PiiMatrix<T>::Traits::row_iterator PiiMatrix<T,-1,-1>::insertRow(int index, Iterator row)
{
  detach();
  T* pNewRow = static_cast<T*>(PiiTypelessMatrix::insertRow(index, columns() * sizeof(T)));
  std::copy(row, row + columns(), pNewRow);
  return pNewRow;
}

template <class T> typename PiiMatrix<T>::Traits::row_iterator PiiMatrix<T,-1,-1>::insertRow(int index)
{
  detach();
  std::size_t iBytesPerRow = columns() * sizeof(T);
  T* pNewRow = static_cast<T*>(PiiTypelessMatrix::insertRow(index, iBytesPerRow));
  std::memset(pNewRow, 0, iBytesPerRow);
  return pNewRow;
}

template <class T> typename PiiMatrix<T>::Traits::row_iterator PiiMatrix<T,-1,-1>::insertRow(int index, VaArgType firstElement, ...)
{
  detach();
  T* pNewRow = static_cast<T*>(PiiTypelessMatrix::insertRow(index, columns() * sizeof(T)));
  va_list argp;
  va_start(argp, firstElement);
  Pii::copyVaList(pNewRow, pNewRow + columns(), firstElement, argp);
  va_end(argp);
  return pNewRow;
}

template <class T> typename PiiMatrix<T>::Traits::row_iterator PiiMatrix<T,-1,-1>::appendRow(VaArgType firstElement, ...)
{
  detach();
  T* pNewRow = static_cast<T*>(PiiTypelessMatrix::appendRow(columns() * sizeof(T)));
  va_list argp;
  va_start(argp, firstElement);
  Pii::copyVaList(pNewRow, pNewRow + columns(), firstElement, argp);
  va_end(argp);
  return pNewRow;
}

template <class T>
template <class Matrix>
void PiiMatrix<T,-1,-1>::appendRows(const Matrix& other)
{
  if (this->isEmpty())
    resize(0, other.columns());

  reserve(rows() + other.rows());
  for (int i=0; i<other.rows(); ++i)
    appendRow(other[i]);
}

template <class T>
template <class Iterator>
typename PiiMatrix<T>::Traits::column_iterator PiiMatrix<T,-1,-1>::insertColumn(int index, Iterator column)
{
  detach();
  T* pColumnStart = static_cast<T*>(PiiTypelessMatrix::insertColumn(index, sizeof(T)));
  typename Traits::column_iterator ci(pColumnStart, stride());
  std::copy(column, column + rows(), ci);
  return ci;
}

template <class T> typename PiiMatrix<T>::Traits::column_iterator PiiMatrix<T,-1,-1>::insertColumn(int index)
{
  detach();
  T* pColumnStart = static_cast<T*>(PiiTypelessMatrix::insertColumn(index, sizeof(T)));
  typename Traits::column_iterator ci(pColumnStart, stride());
  std::fill(ci, ci + rows(), T(0));
  return ci;
}

template <class T> typename PiiMatrix<T>::Traits::column_iterator PiiMatrix<T,-1,-1>::insertColumn(int index, VaArgType firstElement, ...)
{
  detach();
  va_list rest;
  va_start(rest, firstElement);
  T* pColumnStart = static_cast<T*>(PiiTypelessMatrix::insertColumn(index, sizeof(T)));
  typename Traits::column_iterator ci(pColumnStart, stride());
  Pii::copyVaList(ci, ci + rows(), firstElement, rest);
  va_end(rest);
  return ci;
}

template <class T>
typename PiiMatrix<T>::Traits::column_iterator PiiMatrix<T,-1,-1>::appendColumn(VaArgType firstElement, ...)
{
  detach();
  va_list rest;
  va_start(rest, firstElement);
  T* pColumnStart = static_cast<T*>(PiiTypelessMatrix::insertColumn(-1, sizeof(T)));
  typename Traits::column_iterator ci(pColumnStart, stride());
  Pii::copyVaList(ci, ci + rows(), firstElement, rest);
  va_end(rest);
  return ci;
}

template <class T> void PiiMatrix<T,-1,-1>::resize(int rows, int columns)
{
  detach();
  int iOldR = this->d->iRows, iOldC = this->d->iColumns;
  PiiTypelessMatrix::resize(rows, columns, sizeof(T));
  if (columns > iOldC)
    {
      const std::size_t iBytes = sizeof(T) * (columns - iOldC);
      for (int i=0; i<rows; ++i)
        std::memset(static_cast<T*>(this->d->row(i)) + iOldC, 0, iBytes);
    }
  if (rows > iOldR)
    {
      const std::size_t iBytes = sizeof(T) * columns;
      if (iBytes > 0)
        for (int i=iOldR; i<rows; ++i)
          std::memset(static_cast<T*>(this->d->row(i)), 0, iBytes);
    }
}

template <class T> PiiMatrix<T> PiiMatrix<T,-1,-1>::identity(int size)
{
  PiiMatrix<T> result(size, size);
  for (int d=0; d<size; ++d)
    result(d,d) = T(1);
  return result;
}

template <class T> PiiMatrix<T> PiiMatrix<T,-1,-1>::column(int column) const
{
  PiiMatrix<T> result(uninitialized(1, rows()));
  std::copy(columnBegin(column), columnEnd(column), result.rowBegin(0));
  return result;
}

template <class T> void PiiMatrix<T,-1,-1>::swapRows(int r1, int r2)
{
  detach();
  T* p1 = row(r1), *p2 = row(r2);
  const int iColumns = columns();
  for (int i=0; i<iColumns; ++i)
    qSwap(p1[i], p2[i]);
}
