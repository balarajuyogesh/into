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

#ifndef _PII_TRANSPOSEDMATRIX_H
#define _PII_TRANSPOSEDMATRIX_H

#include "PiiConceptualMatrix.h"
#include <PiiMetaTemplate.h>
#include <PiiMath.h>

/// @internal
template <class Matrix> class PiiTransposedMatrixIterator
{
public:
  typedef typename Matrix::const_column_iterator ColumnIterator;
  typedef typename std::iterator_traits<ColumnIterator>::iterator_category iterator_category;
  typedef typename std::iterator_traits<ColumnIterator>::difference_type difference_type;
  typedef typename std::iterator_traits<ColumnIterator>::value_type value_type;
  typedef typename std::iterator_traits<ColumnIterator>::pointer pointer;
  typedef typename std::iterator_traits<ColumnIterator>::reference reference;

  PiiTransposedMatrixIterator(const Matrix* matrix) :
    _pMatrix(matrix),
    _columnBegin(matrix->columnBegin(0)),
    _columnEnd(matrix->columnEnd(0)),
    _iCurrentColumn(0)
  {}

  PiiTransposedMatrixIterator(const Matrix* matrix, int column) :
    _pMatrix(matrix),
    _columnBegin(matrix->columnEnd(column)),
    _columnEnd(_columnBegin),
    _iCurrentColumn(column)
  {}

  PiiTransposedMatrixIterator(const PiiTransposedMatrixIterator& other) :
    _pMatrix(other._pMatrix),
    _columnBegin(other._columnBegin),
    _columnEnd(other._columnEnd),
    _iCurrentColumn(other._iCurrentColumn)
  {}

  value_type operator[] (int index) const { return *(*this + index); }

  PiiTransposedMatrixIterator& operator= (const PiiTransposedMatrixIterator& other)
  {
    _pMatrix = other._pMatrix;
    _columnBegin = other._columnBegin;
    _columnEnd = other._columnEnd;
    _iCurrentColumn = other._iCurrentColumn;
    return *this;
  }

  PiiTransposedMatrixIterator& operator++ ()
  {
    if (++_columnBegin == _columnEnd &&
        _iCurrentColumn < _pMatrix->columns()-1)
      {
        ++_iCurrentColumn;
        _columnBegin = _pMatrix->columnBegin(_iCurrentColumn);
        _columnEnd = _pMatrix->columnEnd(_iCurrentColumn);
      }
    return *this;
  }

  PiiTransposedMatrixIterator& operator-- ()
  {
    if (_columnBegin == _pMatrix->columnBegin(_iCurrentColumn) &&
        _iCurrentColumn > 0)
      {
        --_iCurrentColumn;
        _columnEnd = _pMatrix->columnEnd(_iCurrentColumn);
        _columnBegin = _columnEnd - 1;
      }
    return *this;
  }

  PiiTransposedMatrixIterator& operator++ (int)
  {
    PiiTransposedMatrixIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  PiiTransposedMatrixIterator& operator-- (int)
  {
    PiiTransposedMatrixIterator tmp(*this);
    --(*this);
    return tmp;
  }

  PiiTransposedMatrixIterator& operator+= (int amount)
  {
    int iPos = _columnBegin - _pMatrix->columnBegin(_iCurrentColumn) + amount;
    int iColumn = iPos / _pMatrix->rows();
    int iRow = iPos % _pMatrix->rows();
    _iCurrentColumn +=  iColumn;
    _columnBegin = _pMatrix->columnBegin(_iCurrentColumn) + iRow;
    _columnEnd = _pMatrix->columnBegin(_iCurrentColumn);
    return *this;
  }
  PiiTransposedMatrixIterator& operator-= (int amount)
  {
    return operator+= (-amount);
  }

  PiiTransposedMatrixIterator operator+ (int amount) const
  {
    PiiTransposedMatrixIterator tmp(*this);
    return tmp += amount;
  }

  PiiTransposedMatrixIterator operator- (int amount) const
  {
    PiiTransposedMatrixIterator tmp(*this);
    return tmp += -amount;
  }

  bool operator== (const PiiTransposedMatrixIterator& other) const { return _columnBegin == other._columnBegin; }
  bool operator!= (const PiiTransposedMatrixIterator& other) const { return _columnBegin != other._columnBegin; }

  value_type operator* () const { return *_columnBegin; }

  difference_type operator- (const PiiTransposedMatrixIterator& other) const
  {
    return
      (_columnBegin - _pMatrix->columnBegin(_iCurrentColumn)) -
      (other._columnBegin - other._pMatrix->columnBegin(other._iCurrentColumn)) +
      (_iCurrentColumn - other._iCurrentColumn) * _pMatrix.rows();
  }

private:
  const Matrix* _pMatrix;
  ColumnIterator _columnBegin, _columnEnd;
  int _iCurrentColumn;
};

/// @hide
template <class Matrix> class PiiTransposedMatrix;
template <class Matrix> struct PiiMatrixTraits<PiiTransposedMatrix<Matrix> >
{
  enum { staticRows = Matrix::staticColumns, staticColumns = Matrix::staticRows };

  typedef typename Matrix::value_type value_type;
  typedef typename Matrix::reference reference;
  typedef PiiTransposedMatrixIterator<Matrix> const_iterator;
  typedef const_iterator iterator;
  typedef typename Matrix::const_column_iterator const_row_iterator;
  typedef typename Matrix::const_row_iterator const_column_iterator;
  typedef typename Matrix::column_iterator row_iterator;
  typedef typename Matrix::row_iterator column_iterator;
};
/// @endhide

/**
 * Transposed matrix. This class is a wrapper that exchanges the roles
 * of rows and columns of another matrix. PiiTransposedMatrix makes
 * it possible to use matrix transposes in calculations without
 * actually creating a transposed matrix in memory.
 *
 * ~~~(c++)
 * PiiMatrix<int> mat(5,4);
 * mat = Pii::transpose(mat) * mat;
 * ~~~
 */
template <class Matrix> class PiiTransposedMatrix :
  public PiiConceptualMatrix<PiiTransposedMatrix<Matrix> >
{
public:
  typedef PiiMatrixTraits<PiiTransposedMatrix<Matrix> > Traits;

  PiiTransposedMatrix(const Matrix& matrix) : _matrix(matrix) {}
#ifdef PII_CXX11
  PiiTransposedMatrix(PiiTransposedMatrix&& other) :
    _matrix(std::move(other._matrix))
  {}
  PiiTransposedMatrix& operator= (const PiiTransposedMatrix& other) = delete;
#else
  PiiTransposedMatrix(const PiiTransposedMatrix& other) :
    _matrix(other._matrix)
  {}
#endif

  typename Traits::const_iterator begin() const { return typename Traits::const_iterator(&_matrix); }
  typename Traits::const_iterator end() const { return typename Traits::const_iterator(&_matrix, qMax(_matrix.columns()-1, 0)); }

  typename Traits::const_column_iterator columnBegin(int index) const { return _matrix.rowBegin(index); }
  typename Traits::const_column_iterator columnEnd(int index) const { return _matrix.rowEnd(index); }

  typename Traits::const_row_iterator rowBegin(int index) const { return _matrix.columnBegin(index); }
  typename Traits::const_row_iterator rowEnd(int index) const { return _matrix.columnEnd(index); }

  int rows() const { return _matrix.columns(); }
  int columns() const { return _matrix.rows(); }

private:
  Matrix _matrix;
};

namespace Pii
{
  /**
   * Returns the transpose of *mat*. This function will neither
   * reserve memory for a transposed matrix nor copy the values.
   * Instead, it creates a wrapper that exchanges the roles of rows
   * and columns in *mat*.
   *
   * @relates PiiTransposedMatrix
   */
  template <class Matrix>
  inline PiiTransposedMatrix<Matrix> transpose(const PiiConceptualMatrix<Matrix>& mat)
  {
    return PiiTransposedMatrix<Matrix>(mat.selfRef());
  }
}

#endif //_PII_TRANSPOSEDMATRIX_H
