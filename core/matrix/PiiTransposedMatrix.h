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
  typedef std::forward_iterator_tag iterator_category;
  typedef typename Matrix::const_column_iterator ColumnIterator;
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

  PiiTransposedMatrixIterator& operator= (const PiiTransposedMatrixIterator& other)
  {
    const_cast<Matrix*>(_pMatrix) = other._pMatrix;
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

  PiiTransposedMatrixIterator& operator++ (int)
  {
    PiiTransposedMatrixIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  bool operator== (const PiiTransposedMatrixIterator& other) const { return _columnBegin == other._columnBegin; }
  bool operator!= (const PiiTransposedMatrixIterator& other) const { return _columnBegin != other._columnBegin; }

  value_type operator* () const { return *_columnBegin; }
  
private:
  const Matrix* _pMatrix;
  ColumnIterator _columnBegin, _columnEnd;
  int _iCurrentColumn;
};

/// @cond null
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
/// @endcond

/**
 * Transposed matrix. This class is a wrapper that exchanges the roles
 * of rows and columns of another matrix. PiiTransposedMatrix makes
 * it possible to use matrix transposes in calculations without
 * actually creating a transposed matrix in memory.
 *
 * ~~~
 * PiiMatrix<int> mat(5,4);
 * mat = Pii::transpose(mat) * mat;
 * ~~~
 *
 */
template <class Matrix> class PiiTransposedMatrix :
  public PiiConceptualMatrix<PiiTransposedMatrix<Matrix> >
{
public:
  typedef PiiMatrixTraits<PiiTransposedMatrix<Matrix> > Traits;

  PiiTransposedMatrix(const Matrix& matrix) : _matrix(matrix) {}
  PiiTransposedMatrix(const PiiTransposedMatrix& other) : _matrix(other._matrix) {}
  PiiTransposedMatrix& operator= (const PiiTransposedMatrix& other)
  {
    _matrix = other._matrix;
    return *this;
  }
#ifdef PII_CXX0X
  PiiTransposedMatrix(PiiTransposedMatrix&& other) : _matrix(std::move(other._matrix)) {}
  PiiTransposedMatrix& operator= (PiiTransposedMatrix&& other)
  {
    std::swap(_matrix, other._matrix);
    return *this;
  }
#endif
  
  typename Traits::const_iterator begin() const { return typename Traits::const_iterator(&_matrix); }
  typename Traits::const_iterator end() const { return typename Traits::const_iterator(&_matrix, qMax(_matrix.columns()-1, 0)); }
  
  typename Traits::const_column_iterator columnBegin(int index) const { return _matrix.rowBegin(index); }
  typename Traits::const_column_iterator columnEnd(int index) const { return _matrix.rowEnd(index); }

  typename Traits::const_row_iterator rowBegin(int index) const { return _matrix.columnBegin(index); }
  typename Traits::const_row_iterator rowEnd(int index) const { return _matrix.columnEnd(index); }

  int rows() const { return _matrix.columns(); }
  int columns() const { return _matrix.rows(); }

  typename Traits::value_type operator() (int r, int c) const { return _matrix(c,r); }

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
