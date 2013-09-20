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

#ifndef _PIIMATRIXUTIL_H
#define _PIIMATRIXUTIL_H

#include "PiiMatrix.h"
#include "PiiMath.h"
#include "Pii.h"
#include <functional>
#include <QtAlgorithms>

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

/**
 * Utility functions for dealing with matrices.
 *
 */
namespace Pii
{
  /**
   * Print matrix in a user-defined format.
   *
   * @param out an output stream to write the matrix to. Any class
   * implementing `operator`<<(Separator) and `operator`<<(T) can act as
   * the output stream. Examples of such classes are stl output
   * streams (std::cout and relatives) and QTextStream. Even a
   * QVariantList works in most cases.
   *
   * @param mat the matrix to be written
   *
   * @param columnSeparator separate values on a column with this
   *
   * @param rowSeparator separate each row with this
   *
   * ~~~(c++)
   *
   * PiiMatrix<double> mat(4,4,
   *                       1.0, 2.3, 5.4, 8.0,
   *                       2.3, 5.4, 8.0, 1.0,
   *                       5.4, 8.0, 1.0, 2.3,
   *                       8.0, 1.0, 2.3, 5.4);
   *
   * Pii::printMatrix(std::cout, mat, ", ", "\n");
   *  //output:
   *  1, 2.3, 5.4, 8
   *  2.3, 5.4, 8, 1
   *  5.4, 8, 1, 2.3
   *  8, 1, 2.3, 5.4
   * ~~~
   */
  template <class Stream, class Matrix, class ColSeparator, class RowSeparator>
  void printMatrix(Stream& out,
                   const PiiConceptualMatrix<Matrix>& mat,
                   const ColSeparator& columnSeparator,
                   const RowSeparator& rowSeparator);
  /**
   * Print out a matrix in a matlab-friendly format. The output can be
   * copied and pasted to create a matrix in Matlab.
   *
   * @param out an output stream to write the matrix to. Any class
   * implementing operator<<(const char*) and operator<<(T) can act as
   * the output stream. Examples of such classes are stl output
   * streams (std::cout and relatives) and QTextStream. Even a
   * QVariantList works in most cases.
   *
   * @param mat the matrix to be written
   */
  template <class Stream, class Matrix> void matlabPrint(Stream& out,
                                                         const PiiConceptualMatrix<Matrix>& mat);

  /**
   * Parse a string that describes a matrix in Matlab syntax. The
   * parser accepts non-complex numbers, and does not require newlines
   * to be prefixed with "...". Enclosing square brackets are
   * optional. Separate elements with commas or spaces and rows with
   * semicolons.
   *
   * ~~~(c++)
   * PiiMatrix<double> mat1 = matlabParse("[1 2 3; 4 5 6; 7 8 9]");
   * PiiMatrix<double> mat2 = matlabParse("1.0, 2.0, 3.5; ...\n 4 5 6.8");
   * ~~~
   */
  PII_CORE_EXPORT PiiMatrix<double> matlabParse(const QString& str);

  /**
   * Different ways of extending matrices. Matrices are often enlarged
   * before applying a filtering operation to them to retain the
   * original size.
   *
   * - `ExtendZeros` - pad with zeros
   * - `ExtendReplicate` - replicate the value on the border
   * - `ExtendSymmetric` - symmetrically mirror boundary values
   * - `ExtendPeriodic` - take values from the opposite border
   * - `ExtendNot` - do not extend
   */
  enum ExtendMode { ExtendZeros, ExtendReplicate, ExtendSymmetric, ExtendPeriodic, ExtendNot };

  /**
   * Constant values for matrix borders. A composition of these values
   * is used, for example, with the extend() function. For example, to
   * extend a matrix on right-hand side and on the bottom, use
   * MatrixBottom + MatrixRight.
   */
  enum MatrixBorder { MatrixTop = 1, MatrixBottom = 2, MatrixLeft = 4, MatrixRight = 8 };

  /**
   * Extend a matrix by adding new rows/columns to its sides. Calls
   * extend(PiiMatrix,int,int,int,int,ExtendMode).
   *
   * @param mat the matrix to be extended
   *
   * @param amount the number of rows/columns to add on each side
   *
   * @param mode the extension mode
   *
   * @param sides a bit mask that tells which sides of the matrix are
   * extended. Use a composition of MatrixBorder values, e.g. 
   * MatrixTop | MatrixBottom.
   *
   * @return the extended matrix
   */
  template <class Matrix> PiiMatrix<typename Matrix::value_type>
  extend(const Matrix& mat,
         int amount,
         ExtendMode mode = ExtendZeros,
         int sides = -1);
  
  /**
   * Extend a matrix by adding new rows/columns to its sides. This
   * version of the function adds a different number of rows/columns
   * to each side.
   *
   * @param mat the matrix to be extended
   *
   * @param topRows the number of rows to add on top of the matrix
   *
   * @param bottomRows the number of rows to add to the bottom of the
   * matrix
   *
   * @param leftCols the number of columns to add to the left of the
   * matrix
   *
   * @param rightCols the number of columns to add to the right of the
   * matrix
   *
   * @param mode the extension mode
   *
   * @return the extended matrix. If `mode` is `ExtendNot`, the
   * input matrix will be returned.
   */
  template <class Matrix>
  PiiMatrix<typename Matrix::value_type> extend(const Matrix& mat,
                                                int topRows, int bottomRows, int leftCols, int rightCols,
                                                ExtendMode mode);

  /**
   * Sample a matrix (typically an image) at continuous positions. 
   * This function uses bi-linear interpolation for values that don't
   * match pixels.
   *
   * @param img the input image
   *
   * @param dr double precision row coordinate
   *
   * @param dc double precision column coordinate
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3,
   *                    1,1,2,
   *                    2,1,3,
   *                    2,1,4);
   *
   *  float a = Pii::valueAt(mat,0.5,0.5);
   *  float b = Pii::valueAt(mat,1.0,0.5);
   *  float c = Pii::valueAt(mat,1.0,1.5);
   *  //a = 1.25 b = 1.5 c = 2.0
   * ~~~
   */
  template <class T>
  inline typename Pii::ToFloatingPoint<T>::Type valueAt(const PiiMatrix<T>& img, double dr, double dc)
  {
    typedef typename Pii::ToFloatingPoint<T>::Type Real;
    typedef typename Pii::ToFloatingPoint<T>::PrimitiveType RealScalar;
    int ir = (int)dr, ic = (int)dc;
    dr -= ir; dc -= ic;
    double d1dr = 1.0-dr, d1dc = 1.0-dc;
    const T* row1 = img.row(ir);
    Real result = Real(row1[ic])*RealScalar(d1dr*d1dc);
    if (dr > 0)
      {
        const T* row2 = img.row(ir+1);
        result += Real(row2[ic])*RealScalar(dr*d1dc);
        if (dc > 0)
          result += Real(row2[ic+1])*RealScalar(dc*dr);
      }
    if (dc > 0)
      result += Real(row1[ic+1])*RealScalar(d1dr*dc);
    return result;
  }

  /**
   *
   * Returns *img*(r,c). This function is provided for convenience
   * to allow %valueAt() to be easily used in template code.
   */
  template <class T>
  inline T valueAt(const PiiMatrix<T>& img, int r, int c)
  {
    return img(r,c);
  }

  template <class T>
  inline void setValueAt(T value, PiiMatrix<T>& img, double dr, double dc)
  {
    int ir = (int)dr, ic = (int)dc;
    dr -= ir; dc -= ic;
    double d1dr = 1.0-dr, d1dc = 1.0-dc;
    T* row1 = img.row(ir);
    row1[ic] += Pii::round<T>(value * d1dr * d1dc);
    if (dr > 0)
      {
        T* row2 = img.row(ir+1);
        row2[ic] += Pii::round<T>(value * dr*d1dc);
        if (dc > 0)
          row2[ic+1] += Pii::round<T>(value * dc*dr);
      }
    if (dc > 0)
      row1[ic+1] += Pii::round<T>(value * d1dr*dc);
  }

  /**
   * Converts a collection to a row matrix.
   *
   * @param lst a list of values (QList, QVector, ...)
   *
   * @param direction store values int this direction. By default,
   * values are stored horizontally, single-row matrix.
   *
   * @return a row matrix with lst.size() columns.
   *
   * ~~~(c++)
   * QList<double> lst = QList<double>() << 1.0 << 2.0;
   * PiiMatrix<double> mat = Pii::listToMatrix<double>(lst);
   * // mat = [ 1.0 2.0 ]
   * ~~~
   */
  template <class T, class Collection> PiiMatrix<T> listToMatrix(const Collection& lst,
                                                                 Pii::MatrixDirection direction = Pii::Horizontally);

  /**
   * Get a row from a matrix as a collection (QList, QVector)
   *
   * @param matrix input matrix
   *
   * @param row row index (positive value)
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(2,2, 1,2, 3,4);
   * QList<int> lst = Pii::rowToList<QList<int> >(mat, 1);
   * // lst = { 3, 4 }
   * ~~~
   */
  template <class Collection, class T> Collection rowToList(const PiiMatrix<T>& matrix, int row);

  /**
   * Get a column from a matrix as a collection (QList, QVector)
   *
   * @param matrix input matrix
   *
   * @param column column index (positive value)
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(2,2, 1,2, 3,4);
   * QList<int> lst = Pii::columnToList<QList<int> >(mat, 0);
   * // lst = { 1, 3 }
   * ~~~
   */
  template <class Collection, class T> Collection columnToList(const PiiMatrix<T>& matrix, int column);

  /**
   * Find the first occurrence of an element that matches `Rule`. 
   * Stores the row and column coordinates to `r` and `c`. If no
   * such value is not found, both coordinates will be set to -1.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(2,2,
   *                    1,2,
   *                    3,4);
   * int r,c;
   * Pii::findFirst<std::equal_to<int> >(mat,5, r, c);
   * // r = -1, c = -1
   * ~~~
   */
  template <class Rule, class T> void findFirst(const PiiMatrix<T>& matrix, T value, int &r, int& c);

  /**
   * Find the last occurrence of an element that matches `Rule`. 
   * Stores the row and column coordinates to `r` and `c`. If no
   * such value is not found, both coordinates will be set to -1.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(2,2,
   *                    1,2,
   *                    3,4);
   * int r,c;
   * Pii::findLast<std::greater<int> >(mat, 2, r, c);
   * // r = 1, c = 0
   * ~~~
   */
  template <class Rule, class T> void findLast(const PiiMatrix<T>& matrix, T value, int &r, int& c);

  /**
   * Find the first occurrence (top to bottom, left to right) of 
   * `value` in `matrix`. Stores the row and column coordinates to `r`
   * and `c`. If the value is not found, both coordinates will be set
   * to -1.
   */
  template <class T> inline void findFirst(const PiiMatrix<T>& matrix, T value, int &r, int& c)
  {
    findFirst<std::equal_to<T> >(matrix, value, r, c);
  }

  /**
   * Find the last occurrence (bottom to top, right to left) of 
   * `value` in `matrix`. Stores the row and column coordinates to `r`
   * and `c`. If the value is not found, both coordinates will be set
   * to -1.
   */
  template <class T> void findLast(const PiiMatrix<T>& matrix, T value, int &r, int& c)
  {
    findLast<std::equal_to<T> >(matrix, value, r, c);
  }

  /**
   * Concatenate two matrices. The rows/columns of `mat2` are
   * appended to those of `mat1`. If either matrix is empty, the
   * other one is returned.
   *
   * @param direction `Vertically` means that `mat2` will be added
   * to the bottom of `mat1`. In this case the number of columns in
   * the matrices must be the same. `Horizontally` means `mat2` will
   * be added to the right side of `mat1`. In this case the number of
   * rows must be equal in both matrices.
   *
   * @exception PiiMathException& if matrix sizes don't match
   */
  template <class Matrix1, class Matrix2>
  PiiMatrix<typename Matrix1::value_type> concatenate(const PiiConceptualMatrix<Matrix1>& mat1,
                                                      const PiiConceptualMatrix<Matrix2>& mat2,
                                                      Pii::MatrixDirection direction = Pii::Horizontally);

  /**
   * Replicate a matrix `vertical` times vertically and `horizontal`
   * times horizontally.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(2,2,
   *                    1,0,
   *                    0,1);
   * PiiMatrix<int> mat2(Pii::replicate(mat,2,3));
   * // mat2 = 101010
   * //        010101
   * //        101010
   * //        010101
   * ~~~
   */
  template <class T> PiiMatrix<T> replicate(const PiiMatrix<T>& matrix, int vertical, int horizontal);

  /**
   * A unary function that adds rows to a matrix. See
   * Pii::combinations() for a usage example.
   */
  template <class T> struct MatrixRowAdder
  {
    /**
     * Constructs a function object that inserts new rows to `mat`.
     */
    MatrixRowAdder(PiiMatrix<T>& mat) : matrix(mat) {}
    
    /**
     * Add `row` to `matrix`.
     */
    void operator() (const T* row) const { matrix.appendRow(row); }

    PiiMatrix<T>& matrix;
  };


  /**
   * Sort matrix rows into ascending order based on the value on the
   * specified column. Use `predicate` as the comparison function.
   * 
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3,
   *                    -1,2,3,
   *                    -2,1,3,
   *                    3,0,3);
   * Pii::sortRows(mat, std::greater<int>());
   * //  3 0 3
   * // -1 2 3
   * // -2 1 3
   * ~~~
   *
   * @param matrix the input matrix
   *
   * @param lessThan a binary function used for determining the order
   *
   * @param column the matrix column based on which the sort will be
   * done
   */
  template <class T, class LessThan> void sortRows(PiiMatrix<T>& matrix, LessThan lessThan, int column = 0);

  /**
   * Sort matrix rows into ascending order based on the value on the
   * specified column.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3,
   *                    -1,2,3,
   *                    -2,1,3,
   *                    3,0,3);
   * Pii::sortRows(mat);
   * // -2 1 3
   * // -1 2 3
   * //  3 0 3
   * Pii::sortRows(mat,1);
   * //  3 0 3
   * // -2 1 3
   * // -1 2 3
   * Pii::sortRows(mat,2);
   * //  3 0 3
   * // -2 1 3
   * // -1 2 3
   * ~~~
   *
   * @param matrix the input matrix
   *
   * @param predicate a binary function used for determining the order
   *
   * @param column the matrix column based on which the sort will be
   * done
   *
   * @relates RowComparator
   */
  template <class T> inline void sortRows(PiiMatrix<T>& matrix, int column = 0)
  {
    sortRows(matrix, std::less<T>(), column);
  }

  /**
   * Sort matrix rows. Same as above but for `const` input.
   */
  template <class T> inline PiiMatrix<T> sortedRows(const PiiMatrix<T>& matrix, int column = 0)
  {
    PiiMatrix<T> result(matrix);
    sortRows(result, std::less<T>(), column);
    return result;
  }

  /**
   * Sort matrix rows. Same as above but for `const` input.
   */
  template <class T, class LessThan>
  inline PiiMatrix<T> sortedRows(const PiiMatrix<T>& matrix,
                                 LessThan lessThan,
                                 int column = 0)
  {
    PiiMatrix<T> result(matrix);
    sortRows(result, lessThan, column);
    return result;
  }

  /**
   * Set random elements from range specified by parameters `lowerBound` and
   * `upperBound` to matrix.
   * ! Might not handle limits properly for integer types.
   */
  template <class T> void randomizeMatrix(PiiMatrix<T>& mat, const T& lowerBound, const T& upperBound);

  /**
   * Flip *matrix* in place either horizontally, vertically or both.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3, 3,
   *                    1, 2, 3,
   *                    4, 5, 6,
   *                    7, 8, 9);
   * Pii::flip(mat, Pii::Vertically);
   * // mat = [ 7 8 9; 4 5 6; 1 2 3 ]
   * Pii::flip(mat, Pii::Horizontally);
   * // mat = [ 9 8 7; 6 5 4; 3 2 1 ]
   * Pii::flip(mat, Pii::Horizontally | Pii::Vertically);
   * // mat = [ 1 2 3; 4 5 6; 7 8 9 ]
   * ~~~
   */
  template <class T> void flip(PiiMatrix<T>& matrix, Pii::MatrixDirections directions);

  /**
   *
   * This function doesn't modify the source matrix, but returns a
   * copy.
   */
  template <class T> PiiMatrix<T> flipped(const PiiMatrix<T>& mat, Pii::MatrixDirections directions);
};

#include "PiiMatrixUtil-templates.h"

#endif //_PIIMATRIXUTIL_H
