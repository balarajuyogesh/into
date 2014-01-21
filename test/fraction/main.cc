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

#include "TestPiiFraction.h"

#include <PiiFraction.h>
#include <QtTest>


void TestPiiFraction::create()
{
  QFETCH(int, numerator);
  QFETCH(int, denominator);

  double number = double(numerator)/double(denominator);
  PiiFraction<int> frac = PiiFraction<int>::create(number);
  if (frac.numerator == 1)
    QCOMPARE(double(frac.denominator), 1.0/number);
  else
    {
      QCOMPARE(frac.numerator, numerator);
      QCOMPARE(frac.denominator, denominator);
    }
}

void TestPiiFraction::create_data()
{
  QTest::addColumn<int>("numerator");
  QTest::addColumn<int>("denominator");

  for (int denom=2; denom <= 100; ++denom)
    QTest::newRow(qPrintable(QString("1/%1").arg(denom))) << 1 << denom;

  for (int denom=3; denom <= 201; denom += 2)
    QTest::newRow(qPrintable(QString("2/%1").arg(denom))) << 2 << denom;

  for (int denom=5; denom <= 500; denom += 3)
    QTest::newRow(qPrintable(QString("3/%1").arg(denom))) << 3 << denom;

  for (int denom=1; denom <= 500; ++denom)
    QTest::newRow(qPrintable(QString("17/%1").arg(denom))) << 17 << denom;

  for (int denom=2; denom <= 100; ++denom)
    QTest::newRow(qPrintable(QString("-1/%1").arg(denom))) << -1 << denom;

  QTest::newRow("1/-2") << 1 << -2;

  QTest::newRow("-2/7") << -2 << 7;
}


void TestPiiFraction::operations()
{
  typedef PiiFraction<int> FracType;
  FracType f1(5,36), f2(2,15), f3(1,-5);

  FracType f1Old = f1;
  f1 *= f2;
  QVERIFY(f1 == f1Old*f2);
  QVERIFY(f1 == FracType(1, 54));

  FracType f2Old = f2;
  f2 -= f1;
  QVERIFY(f2 == f2Old - f1);
  QVERIFY(f2 == FracType(31, 270));

  f1Old = f1;
  f1 /= f2;
  QVERIFY(f1 == f1Old / f2);
  QVERIFY(f1 == FracType(5, 31));

  f2Old = f2;
  f2 += f1;
  QVERIFY(f2 == f2Old + f1);
  QVERIFY(f2 == FracType(2311, 8370));
}


void TestPiiFraction::comparison()
{
  typedef PiiFraction<int> FracType;
  const FracType f1(5, 31);
  const FracType f2(2311, 8370);
  QVERIFY(f1 < f2);
  QVERIFY(!(f1 > f2));
  QVERIFY(f1 <= f2);
  QVERIFY(!(f1 >= f2));

  QVERIFY(FracType(1,3) <= FracType(1,3));
  QVERIFY(FracType(1,3) >= FracType(1,3));
  QVERIFY(FracType(5,3) >= FracType(5,3));

  QVERIFY(!(f1 == f2));
  QVERIFY(f1 != f2);
  QVERIFY(!(f1 > 1));
  QVERIFY(!(f1 >= 1));
  QVERIFY(f1 <= 1);
  QVERIFY(!(f1 == 1));
  QVERIFY(f1 != 1);
}


void TestPiiFraction::fractionMatrix()
{
  typedef PiiFraction<long> FracType;
  PiiMatrix<FracType> matObject(2, 2);
  matObject(0,0) = FracType(1,2);
  matObject(0,1) = FracType(1,3);
  matObject(1,0) = FracType(2,3);
  matObject(1,1) = FracType(5,4);
  const PiiMatrix<FracType>& mat = matObject;

  PiiMatrix<FracType> mat2(mat(mat > FracType(1,1)));
  QVERIFY(-(*mat(mat > 1).begin()) == FracType(-5,4));

  mat2 = mat(mat <= 1);
  QCOMPARE(mat2.columns(), 3);
  QVERIFY(mat2(0,0) == FracType(1,2));
  QVERIFY(mat2(0,1) == FracType(1,3));
  QVERIFY(mat2(0,2) == FracType(2,3));

  mat2 = mat(mat > FracType(3,4)) / 5;
  QCOMPARE(mat2.rows(), mat2.columns());
  QCOMPARE(mat2.columns(), 1);
  QVERIFY(mat2(0,0) == FracType(1,4));

  mat2 = mat * Pii::transpose(mat);
  QVERIFY(mat2.columns() == mat2.rows());
  QVERIFY(mat2.columns() == 2);
  QVERIFY(mat2(0,0) == FracType(13,36));
  QVERIFY(mat2(0,1) == FracType(3,4));
  QVERIFY(mat2(1,0) == FracType(3,4));
  QVERIFY(mat2(1,1) == FracType(289,144));

  mat2 = mat + 1;
  QVERIFY(mat2(0,0) == FracType(3,2));
  QVERIFY(mat2(0,1) == FracType(4,3));
  QVERIFY(mat2(1,0) == FracType(5,3));
  QVERIFY(mat2(1,1) == FracType(9,4));
}


QTEST_MAIN(TestPiiFraction)
