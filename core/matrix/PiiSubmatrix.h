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
#  error "Never use <PiiSubmatrix.h> directly; include <PiiConceptualMatrix.h> instead."
#endif

template <class Matrix>
struct PiiMatrixTraits<PiiSubmatrix<Matrix> >
{
  enum { staticRows = -1, staticColumns = -1 };

  typedef typename Matrix::value_type value_type;
  typedef typename Matrix::reference reference;
  typedef PiiMatrixIterator<typename Matrix::const_iterator> const_iterator;
  typedef PiiMatrixIterator<typename Matrix::iterator> iterator;

  typedef typename Matrix::const_row_iterator const_row_iterator;
  typedef typename Matrix::const_column_iterator const_column_iterator;
  typedef typename Matrix::row_iterator row_iterator;
  typedef typename Matrix::column_iterator column_iterator;
};

template <class Derived, class Matrix> class PiiSubmatrixBase :
  public PiiConceptualMatrix<Derived>
{
public:
  typedef PiiMatrixTraits<Derived> Traits;

  typename Traits::const_iterator begin() const
  {
    typename Matrix::const_iterator it = _mat.begin() + (_iRow * _mat.columns() + _iColumn);
    return typename Traits::const_iterator(it, _iColumns, _mat.columns());
  }
  typename Traits::const_iterator end() const
  {
    typename Matrix::const_iterator it = _mat.begin() + (_iRow * _mat.columns() + _iColumn);
    return typename Traits::const_iterator(it, it + ((_iRows - 1) * _mat.columns() + _iColumns),
                                           _iColumns, _mat.columns());
  }
  typename Traits::iterator begin()
  {
    typename Matrix::iterator it = _mat.begin() + (_iRow * _mat.columns() + _iColumn);
    return typename Traits::iterator(it, _iColumns, _mat.columns());
  }
  typename Traits::iterator end()
  {
    typename Matrix::iterator it = _mat.begin() + (_iRow * _mat.columns() + _iColumn);
    return typename Traits::iterator(it, it + ((_iRows - 1) * _mat.columns() + _iColumns),
                                     _iColumns, _mat.columns());
  }

  typename Traits::const_row_iterator rowBegin(int index) const
  {
    return _mat.constRowBegin(index + _iRow) + _iColumn;
  }
  typename Traits::const_column_iterator columnBegin(int index) const
  {
    return _mat.constColumnBegin(index + _iColumn) + _iRow;
  }
  typename Traits::row_iterator rowBegin(int index)
  {
    return _mat.rowBegin(index + _iRow) + _iColumn;
  }
  typename Traits::column_iterator columnBegin(int index)
  {
    return _mat.columnBegin(index + _iColumn) + _iRow;
  }
  typename Traits::const_row_iterator rowEnd(int index) const
  {
    return rowBegin(index) + _iColumns;
  }
  typename Traits::const_column_iterator columnEnd(int index) const
  {
    return columnBegin(index) + _iRows;
  }

  int firstRow() const { return _iRow; }
  int firstColumn() const { return _iColumn; }
  int rows() const { return _iRows; }
  int columns() const { return _iColumns; }

  Matrix& source() const { return _mat; }

protected:
  PiiSubmatrixBase(Matrix& mat, int r, int c, int rows, int columns) :
    _mat(mat), _iRow(r), _iColumn(c), _iRows(rows), _iColumns(columns)
  {}

  Matrix& _mat;
  int _iRow, _iColumn, _iRows, _iColumns;
};

template <class Matrix>
class PiiSubmatrix :
  public PiiSubmatrixBase<PiiSubmatrix<Matrix>, Matrix>
{
public:
  typedef PiiSubmatrixBase<PiiSubmatrix, Matrix> SuperType;

  PiiSubmatrix(Matrix& mat, int r, int c, int rows, int columns) :
    SuperType(mat, r, c, rows, columns)
  {}

  using PiiConceptualMatrix<PiiSubmatrix>::operator=;
};
