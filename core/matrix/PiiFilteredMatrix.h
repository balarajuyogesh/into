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

#ifndef _PIIFILTEREDMATRIX_H
#define _PIIFILTEREDMATRIX_H

template <class Matrix, class Filter> class PiiFilteredMatrix;
template <class Matrix, class Filter> struct PiiMatrixTraits<PiiFilteredMatrix<Matrix, Filter> >
{
  enum
    {
      staticRows = Pii::MinInt<Matrix::staticRows, Filter::staticRows>::intValue,
      staticColumns = Pii::MinInt<Matrix::staticColumns, Filter::staticColumns>::intValue
    };
  
  typedef typename Pii::IfClass<Pii::IsConst<Matrix>,
                                PiiFilteredIterator<typename Matrix::const_iterator,
                                                    typename Filter::const_iterator,
                                                    true>,
                                PiiFilteredIterator<typename Matrix::iterator,
                                                    typename Filter::const_iterator> >::Type iterator;
  typedef iterator const_iterator;
  typedef iterator row_iterator;
  typedef const_iterator const_row_iterator;
  typedef iterator column_iterator;
  typedef const_iterator const_column_iterator;
  
  // const_iterator is always const_iterator
  //typedef PiiFilteredIterator<typename Matrix::const_iterator, typename Filter::const_iterator> const_iterator;
  typedef typename std::iterator_traits<typename Matrix::iterator>::value_type value_type;
  typedef typename std::iterator_traits<typename Matrix::iterator>::reference reference;
};

/**
 * A model of the *matrix* concept that accesses selected elements of
 * another matrix. PiiFilteredMatrix is a 1-by-N matrix in which each
 * element refers to an element in another matrix that matches a
 * filter. An element is a match if the corresponding element in the
 * filter is non-zero.
 *
 * ~~~(c++)
 * PiiMatrix<int> mat(3, 3,
 *                    -3, -2, -1,
 *                    -4,  0,  1,
 *                     2,  3,  4);
 * PiiMatrix<bool> filter(3, 3,
 *                        1, 0, 1,
 *                        1, 0, 0,
 *                        0, 0, 1);
 *
 * PiiFilteredMatrix<PiiMatrix<int>, PiiMatrix<bool> > filtered(mat, filter);
 * // filtered = (-3, -1, -4, 4)
 * ~~~
 *
 * Usually, there is no need to explicitly create an instance of
 * PiiFilteredMatrix. Use the [Pii::filteredMatrix()] function
 * instead. PiiMatrix also has an `operator`() that takes another
 * matrix as a parameter and returns a filtered matrix.
 *
 * ~~~(c++)
 * PiiMatrix<int> filtered(Pii::filteredMatrix(mat, filter));
 * // Even simpler
 * PiiMatrix<int> filtered2(mat(filter));
 * ~~~
 *
 */
template <class Matrix, class Filter>
class PiiFilteredMatrix : public PiiConceptualMatrix<PiiFilteredMatrix<Matrix,Filter> >
{
public:
  typedef PiiConceptualMatrix<PiiFilteredMatrix<Matrix,Filter> > BaseType;
  typedef PiiMatrixTraits<PiiFilteredMatrix<Matrix,Filter> > Traits;
  
  PiiFilteredMatrix(Matrix& matrix, const Filter& filter) :
    _begin(matrix.begin(), matrix.end(), filter.begin()),
    _end(_begin.begin(), matrix.end(), matrix.end(), filter.end()),
    _iColumns(-1)
  {
  }

#ifdef PII_CXX0X
  PiiFilteredMatrix(PiiFilteredMatrix&& other) :
    _begin(other._begin),
    _end(other._end),
    _iColumns(other._iColumns)
  {}
#else
  PiiFilteredMatrix(const PiiFilteredMatrix& other) :
    _begin(other._begin),
    _end(other._end),
    _iColumns(other._iColumns)
  {}
#endif
  PiiFilteredMatrix& operator= (typename Traits::value_type value) { return BaseType::operator=(value); }

  typename BaseType::const_iterator begin() const { return _begin; }
  typename BaseType::const_iterator end() const { return _end; }
  typename BaseType::iterator begin() { return _begin; }
  typename BaseType::iterator end() { return _end; }

  typename BaseType::const_row_iterator rowBegin(int index) const { return _begin; }
  typename BaseType::row_iterator rowBegin(int index) { return _begin; }
  typename BaseType::const_column_iterator columnBegin(int index) const { return _begin + index; }
  typename BaseType::column_iterator columnBegin(int index) { return _end; }

  int rows() const { return 1; }
  int columns() const
  {
    if (_iColumns == -1)
      {
        typedef typename Filter::value_type T;
        _iColumns = std::count_if(_begin.filter(), _end.filter(),
                                  std::bind2nd(std::not_equal_to<T>(), T(0)));
      }
    return _iColumns;
  }

private:
  PiiFilteredMatrix& operator= (const PiiFilteredMatrix& other);
#ifdef PII_CXX0X
  PiiFilteredMatrix(const PiiFilteredMatrix& other);
#endif
  
  typename BaseType::iterator _begin, _end;
  mutable int _iColumns;
};

namespace Pii
{
  /**
   * Creates a PiiFilteredMatrix that accesses those elements of 
   * *matrix* whose corresponding element in *filter* is non-zero. Note
   * that this function does not check that *matrix* and *filter*
   * are equal in size.
   *
   * @relates PiiFilteredMatrix
   */
  template <class Matrix, class Filter>
  inline PiiFilteredMatrix<Matrix, Filter> filteredMatrix(Matrix& matrix, const Filter& filter)
  {
    return PiiFilteredMatrix<Matrix, Filter>(matrix, filter);
  }
}

#endif //_PIIFILTEREDMATRIX_H
