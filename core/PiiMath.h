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

#ifndef _PIIMATH_H
#define _PIIMATH_H

#include "PiiMatrix.h"
#include "PiiTypeTraits.h"
#include "PiiMathDefs.h"
#include "PiiFunctional.h"
#include "PiiHeap.h"
#include "PiiMatrixValue.h"
#include "PiiPreprocessor.h"

#include <cstdlib>
#include <complex>

namespace Pii
{
  /**
   * @group math Mathematics
   *
   * Miscellaneous mathematical routines. This file contains functions
   * for many typical matrix calculations. Some functions come in two
   * versions, one for const and the other for non-const
   * parameters. The non-const versions are faster and more memory
   * efficient because they modify the matrix in place. The const
   * versions return a new matrix.
   *
   * The PiiMath namespace also contains template replacements for
   * many standard math functions such as `log`(), `sqrt`(), `sin`(),
   * `atan2`() etc. The functions accept both scalar and matrix
   * arguments and use C99 functions specific to a type if possible.
   */

  /**
   * A trait structure that converts any input type to a real output
   * type. Many mathematical functions accept any input but return a
   * real number. This structure maps the input type to a suitable
   * output type.
   */
  template <class T> struct RealReturnValue
  {
    /**
     * By default, real numbers are doubles.
     */
    typedef double Type;
  };
  /**
   * A specialization of the RealReturnValue template for `float`.
   */
  template <> struct RealReturnValue<float>
  {
    /**
     * `float` input yields `float` output.
     */
    typedef float Type;
  };

  /**
   * Check if the absolute value of the difference between
   * corresponding elements in two matrices are within the given
   * tolerance. This function is useful if one needs to compare
   * matrices with floating-point values.
   *
   * ~~~(c++)
   * PiiMatrix<double> mat1(1,1, 1.0), mat2(1,1, 1.01);
   * Pii::almostEqual(mat1, mat2, 0.1); //returns true
   * ~~~
   */
  template <class Matrix1, class Matrix2>
  bool almostEqual(const Matrix1& mat1,
                   const Matrix2& mat2,
                   typename Matrix1::value_type tolerance);

  /**
   * Compares each element in the range [*begin*, *end*) to the
   * corresponding element in the range starting at *begin2*. If the
   * absolute difference between any pair of elements is larger than
   * *tolerance*, returns `false`. Otherwise returns `true`.
   */
  template <class InputIterator1, class InputIterator2>
  bool almostEqual(InputIterator1 begin1, InputIterator1 end1,
                   InputIterator2 begin2,
                   typename std::iterator_traits<InputIterator1>::value_type tolerance);

  /**
   * Sets diagonal entries in *matrix* to ones and other entries to
   * zero. If the matrix is non-square, the smaller of its two
   * dimensions will determine the length of the diagonal.
   */
  template <class Matrix> void setIdentity(Matrix& matrix)
  {
    typedef typename Matrix::value_type T;
    matrix = T(0);
    const int iMinSize = qMin(matrix.rows(), matrix.columns());
    for (int i=0; i<iMinSize; ++i)
      matrix(i,i) = T(1);
  }

  /**
   * Sets diagonal entries in *matrix* to the values read from
   * *vector*. If the matrix is non-square, the smaller of its two
   * dimensions will determine the length of the diagonal.
   *
   * @param matrix an m-by-n output matrix
   *
   * @param vector a random-access iterator to the beginning of the
   * diagonal values. Must be valid for at least min(m,n) entries.
   */
  template <class Matrix, class Iterator>
  void setDiagonal(Matrix& matrix,
                   Iterator vector)
  {
    const int iMinSize = qMin(matrix.rows(), matrix.columns());
    for (int i=0; i<iMinSize; ++i)
      matrix(i,i) = vector[i];
  }

  /**
   * Returns the determinant of a matrix.
   *
   * ! This function can be used with floating-point types only.
   * Integer matrices can be used only if the size of the matrix is
   * either 2-by-2 or 3-by-3.
   *
   * @exception PiiMathException& if the matrix is non-square
   */
  template <class T> T determinant(const PiiMatrix<T>& mat);

  /**
   * Returns the determinant of a 2x2 matrix.
   *
   * ! Does not check matrix size.
   */
  template <class T> T determinant2x2(const PiiMatrix<T>& m);

  /**
   * Calculates the determinant of a 3x3 matrix.
   *
   * ! Does not check matrix size.
   */
  template <class T> T determinant3x3(const PiiMatrix<T>& m);

  /**
   * Invert a matrix. Only square, non-singular matrices can be
   * inverted. Gauss-Jordan elimination is used as the inversion
   * algorithm.
   * ! Is not implemented properly for interger type matrix.
   *
   * @return the inverted matrix.
   *
   * @exception PiiMathException& if the matrix is non-square or singular
   */
  template <class T> PiiMatrix<T> inverse(const PiiMatrix<T>& mat);

  //TODO dot product for complex numbers

  /**
   * Calculates the inner product of two vectors `m1` and `m2`. The
   * matrices must be vectors of the same size.
   */
  template <class Matrix1, class Matrix2>
  PII_COMBINE_TYPES(typename Matrix1::value_type, typename Matrix2::value_type) innerProduct(const Matrix1& m1, const Matrix2& m2)
  {
    typedef PII_COMBINE_TYPES(typename Matrix1::value_type, typename Matrix2::value_type) T;
    PII_MATRIX_CHECK_EQUAL_SIZE(m1,m2);
    return innerProduct(m1.begin(), m1.end(), m2.begin(), T(0));
  }

  /**
   * Calculates the cross product of two vectors `m1` and `m2`. The
   * matrices must be either 3-by-1 or 1-by-3 matrices, i.e.
   * 3-dimensional vectors. The function works with both row and
   * column vectors. The orientation of the result vector will be the
   * same as that of the inputs.
   */
  template <class T> PiiMatrix<T> crossProduct(const PiiMatrix<T>& m1, const PiiMatrix<T>& m2);

  /**
   * Multiplies the elements in `m1` by the corresponding elements in
   * `m2`.
   */
  template <class Matrix1, class Matrix2>
  inline void multiply(Matrix1& m1,
                       const Matrix2& m2)
  {
    PII_MATRIX_CHECK_EQUAL_SIZE(m1,m2);
    Pii::map(m1.begin(), m1.end(), m2.begin(), std::multiplies<typename Matrix1::value_type>());
  }

  /**
   * Multiplies the elements in `m1` and `m2`. Returns a new matrix.
   */
  template <class Matrix1, class Matrix2>
  PiiBinaryMatrixTransform<Matrix1, Matrix2,
                           std::multiplies<PII_COMBINE_TYPES(typename Matrix1::value_type, typename Matrix2::value_type)> >
  multiplied(const Matrix1& m1,
             const Matrix2& m2)
  {
    PII_MATRIX_CHECK_EQUAL_SIZE(m1,m2);
    typedef PII_COMBINE_TYPES(typename Matrix1::value_type, typename Matrix2::value_type) R;
    return binaryMatrixTransform(m1, m2, std::multiplies<R>());
  }


  /**
   * Divides the elements in `m1` by the corresponding elements in
   * `m2`.
   */
  template <class Matrix1, class Matrix2>
  inline void divide(Matrix1& m1,
                     const Matrix2& m2)
  {
    PII_MATRIX_CHECK_EQUAL_SIZE(m1,m2);
    Pii::map(m1.begin(), m1.end(), m2.begin(), std::divides<typename Matrix1::value_type>());
  }

  /**
   * Divides the elements in `m1` by the corresponding elements in
   * `m2`. Returns a new matrix.
   */
  template <class Matrix1, class Matrix2>
  PiiBinaryMatrixTransform<Matrix1, Matrix2,
                           std::divides<PII_COMBINE_TYPES(typename Matrix1::value_type, typename Matrix2::value_type)> >
  divided(const Matrix1& m1,
          const Matrix2& m2)
  {
    PII_MATRIX_CHECK_EQUAL_SIZE(m1,m2);
    typedef PII_COMBINE_TYPES(typename Matrix1::value_type, typename Matrix2::value_type) R;
    return binaryMatrixTransform(m1, m2, std::divides<R>());
  }

  /**
   * Returns `true` if *mat* is singular and `false`
   * otherwise. Matrix is singular if it is square and its determinant
   * equals (with a certain tolerance) zero. With the exception of
   * matrix sizes 2x2 and 3x3, singularity can be accurately detected
   * with floating point types only.
   */
  template <class T> bool isSingular(const PiiMatrix<T>& mat);

  /**
   * Returns `rue` is *mat* is diagonal and `false`
   * otherwise. Diagonal matrices have (exactly) zeros everywhere but
   * on the diagonal.
   */
  template <class T> bool isDiagonal(const PiiMatrix<T>& mat);

  /**
   * Returns `true` if *mat* is symmetric about its diagonal and
   * `false` otherwise.
   */
  template <class T> bool isSymmetric(const PiiMatrix<T>& mat);

  /**
   * Returns `true` if *mat* is antisymmetric and `false`
   * otherwise. The transpose of an antisymmetric matrix is equal to
   * its negation.
   */
  template <class T> bool isAntiSymmetric(const PiiMatrix<T>& mat);

  /**
   * Returns `true` is *mat* is square and `false` othewise.
   */
  template <class Matrix> inline bool isSquare(const Matrix& mat)
  {
    return (mat.rows() == mat.columns());
  }

  /**
   * Tests whether the given real matrix is "orthogonal-like".
   * For square matrices returns true iff matrix is orthogonal,
   * meaning that \(A^T == A^{-1}\). It also implies  that row vectors
   * (and column vectors respectively) form orthonormal basis, i.e. vectors
   * are mutually perpendicular and length of every vector is 1.
   *
   * For non-square m*n matrices, returns true iff. either row vectors(m<n) or
   * columns vectors(m>n) are orthonormal.
   * The former case implies \(A*A^T == I\), the latter case \(A^T*A == I\)
   *
   * @param mat Real matrix.
   * @param tolerance Defines the numerical tolerance in comparison.
   */
  template <class T> bool isOrthogonalLike(const PiiMatrix<T>& mat, const T& tolerance);

  /**
   * Overloaded function which calls [isOrthogonalLike()] with default numerical
   * difference tolerance for type T.
   */
  template <class T> bool isOrthogonalLike(const PiiMatrix<T>& mat) {return isOrthogonalLike(mat, Numeric<T>::tolerance());}

  /**
   * Returns the absolute value of the scalar argument
   * *value*. Specializations use fabs() derivatives for floating point
   * and abs() derivatives for integer types. For any other type, it
   * calls a class member function called `abs` and returns its
   * result.
   *
   * ! This function retains the input type and its signedness.
   * Behavior is different from `std::abs` for some types.
   *
   * ~~~(c++)
   * char a = -128;
   * // char(128) = char(-128)
   * int b = Pii::abs(a); //b == -128
   * b = std::abs(a); //b == 128
   * ~~~
   */
  template <class T> inline T abs(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return value.abs();
  }

  /**
   * Calculates the absolute value of a complex number.
   */
  template <class T> inline T abs(const std::complex<T>& value) { return std::abs(value); }

  /**
   * Returns the absolute value of *value* squared. The result is
   * equal to the square of *value* with real types, and the the norm
   * of *value* with complex types.
   */
  template <class T> inline T abs2(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return value*value;
  }

  template <class T> inline T abs2(const std::complex<T>& value)
  {
    return std::norm(value);
  }

  /**
   * Returns the real part of a real number, i.e. the number itself.
   */
  template <class T> inline T real(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return value;
  }
  /**
   * Returns the real part of a complex number.
   */
  template <class T> inline T real(const std::complex<T>& value) { return value.real(); }

  /**
   * Returns the imaginary part of a real number, i.e. zero.
   */
  template <class T> inline T imag(T, typename OnlyNumeric<T>::Type = 0)
  {
    return 0;
  }
  /**
   * Returns the imaginary part of a complex number.
   */
  template <class T> inline T imag(const std::complex<T>& value) { return value.imag(); }

  /**
   * Calculates the complex conjugate of a real number. This function
   * returns the input argument as such.
   */
  template <class T> inline T conj(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return value;
  }
  /**
   * Calculates the conjugate of a complex scalar. This function is a
   * wrapper to std::conj() allowing one to always use Pii::conj()
   * regardless of the type.
   */
  template <class T> inline std::complex<T> conj(const std::complex<T>& value) { return std::conj(value); }

  /**
   * Find the greatest common divisor (gcd) for the given two numbers
   * using Euclid's algorithm. If either of the numbers is zero, the
   * other one is returned. The gcd is always positive, or zero if
   * both arguments are zero.
   */
  template <class T> T gcd(T number1, T number2);

  inline char abs(char value) { return (char)std::abs(value); }
  inline short abs(short value) { return (short)std::abs(value); }
  inline int abs(int value) { return std::abs(value); }
  inline long abs(long value) { return std::labs(value); }
  inline long long abs(long long value)
  {
#ifdef __USE_ISOC99
    return ::llabs(value);
#else
    return value >= 0 ? value : -value;
#endif
  }
  inline unsigned char abs(unsigned char value) { return value; }
  inline unsigned short abs(unsigned short value) { return value; }
  inline unsigned int abs(unsigned int value) { return value; }
  inline unsigned long abs(unsigned long value) { return value; }
  inline unsigned long long abs(unsigned long long value) { return value; }
  inline float abs(float value) { return ::fabsf(value); }
  inline double abs(double value) { return ::fabs(value); }
  inline long double abs(long double value) { return ::fabsl(value); }
  inline bool abs(bool value) { return value; }

  /**
   * Returns the natural logarithm of *value*).
   */
  template <class T> inline double log(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::log(double(value));
  }
  inline float log(float value)
  {
#ifdef __USE_ISOC99
    return ::logf(value);
#else
    return float(::log(value));
#endif
  }

  /**
   * Returns the base 10 logarithm of *value*.
   */
  template <class T> inline double log10(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::log10(double(value));
  }
  inline float log10(float value)
  {
#ifdef __USE_ISOC99
    return ::log10f(value);
#else
    return float(::log10(value));
#endif
  }

  /**
   * Returns the base 2 logarithm of *value*.
   */
  template <class T> inline double log2(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::log10(double(value));
  }
  inline float log2(float value)
  {
#ifdef __USE_ISOC99
    return ::log2f(value);
#else
    return float(::log2(value));
#endif
  }

  /**
   * Returns `exp`(*value*).
   */
  template <class T> inline double exp(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::exp(double(value));
  }
  /**
   * Returns `exp`(*value*).
   */
  inline float exp(float value)
  {
#ifdef __USE_ISOC99
    return ::expf(value);
#else
    return float(::exp(value));
#endif
  }


  /**
   * Returns the *value* * *value*.
   */
  template <class T> inline T square(T value, typename OnlyNumericOrComplex<T>::Type = 0)
  {
    return value*value;
  }

  /**
   * Returns the square root of *value*.
   */
  template <class T> inline double sqrt(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return (std::sqrt(static_cast<double>(value)));
  }

  inline float sqrt(float value)
  {
#ifdef __USE_ISOC99
    return ::sqrtf(value);
#else
    return float(::sqrt(value));
#endif
  }

  inline float fastSqrt(float x)
  {
    union
    {
      float x;
      int i;
    } u;
    u.x = x;
    u.i = 0x5f3759df - (u.i >> 1);  // initial guess
    float xux = x * u.x;
    return xux * (1.5f - .5f * xux * u.x); // Newton step
  }

  /**
   * Returns `sin`(*value*).
   */
  template <class T> inline double sin(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::sin(static_cast<double>(value));
  }

  inline float sin(float value)
  {
#ifdef __USE_ISOC99
    return ::sinf(value);
#else
    return float(::sin(value));
#endif
  }

  template <class T> inline double asin(T value)
  {
    return ::asin(static_cast<double>(value));
  }

  inline float asin(float value)
  {
#ifdef __USE_ISOC99
    return ::asinf(value);
#else
    return float(::asin(value));
#endif
  }

  /**
   * Returns `cos`(*value*).
   */
  template <class T> inline double cos(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::cos(static_cast<double>(value));
  }

  inline float cos(float value)
  {
#ifdef __USE_ISOC99
    return ::cosf(value);
#else
    return float(::cos(value));
#endif
  }

  template <class T> inline double acos(T value)
  {
    return ::acos(static_cast<double>(value));
  }

  inline float acos(float value)
  {
#ifdef __USE_ISOC99
    return ::acosf(value);
#else
    return float(::acos(value));
#endif
  }

  /**
   * Returns `tan`(*value*).
   */
  template <class T> inline double tan(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::tan(static_cast<double>(value));
  }

  inline float tan(float value)
  {
#ifdef __USE_ISOC99
    return ::tanf(value);
#else
    return float(::tan(value));
#endif
  }

  template <class T> inline double atan(T value)
  {
    return ::atan(static_cast<double>(value));
  }

  inline float atan(float value)
  {
#ifdef __USE_ISOC99
    return ::atanf(value);
#else
    return float(::atan(value));
#endif
  }

  /**
   * Rounds a double argument to the closest integer. Uses the C99
   * standard round() function if available.
   */
  inline double round(double value)
  {
#ifdef __USE_ISOC99
    return ::round(value);
#else
    return static_cast<int>(value + (value >= 0 ? 0.5 : -0.5));
#endif
  }

  /**
   * Rounds a float argument to the closest integer. Uses the C99
   * standard roundf() function if available.
   */
  inline float round(float value)
  {
#ifdef __USE_ISOC99
    return ::roundf(value);
#else
    return static_cast<int>(value + (value >= 0 ? 0.5 : -0.5));
#endif
  }

#define PII_ROUND_INT(N, TYPE) \
  inline unsigned TYPE round(unsigned TYPE value) { return value; } \
  inline TYPE round(TYPE value) { return value; }
  PII_FOR_N(PII_ROUND_INT, 4, (char, short, int, long));
#undef PII_ROUND_INT

  /**
   * Rounds *value* (of type `T`) to the closest integer and returns
   * the result as another type (`U`).
   *
   * ~~~(c++)
   * int i = Pii::round<int>(1.5); // returns 2
   * ~~~
   */
  template <class U, class T> inline U round(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return static_cast<U>(round(value));
  }

  /**
   * Returns `ceil`(*value*).
   */
  template <class T> inline double ceil(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::ceil(value);
  }

  inline float ceil(float value)
  {
#ifdef __USE_ISOC99
    return ::ceilf(value);
#else
    return float(::ceil(value));
#endif
  }

  /**
   * Returns `floor`(*value*).
   */
  template <class T> inline double floor(T value, typename OnlyNumeric<T>::Type = 0)
  {
    return ::floor(value);
  }

  inline float floor(float value)
  {
#ifdef __USE_ISOC99
    return ::floorf(value);
#else
    return float(::floor(value));
#endif
  }

  /**
   * Generic modulus function that works with both integer and
   * floating-point types.
   *
   * The floating-point versions of this function can be used in
   * folding a floating point number to a predefined range. Its most
   * typical use is in handling angles.
   *
   * ~~~(c++)
   * double dAngle = Pii::mod(M_PI*3, M_PI*2); // returns M_PI
   * ~~~
   */
  template <class T> inline T mod(T a, T b, typename OnlyNumeric<T>::Type = 0)
  {
    return a % b;
  }

  inline float mod(float d1, float d2) { return fmodf(d1, d2); }
  inline double mod(double d1, double d2) { return fmod(d1, d2); }

  /**
   * Calculates the sum of matrix elements in the specified direction.
   * If *direction* is `Pii::Horizontally`, returns a column matrix.
   * If *direction* is `Pii::Vertically`, returns a row matrix. The
   * returned type can be different from the input type. For example,
   * the sum over unsigned char matrices may require an integer output
   * type.
   *
   * ~~~(c++)
   * PiiMatrix<int> A(2 , 2,
   *                  1 , 2,
   *                  -1,-2);
   * PiiMatrix<int> B(sum<int>(A, Pii::Horizontally));
   * //B =  3
   * //    -3
   * PiiMatrix<int> C(sum<int>(A, Pii::Vertically));
   * //C = 0 0
   * ~~~
   */
  template <class U, class Matrix> PiiMatrix<U> sum(const Matrix& mat,
                                                    MatrixDirection direction);

  /**
   * Different ways of calculating 2-d cumulative sum.
   *
   * - `OrdinaryCumulativeSum` - the size of the result matrix will
   * be the same as that of the input.
   *
   * - `ZeroBorderCumulativeSum` - the size of the result matrix will
   * be one larger than that of the input in both directions. The
   * first row and column will be set to zero. This allows certain
   * optimizations in border checking if the sum matrix is used for
   * calculating moving averages.
   */
  enum CumulativeSumMode
    {
      OrdinaryCumulativeSum = 0,
      ZeroBorderCumulativeSum = 1
    };

  /**
   * Calculates two-dimensional cumulative sum of a matrix. The
   * cumulative sum *X(r,c)* of an input matrix *x(r,c)*
   * can be formally defined as
   *
   * \[
   * X(r,c) = \sum_{i=0}^r \sum_{j=0}^c f(x(i,j))
   * \]
   *
   * where \(f(\cdot)\) is a user-defined function.
   *
   * @param matrix the input matrix
   *
   * @param func the function to apply to the input
   *
   * @param mode calculation mode
   *
   * ~~~(c++)
   * using namespace Pii;
   *
   * PiiMatrix<int> mat;
   * // Calculate cumulative sum of squares. Note that it is very easy
   * // to overflow such a large sum.
   * PiiMatrix<long long> result = cumulativeSum(mat, Square<long long>());
   * ~~~
   */
  template <class UnaryFunction, class Matrix>
  PiiMatrix<typename UnaryFunction::result_type> cumulativeSum(const Matrix& matrix,
                                                               UnaryFunction func,
                                                               CumulativeSumMode mode);
  /**
   * This version uses a preallocated output matrix. It is the
   * responsibility of the caller to make sure the output has correct
   * size.
   */
  template <class UnaryFunction, class InputMatrix, class OutputMatrix>
  void cumulativeSum(const InputMatrix& matrix,
                     OutputMatrix& result,
                     UnaryFunction func,
                     CumulativeSumMode mode);

  /**
   * Calculates two-dimensional cumulative sum of a matrix. In
   * `OrdinaryCumulativeSum`, the size of the returned matrix equals
   * that of the input. In `ZeroBorderCumulativeSum`, the output will
   * be larger. Each element stores the sum of all elements whose row
   * and column coordinates are not larger than those of the element.
   * The cumulative sum *X(r,c)* of an input matrix
   * *x(r,c)* can be formally defined as
   *
   * \[
   * X(r,c) = \sum_{i=0}^r \sum_{j=0}^c x(i,j)
   * \]
   *
   * The returned type can be different from the input type. For
   * example, the sum over unsigned char matrices requires an integer
   * output type.
   *
   * In image processing, the cumulative sum of an image is called an
   * *integral image*. It is typically used in speeding up
   * moving average calculations.
   *
   * ! The cumulative sum easily overflows with large images. If
   * the input type is `unsigned char` and the output type is
   * `int`, the input matrix may have no more than \(2^23-1\)
   * elements (about 8 megapixels) to prevent overflows.
   *
   * @param mat the input matrix
   *
   * @param mode calculation mode
   *
   * ~~~(c++)
   * using namespace Pii;
   *
   * PiiMatrix<int> matrix(3,3,
   *                       1, 2, 3,
   *                       2, 3, 4,
   *                       3, 4, 5);
   * PiiMatrix<int> sum = cumulativeSum<int>(matrix);
   *
   * // sum =
   * // 1  3  6
   * // 3  8 15
   * // 6 15 27
   *
   * sum = cumulativeSum<int>(matrix, ZeroBorderCumulativeSum);
   *
   * // sum =
   * // 0  0  0  0
   * // 0  1  3  6
   * // 0  3  8 15
   * // 0  6 15 27
   * ~~~
   */
  template <class U, class Matrix> inline PiiMatrix<U> cumulativeSum(const Matrix& mat,
                                                                     CumulativeSumMode mode = OrdinaryCumulativeSum)
  {
    return cumulativeSum(mat, Cast<typename Matrix::value_type,U>(), mode);
  }

  /**
   * Calculates the cumulative sum of matrix elements in the specified
   * direction. The size of the returned matrix equals that of the
   * input. Each element stores the sum of all elements left to it (
   * `MatrixRows`) or above it (`MatrixColumns`). The last row/column
   * of the matrix equals the result of [sum()]. The returned type can
   * be different from the input type. For example, the sum over
   * unsigned char matrices may require an integer output type.
   *
   * ~~~(c++)
   * PiiMatrix<int> A(2 , 2,
   *                  1 , 2,
   *                  -1,-2);
   * PiiMatrix<int> B(cumulativeSum<int>(A, Pii::Horizontally));
   * //B =  1 3
   * //    -1 -3
   * PiiMatrix<int> C(cumulativeSum<int>(A, Pii::Vertically));
   * //C = 1 2
   * //    0 0
   * ~~~
   */
  template <class U, class Matrix> PiiMatrix<U> cumulativeSum(const Matrix& mat,
                                                              MatrixDirection direction);

  /**
   * Fast 2-D moving average. This function uses the
   * [cumulativeSum(const PiiMatrix<T>&, CumulativeSumMode)]
   * "cumulative sum" of a matrix to calculate the moving average
   * over `mat`.
   *
   * The overall cumulative sum given as input is used to calculate
   * local cumulative sum around each element. The local sum is
   * divided by the number of elemements within the window. In the
   * example below, three is used as the window size in both
   * directions.
   *
   * ~~~
   * Original   Cumulative sum (input)
   *
   *            0  0  0  0
   * 1 2 3      0  1  3  6
   * 2 3 4  ->  0  3  8  15
   * 3 4 5      0  6  15 27
   *
   * Upper left:
   * (8 + 0 - 0 - 0) / 4 = 2
   * Lower right:
   * (27 + 1 - 6 - 6) / 4 = 4
   * ~~~
   *
   * The calculation time is independent of the size of the averaging
   * window.
   *
   * @param cumulativeSum the cumulative sum of the original input
   * matrix. This function assumes that `ZeroBorderCumulativeSum` was
   * used as the calculation mode.
   *
   * @param windowRows the vertical size of the averaging window
   *
   * @param windowColumns the horizontal size of the averaging window.
   * If this value is non-positive, `windowRows` will be used.
   *
   * ~~~(c++)
   * PiiMatrix<int> matSum = cumulativeSum<int>(matInput, ZeroBorderCumulativeSum);
   * // Calculate mean as float
   * PiiMatrix<float> matSmooth = fastMovingAverage<float>(matSum, 7);
   * ~~~
   */
  template <class U, class T>
  PiiMatrix<U> fastMovingAverage(const PiiMatrix<T>& cumulativeSum,
                                 int windowRows, int windowColumns = 0);

  /**
   * Fast moving average. This function shrinks the averaging window
   * on boundaries. If the window is longer than the input, all output
   * elements will be set to the mean value of the input.
   *
   * @param input an input iterator of at least *n* elements
   *
   * @param n the number of elements to process
   *
   * @param output an output iterator of at least *n* elements. The
   * result of the calculation will be placed here.
   *
   * @param windowSize the size of the averaging window.
   *
   * ~~~(c++)
   * int aInput[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
   * double aOutput[8];
   * Pii::fastMovingAverage(aInput, iCnt, aOutput, 5);
   * // aOutput = { 2, 2, 2, 4, 5, 6, 6, 6 }
   * ~~~
   */
  template <class InputIterator, class OutputIterator>
  void fastMovingAverage(InputIterator input, int n, OutputIterator output, int windowSize);

  /**
   * Returns the sum of all entries in a matrix. Returns the value as
   * a (possibly) different type, denoted by the template parameter
   * `U`.
   */
  template <class T, class Matrix> inline T sum(const Matrix& mat)
  {
    return Pii::accumulate(mat.begin(), mat.end(),
                           std::plus<T>(),
                           T(0));
  }

  /**
   * Returns the mean of all entries in a matrix. Returns the value as
   * a (possibly) different type, denoted by the template parameter
   * `U`. The type U must be capable of storing the sum of all entries
   * in *mat*.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(1,2, 1, 0);
   * double mean = Pii::mean<double>(mat); //returns 0.5
   * ~~~
   */
  template <class U, class Matrix> inline U mean(const Matrix& mat)
  {
    return sum<U>(mat) / U(mat.rows() * mat.columns());
  }

  /**
   * Returns the variance of all elements in a matrix.
   *
   * @param mat the input matrix
   *
   * @param mean store mean to this variable (if non-zero) This is an
   * optimization that makes it unnecessary to call [mean()]
   * separately.
   */
  template <class U, class Matrix> U var(const Matrix& mat, U* mean = 0);

  /**
   * Returns the variance of matrix elements in the specified
   * direction. See [sum()] for more information.
   *
   * @param mat the source matrix
   *
   * @param direction calculate variance in this direction
   */
  template <class U, class Matrix> PiiMatrix<U> var(const Matrix& mat, MatrixDirection direction);

  /**
   * Returns the standard deviation of all elements in a matrix.
   *
   * @param mat the input matrix
   *
   * @param mean store mean to this variable (if non-zero) This is an
   * optimization that makes it unnecessary to call [mean()]
   * separately.
   */
  template <class U, class Matrix> inline U std(const Matrix& mat, U* mean = 0)
  {
    return (U)std::sqrt(var(mat, mean));
  }


  /**
   * Calculates the covariance matrix for a set of measurements. Each
   * measurement is represented as a row in the input matrix `mat`.
   *
   * @param mat a NxM input matrix. Contains N measurements with M
   * dimensions each. Make sure the matrix has at least two rows.
   *
   * @param mean a pointer to a mean matrix. The mean of all M
   * dimensions will be calculated as a side product of covariance. If
   * you need this value later, provide a placeholder for it here. The
   * mean will be a 1xM row matrix.
   *
   * @return a MxM covariance matrix. Diagonal elements represent the
   * variances of the M dimensions. The covariance matrix is
   * symmetric.
   */
  template <class T> PiiMatrix<double> covariance(const PiiMatrix<T>& mat, PiiMatrix<double>* mean = 0);

  /**
   * Calculates the mean of matrix elements in the specified direction.
   * If Pii::Horizontally is specified, a column matrix
   * returned and vice versa.
   */
  template <class U, class Matrix> inline PiiMatrix<U> mean(const Matrix& mat,
                                                            MatrixDirection direction)
  {
    PiiMatrix<U> result(sum<U>(mat, direction));
    if (direction == Vertically)
      result /= (U)mat.rows();
    else
      result /= (U)mat.columns();
    return result;
  }

  /// @internal
  template <class T> void findExtrema(const PiiMatrix<T>& mat, PiiHeap<PiiMatrixValue<T>,16>& heap);

  /**
   * Find `cnt` largest values in `mat`.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(5,5); // Assume this is non-zero
   * PiiHeap<PiiMatrixValue<int> > maxima = Pii::findMaxima(mat, 5);
   * // maxima now contains the five largest values in mat
   * ~~~
   */
  template <class T> PiiHeap<PiiMatrixValue<T> > findMaxima(const PiiMatrix<T>& mat, int cnt)
  {
    PiiHeap<PiiMatrixValue<T> > heap(cnt, PiiMatrixValue<T>(Numeric<T>::minValue()), Pii::InverseHeap);
    findExtrema(mat, heap);
    return heap;
  }

  /**
   * Find `cnt` smallest values in `mat`.
   *
   * @see findMaxima()
   */
  template <class T> PiiHeap<PiiMatrixValue<T> > findMinima(const PiiMatrix<T>& mat, int cnt)
  {
    PiiHeap<PiiMatrixValue<T> > heap(cnt, PiiMatrixValue<T>(Numeric<T>::maxValue()), Pii::NormalHeap);
    findExtrema(mat, heap);
    return heap;
  }

  /**
   * Calculates the maximum among all elements in the specified
   * direction.
   */
  template <class Matrix> PiiMatrix<typename Matrix::value_type> max(const Matrix& mat, MatrixDirection direction);

  /**
   * Calculates the minimum among all elements in the specified
   * direction.
   */
  template <class Matrix> PiiMatrix<typename Matrix::value_type> min(const Matrix& mat, MatrixDirection direction);

  /**
   * Finds the minimum and maximum elements in a matrix.
   *
   * @param mat the source matrix
   *
   * @param minimum store the minimum value to this memory location
   *
   * @param maximum store the maximum value to this memory location
   *
   * @param minR store the row of the minimum value to this memory
   * location (if non-zero)
   *
   * @param minC store the column of the minimum value to this memory
   * location (if non-zero)
   *
   * @param maxR store the row of the maximum value to this memory
   * location (if non-zero)
   *
   * @param maxC store the column of the maximum value to this memory
   * location (if non-zero)
   */
  template <class Matrix> void minMax(const Matrix& mat,
                                      typename Matrix::value_type* minimum,
                                      typename Matrix::value_type* maximum,
                                      int* minR = 0, int* minC = 0,
                                      int* maxR = 0, int* maxC = 0);



  /**
   * Returns the k:th smallest element from array.
   *
   * @param data the array in which k:th smallest value is to be
   * found.
   *
   * @param size is array size
   *
   * @param k is ordinal number. ! now k = 0 means smallest, k = 1
   * second smallest etc.
   *
   * ~~~(c++)
   * int array[8] = {0,2,3,9,4,5,6,8};
   * int element = Pii::kthSmallest<int>(array, 8, 2);
   * //now element is 3.
   * ~~~
   */

  template <class T> T kthSmallest(T* data, int size, int k);


  /**
   * Returns array which is arranged so that; there exist index k
   * which value is restricted to 0 <= k < size. Element
   * in array at index k is `pivot` value and all elements between
   * array begin to k-1 are smaller or equal as `pivot` value, and all elements between k+1 and
   * array end are at least equal to `value` or bigger.
   *
   * @param data the array which is to be arranged (partition).
   *
   * @param pivot is index to element which will be used in partition.
   *
   * @param size array size.
   *
   * @returns index to pivot value
   *
   * ~~~(c++)
   * int array[10] = {1,4,0,5,2,3,7,6,8,9};
   * int value = 3;
   * int k = Pii::partition<int>(array, 10, 5);
   * //now at position k there exist value(3) and array is
   * //rearranged.
   * ~~~
   */

  template <class T> int partition(T* data, int size, int pivot);

  /**
   * Classical insertion sort. Sorts elements in array (in-place) to
   * ascending order. Complexity class is O(n^2) where, n is number of elements in
   * array.
   *
   * @param data is array which will be sorted.
   *
   * @param size is length of array.
   *
   * ~~~(c++)
   * int array[10] = {1,3,2,0,4,5,7,6,9,8};
   * Pii::insertionSort<int>(array, 10);
   * //Now array elements are order :  0,1,2,3,4,5,6,7,8,9
   * ~~~
   */
  template <class T> void insertionSort(T* data, int size);

  /**
   * A Gaussian function. Returns gaussian function value on given
   * point x, with given expected value mu and variance sigma.
   *
   * @param x is point where gaussian function value is wanted to
   * calcualte.
   *
   * @param mu is distribution expected value.
   *
   * @param sigma is variance of distribution. Note that sigma is
   * \(\sigma\), not \(\sigma^2\).
   */
  template <class T> T gaussian(T x, double mu, double sigma);

  /**
   * Returns the median of all elements in `data`. This function
   * modifies `data`.
   *
   * @param data a random access iterator to the beginning of the
   * data. Will be modified during calculation.
   *
   * @param len the number of elements in `data`
   */
  template <class Iterator>
  typename std::iterator_traits<Iterator>::value_type fastMedian(Iterator data, int len);

  /**
   * Returns the median of all elements in *data*. This function does
   * not modify *data*. It is slower than the intrusive fastMedian(),
   * but retains data intact.
   *
   * @param data a random access iterator to the beginning of the
   * data. Will be modified during calculation.
   *
   * @param len the number of elements in *data*
   */
  template <class Iterator>
  typename std::iterator_traits<Iterator>::value_type medianN(Iterator data, int len);

  /**
   * Returns the median of all elements in `mat`.
   */
  template <class Matrix> typename Matrix::value_type median(const Matrix& mat)
  {
    return medianN(mat.begin(), mat.rows() * mat.columns());
  }

  /**
   * Median of three values optimized to the theoretical maximum
   * efficiency.
   *
   * @param data pointer to 3 successive values. The array will be
   * modified.
   */
  template <class Iterator> typename std::iterator_traits<Iterator>::value_type median3(Iterator data);
  /**
   * Median of five values optimized to the theoretical maximum
   * efficiency.
   *
   * @param data pointer to 5 successive values. The array will be
   * modified.
   */
  template <class Iterator> typename std::iterator_traits<Iterator>::value_type median5(Iterator data);
  /**
   * Median of seven values optimized to the theoretical maximum
   * efficiency.
   *
   * @param data pointer to 7 successive values. The array will be
   * modified.
   */
  template <class Iterator> typename std::iterator_traits<Iterator>::value_type median7(Iterator data);
  /**
   * Median of nine values optimized to the theoretical maximum
   * efficiency.
   *
   * @param data pointer to 9 successive values. The array will be
   * modified.
   */
  template <class Iterator> typename std::iterator_traits<Iterator>::value_type median9(Iterator data);
  /**
   * Median of 25 values optimized to the theoretical maximum
   * efficiency.
   *
   * @param data pointer to 25 successive values. The array will be
   * modified.
   */
  template <class Iterator> typename std::iterator_traits<Iterator>::value_type median25(Iterator data);

  /**
   * Difference and approximate derivative. Calculates the nth order
   * difference of the given matrix in the specified direction.
   *
   * @param mat the input matrix. Must have at least `step`*order + 1
   * elements in the calculation direction.
   *
   * @param step the "difference step". Calculates difference over this
   * many elements.
   *
   * @param order the order of the derivative.
   *
   * @param direction direction of calculation. By default rows are
   * treated as vectors, and the difference is calculated
   * horizontally. Use `Pii::Vertically` to change the default
   * behaviour.
   *
   * @return a matrix whose size depends on both `step` and `order`.
   * Specifically, the size will be decreased `step`*order elements
   * in the calculation direction.
   *
   * ~~~(c++)
   * PiiMatrix<int> a(2,4,
   *                  1,   2,  4,   8,
   *                  16, 32, 64, 128);
   * PiiMatrix<int> b(Pii::diff(a));
   * // b = 1   2  4
   *        16 32 64
   * PiiMatrix<int> c(Pii::diff(a,2));
   * // c = 3   6
   *        48 96
   * PiiMatrix<int> d(Pii::diff(a,1,1,Pii::Vertically));
   * // d = 15 30 60 120
   * PiiMatrix<int> e(Pii::diff(a,1,2));
   * // e = 1   2
   * //     16 32
   * ~~~
   */
  template <class T> PiiMatrix<T> diff(const PiiMatrix<T>& mat, int step = 1, int order = 1,
                                       MatrixDirection direction = Horizontally);

  /**
   * Calculates two-point central difference. The difference to [diff()]
   * is that this function calculates the difference to two directions
   * and uses the average as an estimate for the central point. On
   * matrix boundaries, the difference between the two bordermost
   * values will be used. This function is essentially the same as
   * `diff(mat, 2)/2` except that it handles the end points
   * differently.
   *
   * Remember to provide the result type as a template parameter.
   *
   * ~~~(c++)
   * PiiMatrix<int> input(3,5,
   *                      1,2,3,2,0,
   *                      1,3,2,2,1,
   *                      2,4,1,2,0);
   *
   * PiiMatrix<double> result =  Pii::centralDiff<double>(input, Pii::Horizontally);
   * // 1.0  1.0, 0.0 -1.5 -2.0
   * // 2.0  0.5 -0.5 -0.5 -1.0
   * // 2.0 -0.5 -1.0 -0.5 -2.0
   * ~~~
   */
  template <class T, class U> PiiMatrix<T> centralDiff(const PiiMatrix<U>& mat, MatrixDirection direction = Horizontally);

  /**
   * Find the row and column coordinates of all elements in `mat`
   * that match `predicate`. If `mat` is a row or column vector,
   * indices are returned in a row matrix. If `mat` is not a vector,
   * the row and column index for each entry are stored as row vectors
   * in the returned matrix, in this order.
   *
   * @param mat the input matrix.
   *
   * @param cnt the maximum number of indices returned. Zero means
   * all, a negative value means "all but the *-cnt* last ones". That
   * is, a negative value omits the last non-zero entries.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3,
   *                    1,2,3
   *                    4,5,6,
   *                    7,8,9);
   * find(mat, std::bind2nd(std::less<int>(), 3));
   * // returns
   * // 0,0
   * // 0,1
   * ~~~
   */
  template <class T, class UnaryPredicate> PiiMatrix<int> find(const PiiMatrix<T>& mat, UnaryPredicate predicate, int cnt = 0);

  /**
   * Find the row and column coordinates of all non-zero elements in
   * `mat`. If `mat` is a row or column vector, indices are returned
   * in a row matrix. If `mat` is not a vector, the row and column
   * index for each entry are stored as row vectors in the returned
   * matrix, in this order. This is a convenience function that works
   * analogously to its Matlab counterpart.
   *
   * @param mat the matrix to find non-zero entries in. Typically a
   * result of a comparison operator as in find(mat > 1).
   *
   * @param cnt the maximum number of indices returned. Zero means
   * all, a negative value means "all but the *-cnt* last ones". That
   * is, a negative value omits the last non-zero entries.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3,
   *                    1,2,3
   *                    4,5,6,
   *                    7,8,9);
   * find(mat < 3);
   * // returns
   * // 0,0
   * // 0,1
   * ~~~
   */
  template <class T> inline PiiMatrix<int> find(const PiiMatrix<T>& mat, int cnt = 0)
  {
    return find(mat, std::bind2nd(std::not_equal_to<T>(), T(0)), cnt);
  }

  /**
   * Returns the maximum of two arguments.
   */
  template <class T> inline T max(T a, T b, typename OnlyNumeric<T>::Type = 0) { return a >= b ? a : b; }

  /**
   * Returns the maximum of three arguments.
   */
  template <class T> inline T max(T a, T b, T c) { return max(a, max(b, c)); }

  /**
   * Returns the maximum of four arguments.
   */
  template <class T> inline T max(T a, T b, T c, T d) { return max(a, max(b, max(c, d))); }

  /**
   * Returns the minimum of two arguments.
   */
  template <class T> inline T min(T a, T b, typename OnlyNumeric<T>::Type = 0) { return a < b ? a : b; }

  /**
   * Returns the minimum of three arguments.
   */
  template <class T> inline T min(T a, T b, T c) { return min(a, min(b, c)); }

  /**
   * Returns the minimum of four arguments.
   */
  template <class T> inline T min(T a, T b, T c, T d) { return min(a, min(b, min(c, d))); }

  /**
   * A very fast approximation to four-quadrant arctan. This
   * implementation requires (at most) five comparisons, two
   * subtractions, one addition, one multiplication and one division.
   * The accuracy is 0.07 rad (about 4 degrees) at worst. The output
   * range is \([-\pi,\pi]\).
   */
  inline float fastAtan2(float y, float x)
  {
    if (y == 0 && x == 0)
      return 0;

    float angle = (float)M_PI_4;
    float absY = std::fabs(y);

    if (x>=0) // quads I and IV
      {
        if (x != -absY) // prevent divzero
          {
            float r = (x - absY) / (x + absY);
            angle = M_PI_4 - M_PI_4 * r;
          }
      }
    else // quads II and III
      {
        if (x != absY)
          {
            float r = (x + absY) / (absY - x);
            angle = M_PI_4*3 - M_PI_4 * r;
          }
        else
          angle *= 3;
      }
    if (y < 0)
      return(-angle); // negate in quads III and IV
    return(angle);
  }

  /**
   * A fast (but slower than [fastAtan2()]) approximation to
   * four-quadrant arctan. This implementation uses a third-degree
   * polynomial to approximate arctan. The accuracy is 0.01 rad (about
   * 0.6 degrees) at worst. The output range is \([-\pi,\pi]\).
   */
  inline float atan2(float y, float x)
  {
    if (y == 0 && x == 0)
      return 0;

    float angle = (float)M_PI_4;
    float absY = std::fabs(y);

    if (x>=0) // quads I and IV
      {
        if (x != -absY) // prevent divzero
          {
            float r = (x - absY) / (x + absY);
            angle = 0.1963 * r*r*r - 0.9817 * r + M_PI_4;
          }
      }
    else // quads II and III
      {
        if (x != absY)
          {
            float r = (x + absY) / (absY - x);
            angle = 0.1963 * r*r*r - 0.9817 * r + M_PI_4*3;
          }
        else
          angle = 3*(float)M_PI_4;
      }
    if (y < 0)
      return(-angle); // negate in quads III and IV

    return(angle);
  }

  /**
   * A generic algorithm to multiply a column vector by a matrix from
   * left. This function calculates `matrix` * `input` and stores
   * the result to `output`.
   *
   * @param matrix the matrix to multiply `input` with.
   *
   * @param input a forward input iterator to the beginning of the
   * column vector. The iterator must be able to access at least
   * matrix.columns() elements.
   *
   * @param output an output iterator that must be able to hold at
   * least matrix.rows() elements.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(4,3,
   *                    1,2,3,
   *                    4,5,6,
   *                    7,8,9,
   *                    0,1,2);
   * PiiVector<int,3> vector(1,2,3);
   * PiiVector<double,4> result;
   * // Calculate mat*(1,2,3)' and store the resulting 4-by-1 vector
   * // as doubles.
   * Pii::multiply(mat, vector.begin(), result.begin());
   * ~~~
   */
  template <class T, class InputIterator, class OutputIterator> void multiply(const PiiMatrix<T>& matrix,
                                                                              InputIterator input,
                                                                              OutputIterator output)
  {
    for (int r=0; r<matrix.rows(); ++r, ++output)
      {
        InputIterator column(input);
        T sum(0);
        const T* row = matrix[r];
        for (int c=0; c<matrix.columns(); ++c, ++column)
          sum += *column * row[c];
        *output = sum;
      }
  }
  /**
   * A generic algorithm to multiply a row vector by a matrix from
   * right. This function calculates `input` * `matrix` and stores
   * the result to `output`.
   *
   * @param input a forward input iterator to the beginning of the
   * column vector. The iterator must be able to access at least
   * matrix.rows() elements.
   *
   * @param matrix the matrix to multiply `input` with.
   *
   * @param output an output iterator that must be able to hold at
   * least matrix.columns() elements.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(4,3,
   *                    1,2,3,
   *                    4,5,6,
   *                    7,8,9,
   *                    0,1,2);
   * PiiVector<int,4> vector(1,2,3,4);
   * PiiVector<double,3> result;
   * // Calculate (1,2,3,4) * mat and store the resulting 1-by-3 vector
   * // as doubles.
   * Pii::multiply(vector.begin(), mat, result.begin());
   * ~~~
   *
   * ! Make sure `output` is initialized with zeros. The
   * functions saves time by not clearing the output first.
   */
  template <class InputIterator, class T, class OutputIterator> void multiply(InputIterator input,
                                                                              const PiiMatrix<T>& matrix,
                                                                              OutputIterator output)
  {
    for (int r=0; r<matrix.rows(); ++r, ++input)
      {
        OutputIterator column(output);
        const T* row = matrix[r];
        for (int c=0; c<matrix.columns(); ++c, ++column)
          *column += *input * row[c];
      }
  }

  /**
   * Performs *func* on each row of the given matrix.
   *
   * @param matrix the matrix to be modified. Each element will be
   * modified by applying `func` to it.
   *
   * @param input an iterator to the beginning of data that works as
   * the second argument to `func`. Must be valid over at least
   * matrix.columns() elements.
   *
   * @param func the binary function to apply
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3,
   *                    1,2,3,
   *                    4,5,6,
   *                    7,8,9);
   * PiiVector<int,3> vector(1,2,3);
   * Pii::transformRows(mat, vector.begin(), std::minus<int>());
   * // mat is now
   * // 0,0,0
   * // 3,3,3
   * // 6,6,6
   * ~~~
   *
   * @see transformColumns()
   */
  template <class Matrix, class InputIterator, class BinaryFunction>
  Matrix& transformRows(Matrix& matrix,
                        InputIterator input,
                        BinaryFunction func)
  {
    const int iCols = matrix.columns(), iRows = matrix.rows();;
    for (int r=0; r<iRows; ++r)
      {
        typename Matrix::row_iterator row = matrix.rowBegin(r);
        transformN(row, iCols, input, row, func);
      }
    return matrix;
  }

  /**
   * Returns a new matrix.
   */
  template <class Matrix, class InputIterator, class BinaryFunction>
  inline PiiMatrix<typename Matrix::value_type> transformedRows(const Matrix& matrix,
                                                                InputIterator input, BinaryFunction func)
  {
    PiiMatrix<typename Matrix::value_type> result(matrix);
    transformRows(result, input, func);
    return result;
  }

  /**
   * Performs `func` on each column of the given matrix.
   *
   * @param matrix the matrix to be modified. Each element will be
   * modified by applying `func` to it.
   *
   * @param input an iterator to the beginning of data that works as
   * the second argument to `func`. Must be valid over at least
   * matrix.rows() elements.
   *
   * @param func the binary function to apply
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(3,3,
   *                    1,2,3,
   *                    4,5,6,
   *                    7,8,9);
   * PiiVector<int,3> vector(1,2,3);
   * Pii::transformColumns(mat, vector.begin(), std::minus<int>());
   * // mat is now
   * // 0,1,2
   * // 2,3,4
   * // 4,5,6
   * ~~~
   *
   * @see transformRows()
   */
  template <class Matrix, class InputIterator, class BinaryFunction>
  Matrix& transformColumns(Matrix& matrix,
                           InputIterator input, BinaryFunction func)
  {
    const int iCols = matrix.columns(), iRows = matrix.rows();
    for (int r=0; r<iRows; ++r, ++input)
      {
        typename Matrix::row_iterator row = matrix.rowBegin(r);
        for (int c=0; c<iCols; ++c, ++row)
          *row = func(*row, *input);
      }
    return matrix;
  }

  /**
   * Returns a new matrix.
   */
  template <class Matrix, class InputIterator, class BinaryFunction>
  inline PiiMatrix<typename Matrix::value_type> transformedColumns(const Matrix& matrix,
                                                                   InputIterator input,
                                                                   BinaryFunction func)
  {
    PiiMatrix<typename Matrix::value_type> result(matrix);
    transformColumns(result, input, func);
    return result;
  }

  /**
   * Subtracts sample mean from a data set.
   *
   * @param matrix the data set to be moved to zero mean. If
   * *direction* is `Pii::Vertically`, each row represents an
   * observation vector. If *direction* is `Pii::Horizontally`, each
   * column represents an observation vector.
   *
   * @return the mean vector. See [mean()].
   */
  template <class Matrix>
  PiiMatrix<typename Matrix::value_type> subtractMean(Matrix& matrix,
                                                      Pii::MatrixDirection direction = Pii::Vertically)
  {
    typedef typename Matrix::value_type T;
    PiiMatrix<T> matMean = mean<T>(matrix, direction);
    if (direction == Pii::Vertically)
      transformRows(matrix, matMean.rowBegin(0), std::minus<T>());
    else
      transformColumns(matrix, matMean.columnBegin(0), std::minus<T>());
    return matMean;
  }

  /**
   * Applies the unary function *func* to all elements of *matrix*.
   * Returns *matrix*.
   *
   * ~~~(c++)
   * PiiMatrix<int> matInput(1, 3, -1, 2, 3);
   * // Negate all numbers
   * Pii::mapMatrixIf(matInput, std::negate<int>());
   * // matInput = -1 -2 -3
   * ~~~
   */
  template <class Matrix, class UnaryFunction>
  inline Matrix& mapMatrix(Matrix& matrix,
                           UnaryFunction func)
  {
    map(matrix.begin(), matrix.end(), func);
    return matrix;
  }

  /**
   * Applies the unary function *func* to all elements of *matrix*.
   * Returns a new matrix.
   */
  template <class Matrix, class UnaryFunction>
  inline Matrix& mappedMatrix(Matrix& matrix,
                              UnaryFunction func)
  {
    PiiMatrix<typename UnaryFunction::result_type> result(matrix);
    mapMatrix(result, func);
    return result;
  }

  /**
   * Applies the unary function *func* to all elements of
   * *matrix* that match the given *predicate*. Analogous to
   * PiiMatrix::map() except for the *predicate* parameter. Returns
   * *matrix*.
   *
   * @param matrix input matrix, modified in place
   *
   * @param predicate a checker function that determines if *func*
   * will be applied to the current element
   *
   * @param func the operation to apply to matched elements
   *
   * ~~~(c++)
   * PiiMatrix<int> matInput(1, 3, -1, 2, 3);
   * // Negate all positive numbers
   * Pii::mapMatrixIf(matInput,
   *                  std::bind2nd(std::greater<int>(), 0),
   *                  std::negate<int>());
   * // matInput = -1 -2 -3
   * ~~~
   */
  template <class Matrix, class UnaryPredicate, class UnaryFunction>
  inline Matrix& mapMatrixIf(Matrix& matrix,
                             UnaryPredicate predicate,
                             UnaryFunction func)
  {
    mapIf(matrix.begin(), matrix.end(), predicate, func);
    return matrix;
  }

  /**
   * Applies the unary function *func* to all elements of *matrix*
   * that match the given *predicate*. Same as [mapMatrixIf()], but
   * returns a copy.
   */
  template <class Matrix, class UnaryPredicate, class UnaryFunction>
  inline PiiMatrix<typename UnaryFunction::result_type> mappedMatrixIf(const Matrix& matrix,
                                                                       UnaryPredicate predicate,
                                                                       UnaryFunction func)
  {
    PiiMatrix<typename UnaryFunction::result_type> result(matrix);
    mapMatrixIf(result, predicate, func);
    return result;
  }


  /**
   * Normalizes vectors in a matrix. Each row/column of the input
   * matrix is treated as a vector that will be normalized to unit
   * length.
   *
   * @param mat the input matrix. Modified in place.
   *
   * @param direction direction of calculation. By default rows are
   * treated as vectors. Use `Pii::Vertically` to change the
   * default behaviour.
   */
  template <class Matrix> void normalize(Matrix& mat,
                                         MatrixDirection direction = Horizontally);

  /**
   * Normalizes vectors in a matrix. Same as [normalize()] but returns
   * a copy.
   */
  template <class Matrix>
  PiiMatrix<double> normalized(const Matrix& mat,
                               MatrixDirection direction = Horizontally);

  /**
   *
   * @param storage an array/vector of `ints` with space for at least
   * `k` elements to store the current combination. The algorithm
   * will use `storage`'s operator[] to access the elements.
   */
  template <class UnaryFunction, class Storage> void combinations(int n, int k, UnaryFunction func, Storage storage);

  /**
   * Calculates all `k`-element subsets out of `n` elements and apply
   * `func` to each. There will be
   * \(\left(\stackrel{n}{k}\right)\) subsets in total. Since this
   * implementation uses `int`* as the storage type, the unary
   * operator must be a function or functor that accepts a `int`* as
   * input. The current combination is stored in the `k` first
   * elements in the array.
   *
   * ~~~(c++)
   * void function(const int* c)
   * {
   *   cout << "[" << c[0] << " " << c[1] << " " << c[2] << "]\n";
   * }
   *
   * // ...
   *
   * // Find all combinations of 2 values in {0,1,2}.
   * Pii::combinations(3, 2, function);
   *
   * // Prints:
   * // [ 1 2 ]
   * // [ 0 2 ]
   * // [ 0 1 ]
   * ~~~
   *
   * ~~~(c++)
   * PiiMatrix<int> combinations(0,2);
   * // Find all combinations of 2 elements among 5 elements. Store
   * // the combinations as rows into the combinations matrix.
   * Pii::RowAdder<int> adder(combinations);
   * Pii::combinations(5, 2, adder);
   * Pii::matlabPrint(std::cout, combinations);
   * // Prints:
   * // [ 3 4; ...
   * //   2 4; ...
   * //   1 4; ...
   * //   0 4; ...
   * //   2 3; ...
   * //   1 3; ...
   * //   0 3; ...
   * //   1 2; ...
   * //   0 2; ...
   * //   0 1 ];
   * ~~~
   *
   * ! Be sure not to pass arguments as copy if you intend to use
   * a reference:
   *
   * ~~~(c++)
   * struct Combinator
   * {
   *   Combinator() : counter(0) {}
   *   void operator() (int*) { ++counter; }
   *   void func()
   *     {
   *       Pii::combinations(3,2,*this);
   *       // Counter is 0 because *this is passed as a copy.
   *       // This forces the template to use a reference argument
   *       Pii::combinations<Combinator&>(3,2,*this);
   *       // Now, counter is 3.
   *     }
   *   int counter;
   * };
   * ~~~
   */
  template <class UnaryFunction> void combinations(int n, int k, UnaryFunction func)
  {
    int* storage = new int[k];
    combinations<UnaryFunction,int*>(n, k, func, storage);
    delete[] storage;
  }

  /**
   *
   * @param storage an array/vector with space for at least `k`
   * elements to store the current permutation. The algorithm will use
   * `storage`'s operator[] to access the elements. The array must be
   * initialized to 0,1,2,...,n-1.
   */
  template <class UnaryFunction, class Storage> void permutations(int n, UnaryFunction func, Storage storage);

  /**
   * Find all possible permutations of `n` elements. The number of
   * different permutations for `n` elements is n!. Works analogously
   * to combinations().
   *
   * ~~~(c++)
   * PiiMatrix<int> permutations(0,3);
   * Pii::permutations(3, Pii::RowAdder<int>(permutations));
   *
   * // permutations:
   * // 0 1 2
   * // 1 0 2
   * // 1 2 0
   * // 2 1 0
   * // 2 0 1
   * // 0 2 1
   * ~~~
   */
  template <class UnaryFunction> void permutations(int n, UnaryFunction func);

  /**
   * Returns the factorial of `n` (n!).
   * ! `n` doesn't need to be integer: for example
   * factorial(2.5) returns 2.5*1.5.
   */
  template <class T> T factorial(T n);

  /**
   * The way row/column start/ends are handled with [movingAverage()].
   *
   * - `OnlyValidPart` - nothing is assumed. Only the part of the
   * matrix that can be unambiguously averaged will be returned. The
   * size of the output matrix will be that of the input minus (window
   * width - 1).
   *
   * - `AssumeZeros` - assume that everything outside the input
   * matrix is zero. The size of the input will be retained.
   *
   * - `ShrinkWindow` - the size of the averaging window will
   * decrease towards the end points. At the end point, only half of
   * the window will be used. The size of the input matrix will be
   * retained.
   *
   * - `ShrinkWindowSymmetrically` - the size of the averaging window
   * will decrease towards the end points. The functioning is similar
   * to `ShrinkWindow` except that the window shrinks on both sides.
   * Both ends of the input signal will be retained as such. The value
   * next to the end point will be the average of the two border
   * values etc. The size of the input matrix will be retained. In
   * this mode, the `width` parameter needs to be odd. If it isn't,
   * it'll be made so. This mode is useful if you need to ensure that
   * elements on both sides of the current window center always have
   * equal effect.
   */
  enum EndPointHandling { OnlyValidPart, AssumeZeros, ShrinkWindow, ShrinkWindowSymmetrically };

  /**
   * Calculates the sum of neighbors in a sequence bounded by `begin`
   * and `end`. Modify the sequence in place. This function assumes
   * zeros outside of the iterator range.
   *
   * This function can be used to calculate not only the sum of
   * neighboring values but any function of the sum. A typical use is
   * to calculate a moving average.
   *
   * @param begin an iterator to the beginning of a range
   *
   * @param end an iterator to the end of the range
   *
   * @param width the width of the local window
   *
   * @param function apply this function to the sum before storing to
   * the sequence
   *
   * ! The data type used in calculating the local sum is the
   * `argument_type` of the given unary function object.
   *
   * ~~~(c++)
   * float data[5] = { 1, 2, 3, 4, 5 };
   *
   * // Calculate moving average over three neighbors
   * Pii::windowSum(data, data+5, 3, std::bind2nd(std::divides<float>(), 3));
   * // data = 1, 2, 3, 4, 3
   *
   * // Calculate the sum of two neighbors
   * Pii::windowSum(data, data+5, 2, Pii::Identity<float>());
   * // data = 3, 5, 7, 7, 3 (1+2 2+3 3+4 4+3 3+0)
   *
   * unsigned char data2[2] = { 128, 128 };
   * // This overflows due to limited range of data type.
   * Pii::windowSum(data2, data2+2, 2, std::bind2nd(std::divides<unsigned char>(), 2));
   * // data2 = 0, 64 ((128+128)/2 (128+0)/2)
   *
   * data2[0] = data2[1] = 128;
   * // Sum values as unsigned ints and cast the result to unsigned char
   * Pii::windowSum(data2, data2+2, 2, Pii::unaryCompose(Pii::Cast<unsigned int, unsigned char>(),
   *                                                     std::bind2nd(std::divides<unsigned int>(), 2)));
   * // data2 = 128, 64
   * ~~~
   *
   * @see movingAverage(Iterator,Iterator,int)
   */
  template <class Iterator, class UnaryFunction> void windowSum(Iterator begin, Iterator end, int width,
                                                                UnaryFunction function);

  /**
   * Calculates the moving average of a sequence bounded by `begin`
   * and `end`. This is a utility function that uses [windowSum()].
   * The `CalculationType` template parameter determines the
   * intermediate type used to calculate the sum.
   *
   * ~~~(c++)
   * unsigned char data[2] = { 128, 128 };
   * Pii::movingAverage<unsigned int>(data, data+2, 2);
   * // data = 126, 64
   * ~~~
   */
  template <class CalculationType, class Iterator> inline void movingAverage(Iterator begin, Iterator end, int width)
  {
    typedef typename std::iterator_traits<Iterator>::value_type T;
    windowSum(begin, end, width, unaryCompose(Cast<CalculationType, T>(),
                                              std::bind2nd(std::divides<CalculationType>(), width)));
  }

  /**
   * Calculates moving average over the rows/columns of a matrix.
   *
   * @param mat the input matrix
   *
   * @param width the width of the averaging window. Typically an odd
   * number, but can be even in all but `ShrinkWindowSymmetrically`
   * modes.
   *
   * @param direction direction of calculation
   *
   * @param endPointHandling the way row/column end points are handled.
   *
   * @return if `direction` is `Pii::Horizontally`, returns a matrix
   * with the same number of rows than the input. Otherwise returns a
   * matrix with the same number of columns. If `width` is too large,
   * an empty matrix will be returned.
   *
   * ~~~(c++)
   * PiiMatrix<int> input(4,4,
   *                      1,2,3,4,
   *                      3,4,5,6,
   *                      5,6,7,8,
   *                      7,8,9,10);
   *
   * PiiMatrix<double> avg = Pii::movingAverage<double>(input, 4,
   *                                                    Pii::Horizontally,
   *                                                    Pii::ShrinkWindow);
   * // Result:
   * // 1.5, 2.0, 2.5, 3.0
   * // 3.5, 4.0, 4.5, 5.0
   * // 5.5, 6.0, 6.5, 7.0
   * // 7.5, 8.0, 8.5, 9.0
   * ~~~
   */
  template <class T, class U> PiiMatrix<T> movingAverage(const PiiMatrix<U>& mat,
                                                         int width,
                                                         MatrixDirection direction = Horizontally,
                                                         EndPointHandling endPointHandling = OnlyValidPart);

  /**
   * Raises *value* to an integer power (*n*).
   */
  template <class T> T pow(T value, int n, typename OnlyNumeric<T>::Type = 0)
  {
    T tmp(value), result(1);
    int i = n;
    while (i != 0)
      {
        if (i & 1) result *= tmp;
        tmp *= tmp;
        i /= 2;
      }
    if (n >= 0)
      return result;
    return 1/result;
  }

  /**
   * Raises *value* to a floating-point power (*n*).
   */
  inline double pow(double value, double n)
  {
    return ::pow(value, n);
  }

  /**
   * Raises *value* to a floating-point power (*n*).
   */
  inline float pow(float value, float n)
  {
#ifdef __USE_ISOC99
    return ::powf(value, n);
#else
    return ::pow(value, n);
#endif
  }

  /**
   * Rounds *value* to the specified number of decimal places.
   */
  inline double roundToDecimals(double value, int decimals)
  {
    double dMultiplier = pow(10, decimals);
    return round(value * dMultiplier) / dMultiplier;
  }

  /**
   * Returns the sign of *value*.
   *
   * @return 1 for positive numbers, -1 for negative numbers, and 0
   * for zero.
   */
  template <class T> inline T sign(T value, typename OnlyNumeric<T>::Type = 0)
  {
    // PENDING: arithmetic shift for signed integers
    if (value > 0) return 1;
    else if (value < 0) return -1;
    return 0;
  }

  template <class T> inline T copySign(T value, T sign)
  {
    if (sign >= 0)
      return abs(value);
    else
      return -abs(value);
  }

#ifdef __USE_ISOC99
  inline double copySign(double value, double sign)
  {
    return ::copysign(value, sign);
  }

  inline float copySign(float value, float sign)
  {
    return ::copysignf(value, sign);
  }

  inline long double copySign(long double value, long double sign)
  {
    return ::copysignl(value, sign);
  }
#endif

  /**
   * Calculates the hypotenuse given two orthogonal sides of a
   * triangle.
   *
   * @return \(\sqrt{a^2 + b^2}\)
   */
  template <class T> inline T hypotenuse(T a, T b) { return T(sqrt(a*a + b*b)); }
  template <class T> inline T fastHypotenuse(T a, T b) { return T(fastSqrt(a*a + b*b)); }

  /**
   * Calculates the length of the third side of a triangle, given the
   * lengths of two adjacent sides and the angle between them.
   *
   * @param a the known length of side a
   *
   * @param b the known length of side b
   *
   * @param angle the known angle between sides a and b, in radians
   *
   * @return the length of the third side
   */
  double PII_CORE_EXPORT calculateThirdSide(double a, double b, double angle);

  /**
   * Calculates the angle opposite to side `a`, given the known
   * lengths of all sides of a triangle.
   *
   * @param a the known length of side a
   *
   * @param b the known length of side b
   *
   * @param c the known length of side c
   *
   * @return the angle opposite to `a`, in radians
   */
  double PII_CORE_EXPORT calculateOppositeAngle(double a, double b, double c);

  /**
   * Calculates the angle between sides `a` and `b`, given the known
   * lengths of the sides and the angle opposite to side `a`.
   *
   * @param a the known length of side a
   *
   * @param b the known length of side b
   *
   * @param angle the known angle opposite to a, in radians
   *
   * @return the angle between sides `a` and `b`, in radians
   */
  double PII_CORE_EXPORT calculateInterveningAngle(double a, double b, double angle);

  /**
   * Calculates the area of a triangle, given the lenghts of its sides.
   *
   * @param a the known length of side a
   *
   * @param b the known length of side b
   *
   * @param c the known length of side c
   *
   * @return the area of the triangle
   */
  double PII_CORE_EXPORT calculateArea(double a, double b, double c);

  /**
   * Convert an angle from degrees to radians.
   */
  inline double degToRad(double deg) { return deg/180*M_PI; }

  /**
   * Convert an angle from radians to degrees.
   */
  inline double radToDeg(double rad) { return rad/M_PI*180; }

  /**
   * Returns the difference between two angles (in radians). This
   * function always returns a value in [-pi, pi].
   */
  template <class T> inline T angleDiff(T angle1, T angle2, typename Pii::OnlyFloatingPoint<T>::Type = 0)
  {
    T diff = mod(angle1 - angle2, T(2*M_PI));
    if (diff > M_PI) diff -= 2*M_PI;
    else if (diff < -M_PI) diff += 2*M_PI;
    return diff;
  }

  /**
   * Fit a polynomial curve to data. This function fits an Nth order
   * polynomial function into the given data points. The solution is
   * optimal in least squares sense. Note that the result is optimal
   * arithmetically, not geometrically. Fitting a polynomial
   * geometrically requires non-linear optimization methods, for which
   * tools are provided in the PiiOptimization namespace. The
   * arithmetic fit calculated by this function can be used as an
   * initial guess for iterative optimization.
   *
   * @param order the order of the polynomial.
   *
   * @param x the x coordinates of measurements as row vectors or both
   * x and y coordinates as a N-by-2 matrix. The number of
   * measurements must be larger than `order`.
   *
   * @param y the y coordinates of measurements as row vectors, or an
   * empty matrix if `x` contains both coordinates.
   *
   * @return coefficients of the fitted polynomials as a M-by-N+1
   * matrix, where M is the number of rows in `x` and `y`, if both
   * are provided, or 1 if only `x` is given. N is the order of the
   * polynomial. The colum index in the result matrix corresponds to
   * the order of the polynomial.
   *
   * @exception PiiMathException& if the least squares problem turns
   * out to be singular.
   *
   * @exception PiiInvalidArgumentException& input arguments are
   * illegally formed.
   *
   * ~~~(c++)
   * // y = -2x^2 + 3.5x - 1
   * // y2 = -1.75x^2 + 3.25x - 1
   * PiiMatrix<double> coeffs = Pii::fitPolynomial(2,
   *                                               PiiMatrix<double>(2,3,
   *                                                                 0.0, 1.0, 2.0,
   *                                                                 0.0, 1.0, 2.0),
   *                                               PiiMatrix<double>(2,3,
   *                                                                 -1.0, 0.5, -2.0,
   *                                                                 -1.0, 0.5, -1.5));
   *
   * // coeffs = [-1.0 3.5  -2.0 ]
   * //          [-1.0 3.25 -1.75]
   *
   *
   * // y = 2.5x^3 + 2x^2 - 8x + 0.5
   * coeffs = Pii::fitPolynomial(3, PiiMatrix<double>(4,2,
   *                                                  -1.0, 8.0,
   *                                                  0.0, 0.5,
   *                                                  1.0, -3.0,
   *                                                  2.0, 12.5));
   * // coeffs = [0.5 -8.0 2.0 2.5]
   * ~~~
   *
   * ! The numerical accuracy of the algorithm can be improved by
   * normalizing x coordinates before calling this function:
   *
   * ~~~(c++)
   * PiiMatrix<double> x(1,100);
   * // ... fill x somehow ...
   * x = (x - Pii::mean<double>(x)) / Pii::std<double>(x);
   * // And remember to denormalize when you need the original X:
   * // x = mean + std * x;
   * ~~~
   */
  PII_CORE_EXPORT PiiMatrix<double> fitPolynomial(unsigned int order,
                                                  const PiiMatrix<double>& x,
                                                  const PiiMatrix<double>& y = PiiMatrix<double>());

  /**
   * Returns the value of the inverse normal cumulative distribution
   * function (a.k.a the error function, erf) at *p*. The absolute
   * value of the relative error is less than \(1.15 \times 10^{-9}\)
   * in the entire region.
   *
   * @param p a probability, in the range [0,1]
   *
   * @return erf(p). If p < 0 or p > 1, `NaN` will be returned. If p
   * == 0, `-Inf` will be returned. If p == 1, `Inf` will be returned.
   */
  double erf(double p);

  /**
   * Calculates the distance from `abs`(value) to the next real
   * number larger in magnitude. The return value can be seen as the
   * accuracy of the number type at the given magnitude. A crude
   * approximation of `ep3silon`(x) can be obtained with
   * `epsilon(1.0)*abs(x)`.
   *
   * @param value the value whose accuracy is to be measured. If value
   * == 1.0, returns `std::numeric_limits`<Real>::epsilon().
   */
  template <class Real> Real epsilon(Real value = 1.0);

  /**
   * Estimates the number of linearly independent rows or columns of a
   * matrix.
   *
   * @param mat the input matrix
   *
   * @param tolerance tolerance when comparing values to zero. The
   * default value is `epsilon`(0).
   */
  template <class Matrix> int rank(const Matrix& mat,
                                   typename Matrix::value_type tolerance = NAN);

  /**
   * Returns the squared geometric distance between two vectors.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(2, 2,
   *                    1, 2,
   *                    2, 3);
   * // Squared distance between (1, 2) and (2, 3)
   * int dist = Pii::squaredDistance(mat.rowBegin(0), matRowEnd(0),
   *                                 mat.rowBegin(1), 0);
   * // dist = 2
   * ~~~
   */
  template <class InputIterator1, class InputIterator2, class T>
  T squaredDistance(InputIterator1 begin1, InputIterator1 end1,
                    InputIterator2 begin2, T initialValue)
  {
    for (; begin1 != end1; ++begin1, ++begin2)
      initialValue += square(T(*begin1) - T(*begin2));
    return initialValue;
  }

  /**
   * Returns the squared distance between two *n*-dimensional vectors
   * *begin1* and *begin2*.
   */
  template <class InputIterator1, class InputIterator2, class T>
  T squaredDistanceN(InputIterator1 begin1, int n,
                     InputIterator2 begin2, T initialValue)
  {
    for (; n > 0; --n, ++begin1, ++begin2)
      initialValue += square(T(*begin1) - T(*begin2));
    return initialValue;
  }
}

/**
 * Matrix multiplication. Returns *mat1* * *mat2*.
 *
 * @exception PiiMathException& if matrix sizes don't match
 */
template <class Matrix1, class Matrix2>
PiiMatrix<PII_COMBINE_TYPES(typename Matrix1::value_type, typename Matrix2::value_type),
          Matrix1::staticRows, Matrix2::staticColumns>
operator* (const PiiConceptualMatrix<Matrix1>& mat1,
           const PiiConceptualMatrix<Matrix2>& mat2)
{
  const Matrix1& m1 = mat1.selfRef();
  const Matrix2& m2 = mat2.selfRef();
  const int iCols1 = m1.columns();
  if (iCols1 != m2.rows())
    PII_MATRIX_SIZE_MISMATCH;
  const int iRows1 = m1.rows(), iCols2 = m2.columns();

  typedef PII_COMBINE_TYPES(typename Matrix1::value_type, typename Matrix2::value_type) T;
  PiiMatrix<T, Matrix1::staticRows, Matrix2::staticColumns> result(PiiMatrix<T>::uninitialized(iRows1, iCols2));
  for (int r=0; r<iRows1; ++r)
    {
      T* pRow = result[r];
      for (int c=0; c<iCols2; ++c)
        pRow[c] = Pii::innerProductN(m1.rowBegin(r), iCols1, m2.columnBegin(c), T(0));
    }
  return result;
}

template <class T, class Matrix>
PiiMatrix<T>& operator*= (PiiMatrix<T>& mat1,
                          const PiiConceptualMatrix<Matrix>& mat2)
{
  return mat1 = mat1 * mat2.selfRef();
}

/**
 * Perform a matrix division operation.
 *
 * @exception PiiMathException if the dimensions do not match, `mat2`
 * is not square or cannot be inverted due to singularity
 */
template <class T> inline PiiMatrix<T> operator/ (const PiiMatrix<T>& mat1, const PiiMatrix<T>& mat2)
{
  return mat1 * Pii::inverse(mat2);
}

/**
 * Divide a matrix by another matrix in place.
 *
 * @exception PiiMathException& if the dimensions of the matrices do
 * not match, `mat2` is not square or it cannot be inverted due to
 * singularity.
 */
template <class T> inline PiiMatrix<T>& operator/= (PiiMatrix<T>& mat1, const PiiMatrix<T>& mat2)
{
  return mat1 *= Pii::inverse(mat2);
}


#include "PiiMath-templates.h"
#include "PiiMathFunctional.h"

/// @hide
#define PII_MATH_MATRIX_TRANSFORM(FUNCTION, CLASS) \
  template <class Matrix> inline PiiUnaryMatrixTransform<Matrix, CLASS<typename Matrix::value_type> > \
  FUNCTION(const PiiConceptualMatrix<Matrix>& mat) \
  { return unaryMatrixTransform(mat.selfRef(), CLASS<typename Matrix::value_type>()); } namespace PiiDummy {}
/// @endhide

// Functions that depend on math functionals
namespace Pii
{
  /**
   * Returns a matrix whose elements are absolute values of *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(abs, Abs);

  /**
   * Returns a matrix whose elements are absolute values of *mat*
   * squared.
   */
  PII_MATH_MATRIX_TRANSFORM(abs2, Abs2);

  /// @internal
  template <class Matrix, class Function> struct ComplexTransform
  {
    typedef typename Matrix::value_type T;
    typedef typename IfClass<IsComplex<T>,
                             PiiUnaryMatrixTransform<Matrix, Function>,
                             Matrix>::Type ResultType;
    struct ComplexValue
    {
      static PiiUnaryMatrixTransform<Matrix, Function> apply(const Matrix& mat)
      {
        return unaryMatrixTransform(mat, Function());
      }
    };
    struct RealValue
    {
      static Matrix apply(const Matrix& mat)
      {
        return mat;
      }
    };

    static ResultType apply(const Matrix& mat)
    {
      return IfClass<IsComplex<T>, ComplexValue, RealValue>::Type::apply(mat);
    }
  };

  /**
   * Returns a matrix whose elements are the real parts of the
   * corresponding elements in *mat*. Real matrices will be returned
   * unchanged.
   */
  template <class Matrix>
  inline typename ComplexTransform<Matrix, Real<typename Matrix::value_type> >::ResultType
  real(const PiiConceptualMatrix<Matrix>& mat)
  {
    return ComplexTransform<Matrix, Real<typename Matrix::value_type> >::apply(mat.selfRef());
  }

  /**
   * Returns a matrix whose elements are complex conjugates of the
   * corresponding elements in *mat*. Real matrices will be returned
   * unchanged.
   */
  template <class Matrix>
  inline typename ComplexTransform<Matrix, Conj<typename Matrix::value_type> >::ResultType
  conj(const PiiConceptualMatrix<Matrix>& mat)
  {
    return ComplexTransform<Matrix, Conj<typename Matrix::value_type> >::apply(mat.selfRef());
  }

  /**
   * Returns a matrix whose elements are the imaginary parts of the
   * corresponding elements in *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(imag, Imag);


  /**
   * Returns the natural logarithm of all elements in *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(log, Log);

  /**
   * Returns the base 2 logarithm of all elements in *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(log2, Log2);

  /**
   * Returns the base 10 logarithm of all elements in *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(log10, Log10);

  /**
   * Returns the base *e* exponential of all elements in *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(exp, Exp);

  /**
   * Rounds all values in a matrix to the closest integer. Returns a
   * new matrix that has the same type as the input.
   *
   * ~~~(c++)
   * PiiMatrix<double> a(2, 2,
   *                     1.1, 2.1,
   *                     3.5, 4.9);
   * PiiMatrix<double> b = Pii::round(a);
   * // b = 1.0 2.0
   * //     4.0 5.0
   * ~~~
   */
  PII_MATH_MATRIX_TRANSFORM(round, Round);

  /**
   * Rounds all values in a matrix to the closest integer. Returns a
   * new matrix with a type given as the first template parameter.
   *
   * ~~~(c++)
   * PiiMatrix<double> a(2, 2,
   *                     1.1, 2.1,
   *                     3.5, 4.9);
   * PiiMatrix<int> b = Pii::round<int>(a);
   * // b = 1 2
   * //     4 5
   * ~~~
   */
  template <class T, class Matrix>
  inline PiiUnaryMatrixTransform<Matrix, Round<typename Matrix::value_type, T> >
  round(const PiiConceptualMatrix<Matrix>& mat)
  {
    return unaryMatrixTransform(mat.selfRef(), Round<typename Matrix::value_type, T>());
  }

  /**
   * Calculates the `ceil`() of all elements in *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(ceil, Ceil);

  /**
   * Calculates the `floor`() of all elements in *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(floor, Floor);

  /**
   * Calculates the sign of all elements in *mat*.
   */
  PII_MATH_MATRIX_TRANSFORM(sign, Sign);

  /**
   * Checks whether given values are "almost equal" in relativistic
   * sense, i.e. whether the difference is small enough given the
   * magnitude of the compared values.
   */
  template <class T> inline bool almostEqualRel(const T& a, const T& b,
                                                const typename Abs<T>::result_type& tol = Numeric<T>::tolerance())
  {
    const typename Abs<T>::result_type minVal = min<typename Abs<T>::result_type>(std::abs(a), std::abs(b));
    if(minVal != 0)
      return (std::abs(a-b) <= tol * minVal);
    else
      return (std::abs(a-b) <= tol);
  }

  /**
   * Like [max()], but finds the maximum absolute value.
   */
  template <class Matrix> inline typename Abs<typename Matrix::value_type>::result_type
  maxAbs(const Matrix& mat, int* r, int* c)
  {
    typedef typename Matrix::value_type T;
    return findSpecialValue(mat, std::greater<typename Abs<T>::result_type>(), Abs<T>(), typename Abs<T>::result_type(0), r, c);
  }

  /**
   * Like [max()], but finds the maximum absolute value.
   */
  template <class Collection> inline typename Abs<typename Collection::value_type>::result_type
  maxAbs(const Collection& collection)
  {
    typedef typename Collection::value_type T;
    typename Collection::const_iterator i = findSpecialValue(collection.begin(), collection.end(),
                                                             std::greater<typename Abs<T>::result_type>(),
                                                             Abs<T>());
    if (i != collection.end())
      return Pii::abs(*i);
    return 0;
  }

  /**
   * Calculates the maximum of all elements in a matrix.
   *
   * @param mat the source matrix
   *
   * @param r store the row of the maximum value to this memory
   * location
   *
   * @param c store the column of the maximum value to this memory
   * location
   */
  template <class Matrix> inline typename Matrix::value_type max(const Matrix& mat, int* r, int* c)
  {
    typedef typename Matrix::value_type T;
    return findSpecialValue(mat, std::greater<T>(), Identity<T>(), Numeric<T>::minValue(), r, c);
  }

  /**
   * Calculates the minimum of all elements in a matrix.
   *
   * @param mat the source matrix
   *
   * @param r store the row of the minimum value to this memory
   * location
   *
   * @param c store the column of the minimum value to this memory
   * location
   */
  template <class Matrix> inline typename Matrix::value_type min(const Matrix& mat, int* r, int* c)
  {
    typedef typename Matrix::value_type T;
    return findSpecialValue(mat, std::less<T>(), Identity<T>(), Numeric<T>::maxValue(), r, c);
  }

  /**
   * Returns the maximum value in [*begin*, *end*).
   */
  template <class InputIterator>
  inline typename std::iterator_traits<InputIterator>::value_type maxIn(InputIterator begin,
                                                                        InputIterator end)
  {
    typedef typename std::iterator_traits<InputIterator>::value_type T;
    InputIterator i = findSpecialValue(begin, end, std::greater<T>(), Identity<T>());
    if (i != end)
      return *i;
    return Numeric<T>::minValue();
  }

  /**
   * Shorthand for maxIn(collection.begin(), collection.end()).
   */
  template <class Collection> inline typename Collection::value_type max(const Collection& collection)
  {
    return maxIn(collection.begin(), collection.end());
  }

  /**
   * Returns the minimum value in [*begin*, *end*).
   */
  template <class InputIterator>
  inline typename std::iterator_traits<InputIterator>::value_type minIn(InputIterator begin,
                                                                        InputIterator end)
  {
    typedef typename std::iterator_traits<InputIterator>::value_type T;
    InputIterator i = findSpecialValue(begin, end, std::less<T>(), Identity<T>());
    if (i != end)
      return *i;
    return Numeric<T>::maxValue();
  }

  /**
   * Shorthand for minIn(collection.begin(), collection.end()).
   */
  template <class Collection> inline typename Collection::value_type min(const Collection& collection)
  {
    return minIn(collection.begin(), collection.end());
  }


  /**
   * Raises all values in a matrix to nth (integer) power.
   */
  template <class Matrix>
  inline PiiUnaryMatrixTransform<Matrix, std::binder2nd<Pow<typename Matrix::value_type,int> > >
  pow(const PiiConceptualMatrix<Matrix>& matrix, int n)
  {
    return unaryMatrixTransform(matrix.selfRef(), std::bind2nd(Pow<typename Matrix::value_type,int>(), n));
  }

  /**
   * Raises all values in a matrix to nth (double) power.
   */
  template <class Matrix>
  inline PiiUnaryMatrixTransform<Matrix, std::binder2nd<Pow<typename Matrix::value_type,double> > >
  pow(const PiiConceptualMatrix<Matrix>& matrix, double n)
  {
    return unaryMatrixTransform(matrix.selfRef(), std::bind2nd(Pow<typename Matrix::value_type,double>(), n));
  }

  /**
   * Calculates the L1 norm of all elements in a matrix. The L1 norm is
   * defined as \( L_1 = \sum_{i=1}^N |a_i| \), where \(a_i\)
   * denote the elements of `mat` and N is the total number of
   * entries.
   *
   * ! Sum is not calculated with double but with the datatype
   * given by the absolute value functor, which in most cases is
   * the same as the matrix element type. Possible overflow
   * issue.
   */
  template <class Matrix> inline double norm1(const Matrix& mat)
  {
    typedef typename Matrix::value_type T;
    return forEach(mat.begin(), mat.end(),
                   createForEachFunction(Abs<T>(), std::plus<typename Abs<T>::result_type>(), 0))();
    //return sum<T>(abs(mat));
  }

  /// @internal
  template <class Real> struct SvdNorm
  {
    static double calculate(const PiiMatrix<Real>& matrix)
    {
      PiiMatrix<Real> singularValues;
      svd(matrix, static_cast<PiiMatrix<Real>*>(0), &singularValues, static_cast<PiiMatrix<Real>*>(0));
      return singularValues(0,0);
    }
  };

  /// @internal
  template <class Matrix> struct DefaultNorm
  {
    static double calculate(const Matrix& matrix)
    {
      return sqrt(forEach(matrix.begin(), matrix.end(),
                          createForEachFunction(Square<typename Matrix::value_type>(),
                                                std::plus<double>(), 0))());
    }
  };

  /**
   * Calculates the L2 norm of all elements in a matrix. For
   * floating-point types, the norm is calculated by taking the
   * largest singular value of `mat`. For other types, the square
   * root of the sum of squares will be returned.
   *
   * PENDING SVD isn't stable enough yet. The function now uses the
   * integer version for all input types.
   */
  template <class Matrix> inline double norm2(const Matrix& mat)
  {
    //typedef typename IfClass<IsReal<T>, SvdNorm<T>, DefaultNorm<T> >::Type NormCalculator;
    typedef DefaultNorm<Matrix> NormCalculator;
    return NormCalculator::calculate(mat);
  }

  /**
   * Calculates the nth norm of all elements in the matrix. The nth
   * norm of \(\{a_i|i=1 \ldots N\}\) is defined as \(L_n =
   * (\sum_{i=1}^N |a_i|^n)^{1/n}\).
   *
   * ! Remember possibility of overflows. Powers are calculated with
   * the datatype of the input matrix, not with `double`, so an
   * overflow may occur with relatively small values; for example
   * 1500^3 already overflows an `int` on most hardware. Usually, it
   * is not a good idea to calculate the norm with integer types if
   * you cannot ensure a proper range of input values.
   *
   * @param mat the input matrix
   *
   * @param n the norm order. Norms -1 and 2 get special treatment. If
   * *n* == -1, the \(L_\infty\) norm will be returned. If *n* == 2,
   * the function returns the largest singular value of *mat*.
   *
   * @return the `nth` norm of the elements in `mat`
   */
  template <class Matrix> double norm(const Matrix& mat, int n = 2,
                                      typename Pii::OnlyNonComplex<typename Matrix::value_type>::Type = 0)
  {
    typedef typename Matrix::value_type T;
    if (n == 1)
      return norm1(mat);
    else if (n == 2)
      return norm2(mat);
    else if (n == -1)
      return maxAbs(mat);
    else if ((n & 1) == 0) // even exponent -> no need to take absolute value
      return pow(forEach(mat.begin(), mat.end(),
                         createForEachFunction(std::bind2nd(Pow<T>(), n), std::plus<double>(), 0))(), 1.0/n);
    else
      return pow(forEach(mat.begin(), mat.end(),
                         createForEachFunction(std::bind2nd(AbsPow<T>(), n), std::plus<double>(), 0))(), 1.0/n);
  }

  /**
   * Specialization of [norm()] for complex type.
   */
  template <class Matrix> double norm(const Matrix& mat, int n = 2,
                                      typename Pii::OnlyComplex<typename Matrix::value_type>::Type = 0)
  {
    typedef typename Matrix::value_type::value_type T;
    if (n == 1)
      return norm1(mat);
    else if (n == 2)
    {
      return sqrt(forEach(mat.begin(), mat.end(),
                          createForEachFunction(unaryCompose(Square<T>(), Abs<std::complex<T> >()), std::plus<T>(), 0))()) ;
    }
    else if (n == -1)
      return maxAbs(mat);
    else
      return pow(forEach(mat.begin(), mat.end(),
                         createForEachFunction(std::bind2nd(AbsPow<std::complex<T> >(), n), std::plus<T>(), 0))() , T(1.0)/n );
  }

  /**
   * Calculates the standard deviation of matrix elements in the
   * specified direction. See [sum()] for more information.
   *
   * @param mat the source matrix
   *
   * @param direction calculate standard deviation in this
   * direction
   */
  template <class U, class Matrix> PiiMatrix<U> std(const Matrix& mat, MatrixDirection direction)
  {
    PiiMatrix<U> var(var<U>(mat.selfRef(), direction));
    return var.map(Sqrt<U>());
  }

  /**
   * Calculates the square of all elements in a matrix.
   */
  PII_MATH_MATRIX_TRANSFORM(square, Square);

  /**
   * Calculates the square root of all elements in a matrix.
   */
  PII_MATH_MATRIX_TRANSFORM(sqrt, Sqrt);
  /**
   * Calculates the square root of all elements in a matrix using a
   * fast approximation.
   */
  template <class Matrix>
  inline PiiUnaryMatrixTransform<Matrix, FastSqrt>
  fastSqrt(const PiiConceptualMatrix<Matrix>& mat)
  {
    return unaryMatrixTransform(mat.selfRef(), FastSqrt());
  }

  /**
   * Calculates the sine of all elements in a matrix.
   */
  PII_MATH_MATRIX_TRANSFORM(sin, Sin);

  /**
   * Calculates the arcus sine of all elements in a matrix.
   */
  PII_MATH_MATRIX_TRANSFORM(asin, Asin);

  /**
   * Calculates the cosine of all elements in a matrix.
   */
  PII_MATH_MATRIX_TRANSFORM(cos, Cos);

  /**
   * Calculates the arcus cosine of all elements in a matrix.
   */
  PII_MATH_MATRIX_TRANSFORM(acos, Acos);

  /**
   * Calculates the tangent of all elements in a matrix.
   */
  PII_MATH_MATRIX_TRANSFORM(tan, Tan);

  /**
   * Calculates the arcus tangent of all elements in a matrix.
   */
  PII_MATH_MATRIX_TRANSFORM(atan, Atan);

  /**
   * Returns the squared geometric distance between points stored as
   * matrix rows or columns.
   *
   * @param points1 a set of N-dimensional points stored as either row
   * or column vectors
   *
   * @param points2 another set of N-dimensional points stored as
   * either row or column vectors. Size must match `points1`.
   *
   * @param direction the direction of the vectors.
   *
   * @return a matrix that stores the distances between corresponding
   * row/column vectors. If `direction` is `Pii::Horizontally`, a
   * column vector will be returned and vice versa. If the sizes of
   * `points1` and `points2` do not match, an empty matrix will be
   * returned.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat1(2,2), mat2(2,2);
   * PiiMatrix<double> matDist = Pii::squaredDistance(mat1, mat2, Pii::Horizontally);
   * ~~~
   */
  template <class Matrix1, class Matrix2, class T>
  PiiMatrix<double> squaredDistance(const Matrix1& points1,
                                    const Matrix2& points2,
                                    MatrixDirection direction);

  /**
   * Returns the geometric distance between points stored as matrix
   * rows or columns. Analogous to [squaredDistance()], but applies
   * square root to the result.
   */
  template <class Matrix1, class Matrix2>
  inline PiiMatrix<double> distance(const Matrix1& points1,
                                    const Matrix2& points2,
                                    MatrixDirection direction)
  {
    return squaredDistance(points1, points2, direction).map(Sqrt<double>());
  }

  /**
   * Returns the geometric distance between two vectors. Uses
   * [squaredDistance()].
   */
  template <class InputIterator1, class InputIterator2, class T>
  inline typename Sqrt<T>::result_type distance(InputIterator1 begin1, InputIterator1 end1,
                                                InputIterator2 begin2, T initialValue)
  {
    return sqrt(squaredDistance(begin1, end1, begin2, initialValue));
  }

  /**
   *
   * Returns the geometric distance between two *n*-dimensional
   * vectors.
   */
  template <class InputIterator1, class InputIterator2, class T>
  inline typename Sqrt<T>::result_type distanceN(InputIterator1 begin1, int n,
                                                 InputIterator2 begin2, T initialValue)
  {
    return sqrt(squaredDistanceN(begin1, n, begin2, initialValue));
  }

  /**
   * Returns a matrix that contains the minimum of corresponding
   * elements in `a` and `b`.
   *
   * @exception PiiMathException& if matrices are not of the same
   * size.
   */
  template <class Matrix1, class Matrix2>
  inline PiiBinaryMatrixTransform<Matrix1, Matrix2, Min<typename Matrix1::value_type> >
  min(const PiiConceptualMatrix<Matrix1>& m1,
      const PiiConceptualMatrix<Matrix2>& m2)
  {
    return binaryMatrixTransform(m1.selfRef(), m2.selfRef(), Min<typename Matrix1::value_type>());
  }

  /**
   * Returns a matrix that contains the maximum of corresponding
   * elements in `a` and `b`.
   *
   * @exception PiiMathException& if matrices are not of the same
   * size.
   */
  template <class Matrix1, class Matrix2>
  inline PiiBinaryMatrixTransform<Matrix1, Matrix2, Max<typename Matrix1::value_type> >
  max(const PiiConceptualMatrix<Matrix1>& m1,
      const PiiConceptualMatrix<Matrix2>& m2)
  {
    return binaryMatrixTransform(m1.selfRef(), m2.selfRef(), Max<typename Matrix1::value_type>());
  }

  /**
   * Returns a matrix that contains either the original element in `a`
   * or `b`, whichever is smaller.
   */
  template <class Matrix>
  inline PiiUnaryMatrixTransform<Matrix, std::binder2nd<Min<typename Matrix::value_type> > >
  min(const PiiConceptualMatrix<Matrix>& matrix,
      typename Matrix::value_type value)
  {
    return unaryMatrixTransform(matrix.selfRef(), std::bind2nd(Min<typename Matrix::value_type>(), value));
  }

  /**
   * Returns a matrix that contains either the original element in `a`
   * or `b`, whichever is larger.
   */
  template <class Matrix>
  inline PiiUnaryMatrixTransform<Matrix, std::binder2nd<Max<typename Matrix::value_type> > >
  max(const PiiConceptualMatrix<Matrix>& matrix,
      typename Matrix::value_type value)
  {
    return unaryMatrixTransform(matrix.selfRef(), std::bind2nd(Max<typename Matrix::value_type>(), value));
  }

  /**
   * Find a special value such as maximum or minimum in a matrix. The
   * "speciality" is defined by the given function objects.
   *
   * @param mat PiiMatrix from which to search the value.
   *
   * @param op a binary predicate that compares values. Returns
   * `true` if current item return by `f` is "more special" than
   * currently "most special" value, `false` otherwise.
   *
   * @param f a unary function applied to the values before
   * comparison.
   *
   * @param defaultValue default value for the searched property. This
   * value will be returned if the matrix is empty.
   *
   * @param rowIndex a pointer to an `int` where the row index of the
   * special value will placed. Can be zero.
   *
   * @param colIndex a pointer to an `int` where the column index of
   * the special value will placed. Can be zero.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat; // initialize as you wish
   * // Find the maximum absolute value
   * int iMaxAbs = findSpecialValue(mat, std::greater<int>(), Abs<int>(), 0);
   * // Find the minimum value
   * int iMin = findSpecialValue(mat, std::less<int>(), Pii::Identity<int>(), Pii::Numeric<int>::maxValue());
   * ~~~
   */
  template <class Matrix, class UnaryFunction, class BinaryPredicate>
  typename UnaryFunction::result_type findSpecialValue(const Matrix& mat,
                                                       BinaryPredicate op,
                                                       UnaryFunction f,
                                                       typename UnaryFunction::result_type defaultValue,
                                                       int* rowIndex = 0,
                                                       int* colIndex = 0)
  {
    typename Matrix::const_iterator i = findSpecialValue(mat.begin(), mat.end(), op, f);
    if (i != mat.end())
      {
        if (rowIndex != 0) *rowIndex = i.row();
        if (colIndex != 0) *colIndex = i.column();
        return f(*i);
      }
    return defaultValue;
  }

  /// @hide
  template <class Matrix> struct ComplexAdjoint
  {
    typedef typename Matrix::value_type T;
    typedef PiiUnaryMatrixTransform<PiiTransposedMatrix<Matrix>, Conj<T> > Type;

    static Type apply(const Matrix& mat)
    {
      return unaryMatrixTransform(transpose(mat), Conj<typename Matrix::value_type>());
    }

    static void swap(T& a, T& b)
    {
      T tmp = conj(a);
      a = conj(b);
      b = tmp;
    }
  };

  template <class Matrix> struct RealAdjoint
  {
    typedef typename Matrix::value_type T;
    typedef PiiTransposedMatrix<Matrix> Type;

    static Type apply(const Matrix& mat)
    {
      return transpose(mat);
    }

    static void swap(T& a, T& b)
    {
      qSwap(a,b);
    }
  };

  template <class Matrix> struct AdjointTraits :
    IfClass<IsComplex<typename Matrix::value_type>,
            ComplexAdjoint<Matrix>, RealAdjoint<Matrix> >::Type {};

  /// @endhide

  /**
   * Returns the conjugate transpose of *mat*. The conjugate
   * transpose, Hermitian transpose, Hermitian conjugate, or adjoint
   * matrix of an m-by-n matrix A with complex entries is the n-by-m
   * matrix A* obtained from A by taking the transpose and then taking
   * the complex conjugate of each entry. If the input matrix is real,
   * this function is equivalent to transpose().
   */
  template <class Matrix>
  inline typename AdjointTraits<Matrix>::Type adjoint(const Matrix& mat)
  {
    return AdjointTraits<Matrix>::apply(mat);
  }

  /**
   * Transposes the square matrix *mat* in place by swapping elements
   * at opposite sides of the diagonal.
   */
  template <class Matrix>
  inline void inplaceTranspose(Matrix& mat)
  {
    const int iSize = qMin(mat.rows(), mat.columns());
    for (int i=0; i<iSize; ++i)
      {
        typename Matrix::row_iterator row = mat.rowBegin(i);
        typename Matrix::column_iterator column = mat.columnBegin(i);
        for (int j=i+1; j<iSize; ++j)
          qSwap(row[j], column[j]);
      }
  }

  /**
   * Transposes the square matrix *mat* in place and replaces each
   * element with its complex conjugate.
   */
  template <class Matrix>
  inline void inplaceAdjoint(Matrix& mat)
  {
    typedef AdjointTraits<Matrix> Traits;
    const int iSize = qMin(mat.rows(), mat.columns());
    for (int i=0; i<iSize; ++i)
      {
        typename Matrix::row_iterator row = mat.rowBegin(i);
        typename Matrix::column_iterator column = mat.columnBegin(i);
        for (int j=i+1; j<iSize; ++j)
          Traits::swap(row[j], column[j]);
      }
  }
  /// @endgroup
} // namespace Pii

#endif //_PIIMATH_H
