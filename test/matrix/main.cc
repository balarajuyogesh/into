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

#include <QtTest>
#include <PiiMath.h>
#include "TestPiiMatrix.h"
#include <PiiMatrixUtil.h>
#include <QtDebug>
#include <typeinfo>
#include <iostream>

using std::cout;
using std::endl;

template <class T> void showMatrix(const PiiMatrix<T>& mat)
{
  for (int r=0; r<mat.rows(); r++)
    {
      for (int c=0; c<mat.columns(); c++)
        cout << (T)(std::abs(mat(r,c)) > 1e-15 ? mat(r,c) : 0) << " ";
      cout << endl;
    }
}

void TestPiiMatrix::constructors()
{
  {
    PiiMatrix<int> mat;
    QVERIFY(mat.isEmpty());
    QCOMPARE(mat.columns(), 0);
    QCOMPARE(mat.rows(), 0);
  }
  {
    PiiMatrix<int> mat1(5,4);
    QCOMPARE(mat1.stride(), 4*sizeof(int));
    QVERIFY(!mat1.isEmpty());
    QCOMPARE(mat1.rows(), 5);
    QCOMPARE(mat1.columns(), 4);
    for (PiiMatrix<int>::iterator i = mat1.begin(); i != mat1.end(); ++i)
      QCOMPARE(*i, 0);
    PiiMatrix<int> mat2(mat1);
    QCOMPARE(mat2.rows(), 5);
    QCOMPARE(mat2.columns(), 4);
    mat2(0,0) = 1;
    QCOMPARE(mat2(0,0), 1);
    QCOMPARE(mat1(0,0), 0);
  }
  {
    PiiMatrix<char> mat1(3, 3);
    QCOMPARE(mat1.stride(), size_t(4));
    PiiMatrix<char> mat2(1, 7);
    QCOMPARE(mat2.stride(), size_t(8));
  }
  {
    PiiMatrix<int> mat1(3,3);
    mat1 = 5;
    PiiMatrix<double> mat2(mat1);
    QCOMPARE(mat2.rows(), 3);
    QCOMPARE(mat2.columns(), 3);
    QCOMPARE(mat2(2,2), 5.0);
  }
  {
    int values[] = { 1, 2, 3, 4, 5, 6 };
    PiiMatrix<int> mat1(1, 6, values);
    QCOMPARE(mat1(0,5), 6);
    mat1(0,2) = 1;
    QCOMPARE(mat1(0,2), 1);
    QCOMPARE(values[2], 3);
    PiiMatrix<int> mat2(2, 3, static_cast<void*>(values), Pii::RetainOwnership);
    QCOMPARE(mat2(0,5), 6);
    mat2(0,2) = 1;
    QCOMPARE(values[2], 1);
    PiiMatrix<int> mat3(2, 2, static_cast<void*>(values), Pii::RetainOwnership, 3*sizeof(int));
    QCOMPARE(mat3.stride(), 3*sizeof(int));
    QCOMPARE(mat3(1,1), 5);
  }
  {
    void* data = malloc(sizeof(double) * 8);
    PiiMatrix<double> mat(2, 4, data, Pii::ReleaseOwnership);
    mat = 1.0;
    PiiMatrix<double> mat2(2, 4, data, Pii::RetainOwnership);
    QCOMPARE(mat2(1,3), 1.0);
  }
  {
    // Leave padding to the right
    PiiMatrix<float> mat(PiiMatrix<float>::padded(2, 2, 4*sizeof(float)));
    QCOMPARE(mat(0,1), 0.0f);
    mat(1,3) = 0; // out of bounds on purpose
    QCOMPARE(mat(1,3), 0.0f);
  }
  {
    PiiMatrix<int> mat1(2,3,
                        0,1,2,
                        3,4,5);
    QCOMPARE(mat1(1,2), 5);
    PiiMatrix<bool> mat2(2,2,
                         0,1,
                         1,0);
    QCOMPARE(mat2(0,0), false);
    QCOMPARE(mat2(1,0), true);
    PiiMatrix<float> mat3(5,1, 0.0, 0.1, 0.2, 0.3, 0.4);
    QCOMPARE(mat3(4,0), 0.4f);
  }
  {
    PiiMatrix<float> mat1(2,2, 1.0, 2.0, 3.0, 4.0);
    PiiMatrix<char> mat2(2,2, 'a', 'b', 'c', 'd');
    PiiMatrix<unsigned char> mat3(2,2, 'a', 'b', 'c', 'd');
    PiiMatrix<bool> mat4(2,2, true, false, false, true);

    QCOMPARE(mat1(0,0), 1.0f);
    QCOMPARE(mat1(0,1), 2.0f);
    QCOMPARE(mat1(1,0), 3.0f);
    QCOMPARE(mat1(1,1), 4.0f);
    QCOMPARE(mat2(0,0), 'a');
    QCOMPARE(mat2(0,1), 'b');
    QCOMPARE(mat2(1,0), 'c');
    QCOMPARE(mat2(1,1), 'd');
    QCOMPARE(mat3(0,0), (unsigned char)'a');
    QCOMPARE(mat3(0,1), (unsigned char)'b');
    QCOMPARE(mat3(1,0), (unsigned char)'c');
    QCOMPARE(mat3(1,1), (unsigned char)'d');
    QCOMPARE(mat4(0,0), true);
    QCOMPARE(mat4(0,1), false);
    QCOMPARE(mat4(1,0), false);
    QCOMPARE(mat4(1,1), true);
  }
  {
    PiiMatrix<int> empty, another;
    empty = another;
    empty += 1;
    QVERIFY(empty.isEmpty());
    QVERIFY(another.isEmpty());
  }

  // Fixed size cases
  {
    PiiMatrix<float,2,2> mat1(1.0, 2.0, 3.0, 4.0);
    PiiMatrix<char,2,2> mat2('a', 'b', 'c', 'd');
    PiiMatrix<unsigned char,2,2> mat3('a', 'b', 'c', 'd');
    PiiMatrix<bool,2,2> mat4(true, false, false, true);

    QCOMPARE(mat1(0,0), 1.0f);
    QCOMPARE(mat1(0,1), 2.0f);
    QCOMPARE(mat1(1,0), 3.0f);
    QCOMPARE(mat1(1,1), 4.0f);
    QCOMPARE(mat2(0,0), 'a');
    QCOMPARE(mat2(0,1), 'b');
    QCOMPARE(mat2(1,0), 'c');
    QCOMPARE(mat2(1,1), 'd');
    QCOMPARE(mat3(0,0), (unsigned char)'a');
    QCOMPARE(mat3(0,1), (unsigned char)'b');
    QCOMPARE(mat3(1,0), (unsigned char)'c');
    QCOMPARE(mat3(1,1), (unsigned char)'d');
    QCOMPARE(mat4(0,0), true);
    QCOMPARE(mat4(0,1), false);
    QCOMPARE(mat4(1,0), false);
    QCOMPARE(mat4(1,1), true);
  }
  {
    PiiMatrix<int,0,0> empty, another;
    empty = another;
    empty += 1;
    QVERIFY(empty.isEmpty());
    QVERIFY(another.isEmpty());
  }
}

template <class Matrix> void TestPiiMatrix::setTo(Matrix& matrix,
                                                  typename Matrix::value_type value)
{
  matrix = value;
}

template <class Matrix> void TestPiiMatrix::setSubmatrix(Matrix& matrix,
                                                         typename Matrix::value_type value)
{
  setTo(matrix(0,0,-1,-1).selfRef(), value);
}

void TestPiiMatrix::submatrix()
{
  {
    PiiMatrix<int> mat(3,3, 1,2,3,4,5,6,7,8,9);
    PiiMatrix<int> sub1(mat(0,1,3,2));
    QVERIFY(Pii::equals(sub1, PiiMatrix<int>(3,2, 2,3, 5,6, 8,9)));
    QVERIFY(Pii::equals(PiiMatrix<int>(sub1(0,0,2,1)), PiiMatrix<int>(2,1,2,5)));
    // Should not modify original matrix
    sub1(0,0,2,1) += 2;
    QCOMPARE(mat(0,2), 3);
    mat(2,0,-1,-1) *= 2;
    QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,3, 1,2,3,4,5,6,14,16,18)));
    mat(-1, -3, 1, 3) /= 2;
    QVERIFY(Pii::equals(mat(-1, 0, 1, 3), PiiMatrix<int>(1,3, 7,8,9)));
  }

  {
    PiiMatrix<char> mat(8,8);
    mat = '+';
    mat(2,1,3,3) = 'a';
    mat(1,5,6,3) = 'b';
    mat(2,6,4,1) = 'c';
    mat(6,0,2,1) = 'x';
    //showMatrix(mat);

    const char* data =
      "++++++++"
      "+++++bbb"
      "+aaa+bcb"
      "+aaa+bcb"
      "+aaa+bcb"
      "+++++bcb"
      "x++++bbb"
      "x+++++++";
    QVERIFY(Pii::equals(mat, PiiMatrix<char>(8, 8, data)));
  }

  {
    PiiMatrix<double> mat(5,5);
    setTo(mat(1,1,3,3).selfRef(), 1);
    QVERIFY(Pii::equals(mat,
                        PiiMatrix<int>(5,5,
                                       0,0,0,0,0,
                                       0,1,1,1,0,
                                       0,1,1,1,0,
                                       0,1,1,1,0,
                                       0,0,0,0,0)));
    PiiMatrix<double> sub1(mat(0,0,4,4));
    PiiMatrix<double> sub2(mat(1,1,4,4));
    sub1 = 2;
    QCOMPARE(mat(0,0), 0.0);
    QCOMPARE(mat(1,1), 1.0);
    QCOMPARE(sub1(0,0), 2.0);
    QCOMPARE(sub2(0,0), 1.0);
    sub2 = 3;
    QCOMPARE(sub1(1,1), 2.0);
    QCOMPARE(sub2(0,0), 3.0);
    QCOMPARE(mat(4,4), 0.0);

    setSubmatrix(mat(1,1,3,3).selfRef(), 5.0);
    QCOMPARE(mat(2,2), 5.0);
  }
  {
    PiiMatrix<int> mat(3,3);
    QVERIFY(mat(0,0,0,0).isEmpty());
  }
}

void TestPiiMatrix::transpose()
{
  PiiMatrix<int> mat1(PiiMatrix<int>::identity(4));
  QVERIFY(Pii::equals(mat1, Pii::transpose(mat1)));

  PiiMatrix<int> mat2(2, 3,
                      1, 2, 3,
                      4, 5, 6);
  QVERIFY(Pii::equals(Pii::transpose(mat2),
                      PiiMatrix<int>(3, 2,
                                     1, 4,
                                     2, 5,
                                     3, 6)));
  mat2 = Pii::transpose(mat2);
  QVERIFY(Pii::equals(mat2,
                      PiiMatrix<int>(3, 2,
                                     1, 4,
                                     2, 5,
                                     3, 6)));

}

void TestPiiMatrix::integerMath()
{
  PiiMatrix<int> m1(3, 3,
                    1, 1, -1,
                    1, -1, 1,
                    -1, 1, 1);

  PiiMatrix<int> m2(-m1);
  QVERIFY(Pii::equals(PiiMatrix<int>(m1 + m2), PiiMatrix<int>(m1.rows(), m1.columns())));
  QVERIFY(Pii::equals(PiiMatrix<int>(m1 * m2), PiiMatrix<int>(3,3, -3,1,1, 1,-3,1, 1,1,-3)));
  PiiMatrix<int> minusOne(3,3);
  minusOne = -1;
  QVERIFY(Pii::equals(Pii::multiplied(m1,m2), minusOne));
  QVERIFY(Pii::equals(Pii::divided(m1,m2), minusOne));
  QVERIFY(Pii::isSymmetric(m1));
  QVERIFY(!Pii::isAntiSymmetric(m1));
  QVERIFY(!Pii::isDiagonal(m1));
  Pii::flip(m1, Pii::Vertically | Pii::Horizontally);
  QVERIFY(Pii::equals(m1, PiiMatrix<int>(3,3, 1,1,-1, 1,-1,1, -1,1,1)));

  PiiMatrix<uchar> m3(2, 2,
                      0, 2,
                      4, 8),
    m4(4,4);
  m4(1,1,2,2) << 255 - m3;
  QVERIFY(Pii::equals(m4,
                      PiiMatrix<uchar>(4, 4,
                                       0, 0, 0, 0,
                                       0, 255, 253, 0,
                                       0, 251, 247, 0,
                                       0, 0, 0, 0)));
}

void TestPiiMatrix::doubleMath()
{
  PiiMatrix<double> m1(4, 4,
                       1.0, -2.0, 1.0, -3.0,
                       0.0, -2.0, -1.0, 2.0,
                       -1.0, 3.0, 2.0, 1.0,
                       -2.0, -1.0, 3.0, 2.0);
  QVERIFY(Pii::almostEqual(m1/m1, PiiMatrix<double>::identity(4), 1e-10));

  QVERIFY(Pii::almostEqual(m1*Pii::transpose(m1),
                               PiiMatrix<double>(PiiMatrix<int>(4,4,
                                                                15, -3, -8, -3,
                                                                -3, 9, -6, 3,
                                                                -8, -6, 15, 7,
                                                                -3, 3, 7, 18)),
                               1e-10));

  QCOMPARE(Pii::determinant(m1), 30.0);

  PiiMatrix<double> original = m1;
  Pii::flip(m1, Pii::Vertically);
  m1 = m1+2;
  m1 = m1-2;
  m1 = m1+m1;
  m1 /= 2;
  m1 = m1 - m1/2;
  m1 *= 2;
  m1 -= 7;
  Pii::flip(m1, Pii::Horizontally);
  m1 += 7;
  m1 = Pii::transpose(m1);
  m1 = m1/-4;
  m1 = Pii::transpose(m1);
  m1 = m1*-4;
  Pii::flip(m1, Pii::Vertically);
  m1 *= PiiMatrix<double>::identity(4);
  m1 /= PiiMatrix<double>::identity(4);
  Pii::flip(m1, Pii::Horizontally);
  QVERIFY(Pii::equals(original, m1));
}

void TestPiiMatrix::logic()
{
  PiiMatrix<int> mat(3, 3,
                     1, 2, 3,
                     -1, -2, -3,
                     0, 1, -1);

  PiiMatrix<bool> result(!((mat <= -1 || mat >= 1) && mat != 0 && (mat < -1 || mat > 1)));
  QVERIFY(Pii::equals(result, PiiMatrix<bool>(3,3, 1,0,0, 1,0,0, 1,1,1)));

  PiiMatrix<int> mat2(2, 2,
                      2, 3,
                      -2, 0);
  result = ((mat(0,1,2,2) != mat2) == ((mat(0,0,2,2) < mat2 && mat(0,1,2,2) > mat2) || (mat(1,0,2,2) > mat2 && mat(1,1,2,2) < mat2)));
  QVERIFY(Pii::equals(result, PiiMatrix<bool>(2,2, 1,1, 1,1)));

  mat(mat < 0) << -mat(mat < 0);
  QVERIFY(Pii::equals(mat, Pii::abs(mat)));
  mat(mat == 0) = -1;
  mat(mat == 2) << mat(mat > 2) * 3;
  mat(mat == 1) *= -1;
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,3, -1,9,3, -1,9,3, -1,-1,-1)));
}

void TestPiiMatrix::rowHandling()
{
  {
    PiiMatrix<int> mat(0,2);
    mat.appendRow();
    mat.appendRow();
    mat(0,0) = 1;
    mat(1,1) = 2;
    QCOMPARE(mat.rows(), 2);
    mat.appendRow();
    mat(2,0) = 3;
    mat.appendRow();
    mat(3,1) = 4;
    QVERIFY(Pii::equals(mat, PiiMatrix<int>(4,2, 1,0, 0,2, 3,0, 0,4)));
    mat.removeRow(1);
    QCOMPARE(mat(1,0), 3);
    QCOMPARE(mat(1,1), 0);
    mat.removeRow(1);
    QCOMPARE(mat(1,0), 0);
    QCOMPARE(mat(1,1), 4);
    mat.insertRow(1);
    mat(1,0) = 2;
    mat(1,1) = 3;
    QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,2, 1,0, 2,3, 0,4)));
    QVERIFY(Pii::equals(mat - mat, PiiMatrix<int>(mat.rows(), mat.columns())));
    PiiMatrix<int> row(1,2, 1,2);
    mat.appendRow(row[0]);
    mat.appendRow(row[0]);
    mat.appendRow(row[0]);
    mat.appendRow(row[0]);
    mat.appendRow(row[0]);
    PiiMatrix<int> sub(mat(0,1, 8,1));
    sub.removeRow(0);
    sub.removeRow(0);
    QCOMPARE(sub(0,0), 4);
    sub.removeRow(0);
    QCOMPARE(sub.rows(), 5);
    QCOMPARE(Pii::sum<int>(sub), 10);
  }
  {
    PiiMatrix<double> a(3,5);
    PiiMatrix<double> row(1,5);
    a.appendRow(row[0]);
    a = PiiMatrix<double>(3,5);
    a.appendRow(row[0]);

    a.removeRow(0);
    a.removeRow(0);
    a.removeRow(0);
    a.removeRow(0);
    a.insertRow(0, 1.0, 2.0, 3.0, 4.0, 5.0);
    QCOMPARE(a.rows(), 1);
    QCOMPARE(a(0,0), 1.0);
    QCOMPARE(a(0,4), 5.0);
  }
  {
    PiiMatrix<double> a(0,5);
    a.insertRow(-1, 1.0, 2.0, 3.0, 4.0, 5.0);
    QCOMPARE(a.rows(), 1);
    QCOMPARE(a(0,0), 1.0);
    QCOMPARE(a(0,4), 5.0);
  }
  {
    PiiMatrix<int> a(0,1);
    a.appendRow(1);
    a.appendRow(2);
    a.appendRow(3);
    QVERIFY(Pii::equals(a, PiiMatrix<int>(3,1, 1,2,3)));
  }
}

void TestPiiMatrix::removeRows()
{
  {
    PiiMatrix<int> a(5,1,
                     1,2,3,4,5);
    a.removeRows(1,3);
    QVERIFY(Pii::equals(a, PiiMatrix<int>(2,1, 1, 5)));
  }
  {
    PiiMatrix<int> a(5,1,
                     1,2,3,4,5);
    a.removeRows(0,3);
    QVERIFY(Pii::equals(a, PiiMatrix<int>(2,1, 4, 5)));
  }
  {
    PiiMatrix<int> a(5,1,
                     1,2,3,4,5);
    a.removeRows(0,5);
    QVERIFY(a.isEmpty());
  }
}

void TestPiiMatrix::removeColumns()
{
  {
    PiiMatrix<int> a(1,5,
                     1,2,3,4,5);
    a.removeColumns(1,3);
    QVERIFY(Pii::equals(a, PiiMatrix<int>(1,2, 1, 5)));
  }
  {
    PiiMatrix<int> a(1,5,
                     1,2,3,4,5);
    a.removeColumns(0,3);
    QVERIFY(Pii::equals(a, PiiMatrix<int>(1,2, 4, 5)));
  }
  {
    PiiMatrix<int> a(1,5,
                     1,2,3,4,5);
    a.removeColumns(0,5);
    QVERIFY(a.isEmpty());
  }
}

void TestPiiMatrix::assignment()
{
  PiiMatrix<int> a(5,5);
  PiiMatrix<int> b(a);
  // Should share the same memory location
  QCOMPARE(a.constRowBegin(0), b.constRowBegin(0));
  // Should detach a
  a(1,1) = 5;
  QVERIFY(a.constRowBegin(0) != b.constRowBegin(0));
  PiiMatrix<int> c(a(2,2,2,2));
  QCOMPARE(c.rows(), 2);
  QCOMPARE(c.columns(), 2);
  c = 4;
  QVERIFY(Pii::equals(c, PiiMatrix<int>::constant(2,2,4)));

  QVERIFY(Pii::equals(b, PiiMatrix<int>(5,5)));
  QCOMPARE(a(1,1), 5);
  QCOMPARE(a(2,2), 0);

  a = c;
  QCOMPARE(a.constRowBegin(0), c.constRowBegin(0));
  b = a;
  QCOMPARE(a.constRowBegin(0), b.constRowBegin(0));
  QCOMPARE(c.constRowBegin(0), b.constRowBegin(0));
  // Sould detach c
  c -= 4;
  QVERIFY(a.constRowBegin(0) != c.constRowBegin(0));
  QCOMPARE(a.constRowBegin(0), b.constRowBegin(0));
  // Should detach and reassign b
  b = b(1,1,1,1);
  QVERIFY(a.constRowBegin(0) != b.constRowBegin(0));
  QCOMPARE(b.rows(), 1);
  QCOMPARE(b.columns(), 1);
  // B was 2x2, and now it is just an immutable sub-window reference
  QCOMPARE(b.stride(), 2*sizeof(int));
  QCOMPARE(b(0,0), 4);
  // We called a non-const function, which should detach the data.
  QCOMPARE(b.stride(), sizeof(int));
}

void TestPiiMatrix::selfAssignment()
{
  PiiMatrix<int> a(4,4);
  PiiMatrix<int> &b(a);
  PiiMatrix<int> c(a);
  const int* pRow = a.constRowBegin(0);
  QCOMPARE(pRow, b.constRowBegin(0));
  // These should do nothing
  a = b;
  b = b;
  a = a;
  QCOMPARE(pRow, a.constRowBegin(0));
  QCOMPARE(pRow, b.constRowBegin(0));

  // This detaches a and b from c
  b(1,1) = 9;
  QCOMPARE(a(1,1), 9);
  QCOMPARE(a.constRowBegin(0), b.constRowBegin(0));
  QVERIFY(a.constRowBegin(0) != c.constRowBegin(0));

  QCOMPARE(c(1,1), 0);
  a = c;
  QCOMPARE(a(1,1), 0);
  QCOMPARE(b(1,1), 0);

  a = PiiMatrix<int>::constant(2, 2, 2);
  a = a/2+4;
  QVERIFY(Pii::equals(a, PiiMatrix<int>::constant(2, 2, 5)));
  a << a + 2;
  QVERIFY(Pii::equals(a, PiiMatrix<int>::constant(2, 2, 7)));
  a(0, 0, 1, -1) = 0;
  QVERIFY(Pii::equals(a, PiiMatrix<int>(2, 2, 0, 0, 7, 7)));
  a(1, 0, -1, -1) << a(0, 0, 1, -1);
  QVERIFY(Pii::equals(a, PiiMatrix<int>(2, 2)));

  a = PiiMatrix<int>(2, 4,
                     1, 2, 3, 4,
                     5, 6, 7, 8);
  /* TODO make this work
    a(0, 0, 2, 2) << (Pii::transpose(a)(2, 0, 2, 2) - 3);
  QVERIFY(Pii::equals(a, PiiMatrix<int>(2, 4,
                                        0, 4, 3, 4,
                                        1, 5, 7, 8)));
  */
}

void TestPiiMatrix::iterators()
{
  {
    // Iterating over an empty matrix should work fine.
    PiiMatrix<bool> empty;
    QCOMPARE(empty.constBegin() - empty.constEnd(), ptrdiff_t(0));
    QCOMPARE(empty.begin() - empty.end(), ptrdiff_t(0));
    QCOMPARE(empty.constRowBegin(0), static_cast<const bool*>(0));
    QVERIFY(empty.constRowBegin(0) == empty.constRowEnd(0));
    QCOMPARE(empty.constRowBegin(0) - empty.constRowEnd(0), ptrdiff_t(0));
    QVERIFY(empty.constColumnBegin(0) == empty.constColumnEnd(0));
    QCOMPARE(empty.constColumnBegin(0) - empty.constColumnEnd(0), ptrdiff_t(0));
    for (PiiMatrix<bool>::iterator i = empty.begin(); i != empty.end(); ++i) *i = false;
  }
  {
    const PiiMatrix<int> matInt(2,3, -8, 6, 2, 0, 4, 8);
    PiiMatrix<int>::const_iterator iter = matInt.begin();
    const int rows = matInt.rows();
    const int cols = matInt.columns();
    for(int r = 0; r<rows; ++r)
      {
        for(int c = 0; c < cols; ++c)
          {
            QCOMPARE(matInt(r,c), *iter);
            QCOMPARE(iter.row(), r);
            QCOMPARE(iter.column(), c);
            ++iter;
          }
      }
    QVERIFY(matInt.end() == iter);
    QCOMPARE(matInt.end() - matInt.begin(), ptrdiff_t(6));
    QCOMPARE(matInt.begin() - matInt.end(), ptrdiff_t(-6));
    iter = matInt.begin();
    QCOMPARE(iter[0], -8);
    QCOMPARE(iter[1], 6);
    QCOMPARE(iter[2], 2);
    QCOMPARE(iter[3], 0);
    QCOMPARE(iter[4], 4);
    QCOMPARE(iter[5], 8);
    ++iter;
    QCOMPARE(*iter, 6);
    iter++;
    QCOMPARE(*iter, 2);
    QCOMPARE(*(iter++), 2);
    QCOMPARE(*(++iter), 4);
    QCOMPARE(*iter, 4);
    QCOMPARE(*(--iter), 0);
    QCOMPARE(*(iter--), 0);
    QCOMPARE(*iter, 2);
    iter -= 2;
    QCOMPARE(*iter, -8);
    iter += 5;
    QCOMPARE(*iter, 8);
  }

  {
    PiiMatrix<double> matD(1, 2, 5.0, -9.0);
    for(PiiMatrix<double>::iterator iter = matD.begin(); iter != matD.end(); ++(*iter), ++iter) ;
    QVERIFY(Pii::equals(matD, PiiMatrix<double>(1,2, 6.0, -8.0)));
    PiiMatrix<double>::row_iterator it = matD.rowBegin(0);
    QCOMPARE(it[0], 6.0);
    QCOMPARE(it[1], -8.0);
    QCOMPARE(*(++it), -8.0);
    QCOMPARE(*(it++), -8.0);
    QVERIFY(it == matD.rowEnd(0));
    it -= 2;
    QCOMPARE(*it, 6.0);
    it += 1;
    QCOMPARE(*it, -8.0);
  }

  {
    PiiMatrix<char> mat(3, 2,
                        1, 2,
                        3, 4,
                        5, 6);
    QCOMPARE(mat.stride(), size_t(4));
    for (int r=0; r<mat.rows(); ++r)
      for (PiiMatrix<char>::row_iterator i = mat.rowBegin(r); i < mat.rowEnd(r); ++i)
        *i += 2;
    for (int c=0; c<mat.columns(); ++c)
      for (PiiMatrix<char>::column_iterator i = mat.columnBegin(c); i < mat.columnEnd(c); ++i)
        *i -= 3;
    QVERIFY(Pii::equals(mat, PiiMatrix<char>(3, 2,
                                             0, 1,
                                             2, 3,
                                             4, 5)));
    QCOMPARE(*(mat.begin() + 5), char(5));
    QCOMPARE(*(mat.end() - 1), char(5));
    mat.begin()[2] = -2;
    QCOMPARE(mat.begin()[2], char(-2));

    mat.columnBegin(0)[1] = 2;
    QCOMPARE(*(mat.columnBegin(0)+1), char(2));
    QCOMPARE(*(mat.columnEnd(1)-2), char(3));

    {
      for (PiiMatrix<char>::iterator i = mat.end(); i-- != mat.begin(); )
        *i += 1;
      char index = 1;
      for (PiiMatrix<char>::const_iterator i = mat.constBegin(); i != mat.constEnd(); ++i, ++index)
        QCOMPARE(*i, index);
    }

    {
      QVERIFY(Pii::equals(mat, PiiMatrix<char>(3, 2,
                                               1, 2,
                                               3, 4,
                                               5, 6)));
      PiiMatrix<char>::iterator i = mat.begin();
      i += 4;
      QCOMPARE(i[-4], char(1));
      QCOMPARE(i - mat.begin(), ptrdiff_t(4));

      i -= 3;
      QCOMPARE(*i, char(2));
      QCOMPARE(i - mat.begin(), ptrdiff_t(1));
      QCOMPARE(i[4], char(6));
    }

    QCOMPARE(mat.columnBegin(0) - mat.columnEnd(0), ptrdiff_t(-3));

    {
      for (PiiMatrix<char>::column_iterator i = mat.columnEnd(1); i-- != mat.columnBegin(1); )
        *i += 1;
      char index = 3;
      for (PiiMatrix<char>::const_column_iterator i = mat.constColumnBegin(1); i != mat.constColumnEnd(1); ++i, index += 2)
        QCOMPARE(*i, index);
    }

    {
      PiiMatrix<int,3,3> mat(1,2,3,
                             4,5,6,
                             7,8,9);
      int iVal = 1;
      for (PiiMatrix<int,3,3>::iterator i=mat.begin(); i!=mat.end(); ++i, ++iVal)
        *i -= iVal;
      for (PiiMatrix<int,3,3>::iterator i=mat.begin(); i!=mat.end(); ++i)
        QCOMPARE(*i, 0);
    }
    {
      PiiMatrix<int,3,3> mat;
      int iVal = 0;
      for (int c=0; c<3; ++c)
        for (PiiMatrix<int,3,3>::column_iterator i=mat.columnBegin(c); i!=mat.columnEnd(c); ++i, ++iVal)
          *i = iVal;
      QVERIFY(Pii::equals(mat, PiiMatrix<int,3,3>(0,3,6,
                                                  1,4,7,
                                                  2,5,8)));
    }
  }
}

void TestPiiMatrix::alignment()
{
  PiiMatrix<int> mat1(11,11);
  PiiMatrix<int> mat2(8,8);
  PiiMatrix<int> mat3(1,1);
  QCOMPARE(long(mat1[0]) & 0x3, 0l);
  QCOMPARE(long(mat2[0]) & 0x3, 0l);
  QCOMPARE(long(mat3[0]) & 0x3, 0l);
}

void TestPiiMatrix::multiply()
{
}

void TestPiiMatrix::resizing()
{
  PiiMatrix<int> mat(3,3,
                     1,2,3,
                     4,5,6,
                     7,8,9);

  // This should reallocate the matrix.
  mat.insertRow(1);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(4,3,
                                          1,2,3,
                                          0,0,0,
                                          4,5,6,
                                          7,8,9)));
  // This should again reallocate the matrix.
  mat.insertColumn(1);
  QCOMPARE(mat.stride(), 4*sizeof(int));
  int* pFirstRow = mat[0];
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(4,4,
                                          1,0,2,3,
                                          0,0,0,0,
                                          4,0,5,6,
                                          7,0,8,9)));


  // This should go without reallocation -> first row pointer remains
  // the same.
  mat.removeColumn(1);
  QCOMPARE(pFirstRow, mat[0]);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(4,3,
                                          1,2,3,
                                          0,0,0,
                                          4,5,6,
                                          7,8,9)));
  mat.appendColumn();
  QCOMPARE(pFirstRow, mat[0]);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(4,4,
                                          1,2,3,0,
                                          0,0,0,0,
                                          4,5,6,0,
                                          7,8,9,0)));
  mat.removeRow(1);
  QCOMPARE(pFirstRow, mat[0]);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,4,
                                          1,2,3,0,
                                          4,5,6,0,
                                          7,8,9,0)));
  mat.appendRow();
  QCOMPARE(pFirstRow, mat[0]);

  mat.resize(5,5);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(5,5,
                                          1,2,3,0,0,
                                          4,5,6,0,0,
                                          7,8,9,0,0,
                                          0,0,0,0,0,
                                          0,0,0,0,0)));

  mat.removeRow(4);
  mat.removeRow(1);
  mat.removeColumn(4);
  mat.removeColumn(1);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,3,
                                          1,3,0,
                                          7,9,0,
                                          0,0,0)));

  mat = PiiMatrix<int>(3,3,
                       1,2,3,
                       4,5,6,
                       7,8,9);
  mat.appendColumn(mat(0,2,3,1).columnBegin(0));
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,4,
                                          1,2,3,3,
                                          4,5,6,6,
                                          7,8,9,9)));
  mat.insertColumn(0, 0, 1, 2);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,5,
                                          0,1,2,3,3,
                                          1,4,5,6,6,
                                          2,7,8,9,9)));
  mat.insertRow(0, mat(0,0,1,5)[0]);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(4,5,
                                          0,1,2,3,3,
                                          0,1,2,3,3,
                                          1,4,5,6,6,
                                          2,7,8,9,9)));
  mat.insertRow(0, 0, 1, 2, 3, 4);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(5,5,
                                          0,1,2,3,4,
                                          0,1,2,3,3,
                                          0,1,2,3,3,
                                          1,4,5,6,6,
                                          2,7,8,9,9)));
  mat.resize(3,3);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,3,
                                          0,1,2,
                                          0,1,2,
                                          0,1,2)));

  const int pVector[] = { 0, 1, 2, 3, 4 };

  mat.appendRow(pVector);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(4,3,
                                          0,1,2,
                                          0,1,2,
                                          0,1,2,
                                          0,1,2)));

  mat.appendColumn(pVector);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(4,4,
                                          0,1,2,0,
                                          0,1,2,1,
                                          0,1,2,2,
                                          0,1,2,3)));
  mat.resize(1,0);
  QCOMPARE(mat.rows(), 1);
  QCOMPARE(mat.columns(), 0);
  mat.resize(1,0);
  QCOMPARE(mat.rows(), 1);
  QCOMPARE(mat.columns(), 0);

  mat.resize(0,0);
  QCOMPARE(mat.rows(), 0);
  QCOMPARE(mat.columns(), 0);

  mat = PiiMatrix<int>(0,2);
  for (int i=0; i<10; ++i)
    mat.appendRow();
  QCOMPARE(mat.rows(), 10);
  QCOMPARE(mat.columns(), 2);
  mat.resize(0,5);
  QCOMPARE(mat.columns(), 5);
  QCOMPARE(mat.rows(), 0);
  mat.resize(0,1000);
  QCOMPARE(mat.rows(), 0);
  QCOMPARE(mat.columns(), 1000);
  mat.resize(0,2);
  mat.insertRow(0);
  QCOMPARE(mat.rows(), 1);
  QCOMPARE(mat.columns(), 2);

  mat = PiiMatrix<int>();
  mat.resize(2, 10);
  QCOMPARE(mat.rows(), 2);
  QCOMPARE(mat.columns(), 10);
}

void TestPiiMatrix::uninitialized()
{
  PiiMatrix<int> matEmpty(PiiMatrix<int>::uninitialized(5,5));
  QCOMPARE(matEmpty.rows(), 5);
  QCOMPARE(matEmpty.columns(), 5);
}

void TestPiiMatrix::reserve()
{
  PiiMatrix<int> mat, mat2;
  mat.resize(0,16);
  mat.reserve(16);
  QCOMPARE(mat.rows(), 0);
  QCOMPARE(mat.stride() * mat.capacity(), 16 * 16 * sizeof(int));

  for (int i=0; i<16; ++i)
    mat.appendRow();

  QCOMPARE(mat.rows(), 16);
  QCOMPARE(mat.stride() * mat.capacity(), 16 * 16 * sizeof(int));

  mat2.resize(0,2);
  mat2.reserve(0);
  QCOMPARE(mat2.rows(), 0);
  mat2.appendRow();
  mat2.reserve(0);
  QCOMPARE(mat2.rows(), 1);
}

QTEST_MAIN(TestPiiMatrix)
