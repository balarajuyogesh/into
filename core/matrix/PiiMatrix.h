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
#define _PIIMATRIX_H

#include "PiiMatrixData.h"
#include "PiiFunctional.h"
#include "Pii.h"
#include "PiiConceptualMatrix.h"
#include "PiiFilteredMatrix.h"
#include <functional>
#include <algorithm>

/// @hide
template <class T> class PiiMatrixIterator;
template <class T> class PiiMatrixColumnIterator;

template <class T, int rowCount, int columnCount> class PiiMatrix;
template <class T, int rowCount, int columnCount>
struct PiiMatrixTraits<PiiMatrix<T, rowCount, columnCount> >
{
  enum { staticRows = rowCount, staticColumns = columnCount };

  typedef T value_type;
  typedef T& reference;
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef PiiMatrixColumnIterator<T> column_iterator;
  typedef PiiMatrixColumnIterator<const T> const_column_iterator;
  typedef T* row_iterator;
  typedef const T* const_row_iterator;
};
/// @endhide

template <class T, int rowCount = -1, int columnCount = -1> class PiiMatrix :
  public PiiConceptualMatrix<PiiMatrix<T, rowCount, columnCount> >
{
public:
  typedef PiiMatrixTraits<PiiMatrix<T, rowCount, columnCount> > Traits;
  typedef typename Pii::VaArg<T>::Type VaArgType;

  PiiMatrix()
  {
    std::memset(_data, 0, byteCount());
  }

  PiiMatrix(const PiiMatrix& other)
  {
    std::memcpy(_data, other._data, byteCount());
  }

  template <class Matrix> PiiMatrix(const Matrix& other,
                                    typename Pii::OnlyIf<rowCount == Matrix::staticRows &&
                                    columnCount == Matrix::staticColumns>::Type = 0)
  {
    Pii::copy(other.begin(), other.end(), begin());
  }

  template <class Matrix> PiiMatrix(const Matrix& other,
                                    typename Pii::OnlyIf<rowCount != Matrix::staticRows ||
                                    columnCount != Matrix::staticColumns>::Type = 0)
  {
    PII_MATRIX_CHECK_EQUAL_SIZE(*this, other);
    Pii::copy(other.begin(), other.end(), begin());
  }

  PiiMatrix(VaArgType firstElement, ...)
  {
    va_list argp;
    va_start(argp, firstElement);
    Pii::copyVaList(begin(), end(), firstElement, argp);
    va_end(argp);
  }

  PiiMatrix& operator= (const PiiMatrix& other)
  {
    std::memcpy(_data, other._data, byteCount());
    return *this;
  }

  template <class Matrix> PiiMatrix& operator= (const PiiConceptualMatrix<
                                                typename Pii::OnlyIf<rowCount == Matrix::staticRows &&
                                                columnCount == Matrix::staticColumns, Matrix>::Type >& other)
  {
    Pii::copy(other.self()->begin(), other.self()->end(), begin());
    return *this;
  }

  template <class Matrix> PiiMatrix& operator= (const PiiConceptualMatrix<
                                                typename Pii::OnlyIf<rowCount != Matrix::staticRows ||
                                                columnCount != Matrix::staticColumns, Matrix>::Type >& other)
  {
    PII_MATRIX_CHECK_EQUAL_SIZE(*this, other);
    Pii::copy(other.self()->begin(), other.self()->end(), begin());
    return *this;
  }

  PiiMatrix& operator= (T value) { return PiiConceptualMatrix<PiiMatrix<T,rowCount,columnCount> >::operator= (value); }

  int rows() const { return rowCount; }
  int columns() const { return columnCount; }

  T* begin() { return _data; }
  const T* begin() const { return _data; }
  T* end() { return row(rowCount); }
  const T* end() const { return row(rowCount); }

  T* rowBegin(int r) { return row(r); }
  const T* rowBegin(int r) const { return row(r); }
  T* rowEnd(int r) { return row(r+1); }
  const T* rowEnd(int r) const { return row(r+1); }

  typename Traits::column_iterator columnBegin(int colIndex)
  {
    return typename Traits::column_iterator(_data + colIndex, stride());
  }
  typename Traits::const_column_iterator columnBegin(int colIndex) const
  {
    return typename Traits::const_column_iterator(_data + colIndex, stride());
  }
  typename Traits::column_iterator columnEnd(int colIndex)
  {
    return typename Traits::column_iterator(row(rowCount) + colIndex, stride());
  }
  typename Traits::const_column_iterator columnEnd(int colIndex) const
  {
    return typename Traits::const_column_iterator(row(rowCount) + colIndex, stride());
  }

  T* row(int r) { return _data + r*columnCount; }
  const T* row(int r) const { return _data + r*columnCount; }

  static std::size_t stride() { return columnCount * sizeof(T); }

private:
  static inline std::size_t byteCount() { return rowCount * columnCount * sizeof(T); }

  T _data[rowCount * columnCount];
};

/**
 * A two-dimensional dynamic array of non-typed data. This class
 * provides basic functionality for accessing the data of a matrix in
 * a type-agnostic manner. PiiMatrix provides type-specific functions
 * and usual matrix manipulation operations.
 *
 * PiiTypelessMatrix holds a pointer to a data structure that may be
 * shared among many matrices. The class implements the
 * *copy*-on-write paradigm; all copies are shallow until a modification
 * is going to happen. The data will be [detached](PiiMatrix::detach())
 * in the beginning of all non-const functions.
 *
 */
class PII_CORE_EXPORT PiiTypelessMatrix
{
public:
  /// Releases the internal data pointer.
  ~PiiTypelessMatrix() { d->release(); }

  /**
   * Returns the number of rows in the matrix.
   */
  int rows() const { return d->iRows; }
  /**
   * Returns the number of columns in the matrix.
   */
  int columns() const { return d->iColumns; }

  /**
   * Returns the number of bytes between the beginnings of successive
   * rows. The stride may be different from sizeof(datatype) *
   * columns() for two reasons:
   *
   * - Matrix rows are aligned to four-byte boundaries. For example,
   * if the data type is `char`, and the matrix has three columns
   * (three bytes per row), *stride* will be four.
   *
   * - The matrix references external data. In this case the stride
   * may be anything, but always larger than or equal to the number of
   * columns.
   */
  std::size_t stride() const { return d->iStride; }

  /**
   * Returns the maximum number or rows that can be stored in the
   * matrix without reallocation. If the matrix references external
   * data, the capacity is zero.
   */
  int capacity() const { return d->iCapacity; }

  /**
   * Releases all memory allocated by the matrix and resizes the
   * matrix to 0-by-0.
   */
  void clear();

protected:
  /// @hide
  PiiTypelessMatrix() : d(PiiMatrixData::sharedNull()) { d->reserve(); }

  PiiTypelessMatrix(const PiiTypelessMatrix& other) : d(other.d) { d->reserve(); }

  PiiTypelessMatrix(PiiMatrixData* data) : d(data) {}

  PiiMatrixData* createReference(int rows, int columns, void* buffer) const;
  void cloneAndReplaceData(int capacity, std::size_t bytesPerRow);

  void* appendRow(std::size_t bytesPerRow);
  void* insertRow(int index, std::size_t bytesPerRow);
  void removeRow(int index, std::size_t bytesPerRow);
  void removeRows(int index, int cnt, std::size_t bytesPerRow);
  void reserve(int rows, std::size_t bytesPerRow);
  void* appendColumn(std::size_t bytesPerItem);
  void* insertColumn(int index, std::size_t bytesPerItem);
  void removeColumn(int index, std::size_t bytesPerItem);
  void removeColumns(int index, int cnt, std::size_t bytesPerItem);
  void resize(int rows, int columns, std::size_t bytesPerItem);

  PiiMatrixData* d;
  /// @endhide
};

/// @hide
template <class T> struct PiiMatrixTraits<PiiMatrix<T,-1,-1> >
{
  enum { staticRows = -1, staticColumns = -1 };

  typedef T value_type;
  typedef T& reference;
  typedef PiiMatrixIterator<T*> iterator;
  typedef PiiMatrixIterator<const T*> const_iterator;
  typedef PiiMatrixColumnIterator<T> column_iterator;
  typedef PiiMatrixColumnIterator<const T> const_column_iterator;
  typedef T* row_iterator;
  typedef const T* const_row_iterator;
};

/// @endhide


/**
 * A dynamic two-dimensional array that models the *matrix* concept.
 *
 * PiiMatrix only supports POD (plain old data) types as the content
 * type. It will call neither constructors nor destructors. The data of
 * a matrix is cleared by simply setting all bytes to zero, and
 * assignments may be performed with std::memcpy(). If matrix atrithmetic
 * is to be performed, the corresponding operators of the content type
 * must be defined. Furthermore, for some operations, there must be a
 * constructor for a single numeric argument. An example of a class
 * that can be used as the content type:
 *
 * ~~~(c++)
 * struct MyClass
 * {
 *   MyClass(int i=0, double d=0.0);
 *
 *   void operator+= (const MyClass&);
 *   void operator-= (const MyClass&);
 *   void operator*= (const MyClass&);
 *   void operator/= (const MyClass&);
 *   MyClass operator+ (const MyClass&) const;
 *   MyClass operator- (const MyClass&) const;
 *   MyClass operator* (const MyClass&) const;
 *   MyClass operator/ (const MyClass&) const;
 *
 *   int iValue;
 *   double dValue;
 * };
 *
 *
 * PiiMatrix<MyClass> mat(1,1); //resets both ivalue and dvalue to zero
 * MyClass mc(1, 2.0);
 * mat(0,0) = mc;
 * mat *= mc;
 * ~~~
 *
 * Row and column indices in PiiMatrix are always zero-based. The
 * convention in matrix math is to index rows first, and that is what
 * PiiMatrix also does. For performance reasons there is no bound
 * checking. Thus, make sure you don't reference memory outside of the
 * matrix.
 *
 * Most routines that perform matrix math may throw a
 * PiiMathException. Such an exception may be thrown if the sizes of
 * two matrices do not match for calculation.
 *
 * The data within a matrix is organized so that the items in a row
 * (scan line) always occupy adjacent memory locations. The pointer to
 * the beginning to each row is returned by [row(int)]. Each row is
 * aligned at a four-byte boundary (unless initialized with an
 * external non-aligned buffer). Therefore, if the data type is less
 * than four bytes wide, it may happen that rows are not stored
 * sequentially.
 *
 * Usually, matrices use an internally allocated buffer to store the
 * elements. It is however possible to create matrices that refer to
 * other matrices or externally allocated buffers. A modifiable
 * reference to another matrix is created by the
 * operator(int,int,int,int). Changing the contents of such a matrix
 * will change the contents of the original matrix. Reference can be
 * chained; a reference to a reference modifies the original.
 *
 * ~~~(c++)
 * PiiMatrix<char> mat(8,8);
 * mat = '+';
 * mat(2,1,3,3) = 'a';
 * mat(1,5,6,3) = 'b';
 * mat(2,6,4,1) = 'c';
 * mat(6,0,2,1) = 'x';
 *
 * //mat looks like this now:
 * + + + + + + + +
 * + + + + + b b b
 * + a a a + b c b
 * + a a a + b c b
 * + a a a + b c b
 * + + + + + b c b
 * x + + + + b b b
 * x + + + + + + +
 * ~~~
 *
 */
template <class T> class PiiMatrix<T,-1,-1> :
  public PiiTypelessMatrix,
  public PiiConceptualMatrix<PiiMatrix<T,-1,-1> >
{
public:
  typedef PiiMatrixTraits<PiiMatrix<T,-1,-1> > Traits;

  /**
   * Type for arguments passed through va_args.
   */
  typedef typename Pii::VaArg<T>::Type VaArgType;

  /**
   * Constructs an empty matrix.
   */
  PiiMatrix() {}

  /**
   * Constucts a shallow copy of *other*. This constructor only
   * increases the reference count of the internal data structure.
   *
   * ~~~(c++)
   * PiiMatrix<int> a(5,5);
   * PiiMatrix<int> b(a(1,1,2,2)); // 2-by-2 reference to a
   * PiiMatrix<int> c(a);
   * PiiMatrix<int> c(b);          // deep copy of b
   * ~~~
   */
  PiiMatrix(const PiiMatrix& other) : PiiTypelessMatrix(other) {}

  /**
   * Constructs a deep copy of *other* by copying and typecasting
   * each individual element.
   */
  template <class Matrix> explicit PiiMatrix(const PiiConceptualMatrix<Matrix>& other) :
    PiiTypelessMatrix(PiiMatrixData::createUninitializedData(other.self()->rows(),
                                                             other.self()->columns(),
                                                             other.self()->columns() * sizeof(T)))
  {
    Pii::transform(other.self()->begin(), other.self()->end(), begin(),
                   Pii::Cast<typename Matrix::value_type,T>());
  }

  /**
   * Constucts a *rows*-by-*columns* matrix with all entries
   * initialized to zero.
   */
  PiiMatrix(int rows, int columns) :
    PiiTypelessMatrix(PiiMatrixData::createInitializedData(rows, columns, columns * sizeof(T)))
  {}

  /**
   * Constructs a *rows*-by-*columns* matrix whose initial contents
   * are taken from the array pointed to by *data*. The array must
   * hold at least *rows* * *columns* entries. If *stride* is set
   * to a value larger than `sizeof`(T) * *columns*, row padding is
   * assumed. The *data* pointer must remain valid throughout the
   * lifetime of the matrix and any shallow copies of it.
   *
   * ~~~(c++)
   * const double data[] = { 1, 2, 3, 4 };
   * PiiMatrix<double> mat(2, 2, data);
   * QCOMPARE(mat(1,1), 4);
   *
   * const char* padded = "ABCDEFGH";
   * PiiMatrix<char> mat(3, 3, padded, 4);
   * QCOMPARE(mat(1,0), 'E');
   * ~~~
   *
   * Note that *stride* is always in bytes.
   *
   * @see PiiTypelessMatrix::stride()
   */
  PiiMatrix(int rows, int columns, const T* data, std::size_t stride = 0) :
    PiiTypelessMatrix(PiiMatrixData::createReferenceData(rows, columns,
                                                         qMax(stride, sizeof(T)*columns),
                                                         const_cast<T*>(data))->makeImmutable())
  {}

  /**
   * Constructs a *rows*-by-*columns* matrix that uses *data* as
   * its data buffer. This is an overloaded constructor that behaves
   * essentially the same way as the one above. The difference is that
   * accesses to this matrix will modify *data*. Furthermore, if you
   * set the *ownership* flag to `Pii::ReleaseOwnership`, the data
   * pointer will be deallocated with `free`() when the matrix is
   * destroyed.
   *
   * ~~~(c++)
   * double data[] = { 1, 2, 3, 4 };
   * PiiMatrix<double> mat(2, 2, data, Pii::RetainOwnership);
   * mat(1,1) = 3;
   * QCOMPARE(data[3], 3);
   *
   * void* bfr = malloc(sizeof(int) * 16);
   * // bfr will be deallocated with free() when the matrix is destroyed.
   * PiiMatrix<int> mat(2, 7, bfr, Pii::ReleaseOwnership, 8 * sizeof(int));
   * mat = 0; // sets all elements in bfr to zeros
   * ~~~
   */
  PiiMatrix(int rows, int columns, void* data, Pii::PtrOwnership ownership, std::size_t stride = 0) :
    PiiTypelessMatrix(PiiMatrixData::createReferenceData(rows, columns,
                                                         qMax(stride, sizeof(T)*columns),
                                                         data))
  {
    if (ownership == Pii::ReleaseOwnership)
      d->bufferType = PiiMatrixData::ExternalOwnBuffer;
  }

  /**
   * Constructs a matrix with the given number of *rows* and
   * *columns*. Matrix contents are given as a variable-length parameter
   * list in a horizontal raster-scan order. It is handy if you know
   * what you do, but proper care must be taken to ensure correct
   * functioning.
   *
   * Only elementary types can be used with this constructor. Complex
   * types cause a compile-time error.
   *
   * ! Take extreme care to ensure that the elements you give in the
   * parameter list are of correct type. Examples:
   *
   * ~~~(c++)
   * PiiMatrix<char> a(3, 1, 'a', 'b', 'c');     //correct, chars are passed as ints
   * PiiMatrix<char> b(3, 1, 1, 2, 3);           //correct
   * PiiMatrix<float> c(3, 1, 1, 2, 3);          //WRONG! values are passed as ints
   * PiiMatrix<float> d(3, 1, 1.0, 2.0, 3.0);    //correct
   * PiiMatrix<float> e(3, 1, 1.0f, 2.0f, 3.0f); //WRONG! float constants cannot be used as va_args
   * PiiMatrix<double> f(3, 1, 1.0, 2.0, 3.0);   //correct
   * PiiMatrix<int> g(3, 1, 1, 2, 3);            //correct
   * PiiMatrix<int> g(3, 1, 1, 2);               //WRONG! too few parameters
   * ~~~
   *
   * @param rows the number of rows
   *
   * @param columns the number of columns
   *
   * @param firstElement the first element
   *
   * @param ... rest of the matrix data
   */
  PiiMatrix(int rows, int columns, VaArgType firstElement, ...);

  /// @hide
#ifdef PII_CXX11
  PiiMatrix(PiiSubmatrix<PiiMatrix>&& other) :
    PiiTypelessMatrix(other.source().createReference(other.rows(),
                                                     other.columns(),
                                                     other.rowBegin(0))
                      ->makeImmutable())
  {}

  PiiMatrix& operator= (PiiSubmatrix<PiiMatrix>&& other)
  {
    auto pNewD = other.source().createReference(other.rows(),
                                                other.columns(),
                                                other.rowBegin(0))
      ->makeImmutable();
    d->release();
    d = pNewD;
    return *this;
  }
#else
  PiiMatrix(const PiiSubmatrix<PiiMatrix>& other) :
    PiiTypelessMatrix(other.source().createReference(other.rows(),
                                                     other.columns(),
                                                     const_cast<T*>(other.rowBegin(0)))
                      ->makeImmutable())
  {}

  PiiMatrix& operator= (const PiiSubmatrix<PiiMatrix>& other)
  {
    PiiMatrixData* pNewD = other.source().createReference(other.rows(),
                                                          other.columns(),
                                                          const_cast<T*>(other.rowBegin(0)))
      ->makeImmutable();
    d->release();
    d = pNewD;
    return *this;
  }
#endif
  /// @endhide

  /// Destroys the matrix.
  ~PiiMatrix() {}

  /**
   * Assigns *other* to `this` and returns a reference to `this`.
   */
  PiiMatrix& operator= (const PiiMatrix& other)
  {
    // Reserve/release is always safe, and costs a bit extra only in
    // the rare case of self-assignment.
    other.d->reserve();
    d->release();
    d = other.d;
    return *this;
  }

  /**
   * Creates a deep copy of *other* and returns a reference to
   * `this`.
   */
  template <class Matrix> PiiMatrix& operator= (const PiiConceptualMatrix<Matrix>& other);

  PiiMatrix& operator= (T value) { return PiiConceptualMatrix<PiiMatrix<T,-1,-1> >::operator= (value); }

  using PiiTypelessMatrix::rows;
  using PiiTypelessMatrix::columns;

  /**
   * Detaches the matrix from shared data. This function makes sure
   * that there are no other references to the shared data, and
   * creates a clone of the data if necessary.
   */
  void detach()
  {
    if (d->iRefCount != 1)
      cloneAndReplaceData(capacity(), columns() * sizeof(T));
  }

  /**
   * Allocates memory for at least *rows* matrix rows. If you know in
   * advance how large the matrix will be, you can avoid unnecessary
   * reallocations while adding new rows. Trying to set *rows* to a
   * value less than the current capacity has no effect.
   *
   * @see capacity()
   */
  void reserve(int rows) { PiiTypelessMatrix::reserve(rows, columns() * sizeof(T)); }

  /**
   * Returns a random-access iterator to the start of the matrix data.
   */
  typename Traits::iterator begin() { return typename Traits::iterator(row(0), columns(), stride()); }
  typename Traits::const_iterator begin() const { return typename Traits::const_iterator(row(0), columns(), stride()); }
  /**
   * Returns a random-access iterator to the end of the matrix data.
   */
  typename Traits::iterator end() { return typename Traits::iterator(row(0), row(rows()), columns(), stride()); }
  typename Traits::const_iterator end() const { return typename Traits::const_iterator(row(0), row(rows()), columns(), stride()); }

  /**
   * Returns a random-access iterator to the start of the row at
   * *rowIndex*.
   */
  typename Traits::row_iterator rowBegin(int rowIndex) { return row(rowIndex); }
  typename Traits::const_row_iterator rowBegin(int rowIndex) const { return row(rowIndex); }
  /**
   * Returns a random-access iterator to the end of the row at
   * *rowIndex*.
   */
  typename Traits::row_iterator rowEnd(int rowIndex) { return row(rowIndex) + columns(); }
  typename Traits::const_row_iterator rowEnd(int rowIndex) const { return row(rowIndex) + columns(); }

  /**
   * Returns a random-access iterator to the start of the column at
   * *colIndex*.
   */
  typename Traits::column_iterator columnBegin(int colIndex) { return typename Traits::column_iterator(*this, colIndex); }
  typename Traits::const_column_iterator columnBegin(int colIndex) const { return typename Traits::const_column_iterator(*this, colIndex); }
  /**
   * Returns a random-access iterator to the end of the column at
   * *colIndex*.
   */
  typename Traits::column_iterator columnEnd(int colIndex) { return typename Traits::column_iterator(*this, rows(), colIndex); }
  typename Traits::const_column_iterator columnEnd(int colIndex) const { return typename Traits::const_column_iterator(*this, rows(), colIndex); }

  /**
   * Returns a pointer to the beginning of row at *index*.
   */
  const T* row(int index) const { return static_cast<const T*>(d->row(index)); }
  T* row(int index) { detach(); return static_cast<T*>(d->row(index)); }

  /**
   * A utility function that returns a reference to the memory
   * location at the beginning of the given row as the specified type.
   * Use this function only if you are absolutely sure that the memory
   * arrangement of the type `T` matches that of a matrix row.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(1,3, 1,2,3);
   * PiiVector<int,3>& vec = mat.rowAs<PiiVector<int,3> >(0);
   * vec[0] = 2;
   * QCOMPARE(mat(0,0), 2);
   * ~~~
   */
  template <class U> U& rowAs(int index) { detach(); return *static_cast<U*>(d->row(index)); }
  template <class U> const U& rowAs(int index) const { return *static_cast<const U*>(d->row(index)); }

  /**
   * Returns a reference to an item in the matrix. Equal to row(
   * `row`)[`column`]. No bound checking will be done for performance
   * reasons.
   *
   * @param r the row index
   *
   * @param c the column index
   *
   * @return a reference to the matrix item at (r,c)
   */
  T& operator() (int r, int c) { return row(r)[c]; }

  /**
   * Returns a copy of an item in the matrix.
   *
   * @param r the row index
   *
   * @param c the column index
   *
   * @return the matrix item at (r,c)
   */
  T operator() (int r, int c) const { return row(r)[c]; }

  /**
   * Access the matrix along its first non-singleton dimension. If the
   * matrix is actually a vector, it is more convenient not to repeat
   * the zero row or column index. This function works with both row and
   * column vectors. If the matrix is not a vector, the first element
   * of the indexth row is returned.
   *
   * ~~~(c++)
   * // Row vector
   * PiiMatrix<int> mat(1, 3, 0,1,2);
   * QCOMPARE(mat(2), 2);
   * // Column vector
   * PiiMatrix<int> mat2(3, 1, 0,1,2);
   * QCOMPARE(mat2(2), 2);
   * // Matrix
   * PiiMatrix<int> mat3(3, 2,
   *                     0, 1, 2,
   *                     3, 4, 5);
   * QCOMPARE(mat3(1), 3);
   * ~~~
   */
  T& operator() (int index) { return (rows() > 1 ? row(index)[0] : row(0)[index]); }
  T operator() (int index) const { return (rows() > 1 ? row(index)[0] : row(0)[index]); }


  /**
   * Returns a pointer to the beginning of the given row. Same as
   * row(r). The purpose of this function is to allow the use of a
   * matrix as a two-dimensional array:
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(2, 2,
   *                    1, 2,
   *                    3, 4);
   * QVERIFY(mat[0][1] == mat(0,1));
   * ~~~
   */
  T* operator[] (int r) { return row(r); }
  const T* operator[] (int r) const { return row(r); }

  /**
   * Returns a mutable reference to a sub-matrix. Take care that the
   * dimensions of the matrix are not exceeded. If you modify the
   * returned result, the data within this matrix will also change.
   * The const version returns a copy.
   *
   * @param r the row of the upper left column of the sub-matrix. If
   * this is negative, it is treated as a backwards index from the
   * last row. -1 means the last row and so on.
   *
   * @param c the column of the upper left column of the sub-matrix.
   * Negative index is relative to the last column.
   *
   * @param rows the number of rows to include. Negative value means
   * "up to the nth last row".
   *
   * @param columns the number of columns to include. Negative value
   * means "up to the nth last column".
   *
   * ~~~(c++)
   * PiiMatrix<int> a(3, 3,
   *                  1, 2, 3,
   *                  4, 5, 6,
   *                  7, 8, 9);
   *
   * a(0,1,1,2) = 8;
   * a(-2,-2,-1,1) = 0;
   *
   * // a = 1, 8, 8,
   * //     4, 0, 6,
   * //     7, 0, 9);
   * ~~~
   */
  using PiiConceptualMatrix<PiiMatrix>::operator();
  const PiiMatrix operator() (int r, int c, int rows, int columns) const
  {
    this->fixIndices(r, c, rows, columns);
    return PiiMatrix(this->createReference(rows, columns, const_cast<T*>(row(r) + c))->makeImmutable());
  }

  template <class Matrix> PiiFilteredMatrix<const PiiMatrix, Matrix>
  operator() (const PiiConceptualMatrix<Matrix>& mask) const;

  template <class Matrix> PiiFilteredMatrix<PiiMatrix, Matrix>
  operator() (const PiiConceptualMatrix<Matrix>& mask);

  /**
   * Cast the contents of a matrix to another type. If the contents
   * of the original matrix are not of any elementary type, then
   * an appropriate typecast operator for the content class must
   * be defined.
   */
  template <class U> operator PiiMatrix<U>() const
  {
    PiiMatrix<U> result(PiiMatrix<U>::uninitialized(rows(), columns()));
    Pii::transform(begin(), end(), result.begin(), Pii::Cast<T,U>());
    return result;
  }

  /**
   * Returns a **row** vector that contains the elements of the row at
   * *index*. If you need a column vector, use
   * [operator()](int,int,int,int).
   */
  PiiMatrix column(int index) const;

  /**
   * Resizes the matrix to *rows*-by-*columns*. The function
   * preserves as much data as possible and sets any new entries to
   * zero. If the currently reserved space is not large enough, matrix
   * data will be reallocated. No reallocation will occur if the size
   * is decreased.
   */
  void resize(int rows, int columns);

  /**
   * Appends a new row to the end of the matrix. The contents of the
   * new row will be set to zero. Returns a row iterator to the
   * beginning of the new row. New rows can be appended without
   * reallocation until [capacity()] is exceeded.
   */
  typename Traits::row_iterator appendRow()
  {
    detach();
    std::size_t iBytesPerRow = sizeof(T) * d->iColumns;
    void* pRow = PiiTypelessMatrix::appendRow(iBytesPerRow);
    std::memset(pRow, 0, iBytesPerRow);
    return static_cast<T*>(pRow);
  }

  /**
   * Appends the given vector to the end of this matrix.
   *
   * @see insertRow(int, const T*)
   */
  template <class Iterator> typename Traits::row_iterator appendRow(Iterator row)
  {
    return insertRow(-1, row);
  }

  /**
   * Appends all rows in *other* to the end of this matrix. The number
   * of columns in *other* must equal to that of this matrix, unless
   * this matrix is empty. In that case this matrix will be set equal
   * to *other*.
   */
  template <class Matrix> void appendRows(const Matrix& other);

  /**
   * Appends the given elements as a new row to the end of this
   * matrix. The number of elements must match the number of columns
   * in this matrix.
   *
   * @see insertRow(int, VaArgType, ...)
   */
  typename Traits::row_iterator appendRow(VaArgType firstElement, ...);

  /**
   * Inserts a new at position *index* and moves all following rows
   * forwards. The contents of the new row will be set to zero.
   * Returns a row iterator to the beginning of the new row.
   *
   * @param index the row index of the new row. -1 means last.
   *
   * @return an iterator to the beginning of the newly added row
   */
  typename Traits::row_iterator insertRow(int index);

  /**
   * Inserts the given row at the given row index. The data of the new
   * row will be copied from *row*, which must hold at least
   * [columns()] elements.
   *
   */
  template <class Iterator>
  typename Traits::row_iterator insertRow(int index, Iterator row);

  /**
   * Insert a row with the given data elements at the given row index.
   * The number of element arguments must be equal to the number of
   * columns in the matrix.
   *
   */
  typename Traits::row_iterator insertRow(int index, VaArgType firstElement, ...);

  /**
   * Appends a new column to the right of the last column.
   *
   * @see insertColumn(int)
   */
  typename Traits::column_iterator appendColumn()
  {
    return insertColumn(-1);
  }

  /**
   * Appends the given vector as a new column to the right of the last
   * column.
   *
   * @see insertColumn(int, const T*)
   */
  template <class Iterator>
  typename Traits::column_iterator appendColumn(Iterator column)
  {
    return insertColumn(-1, column);
  }

  /**
   * Appends the given elements as a new column to the right of the
   * last column.
   *
   * @see insertColumn(int, VaArgType, ...)
   */
  typename Traits::column_iterator appendColumn(VaArgType firstElement, ...);

  /**
   * Inserts an empty column at the given index. Any subsequent
   * columns will be shifted forwards. The elements on the new column
   * will be set to zeros.
   *
   * ! New columns can be added without reallocation if the stride
   * is large enough. If there is no more free space, the matrix data
   * will be reallocated with [resize()].
   *
   * @param index the row index of the new column. -1 means last.
   *
   * @return an iterator to the beginning of the newly added column
   */
  typename Traits::column_iterator insertColumn(int index = -1);

  /**
   * Inserts the given column at the given column index. The data of
   * the new column will be copied from *column*, which must hold at
   * least [rows()] elements.
   */
  template <class Iterator>
  typename Traits::column_iterator insertColumn(int index, Iterator column);

  /**
   * Inserts a column with the given data elements at the given column
   * index. Any subsequent columns will be shifted forwards. The
   * number of elements must be equal to the number of rows in the
   * matrix.
   */
  typename Traits::column_iterator insertColumn(int index, VaArgType firstElement, ...);

  /**
   * Removes the row at *index*. All rows below the removed one will
   * be moved up.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3, 1,2,3, 4,5,6, 7,8,9);
   * mat.removeRow(1);
   * // mat = 1 2 3
   * //       7 8 9
   * ~~~
   */
  void removeRow(int index) { detach(); PiiTypelessMatrix::removeRow(index, sizeof(T) * d->iColumns); }

  /**
   * Removes *count* successive rows starting at *index*. All rows
   * below the last removed one will be moved up.
   */
  void removeRows(int index, int count)
  {
    detach();
    PiiTypelessMatrix::removeRows(index, count, sizeof(T) * d->iColumns);
  }

  /**
   * Removes a column from the matrix. All data right of *index* will
   * be moved left. The stride of the matrix will not change. If you
   * later add a column to the matrix, the data will not be
   * reallocated.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3, 1,2,3, 4,5,6, 7,8,9);
   * mat.removeColumn(1);
   * // mat = 1  3
   * //       4  6
   * //       7  9
   * ~~~
   */
  void removeColumn(int index) { detach(); PiiTypelessMatrix::removeColumn(index, sizeof(T)); }

  /**
   * Removes *count* successive columns starting at *index*. All columns
   * right of the last removed one will be moved left.
   */
  void removeColumns(int index, int count)
  {
    detach();
    PiiTypelessMatrix::removeColumns(index, count, sizeof(T));
  }

  /**
   * Swaps the places of *row1* and *row2*.
   */
  void swapRows(int row1, int row2);

  /**
   * Creates a *size*-by-*size* identity matrix.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(PiiMatrix<int>::identity(3));
   * // 1 0 0
   * // 0 1 0
   * // 0 0 1
   * ~~~
   */
  static PiiMatrix identity(int size);

  /**
   * Creates a *rows*-by-*columns* matrix whose initial contents are
   * set to *value*. This is faster than first creating and clearing
   * a matrix to zeros and then setting all entries.
   *
   * ~~~(c++)
   * PiiMatrix<float> mat(PiiMatrix<float>::constant(2, 3, 1.0));
   * // 1.0 1.0 1.0
   * // 1.0 1.0 1.0
   * ~~~
   */
  static PiiMatrix constant(int rows, int columns, T value)
  {
    PiiMatrix result(uninitialized(rows, columns));
    result = value;
    return result;
  }

  /**
   * Creates an uninitialized *rows*-by-*columns* matrix. This
   * function leaves the contents of the matrix in an unspecified
   * state. It is useful as an optimization if you know you are going
   * to set all matrix entries anyway. If *stride* is set to a value
   * larger than `sizeof(T)*columns`, matrix rows will be
   * padded to *stride* bytes.
   */
  static PiiMatrix uninitialized(int rows, int columns, std::size_t stride = 0)
  {
    return PiiMatrix(PiiMatrixData::createUninitializedData(rows, columns,
                                                            columns * sizeof(T),
                                                            stride));
  }

  /**
   * Creates a *rows*-by-*columns* matrix that is filled with the
   * elements given as the rest of the parameters. This function
   * should be preferred over the corresponding constructor to avoid
   * ambiguities in overload resolution.
   */
  static PiiMatrix filled(int rows, int columns, VaArgType firstElement, ...)
  {
    PiiMatrix matResult(uninitialized(rows,columns));
    va_list argp;
    va_start(argp, firstElement);
    Pii::copyVaList(matResult.begin(), matResult.end(), firstElement, argp);
    va_end(argp);
    return matResult;
  }

  /**
   * Creates *rows*-by-*columns* matrix with initial contents set to
   * zero. If *stride* is set to a value larger than
   * `sizeof(T)*columns`, matrix rows will be padded to
   * *stride* bytes.
   */
  static PiiMatrix padded(int rows, int columns, std::size_t stride)
  {
    return PiiMatrix(PiiMatrixData::createInitializedData(rows, columns,
                                                          columns * sizeof(T),
                                                          stride));
  }

private:
  PiiMatrix(PiiMatrixData* d) : PiiTypelessMatrix(d) {}
};

#include "PiiMatrix-templates.h"

namespace Pii
{
  /**
   * Returns a deep copy of `mat`. This function is useful if you
   * need a concrete copy of a matrix concept.
   *
   * ~~~(c++)
   * template <class T> void func(const PiiMatrix<T>& mat);
   *
   * // ...
   * PiiMatrix<int> mat1, mat2;
   * func(mat1 + mat2); // Error, mat1 + mat2 is a PiiBinaryMatrixTransform
   * func(Pii::matrix(mat1 + mat2)); // calls func(const PiiMatrix<int>&)
   * ~~~
   *
   * @relates PiiMatrix
   */
  template <class Matrix>
  inline PiiMatrix<typename Matrix::value_type, Matrix::staticRows, Matrix::staticColumns>
  matrix(const PiiConceptualMatrix<Matrix>& mat)
  {
    return PiiMatrix<typename Matrix::value_type, Matrix::staticRows, Matrix::staticColumns>(mat);
  }

  /**
   * This specialization just returns `mat`.
   */
  template <class T, int rows, int cols>
  inline PiiMatrix<T,rows,cols> matrix(const PiiMatrix<T,rows,cols>& mat) { return mat; }

  /**
   * Returns a deep copy of `mat` allocated from the heap.
   *
   * @see matrix()
   * @relates PiiMatrix
   */
  template <class Matrix>
  inline PiiMatrix<typename Matrix::value_type>* newMatrix(const PiiConceptualMatrix<Matrix>& mat)
  {
    return new PiiMatrix<typename Matrix::value_type, Matrix::staticRows, Matrix::staticColumns>(mat);
  }
}

#endif //_PIIMATRIX_H
