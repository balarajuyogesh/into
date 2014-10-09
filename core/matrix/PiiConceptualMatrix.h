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

#ifndef _PIICONCEPTUALMATRIX_H
#define _PIICONCEPTUALMATRIX_H

#include <functional>
#include <algorithm>
#include "PiiFunctional.h"
#include "PiiAlgorithm.h"
#include "PiiIterator.h"
#include "PiiTypeTraits.h"
#include "PiiMetaTemplate.h"
#include <QString>

/**
 * Throws a PiiInvalidArgumentException due to invalid matrix sizes.
 */
#define PII_MATRIX_SIZE_MISMATCH PII_THROW(PiiInvalidArgumentException, Pii::sizeMismatchErrorMessage())

/**
 * Checks that the matrices (references) `a` and `b` are of equal
 * size. If they aren't, throws a PiiInvalidArgumentException.
 */
#define PII_MATRIX_CHECK_EQUAL_SIZE(a,b) if ((a).self()->rows() != (b).self()->rows() || \
                                             (a).self()->columns() != (b).self()->columns()) PII_MATRIX_SIZE_MISMATCH

/**
 * Checks that matrix `a` is square. If it isn't, throws a
 * PiiInvalidArgumentException.
 */
#define PII_MATRIX_CHECK_SQUARE(a) if ((a).rows() != (a).columns()) PII_THROW(PiiInvalidArgumentException, Pii::notSquareErrorMessage())

namespace Pii
{
  /// @internal
  PII_CORE_EXPORT QString notSquareErrorMessage();
  /// @internal
  PII_CORE_EXPORT QString sizeMismatchErrorMessage();
}

template <class Matrix> struct PiiMatrixTraits;
template <class Matrix, class UnaryFunction> class PiiUnaryMatrixTransform;
template <class Matrix1, class Matrix2, class BinaryFunction> class PiiBinaryMatrixTransform;

#define PII_MATRIX_SCALAR_ASSIGNMENT_OPERATOR(OPERATOR, FUNCTION) \
Derived& operator OPERATOR ## = (typename PiiMatrixTraits<Derived>::value_type value) \
{ \
  Pii::map(self()->begin(), self()->end(), \
           std::bind2nd(FUNCTION<typename PiiMatrixTraits<Derived>::value_type>(), value)); \
  return selfRef(); \
}

#define PII_MATRIX_MATRIX_ASSIGNMENT_OPERATOR_DECL(OPERATOR) \
template <class Matrix> \
Derived& operator OPERATOR ## = (const PiiConceptualMatrix<Matrix>& other);

#define PII_MATRIX_MATRIX_ASSIGNMENT_OPERATOR_DEF(OPERATOR, FUNCTION) \
template <class Derived> template <class Matrix> \
Derived& PiiConceptualMatrix<Derived>::operator OPERATOR ## = (const PiiConceptualMatrix<Matrix>& other) \
{ \
  PII_MATRIX_CHECK_EQUAL_SIZE(*this, other); \
  Pii::map(self()->begin(), self()->end(), other.self()->begin(), \
           FUNCTION<typename PiiMatrixTraits<Derived>::value_type>()); \
  return selfRef(); \
}

#define PII_BOTH_MATRIX_ASSIGNMENT_OPERATORS(OPERATOR, FUNCTION) \
  PII_MATRIX_MATRIX_ASSIGNMENT_OPERATOR_DECL(OPERATOR) \
  PII_MATRIX_SCALAR_ASSIGNMENT_OPERATOR(OPERATOR, FUNCTION)

template <class Matrix> class PiiSubmatrix;
/// @endhide

/**
 * A superclass for classes that implement the *matrix* concept. A
 * model of the matrix concept
 *
 * - must define functions called [rows()] and [columns()], for
 * getting the number of rows and columns, respectively.
 *
 * - must provide at least input iterators for accessing the data of
 * the matrix as a whole. The class must have [begin()] and [end()]
 * functions for getting the iterators. The iterators must be randomly
 * accessible. At least `const` versions of the iterators must be
 * provided. The iterators must scan the matrix in row-major order:
 * the end of row N is immediately followed by the start of row N+1.
 *
 * - may optionally provide iterators for accessing individual rows
 * and columns. If no such iterators are provided, default
 * implementations will be provided.
 *
 * Furthermore, all models of the matrix concept must provide a
 * specialization of the PiiMatrixTraits structure. The specialization
 * must define types related to the matrix model:
 *
 * - `value_type` for the content type.
 * - `reference` for a reference to the content type.
 * - `const_iterator` for read-only access.
 * - `iterator` for read-write access.
 *
 * - `column_iterator` for read-write column-wise iterators.
 * - `const_column_iterator` for read-only column-wise iterators.
 * - `row_iterator` for read-write row-wise iterators.
 * - `const_row_iterator` for read-only row-wise iterators.
 *
 * If you create a class that models the matrix concept, you can mark
 * it as such as using the curiously recurring template pattern (CRTP)
 * as follows:
 *
 * ~~~(c++)
 * // First declare types related to your matrix implementation. This
 * // needs to come first because PiiConceptualMatrix matrix needs it
 * // when instantiated. Since My3x3Matrix is not declared yet, a
 * // forward declaration is needed.
 * class My3x3Matrix;
 * template <> struct PiiMatrixTraits<My3x3Matrix>
 * {
 *   typedef int value_type;
 *   typedef int& reference;
 *   typedef int* iterator;
 *   typedef const int* const_iterator;
 * };
 *
 * class My3x3Matrix : public PiiConceptualMatrix<My3x3Matrix>
 * {
 * public:
 *   typedef PiiConceptualMatrix<My3x3Matrix> BaseType;
 *
 *   const_iterator begin() const { return _data; }
 *   const_iterator end() const { return _data + 9; }
 *   int rows() const { return 3; }
 *   int columns() const { return 3; }
 *   // Now we have the minimum functionality.
 *   // Other functions may be added to modify the data etc.
 * private:
 *   int _data[9];
 * };
 * ~~~
 *
 * Once you have done this, your matrix class can be used with other
 * matrix classes in arithmetic operations. For example, you can do
 * the following:
 *
 * ~~~(c++)
 * PiiMatrix<int> mat(3,3);
 * My3x3Matrix mat2;
 * mat += mat2 / 4 + 5;
 * mat -= mat2 + mat;
 * ~~~
 */
template <class Derived> class PiiConceptualMatrix
{
public:
  typedef Derived DerivedType;
  typedef PiiMatrixTraits<Derived> Traits;
  enum { staticRows = Traits::staticRows, staticColumns = Traits::staticColumns };

  typedef typename Traits::const_iterator const_iterator;
  typedef typename Traits::iterator iterator;
  typedef typename std::iterator_traits<iterator>::value_type value_type;
  typedef typename std::iterator_traits<iterator>::reference reference;

  typedef typename Traits::column_iterator column_iterator;
  typedef typename Traits::const_column_iterator const_column_iterator;
  typedef typename Traits::row_iterator row_iterator;
  typedef typename Traits::const_row_iterator const_row_iterator;

  inline Derived* self() { return static_cast<Derived*>(this); }
  inline const Derived* self() const { return static_cast<const Derived*>(this); }
  inline Derived& selfRef() { return *static_cast<Derived*>(this); }
  inline const Derived& selfRef() const { return *static_cast<const Derived*>(this); }
  inline Derived& mutableRef() const { return const_cast<Derived&>(selfRef()); }

  /**
   * Returns `true` if the matrix is empty, and `false` otherwise.
   * An empty matrix cannot hold a single element, i.e. there are
   * either zero rows or zero columns. Any access to an element within
   * an empty matrix will reference illegal memory.
   */
  bool isEmpty() const { return self()->rows() * self()->columns() == 0; }

  const_iterator constBegin() const { return self()->begin(); }
  const_iterator constEnd() const { return self()->end(); }

  const_row_iterator operator[] (int index) const { return self()->rowBegin(index); }
  const_row_iterator rowEnd(int index) const { return self()->rowBegin(index) + self->columns(); }
  const_row_iterator constRowBegin(int index) const { return self()->rowBegin(index); }
  const_row_iterator constRowEnd(int index) const { return self()->rowEnd(index); }
  row_iterator operator[] (int index) { return self()->rowBegin(index); }
  row_iterator rowEnd(int index) { return self()->rowBegin(index) + self()->columns(); }
  const_column_iterator columnEnd(int index) const { return self()->columnBegin(index) + self()->rows(); }
  const_column_iterator constColumnBegin(int index) const { return self()->columnBegin(index); }
  const_column_iterator constColumnEnd(int index) const { return self()->columnEnd(index); }
  column_iterator columnEnd(int index) { return self()->columnBegin(index) + self()->rows(); }

  inline PiiSubmatrix<const Derived> operator() (int r, int c, int rows, int columns) const;
  inline PiiSubmatrix<Derived> operator() (int r, int c, int rows, int columns);

  typename Traits::value_type operator() (int r, int c) const { return self()->rowBegin(r)[c]; }
  typename Traits::reference operator() (int r, int c) { return self()->rowBegin(r)[c]; }

  /**
   * Returns a copy of an item in the matrix.
   */
  typename Traits::value_type at(int r, int c) const { return self()->rowBegin(r)[c]; }

  PII_BOTH_MATRIX_ASSIGNMENT_OPERATORS(+, std::plus)
  PII_BOTH_MATRIX_ASSIGNMENT_OPERATORS(-, std::minus)
  PII_BOTH_MATRIX_ASSIGNMENT_OPERATORS(|, Pii::BinaryOr)
  PII_BOTH_MATRIX_ASSIGNMENT_OPERATORS(&, Pii::BinaryAnd)

  PII_MATRIX_SCALAR_ASSIGNMENT_OPERATOR(*, std::multiplies)
  PII_MATRIX_SCALAR_ASSIGNMENT_OPERATOR(/, std::divides)

  /**
   * Sets all elements to *value* and returns a reference to self().
   */
  Derived& operator= (value_type value)
  {
    std::fill(self()->begin(), self()->end(), value);
    return selfRef();
  }

  /**
   * Moves the contents of *other* to this matrix element-by-element.
   *
   * @exception PiiInvalidArgumentException& if this matrix is not
   * equal to *other* in size.
   */
  template <class Matrix>
  Derived& operator<< (const Matrix& other);

  /**
   * Applies the *adaptable binary function* to all elements of
   * this matrix and the corresponding element in *other*. The
   * matrices must be of equal size. The result is stored in this
   * matrix. For example, to apply the operator-= the other way, do
   * this:
   *
   * ~~~(c++)
   * PiiMatrix<int> a, b;
   * a.map(std::minus<int>(), b);
   * ~~~
   *
   * @exception PiiInvalidArgumentException& if this matrix is not
   * equal to *other* in size.
   */
  template <class BinaryFunc, class Matrix>
  Derived& map(BinaryFunc op, const PiiConceptualMatrix<Matrix>& other);

  /**
   * Applies a binary function to all elements of this matrix and the
   * scalar *value*. The result is stored in this matrix. An example:
   *
   * ~~~(c++)
   * PiiMatrix<int> a;
   * a.map(std::minus<int>(), 5);
   * // The same can be achieved with
   * a.map(std::bind2nd(std::minus<int>(), 5));
   * // ... but which one is more readable?
   * ~~~
   */
  template <class BinaryFunc>
  Derived& map(BinaryFunc op, typename BinaryFunc::second_argument_type value)
  {
    Pii::map(self()->begin(), self()->end(), std::bind2nd(op, value));
    return selfRef();
  }

  /**
   * Applies a unary function to all elements in this matrix. For
   * example, to negate all elements in a matrix, do the following:
   *
   * ~~~(c++)
   * PiiMatrix<int> a;
   * a.map(std::negate<int>());
   * ~~~
   */
  template <class UnaryFunc>
  Derived& map(UnaryFunc op)
  {
    Pii::map(self()->begin(), self()->end(), op);
    return selfRef();
  }

  /**
   * Applies the *adaptable binary function* `op` to all
   * elements of this matrix and the corresponding elements in
   * *other*. The matrices must be of equal size. The result is returned
   * in a new matrix. For example, to explicitly apply the addition
   * operation, do this:
   *
   * ~~~(c++)
   * PiiMatrix<int> a, b;
   * PiiMatrix<int> c(a.mapped(std::plus<int>(), b));
   * ~~~
   *
   * @exception PiiInvalidArgumentException& if this matrix is not
   * equal to *other* in size.
   */
  template <class BinaryFunc, class Matrix>
  PiiBinaryMatrixTransform<Derived, Matrix, BinaryFunc>
  mapped(BinaryFunc op, const PiiConceptualMatrix<Matrix>& other) const;

  /**
   * Applies a binary function to all elements of this matrix and the
   * scalar *value*. The result is returned in a new matrix. An
   * example:
   *
   * ~~~(c++)
   * PiiMatrix<int> a;
   * PiiMatrix<int> c(a.mapped(std::plus<int>(), 5));
   * ~~~
   */
  template <class BinaryFunc>
  PiiUnaryMatrixTransform<Derived, std::binder2nd<BinaryFunc> >
  mapped(BinaryFunc op, typename BinaryFunc::second_argument_type value) const;

  /**
   * Creates a matrix that contains the result of applying a unary
   * function to all elements in this matrix. For example, to create a
   * negation of a matrix, do the following:
   *
   * ~~~(c++)
   * PiiMatrix<int> a;
   * PiiMatrix<int> b(a.mapped(std::negate<int>());
   *
   * // Different result type (convert complex numbers to real numbers)
   * PiiMatrix<std::complex<float> > a;
   * PiiMatrix<float> b(a.mapped(Pii::Abs<std::complex<float> >()));
   * ~~~
   */
  template <class UnaryFunc>
  PiiUnaryMatrixTransform<Derived, UnaryFunc> mapped(UnaryFunc op) const;

  template <class T>
  PiiUnaryMatrixTransform<Derived, Pii::Cast<value_type,T> > cast() const
  {
    return mapped(Pii::Cast<value_type,T>());
  }

protected:
  void fixIndices(int &r, int &c, int &rows, int &columns) const
  {
    if (r < 0) r += self()->rows();
    if (c < 0) c += self()->columns();
    if (rows < 0) rows += self()->rows() - r + 1;
    if (columns < 0) columns += self()->columns() - c + 1;
  }
};

#include "PiiMatrixIterator.h"
#include "PiiSubmatrix.h"

template <class Derived>
PiiSubmatrix<const Derived> PiiConceptualMatrix<Derived>::operator() (int r, int c, int rows, int columns) const
{
  fixIndices(r, c, rows, columns);
  return PiiSubmatrix<const Derived>(selfRef(), r, c, rows, columns);
}

template <class Derived>
PiiSubmatrix<Derived> PiiConceptualMatrix<Derived>::operator() (int r, int c, int rows, int columns)
{
  fixIndices(r, c, rows, columns);
  return PiiSubmatrix<Derived>(selfRef(), r, c, rows, columns);
}


template <class Matrix, class UnaryFunction>
struct PiiMatrixTraits<PiiUnaryMatrixTransform<Matrix, UnaryFunction> >
{
  enum { staticRows = Matrix::staticRows, staticColumns = Matrix::staticColumns };

  typedef typename UnaryFunction::result_type value_type;
  typedef value_type reference;
  typedef PiiUnaryFunctionIterator<typename Matrix::const_iterator, UnaryFunction> const_iterator;
  typedef const_iterator iterator;

  typedef PiiUnaryFunctionIterator<typename Matrix::const_row_iterator, UnaryFunction> const_row_iterator;
  typedef PiiUnaryFunctionIterator<typename Matrix::const_column_iterator, UnaryFunction> const_column_iterator;
  typedef const_row_iterator row_iterator;
  typedef const_column_iterator column_iterator;
};

/**
 * A matrix that models the *matrix* concept by applying a unary
 * function to another matrix. Given a model of the matrix concept,
 * `mat`, and a function `func`, this matrix works as if it was the
 * result of calculating `func(mat)`. Note that
 * PiiUnaryMatrixTransform is immutable. It provides no way of
 * modifying the source data.
 *
 * @see PiiConceptualMatrix
 *
 */
template <class Matrix, class UnaryFunction> class PiiUnaryMatrixTransform :
  public PiiConceptualMatrix<PiiUnaryMatrixTransform<Matrix, UnaryFunction> >
{
public:
  typedef PiiMatrixTraits<PiiUnaryMatrixTransform<Matrix, UnaryFunction> > Traits;

  PiiUnaryMatrixTransform(const Matrix& mat, const UnaryFunction& func) :
    _mat(mat), _func(func)
  {}

  PiiUnaryMatrixTransform(const PiiUnaryMatrixTransform& other) :
    _mat(other._mat), _func(other._func)
  {}

  PiiUnaryMatrixTransform& operator= (const PiiUnaryMatrixTransform& other)
  {
    _mat = other._mat;
    _func = other._func;
    return *this;
  }

  int rows() const { return _mat.rows(); }
  int columns() const { return _mat.columns(); }

  typename Traits::value_type operator() (int r, int c) const { return _func(_mat(r,c)); }

  typename Traits::const_iterator begin() const
  {
    return typename Traits::const_iterator(_mat.begin(), _func);
  }
  typename Traits::const_iterator end() const
  {
    return typename Traits::const_iterator(_mat.end(), _func);
  }

  typename Traits::const_row_iterator rowBegin(int index) const
  {
    return typename Traits::const_row_iterator(_mat.rowBegin(index), _func);
  }
  typename Traits::const_column_iterator columnBegin(int index) const
  {
    return typename Traits::const_column_iterator(_mat.columnBegin(index), _func);
  }
  typename Traits::const_row_iterator rowEnd(int index) const
  {
    return typename Traits::const_row_iterator(_mat.roEnd(index), _func);
  }
  typename Traits::const_column_iterator columnEnd(int index) const
  {
    return typename Traits::const_column_iterator(_mat.columnEnd(index), _func);
  }

private:
  const Matrix& _mat;
  UnaryFunction _func;
};

template <class Matrix1, class Matrix2, class BinaryFunction>
struct PiiMatrixTraits<PiiBinaryMatrixTransform<Matrix1, Matrix2, BinaryFunction> >
{
  enum
    {
      staticRows = Pii::MinInt<Matrix1::staticRows, Matrix2::staticRows>::intValue,
      staticColumns = Pii::MinInt<Matrix1::staticColumns, Matrix2::staticColumns>::intValue
    };

  typedef PiiBinaryFunctionIterator<typename Matrix1::const_iterator,
                                    typename Matrix2::const_iterator,
                                    BinaryFunction> const_iterator;
  typedef const_iterator iterator;
  typedef typename BinaryFunction::result_type value_type;
  typedef value_type reference;

  typedef PiiBinaryFunctionIterator<typename Matrix1::const_row_iterator,
                                    typename Matrix2::const_row_iterator,
                                    BinaryFunction> const_row_iterator;
  typedef PiiBinaryFunctionIterator<typename Matrix1::const_column_iterator,
                                    typename Matrix2::const_column_iterator,
                                    BinaryFunction> const_column_iterator;
  typedef const_row_iterator row_iterator;
  typedef const_column_iterator column_iterator;
};

/**
 * A matrix that models the *matrix* concept by using a binary
 * function that operates on two other matrices. Given two models of
 * the matrix concept, `mat1` and `mat2`, and a binary function
 * `func`, this matrix works as if it was the result of calculating
 * `func(mat1, mat2)`. Note that PiiBinaryMatrixTransform is
 * immutable. It provides no way of modifying the source data.
 *
 * @see PiiConceptualMatrix
 *
 */
template <class Matrix1, class Matrix2, class BinaryFunction> class PiiBinaryMatrixTransform :
public PiiConceptualMatrix<PiiBinaryMatrixTransform<Matrix1, Matrix2, BinaryFunction> >
{
public:
  typedef PiiMatrixTraits<PiiBinaryMatrixTransform<Matrix1, Matrix2, BinaryFunction> > Traits;

  PiiBinaryMatrixTransform(const Matrix1& mat1, const Matrix2& mat2, const BinaryFunction& func) :
    _mat1(mat1), _mat2(mat2), _func(func)
  {}

  PiiBinaryMatrixTransform(const PiiBinaryMatrixTransform& other) :
    _mat1(other._mat1), _mat2(other._mat2), _func(other._func)
  {}

  PiiBinaryMatrixTransform& operator= (const PiiBinaryMatrixTransform& other)
  {
    _mat1 = other._mat1;
    _mat2 = other._mat2;
    _func = other._func;
    return *this;
  }

  int rows() const { return _mat1.rows(); }
  int columns() const { return _mat1.columns(); }

  typename Traits::value_type operator() (int r, int c) const { return _func(_mat1(r,c), _mat2(r,c)); }

  typename Traits::const_iterator begin() const
  {
    return typename Traits::const_iterator(_mat1.begin(), _mat2.begin(), _func);
  }
  typename Traits::const_iterator end() const { return typename Traits::const_iterator(_mat1.end(), _mat2.end(), _func); }

  typename Traits::const_row_iterator rowBegin(int index) const
  {
    return typename Traits::const_row_iterator(_mat1.rowBegin(index), _mat2.rowBegin(index), _func);
  }
  typename Traits::const_row_iterator rowEnd(int index) const
  {
    return typename Traits::const_row_iterator(_mat1.rowEnd(index), _mat2.rowEnd(index), _func);
  }
  typename Traits::const_column_iterator columnBegin(int index) const
  {
    return typename Traits::const_column_iterator(_mat1.columnBegin(index), _mat2.columnBegin(index), _func);
  }
  typename Traits::const_column_iterator columnEnd(int index) const
  {
    return typename Traits::const_column_iterator(_mat1.columnEnd(index), _mat2.columnEnd(index), _func);
  }

private:
  const Matrix1& _mat1;
  const Matrix2& _mat2;
  BinaryFunction _func;
};


namespace Pii
{
  /**
   * Creates a PiiUnaryMatrixTransform that returns *func*(*mat*).
   *
   * @relates PiiUnaryMatrixTransform
   */
  template <class Matrix, class UnaryFunction>
  inline PiiUnaryMatrixTransform<Matrix, UnaryFunction> unaryMatrixTransform(const Matrix& mat,
                                                                             const UnaryFunction& func)
  {
    return PiiUnaryMatrixTransform<Matrix, UnaryFunction>(mat, func);
  }

  /**
   * Creates a PiiBinaryMatrixTransform that returns *func*(*mat1*,
   * *mat2*). Note that this function does not check that *matrix*
   * and *filter* are equal in size.
   *
   * @relates PiiBinaryMatrixTransform
   */
  template <class Matrix1, class Matrix2, class BinaryFunction>
  inline PiiBinaryMatrixTransform<Matrix1, Matrix2, BinaryFunction> binaryMatrixTransform(const Matrix1& mat1,
                                                                                          const Matrix2& mat2,
                                                                                          const BinaryFunction& func)
  {
    return PiiBinaryMatrixTransform<Matrix1, Matrix2, BinaryFunction>(mat1, mat2, func);
  }

}

namespace Pii
{
  /**
   * Compares two matrices. Matrices are equal if all of their entries
   * are equal.
   */
  template <class Matrix1, class Matrix2>
  bool equals(const PiiConceptualMatrix<Matrix1>& mat1,
              const PiiConceptualMatrix<Matrix2>& mat2)
  {
    if (mat1.self()->rows() != mat2.self()->rows() ||
        mat1.self()->columns() != mat2.self()->columns())
      return false;
    return std::equal(mat1.self()->begin(), mat1.self()->end(), mat2.self()->begin());
  }
}

/// @hide
#include "PiiInvalidArgumentException.h"

PII_MATRIX_MATRIX_ASSIGNMENT_OPERATOR_DEF(+, std::plus)
PII_MATRIX_MATRIX_ASSIGNMENT_OPERATOR_DEF(-, std::minus)
PII_MATRIX_MATRIX_ASSIGNMENT_OPERATOR_DEF(|, Pii::BinaryOr)
PII_MATRIX_MATRIX_ASSIGNMENT_OPERATOR_DEF(&, Pii::BinaryAnd)

template <class Derived> template <class Matrix>
Derived& PiiConceptualMatrix<Derived>::operator<< (const Matrix& other)
{
  PII_MATRIX_CHECK_EQUAL_SIZE(*this, other);
  std::transform(other.begin(), other.end(), self()->begin(),
                 Pii::Cast<typename Matrix::value_type, value_type>());
  return selfRef();
}

template <class Derived> template <class UnaryFunc>
PiiUnaryMatrixTransform<Derived, UnaryFunc>
PiiConceptualMatrix<Derived>::mapped(UnaryFunc func) const
{
  return Pii::unaryMatrixTransform(selfRef(), func);
}

template <class Derived> template <class BinaryFunc>
PiiUnaryMatrixTransform<Derived, std::binder2nd<BinaryFunc> >
PiiConceptualMatrix<Derived>::mapped(BinaryFunc func, typename BinaryFunc::second_argument_type value) const
{
  return Pii::unaryMatrixTransform(selfRef(), std::bind2nd(func, value));
}

template <class Derived> template <class BinaryFunc, class Matrix>
Derived& PiiConceptualMatrix<Derived>::map(BinaryFunc op, const PiiConceptualMatrix<Matrix>& other)
{
  PII_MATRIX_CHECK_EQUAL_SIZE(other, *this);
  Pii::map(self()->begin(), self()->end(), other.self()->begin(), op);
  return selfRef();
}

template <class Derived> template <class BinaryFunc, class Matrix>
PiiBinaryMatrixTransform<Derived, Matrix, BinaryFunc>
PiiConceptualMatrix<Derived>::mapped(BinaryFunc func, const PiiConceptualMatrix<Matrix>& matrix) const
{
  PII_MATRIX_CHECK_EQUAL_SIZE(selfRef(), matrix);
  return Pii::binaryMatrixTransform(selfRef(), matrix.selfRef(), func);
}

#define PII_COMBINE_TYPES(T,U) typename Pii::Combine<T,U>::Type

// For operators like operator- () and operator! ()
#define PII_UNARY_MATRIX_OPERATOR(OPERATOR, FUNCTION) \
template <class Matrix> \
PiiUnaryMatrixTransform<Matrix, FUNCTION<typename Matrix::value_type> > \
operator OPERATOR (const PiiConceptualMatrix<Matrix>& matrix) \
{ \
  return Pii::unaryMatrixTransform(matrix.selfRef(), FUNCTION<typename Matrix::value_type>()); \
} namespace PiiDummy {}

// For operators like operator+ (matrix, scalar)
#define PII_MATRIX_SCALAR_OPERATOR(OPERATOR, FUNCTION) \
template <class Matrix> \
PiiUnaryMatrixTransform<Matrix, std::binder2nd<FUNCTION<typename Matrix::value_type> > > \
operator OPERATOR (const PiiConceptualMatrix<Matrix>& matrix, typename Matrix::value_type value) \
{ \
  return Pii::unaryMatrixTransform(matrix.selfRef(), std::bind2nd(FUNCTION<typename Matrix::value_type>(), value)); \
} namespace PiiDummy {}

// For operators like operator+ (matrix, matrix)
#define PII_MATRIX_MATRIX_OPERATOR(OPERATOR, FUNCTION) \
template <class Matrix1, class Matrix2> \
PiiBinaryMatrixTransform<Matrix1, Matrix2, FUNCTION<PII_COMBINE_TYPES(typename Matrix1::value_type, \
                                                                      typename Matrix2::value_type)> > \
operator OPERATOR (const PiiConceptualMatrix<Matrix1>& matrix1, const PiiConceptualMatrix<Matrix2>& matrix2) \
{ \
  PII_MATRIX_CHECK_EQUAL_SIZE(matrix1, matrix2); \
  return Pii::binaryMatrixTransform(matrix1.selfRef(), matrix2.selfRef(), \
                                    FUNCTION<PII_COMBINE_TYPES(typename Matrix1::value_type, \
                                                               typename Matrix2::value_type)>()); \
}

#define PII_BOTH_MATRIX_OPERATORS(OPERATOR, FUNCTION) \
  PII_MATRIX_MATRIX_OPERATOR(OPERATOR, FUNCTION); \
  PII_MATRIX_SCALAR_OPERATOR(OPERATOR, FUNCTION)

PII_BOTH_MATRIX_OPERATORS(+, std::plus);
PII_BOTH_MATRIX_OPERATORS(-, std::minus);

PII_MATRIX_SCALAR_OPERATOR(*, std::multiplies);
PII_MATRIX_SCALAR_OPERATOR(/, std::divides);

PII_BOTH_MATRIX_OPERATORS(<, std::less);
PII_BOTH_MATRIX_OPERATORS(<=, std::less_equal);
PII_BOTH_MATRIX_OPERATORS(>, std::greater);
PII_BOTH_MATRIX_OPERATORS(>=, std::greater_equal);
PII_BOTH_MATRIX_OPERATORS(==, std::equal_to);
PII_BOTH_MATRIX_OPERATORS(!=, std::not_equal_to);

PII_BOTH_MATRIX_OPERATORS(&&, std::logical_and);
PII_BOTH_MATRIX_OPERATORS(||, std::logical_or);

PII_BOTH_MATRIX_OPERATORS(&, Pii::BinaryAnd);
PII_BOTH_MATRIX_OPERATORS(|, Pii::BinaryOr);
PII_BOTH_MATRIX_OPERATORS(^, Pii::BinaryXor);

PII_UNARY_MATRIX_OPERATOR(-, std::negate);
PII_UNARY_MATRIX_OPERATOR(!, std::logical_not);
PII_UNARY_MATRIX_OPERATOR(~, Pii::BinaryNot);

template <class Matrix>
PiiUnaryMatrixTransform<Matrix, std::binder2nd<std::plus<typename Matrix::value_type> > >
operator+ (typename Matrix::value_type value, const PiiConceptualMatrix<Matrix>& matrix)
{
  return Pii::unaryMatrixTransform(matrix.selfRef(), std::bind2nd(std::plus<typename Matrix::value_type>(), value));
}

template <class Matrix>
PiiUnaryMatrixTransform<Matrix,
                        Pii::UnaryCompose<std::binder2nd<std::plus<typename Matrix::value_type> >,
                                          std::negate<typename Matrix::value_type> > >
operator- (typename Matrix::value_type value, const PiiConceptualMatrix<Matrix>& matrix)
{
  typedef typename Matrix::value_type T;
  return Pii::unaryMatrixTransform(matrix.selfRef(),
                                   Pii::unaryCompose(std::bind2nd(std::plus<T>(), value),
                                                     std::negate<T>()));
}

template <class Matrix>
PiiUnaryMatrixTransform<Matrix, std::binder2nd<std::multiplies<typename Matrix::value_type> > >
operator* (typename Matrix::value_type value, const PiiConceptualMatrix<Matrix>& matrix)
{
  return Pii::unaryMatrixTransform(matrix.selfRef(), std::bind2nd(std::multiplies<typename Matrix::value_type>(), value));
}

/// @endhide

#endif //_PIICONCEPTUALMATRIX_H
