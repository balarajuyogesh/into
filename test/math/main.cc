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

//Utility for printing matrix for debug purposes.
#define PrintMatrix(x)                                  \
  std::cout << "Matrix "#x << std::endl;                \
  Pii::printMatrix(std::cout, x, " ", "\n");  \
  std::cout << std::endl << std::endl;

#include <iostream>
using std::cout;
using std::endl;

#include "TestPiiMath.h"
#include <PiiMatrixUtil.h>
#include <PiiPseudoInverse.h>
#include <PiiMath.h>
#include <QtTest>
#include <algorithm>
#include <PiiVector.h>
#include <cstdlib>
#include <ctime>


const double tol = 1e-10;
const float tolF = 1e-5F;


//Utility matrix which can used in tests.
const PiiMatrix<int> matItest(2,3,
                              9,   -10546765, 189811,
                              5774, 1651652, -1);
const PiiMatrix<int> matItestAbs(2,3,
                                 9,    10546765, 189811,
                                 5774, 1651652,  1);
const int matItestMaxElem = 1651652;
const int matItestMinElem = -10546765;
const int matItestMaxAbsElem = 10546765;
const int matItestMaxElemRow = 1;
const int matItestMaxElemColumn = 1;
const int matItestMinElemRow = 0;
const int matItestMinElemColumn = 1;
const double matItestMeanAll = -1449920.0;
const PiiMatrix<double> matItestMeanAlongRow(2, 1, -3452315.0, 552475.0);
const PiiMatrix<double> matItestMeanAlongColumn(1, 3, 2891.5, -4447556.5, 94905.0);



//Utility function for comparing floating point numbers.
template<class T>
bool almostEqual(const T& a, const T& b);

template<>
inline bool almostEqual<float>(const float& a, const float& b) { return qFuzzyCompare(a,b); }

template<>
inline bool almostEqual<double>(const double& a, const double& b) { return qFuzzyCompare(a,b); }


void TestPiiMath::gaussian()
{
  double sigma = 0.44721;
  double x = 0.0;
  double mu = 0.0;

  double funcValue = Pii::gaussian(x,mu,sigma);
  QVERIFY(Pii::almostEqualRel(funcValue, 0.892069,1e-3));
}


void TestPiiMath::median()
{
  int a2[] = { 5, -1 };
  double a3[] = { 10, 0.1, 100 };
  int a5[] = { 5, 1, 4, 2, 3 };
  float a7[] = { -5, 2.0, 1.1, 10000, -100, -3, 0 };
  unsigned int a9[] = { 1,2,3,4,5,6,7,8,9 };
  short a25[] = { 5,2,3,4,1,
                  1,3,2,4,5,
                  5,2,4,1,3,
                  1,3,5,4,2,
                  4,2,3,1,5 };

  QCOMPARE(Pii::medianN(a2,2),-1);

  QCOMPARE(Pii::fastMedian(a3,3),10.0);
  QCOMPARE(Pii::medianN(a3,3),10.0);
  QCOMPARE(Pii::median3(a3),10.0);

  QCOMPARE(Pii::fastMedian(a5,5),3);
  QCOMPARE(Pii::medianN(a5,5),3);
  QCOMPARE(Pii::median5(a5),3);

  QCOMPARE(Pii::fastMedian(a7,7),0.0f);
  QCOMPARE(Pii::medianN(a7,7),0.0f);
  QCOMPARE(Pii::median7(a7),0.0f);

  QCOMPARE(Pii::fastMedian(a9,9),5u);
  QCOMPARE(Pii::medianN(a9,9),5u);
  QCOMPARE(Pii::median9(a9),5u);

  QCOMPARE(Pii::fastMedian(a25,25),static_cast<short>(3));
  QCOMPARE(Pii::medianN(a25,25),static_cast<short>(3));
  QCOMPARE(Pii::median25(a25),static_cast<short>(3));

  PiiMatrix<int> mat(4,4,
                     1,2,3,4,
                     1,2,3,4,
                     1,2,3,4,
                     1,2,3,4);
  QCOMPARE(Pii::median(mat), 2);
}

void TestPiiMath::round()
{
  PiiMatrix<double> d(2, 2,
                      1.0, 2.3,
                      19.4, -2.5);
  QVERIFY(Pii::equals(Pii::round(d), PiiMatrix<double>(2, 2,
                                                           1.0, 2.0,
                                                           19.0, -3.0)));
  QVERIFY(Pii::equals(Pii::round<int>(d), PiiMatrix<int>(2, 2,
                                                             1, 2,
                                                             19, -3)));
}

void TestPiiMath::ceil()
{
  QVERIFY(Pii::equals(Pii::ceil(PiiMatrix<double>(2,2,
                                                      -0.1, 2.3,
                                                      0.1, -2.3)),
                      PiiMatrix<double>(2,2,
                                        -0.0, 3.0,
                                        1.0, -2.0)));
}

void TestPiiMath::floor()
{
  QVERIFY(Pii::equals(Pii::floor(PiiMatrix<double>(2,2,
                                                       -0.1, 2.3,
                                                       0.1, -2.3)),
                      PiiMatrix<double>(2,2,
                                        -1.0, 2.0,
                                        0.0, -3.0)));
}

void TestPiiMath::divide()
{
  PiiMatrix<int> a(2, 2,
                   2, 4,
                   6, 8);
  Pii::divide(a, PiiMatrix<int>(2, 2,
                                2, 4,
                                6, 8));
  QVERIFY(Pii::equals(a, PiiMatrix<int>(2, 2,
                                        1, 1,
                                        1, 1)));
}

void TestPiiMath::atan2()
{
  float maxDiff = 0;
  for (double y=-M_PI; y<=M_PI; y+=M_PI/16)
    for (double x=-M_PI; x<=M_PI; x+=M_PI/16)
      {
        float diff = Pii::abs(atan2f(y,x) - Pii::atan2(y,x));
        if (diff > maxDiff)
          maxDiff = diff;
      }

  QVERIFY(maxDiff < 0.6f/180.0f*M_PI);
}

void TestPiiMath::fastAtan2()
{
  float maxDiff = 0;
  for (double y=-M_PI; y<=M_PI; y+=M_PI/16)
    for (double x=-M_PI; x<=M_PI; x+=M_PI/16)
      {
        float diff = Pii::abs(atan2f(y,x) - Pii::fastAtan2(y,x));
        if (diff > maxDiff)
          maxDiff = diff;
      }

  QVERIFY(maxDiff < 4.1f/180.0f*M_PI);
}

void TestPiiMath::normalize()
{
  PiiMatrix<double> vector(1,4, 1.0, 2.0, -4.0, 1.5);
  QCOMPARE(Pii::norm(Pii::normalized(vector),2), 1.0);

  QCOMPARE(Pii::norm(Pii::normalized(Pii::transpose(vector), Pii::Vertically),2), 1.0);

  Pii::normalize(vector);
  QCOMPARE(Pii::norm(vector,2), 1.0);
}

void TestPiiMath::combinations()
{
  PiiMatrix<int> combinations(0,2);
  Pii::combinations(5, 2, Pii::MatrixRowAdder<int>(combinations));
  QVERIFY(Pii::equals(combinations, PiiMatrix<int>(10,2,
                                                   3,4,
                                                   2,4,
                                                   1,4,
                                                   0,4,
                                                   2,3,
                                                   1,3,
                                                   0,3,
                                                   1,2,
                                                   0,2,
                                                   0,1)));

  combinations.resize(0,5);
  // Try to take five elements out of a set with only one element
  Pii::combinations(1,5,Pii::MatrixRowAdder<int>(combinations));
  QVERIFY(combinations.isEmpty());

  combinations.resize(0,1000);
  Pii::combinations(1000,1000,Pii::MatrixRowAdder<int>(combinations));
  QCOMPARE(combinations.rows(), 1);
  for (int i=0; i<1000; ++i)
    QCOMPARE(combinations(i),i);
}

void TestPiiMath::permutations()
{
  PiiMatrix<int> permutations(0,3);
  Pii::permutations(3, Pii::MatrixRowAdder<int>(permutations));
  QCOMPARE(permutations.rows(), 3*2);
  QVERIFY(Pii::equals(permutations, PiiMatrix<int>(6,3,
                                                   0,1,2,
                                                   1,0,2,
                                                   1,2,0,
                                                   2,1,0,
                                                   2,0,1,
                                                   0,2,1)));
  permutations.resize(0,7);
  Pii::permutations(7, Pii::MatrixRowAdder<int>(permutations));
  QCOMPARE(permutations.rows(), 7*6*5*4*3*2);
}

void TestPiiMath::transformRows()
{
  const PiiMatrix<double> mat(3,3,
                              1.0,2.0,3.0,
                              2.0,3.0,4.0,
                              3.0,4.0,5.0);
  double array[] = { 1, 2, 3 };
  QVERIFY(Pii::equals(Pii::transformedRows(mat, array, std::minus<double>()),
                      PiiMatrix<double>(3,3,
                                        0.0,0.0,0.0,
                                        1.0,1.0,1.0,
                                        2.0,2.0,2.0)));

}

void TestPiiMath::transformColumns()
{
  const PiiMatrix<double> mat(3,3,
                              1.0,2.0,3.0,
                              2.0,3.0,4.0,
                              3.0,4.0,5.0);
  double array[] = { 1, 2, 3 };
  QVERIFY(Pii::equals(Pii::transformedColumns(mat, array, std::minus<double>()),
                      PiiMatrix<double>(3,3,
                                        0.0,1.0,2.0,
                                        0.0,1.0,2.0,
                                        0.0,1.0,2.0)));

}

void TestPiiMath::forEach()
{
  // Mean of elements
  const PiiMatrix<int> mat(1,4, -1,-2,1,2);
  QCOMPARE(Pii::forEach(mat.begin(), mat.end(),
                        Pii::Mean<int,float>()).mean(),
           0.0f);
}

void TestPiiMath::forEachIf()
{
  // Mean of positive elements
  const PiiMatrix<int> mat(1,4, -1,-2,1,2);
  QCOMPARE(Pii::forEachIf(mat.begin(), mat.end(),
                          std::bind2nd(std::greater<int>(), 0),
                          Pii::Mean<int,float>()).mean(),
           1.5f);
}

void TestPiiMath::mapIf()
{
  PiiMatrix<int> matInput(1, 3, -1, 2, 3);
  // Negate all positive numbers
  Pii::mapMatrixIf(matInput,
                   std::bind2nd(std::greater<int>(), 0),
                   std::negate<int>());
  QVERIFY(Pii::equals(matInput, PiiMatrix<int>(1,3, -1, -2, -3)));

  // Negate all negative elements
  QVERIFY(Pii::equals(Pii::mappedMatrixIf(matInput,
                                          std::bind2nd(std::less<int>(), 0),
                                          std::negate<int>()),
                      PiiMatrix<int>(1,3, 1,2,3)));
}


void TestPiiMath::movingAverage()
{
  PiiMatrix<int> input(4,4,
                       1,2,3,4,
                       3,4,5,6,
                       5,6,7,8,
                       7,8,9,10);

  PiiMatrix<int> tmp(Pii::movingAverage<int>(input,2,Pii::Vertically));

  QVERIFY(Pii::equals(Pii::movingAverage<int>(input,2,Pii::Vertically),
                      PiiMatrix<int>(3,4,
                                     2,3,4,5,
                                     4,5,6,7,
                                     6,7,8,9)));
  QVERIFY(Pii::equals(Pii::movingAverage<int>(input,3,Pii::Vertically),
                      PiiMatrix<int>(2,4,
                                     3,4,5,6,
                                     5,6,7,8)));
  QVERIFY(Pii::equals(Pii::movingAverage<int>(input,4,Pii::Vertically), PiiMatrix<int>(1,4, 4,5,6,7)));
  QCOMPARE(Pii::movingAverage<int>(input,5,Pii::Vertically).columns(), 4);
  QCOMPARE(Pii::movingAverage<int>(input,5,Pii::Vertically).rows(), 0);

  QVERIFY(Pii::equals(Pii::movingAverage<double>(input,4), PiiMatrix<double>(4,1, 2.5, 4.5, 6.5, 8.5)));

  PiiMatrix<double> avg = Pii::movingAverage<double>(input, 4, Pii::Horizontally, Pii::ShrinkWindow);
  QVERIFY(Pii::equals(avg, PiiMatrix<double>(4,4,
                                             1.5, 2.0, 2.5, 3.0,
                                             3.5, 4.0, 4.5, 5.0,
                                             5.5, 6.0, 6.5, 7.0,
                                             7.5, 8.0, 8.5, 9.0)));

  avg = Pii::movingAverage<double>(input, 3, Pii::Horizontally, Pii::ShrinkWindow);
  QVERIFY(Pii::equals(avg, PiiMatrix<double>(4,4,
                                             1.5, 2.0, 3.0, 3.5,
                                             3.5, 4.0, 5.0, 5.5,
                                             5.5, 6.0, 7.0, 7.5,
                                             7.5, 8.0, 9.0, 9.5)));

  avg = Pii::movingAverage<double>(input, 3, Pii::Horizontally, Pii::ShrinkWindowSymmetrically);
  QVERIFY(Pii::equals(avg, PiiMatrix<double>(input)));

  avg = Pii::movingAverage<double>(input, 4, Pii::Horizontally, Pii::AssumeZeros);
  QVERIFY(Pii::equals(avg, PiiMatrix<double>(4,4,
                                             3.0, 6.0, 10.0, 9.0,
                                             7.0, 12.0, 18.0, 15.0,
                                             11.0, 18.0, 26.0, 21.0,
                                             15.0, 24.0, 34.0, 27.0) / 4.0));

  avg = Pii::movingAverage<double>(input, 4, Pii::Horizontally, Pii::ShrinkWindowSymmetrically);
  QVERIFY(avg.isEmpty());

  QVERIFY(Pii::equals(Pii::movingAverage<double>(PiiMatrix<int>(1,7,1,2,3,4,5,6,7), 7, Pii::Horizontally, Pii::OnlyValidPart),
                      PiiMatrix<int>(1,1,4)));
}

void TestPiiMath::diff()
{
  PiiMatrix<int> input(4,4,
                       1,2,3,4,
                       3,4,5,6,
                       5,6,7,8,
                       7,8,9,10);

  QVERIFY(Pii::equals(Pii::diff(input,1,1,Pii::Vertically), PiiMatrix<int>(3,4,
                                                                               2,2,2,2,
                                                                               2,2,2,2,
                                                                               2,2,2,2)));
  QVERIFY(Pii::equals(Pii::diff(input,2,1,Pii::Horizontally), PiiMatrix<int>(4,2,
                                                                                 2,2,
                                                                                 2,2,
                                                                                 2,2,
                                                                                 2,2)));

  QVERIFY(Pii::equals(Pii::diff(input,1,2,Pii::Vertically), PiiMatrix<int>(2,4)));
  QVERIFY(Pii::diff(input,1,4,Pii::Vertically).isEmpty());
  QVERIFY(Pii::diff(input,1,4,Pii::Horizontally).isEmpty());
}

void TestPiiMath::centralDiff()
{
  PiiMatrix<int> input(3,5,
                       1,2,3,2,0,
                       1,3,2,2,1,
                       2,4,1,2,0);

  QVERIFY(Pii::equals(Pii::centralDiff<double>(input, Pii::Horizontally), PiiMatrix<double>(3,5,
                                                                                                1.0, 1.0, 0.0, -1.5, -2.0,
                                                                                                2.0, 0.5, -0.5, -0.5, -1.0,
                                                                                                2.0, -0.5, -1.0, -0.5, -2.0)));
  QVERIFY(Pii::equals(Pii::centralDiff<double>(input, Pii::Vertically), PiiMatrix<double>(3,5,
                                                                                              0.0, 1.0, -1.0, 0.0, 1.0,
                                                                                              0.5, 1.0, -1.0, 0.0, 0.0,
                                                                                              1.0, 1.0, -1.0, 0.0, -1.0)));
  QVERIFY(Pii::equals(Pii::centralDiff<int>(PiiMatrix<int>(1,4), Pii::Vertically), PiiMatrix<int>(1,4)));
  QVERIFY(Pii::equals(Pii::centralDiff<int>(PiiMatrix<int>(2,1), Pii::Horizontally), PiiMatrix<int>(2,1)));
}

void TestPiiMath::sum()
{
  PiiMatrix<int> input(2,2,
                       1, 2,
                       -1,-2);
  QCOMPARE(Pii::sum<int>(input), 0);
  QVERIFY(Pii::equals(Pii::sum<int>(input,Pii::Vertically), PiiMatrix<int>(1,2)));
  QVERIFY(Pii::equals(Pii::sum<int>(input,Pii::Horizontally), PiiMatrix<int>(2,1,3,-3)));
}

void TestPiiMath::sqrt()
{
  QVERIFY(Pii::almostEqualRel(4.0, Pii::sqrt(char(16))) );
  QVERIFY(Pii::almostEqualRel(4.0, Pii::sqrt(16)) );
  QCOMPARE(Pii::sqrt(25.0), 5.0);
  QCOMPARE(Pii::sqrt(36.0f), 6.0f);

  PiiMatrix<double> input(2,2,
                          4.0, 9.0,
                          16.0, 25.0);
  QVERIFY(Pii::equals(Pii::sqrt(input), PiiMatrix<double>(2,2,
                                                              2.0, 3.0,
                                                              4.0, 5.0)));
}

void TestPiiMath::distanceAlong()
{
  {
    PiiMatrix<int> pt1(4,2);
    PiiMatrix<int> pt2(4,2,
                       1,1,
                       2,2,
                       3,3,
                       4,4);
    PiiMatrix<double> result(4,1, 2.0, 8.0, 18.0, 32.0);
    QVERIFY(Pii::equals(Pii::squaredDistance(pt1, pt2, Pii::Horizontally), result));
    PiiMatrix<double> result2(1,2, 30.0, 30.0);
    QVERIFY(Pii::equals(Pii::squaredDistance(pt1, pt2, Pii::Vertically), result2));
  }

  {
    PiiMatrix<double> mat1(4,2);
    PiiMatrix<double> mat2(4,2,
                           1.0,1.0,
                           2.0,2.0,
                           3.0,3.0,
                           4.0,4.0);

    PiiMatrix<double> result(4,1,
                             1.4142,
                             2.8284,
                             4.2426,
                             5.6568);

    PiiMatrix<double> distance = Pii::distance(mat1, mat2, Pii::Horizontally);
    for(int r = 0; r < mat1.rows(); ++r)
      QVERIFY(Pii::almostEqualRel(distance(r,0), result(r,0), 1e-4));
  }

  {
    PiiMatrix<double> mat1(1,2,
                           0.5,0.5);
    PiiMatrix<double> mat2(1,2,
                           1.0,0.8);
    PiiMatrix<double> result(1,1,0.5830);

    PiiMatrix<double> distance = Pii::distance(mat1, mat2, Pii::Horizontally);
    QVERIFY(Pii::almostEqualRel(result(0,0), distance(0,0), 1e-3));
  }
}

void TestPiiMath::pow()
{
  QCOMPARE(Pii::pow(2,4), 16);
  QCOMPARE(Pii::pow(2.0, 5), 32.0);
  QVERIFY(Pii::equals(Pii::pow(PiiMatrix<double>(1,1,M_PI), 0.0), PiiMatrix<double>(1,1,1.0)));
  QVERIFY(Pii::equals(Pii::pow(PiiMatrix<int>(1,1,1), 15), PiiMatrix<int>(1,1,1)));
  QCOMPARE(Pii::pow(PiiMatrix<double>(1,1,4.0), 0.5)(0,0), 2.0);
}

void TestPiiMath::fitPolynomial()
{
  // y = -2x^2 + 3.5x - 1
  PiiMatrix<double> coeffs = Pii::fitPolynomial(2,
                                                PiiMatrix<double>(1,3, 0.0, 1.0, 2.0),
                                                PiiMatrix<double>(1,3, -1.0, 0.5, -2.0));

  PiiMatrix<double> coeffs2 = Pii::fitPolynomial(2, PiiMatrix<double>(3,2,
                                                                      0.0, -1.0,
                                                                      1.0, 0.5,
                                                                      2.0, -2.0));

  QVERIFY(Pii::equals(coeffs, coeffs2));
  QCOMPARE(coeffs(0), -1.0);
  QCOMPARE(coeffs(1), 3.5);
  QCOMPARE(coeffs(2), -2.0);

  // y = 2.5x^3 + 2x^2 - 8x + 0.5
  coeffs = Pii::fitPolynomial(3, PiiMatrix<double>(4,2,
                                                   -1.0, 8.0,
                                                   0.0, 0.5,
                                                   1.0, -3.0,
                                                   2.0, 12.5));
  QCOMPARE(coeffs(0), 0.5);
  QCOMPARE(coeffs(1), -8.0);
  QCOMPARE(coeffs(2), 2.0);
  QCOMPARE(coeffs(3), 2.5);

  // y = -2x^2 + 3.5x - 1
  // y2 = -1.75x^2 + 3.25x - 1
  coeffs = Pii::fitPolynomial(2,
                              PiiMatrix<double>(2,3,
                                                0.0, 1.0, 2.0,
                                                0.0, 1.0, 2.0),
                              PiiMatrix<double>(2,3,
                                                -1.0, 0.5, -2.0,
                                                -1.0, 0.5, -1.5));
  QCOMPARE(coeffs(0,0), -1.0);
  QCOMPARE(coeffs(0,1), 3.5);
  QCOMPARE(coeffs(0,2), -2.0);
  QCOMPARE(coeffs(1,0), -1.0);
  QCOMPARE(coeffs(1,1), 3.25);
  QCOMPARE(coeffs(1,2), -1.75);
}

void TestPiiMath::min()
{
  PiiMatrix<int> a(3,3,
                   1,2,3,
                   4,5,6,
                   7,8,9);
  PiiMatrix<int> b(3,3,
                   9,8,7,
                   6,5,4,
                   3,2,1);
  QVERIFY(Pii::equals(Pii::min(a,b), PiiMatrix<int>(3,3,
                                                    1,2,3,
                                                    4,5,4,
                                                    3,2,1)));
  QVERIFY(Pii::equals(Pii::min(a,5), PiiMatrix<int>(3,3,
                                                    1,2,3,
                                                    4,5,5,
                                                    5,5,5)));

  const PiiMatrix<int> mat(3,2, 5, 8, 9, 0, 502033, -100023);
  int r,c;
  QCOMPARE( Pii::min(mat, &r,&c), -100023);
  QCOMPARE( r, 2 );
  QCOMPARE( c, 1 );

  Pii::min(matItest, &r, &c);
  QCOMPARE(r, matItestMinElemRow);
  QCOMPARE(c, matItestMinElemColumn);

  QCOMPARE( Pii::min(PiiMatrix<int>(2,1, -1, -2)), -2 );
  QCOMPARE( Pii::min(PiiMatrix<int>(2,1, 1, 2)), 1 );
  QCOMPARE( Pii::min(PiiMatrix<float>(2,1, -1.0, -2.0)), -2.0f );
  QCOMPARE( Pii::min(PiiMatrix<float>(2,1, 1.0, 2.0)), 1.0f );
}

void TestPiiMath::max()
{
  PiiMatrix<int> a(3,3,
                   1,2,3,
                   4,5,6,
                   7,8,9);
  PiiMatrix<int> b(3,3,
                   9,8,7,
                   6,5,4,
                   3,2,1);
  QVERIFY(Pii::equals(Pii::max(a,b), PiiMatrix<int>(3,3,
                                                    9,8,7,
                                                    6,5,6,
                                                    7,8,9)));
  QVERIFY(Pii::equals(Pii::max(a,5), PiiMatrix<int>(3,3,
                                                    5,5,5,
                                                    5,5,6,
                                                    7,8,9)));
  int r,c;
  QCOMPARE( Pii::max(matItest), matItestMaxElem );
  Pii::max(matItest, &r, &c);
  QCOMPARE(r, matItestMaxElemRow);
  QCOMPARE(c, matItestMaxElemColumn);

  QCOMPARE( Pii::max(PiiMatrix<int>(2,1, -1, -2)), -1 );
  QCOMPARE( Pii::max(PiiMatrix<int>(2,1, 1, 2)), 2 );
  QCOMPARE( Pii::max(PiiMatrix<float>(2,1, -1.0, -2.0)), -1.0f );
  QCOMPARE( Pii::max(PiiMatrix<float>(2,1, 1.0, 2.0)), 2.0f );
}


void TestPiiMath::invert()
{
  PiiMatrix<double> mat(1,1, 0.5);
  mat.map(Pii::Invert<double>());
  QCOMPARE(mat(0,0), 2.0);
}

void TestPiiMath::matrixArithmetic()
{
  QVERIFY(Pii::equals(4 + PiiMatrix<int>(1,1, 5), PiiMatrix<int>(1,1, 9)));
  QVERIFY(Pii::equals(1 - PiiMatrix<int>(1,1, 5), PiiMatrix<int>(1,1, -4)));
  QVERIFY(Pii::equals(3 * PiiMatrix<int>(1,1, 5), PiiMatrix<int>(1,1, 15)));
  QVERIFY(Pii::equals(PiiMatrix<double>(1,1, 25.0)/PiiMatrix<double>(1,1, 5.0), PiiMatrix<double>(1,1, 5.0)));
}


void TestPiiMath::var()
{
  PiiMatrix<int> mat(3, 2, 1, 1, -1, -1, 2, -2);
  QVERIFY( almostEqual(Pii::var<double>(mat, 0), 2.0) );
  QVERIFY((Pii::almostEqual(Pii::var<double>(mat, Pii::Horizontally), PiiMatrix<double>(3, 1, 0.0, 0.0, 4.0), tol)) );
  QVERIFY((Pii::almostEqual(Pii::var<double>(mat, Pii::Vertically), PiiMatrix<double>(1, 2, 14.0/9.0, 14.0/9.0), tol)) );
}

void TestPiiMath::std()
{
  {
    PiiMatrix<char> mat(2, 3, 1, 0, 1, -1, 0, -1);
    QVERIFY(Pii::almostEqual(Pii::std<double>(mat, Pii::Horizontally), PiiMatrix<double>(2, 1, ::sqrt(2.0)/3.0, ::sqrt(2.0)/3.0), tol));
    QVERIFY(Pii::almostEqual(Pii::std<double>(mat, Pii::Vertically), PiiMatrix<double>(1, 3, 1.0, 0.0, 1.0), tol));
  }
  {
    PiiMatrix<char> mat(2, 3, 0, 1, 0, 1, 1, 0);
    QVERIFY(almostEqual(Pii::std<double>(mat), 0.5));
  }
}

void TestPiiMath::squaredDistance()
{
#define ARRAYELEMCOUNT(x) (sizeof(x) / sizeof(x[0]))
  char array0[2] = {1, 127};
  char array1[2] = {-127,-1};
  QVERIFY(almostEqual(Pii::squaredDistanceN(array0, ARRAYELEMCOUNT(array0), array1, 0.0), 32768.0) );

  {
    PiiMatrix<char> mat0(2, 1, 2, 3);
    PiiMatrix<char> mat1(2, 1, -1, -1);
    QVERIFY(almostEqual(Pii::squaredDistance(mat0.columnBegin(0), mat0.columnEnd(0), mat1.columnBegin(0), 0.0), 25.0) );
  }

  {
    PiiMatrix<char> mat0(2, 1, 100, 90);
    PiiMatrix<char> mat1(2, 1, -38, -80);
    QCOMPARE(Pii::squaredDistanceN(mat0.columnBegin(0), 2, mat1.columnBegin(0), 0.0), 47944.0);
  }

  //Squared distance for arrays.
  {
    PiiMatrix<char> mat0(2, 2,  1,  1,  1,  1);
    PiiMatrix<char> mat1(2, 2, -1, -1, -1, -1);
    QCOMPARE(Pii::squaredDistance(mat0.begin(), mat0.end(), mat1.begin(), 0.0), 16.0);
  }
}


void TestPiiMath::square()
{
  QCOMPARE(Pii::square(-4), 16);
  QCOMPARE(Pii::square(3.0), 9.0);
  QVERIFY(Pii::equals(Pii::square(PiiMatrix<int>(2, 1, -2, 3)), PiiMatrix<int>(2, 1, 4, 9)));
}


void TestPiiMath::pseudoInverse()
{
  PiiMatrix<float> mat(3,2, 4., -1., 0., 8., 9., 1.);
  QVERIFY( Pii::almostEqual(Pii::pseudoInverse(mat), PiiMatrix<float>(2,3, 0.042182844598, -0.006272541948, 0.092363180179, -0.018347185197, 0.121687313784, 0.008154304532), tolF)  );
}


void TestPiiMath::pivot()
{
  PiiMatrix<int> mat(3,3,
                     7, 3, 0,
                     3, -10, 0,
                     1, 5, 0);
  QCOMPARE(Pii::pivot(mat.columnBegin(0), 3), 0);
  QCOMPARE(Pii::pivot(mat.columnBegin(1), 3), 1);
  QCOMPARE(Pii::pivot(mat.columnBegin(2), 3), -1);
}


void TestPiiMath::norm()
{
  PiiMatrix<int> matI(2,3,  1,2,3,
                      -1300,-5,6);
  PiiMatrix<double> matD(2,3, 1.0, 2.0, 3.0,
                         -1300.0, -5.0, 6.0);

  PiiMatrix<char> matCh(1,2, 11, 11);
  PiiMatrix<char> matCh2(1,2, 100, 100);


  PiiMatrix<std::complex<double> > matC(1,2);
  matC(0,0) = std::complex<double>(3,4);
  matC(0,1) = std::complex<double>(0,2);

  PiiMatrix<std::complex<float> > matCF(1,2);
  matCF(0,0) = std::complex<float>(3,4);
  matCF(0,1) = std::complex<float>(0,2);

  //L1
  QCOMPARE( Pii::norm1(matI), 1317.0 );
  QCOMPARE( Pii::norm(matI, 1), Pii::norm1(matI) );
  QVERIFY( almostEqual(Pii::norm1(matD), 1317.0) );
  QCOMPARE( Pii::norm(matC, 1), 7.0 );
  //QCOMPARE( Pii::norm(matCh2, 1), 200.0 );
  QCOMPARE( Pii::norm(matCF, 1), 7.0 );

  //L2
  QVERIFY( almostEqual(Pii::norm(matI, 2), 1300.02884583381) );
  QVERIFY( almostEqual(Pii::norm(matD, 2), 1300.02884583381) );
  QCOMPARE( Pii::norm(matC, 2), ::sqrt(29.0) );
  QVERIFY( Pii::almostEqualRel(float(Pii::norm(matCF, 2)), ::sqrtf(29.0F)) );
  QCOMPARE( Pii::norm(matCh, 2), ::sqrt(242.0) );

  //L3
  QVERIFY( almostEqual(Pii::norm(matD, 3), 1300.00007435897) );
  QCOMPARE( Pii::norm(matC, 3), ::pow(133.0, 1.0/3.0) );
  QVERIFY( Pii::almostEqualRel(float(Pii::norm(matCF, 3)), ::powf(133.0F, 1.0F/3.0F)) );

  //L12
  QCOMPARE( Pii::norm(matD, 12), ::pow(1.0 + 4096.0 + 531441.0 + ::pow(1300.0, 12.0) + ::pow(5.0, 12.0) + ::pow(6.0, 12.0), 1.0/12.0) );
  QCOMPARE( Pii::norm(matC, 12), ::pow(244144721.0, 1.0/12.0) );
  QVERIFY( Pii::almostEqualRel( float(Pii::norm(matCF, 12)), ::powf(244144721.0F, 1.0F/12.0F)) );

  //L_inf
  QCOMPARE( Pii::norm(matI, -1), 1300.0);
  QCOMPARE( Pii::norm(matD, -1), 1300.0);
  QCOMPARE( Pii::norm(matC, -1), 5.0);
  QCOMPARE( Pii::norm(matCF, -1), 5.0);
}


void TestPiiMath::multiplyTransposed()
{
  PiiMatrix<int> a(3,2, 4, -1,
                   4, 6,
                   0, 9);
  PiiMatrix<int> b(2,3, 2, 3, 1,
                   -8, 3, 4);
  const PiiMatrix<int> ab(3,3, 16, 9, 0,
                          -40, 30, 28,
                          -72, 27, 36);
  const PiiMatrix<int> ba(2,2, 20, 25,
                          -20, 62);
  const PiiMatrix<int> ba_aT(2, 3, 55, 230, 225,
                             -142, 292, 558);
  const PiiMatrix<int> bT_ba(3, 2, 200, -446,
                             0,   261,
                             -60,  273);
  const PiiMatrix<int> bT_aT(3, 3, 16, -40, -72,
                             9,  30, 27,
                             0,  28, 36);
  QVERIFY(Pii::equals(ab, a*b));
  QVERIFY(Pii::equals(ba, b*a));
  QVERIFY(Pii::equals(ba_aT, ba * Pii::transpose(a)));
  QVERIFY(Pii::equals(bT_ba, Pii::transpose(b) * ba));
  QVERIFY(Pii::equals(bT_aT, Pii::transpose(b) * Pii::transpose(a)));

  //Test multiplication of incompatible matrices (should throw exception).
  try
    {
      a = Pii::transpose(a) * b;
      QFAIL("Matrix sizes don't match. Multiplication should fail.");
    }
  catch (PiiException&)
    {
    }
}

void TestPiiMath::multiply()
{
  {
    const PiiMatrix<int> mat(2,3, -4, 5 , 6,
                             4, 10, 0);
    int array[3] = {1,2,3};
    int result[2];
    const int resultExpected[2] = {24, 24};
    Pii::multiply(mat, array, result);
    QVERIFY( !std::memcmp(result, resultExpected, sizeof(result)) );
  }

  {
    PiiMatrix<int> mat(4,3,
                       1,2,3,
                       4,5,6,
                       7,8,9,
                       0,1,2);
    PiiVector<int,4> vector(1,2,3,4);
    PiiVector<double,3> result;
    // Calculate (1,2,3,4) * mat and store the resulting 1-by-3 vector
    // as doubles.
    Pii::multiply(vector.begin(), mat, result.begin());
    QCOMPARE(result[0], double(1*1+2*4+7*3+0*4));
    QCOMPARE(result[1], double(1*2+2*5+3*8+4*1));
    QCOMPARE(result[2], double(1*3+2*6+3*9+4*2));
  }
}

void TestPiiMath::multiplied()
{
  const PiiMatrix<int> mat1(2,3, -4, 6, 2,
                            1, 0, 8);
  const PiiMatrix<int> mat2(2,3, -7, 1, 2,
                            1, 8, 3);
  const PiiMatrix<int> matM(2,3, 28, 6, 4, 1, 0, 24);
  QVERIFY(Pii::equals(Pii::multiplied(mat1, mat2), matM));
}


void TestPiiMath::minMax()
{
  PiiMatrix<int> mat(2,3, 8, 51023, -15613032, 51600320, 891000, 0, 4);
  int min, max;
  int minR, minC, maxR, maxC;
  Pii::minMax(mat, &min, &max);
  Pii::minMax(mat, &min, &max, &minR, &minC, &maxR, &maxC);
  QCOMPARE( min, -15613032 );
  QCOMPARE( max, 51600320 );
  QCOMPARE( minR, 0 );
  QCOMPARE( minC, 2 );
  QCOMPARE( maxR, 1 );
  QCOMPARE( maxC, 0 );
}

void TestPiiMath::mean()
{
  QVERIFY(Pii::almostEqual(Pii::mean<double>(matItest, Pii::Horizontally), matItestMeanAlongRow, tol));
  QVERIFY(Pii::almostEqual(Pii::mean<double>(matItest, Pii::Vertically), matItestMeanAlongColumn, tol));

  PiiMatrix<char> mat(2,2,100,100,100,100);
  QVERIFY(Pii::almostEqual(Pii::mean<double>(mat, Pii::Vertically), PiiMatrix<double>(1,2,100.0,100.0), tol));
  QVERIFY(Pii::almostEqual(Pii::mean<double>(mat, Pii::Horizontally), PiiMatrix<double>(2,1,100.0,100.0), tol));

  QVERIFY(almostEqual(Pii::mean<double>(matItest), matItestMeanAll) );

  PiiMatrix<char> mat2(1,2,100,100);
  QVERIFY( almostEqual(Pii::mean<double>(mat2), 100.0) );
}


void TestPiiMath::isSingular()
{
  PiiMatrix<float> matSingular(2,2, M_PI, M_PI, M_E * M_PI, M_E * M_PI);
  QVERIFY( Pii::isSingular(matSingular) == true );
  matSingular(0,0) += 0.00001F;
  QVERIFY( Pii::isSingular(matSingular) == false );

  PiiMatrix<int> mat(2,2, 1, 1, 3, 2);
  QVERIFY( Pii::isSingular(mat) == false);

  PiiMatrix<int> mat2(3,3, 1, 1, 1, 2, 2, 2, 3, 3, 3);
  QVERIFY( Pii::isSingular(mat2) == true);

  QVERIFY( Pii::isSingular(matItest) == false );
}


void TestPiiMath::isDiagonal()
{
  QVERIFY( Pii::isDiagonal(matItest) == false );
  PiiMatrix<float> mat(2,2, 1.5, 0.0, 0.0, 5.8);
  QVERIFY( Pii::isDiagonal(mat) == true );
  mat(1,0) = 10;
  QVERIFY( Pii::isDiagonal(mat) == false );
}


void TestPiiMath::isAntiSymmetric()
{
  QVERIFY( Pii::isAntiSymmetric(matItest) == false );
  PiiMatrix<float> mat(2,2, 0.0, 5.0, 5.0, 0.0);
  QVERIFY( Pii::isAntiSymmetric(mat) == false );
  mat(0,1) = -mat(0,1);
  QVERIFY( Pii::isAntiSymmetric(mat) == true );
}


void TestPiiMath::isSymmetric()
{
  QVERIFY( Pii::isSymmetric(matItest) == false );
  PiiMatrix<float> mat(2,2, 0.0, 5.0, 5.0, 0.0);
  QVERIFY( Pii::isSymmetric(mat) == true );
  mat(0,1) = -mat(0,1);
  QVERIFY( Pii::isSymmetric(mat) == false );
}


void TestPiiMath::inverse()
{
  PiiMatrix<float> mat(4,4, 2.0,  1.0, 1.0,   2.0,
                       -5.7,  2.5, 144.0, 4.12,
                       84.1, 1.1, 1.0,    0.2,
                       5.4,  2.4, 4.7,    0.1);
  QVERIFY( Pii::almostEqual(mat * Pii::inverse(mat), PiiMatrix<float>::identity(4), tolF) );
}


void TestPiiMath::gcd()
{
  QCOMPARE(Pii::gcd(50, 25), 25);
  QCOMPARE(Pii::gcd(5008, 630), 2);
  QCOMPARE(Pii::gcd(25290, 14265), 45);
  QCOMPARE(Pii::gcd(25290, 0), 25290);
  QCOMPARE(Pii::gcd(0, 0), 0);
}


void TestPiiMath::find()
{
  PiiMatrix<int> a = Pii::find(matItest);
  QVERIFY(Pii::equals(a, PiiMatrix<int>(6,2, 0,0, 0,1, 0,2, 1,0, 1,1, 1,2)));
  PiiMatrix<float> b(2,2, 0.0, 2.0, 0.0, 1.0);
  a = Pii::find(b);
  QVERIFY(Pii::equals(a, PiiMatrix<int>(2,2, 0,1, 1,1)));

  QVERIFY(Pii::equals(Pii::find(b, -1), Pii::find(b, 1)));
  QVERIFY(Pii::equals(Pii::find(b), Pii::find(b, 2)));
  QVERIFY(Pii::equals(Pii::find(b), Pii::find(b, 10)));
  QCOMPARE(Pii::find(b, -2).rows(), 0);
  QCOMPARE(Pii::find(b, -3).rows(), 0);

  QVERIFY(Pii::equals(Pii::find(matItest, std::bind2nd(std::greater<int>(), 0)),
                      PiiMatrix<int>(4,2, 0,0, 0,2, 1,0, 1,1)));
}


void TestPiiMath::factorial()
{
  QCOMPARE(Pii::factorial(0), 1);
  QCOMPARE(Pii::factorial(11), 39916800);
  QVERIFY( almostEqual(Pii::factorial(18.0), 6402373705728000.0) );
}



void TestPiiMath::innerProduct()
{
  //innerProduct calculates something also for squarematrices.
  {
    PiiMatrix<int> m1(2,2, 5, 3, 8, -1);
    PiiMatrix<int> m2(2,2, 7, 0, -9, 500);
    QCOMPARE(Pii::innerProduct(m1, m2), -537);
  }

  //Dotproduct of matrices with different size should throw exception.
  try
    {
      PiiMatrix<double> m1(1,2, 5, 3);
      PiiMatrix<double> m2(2,2, 7, 0, -9, 500);
      Pii::innerProduct(m1, m2);
      QVERIFY(false);
    }
  catch(PiiException&)
    {
      QVERIFY(true);
    }

  {
    PiiMatrix<double> m1(3,1, 8.4, -3.7, 0.002);
    PiiMatrix<double> m2(3,1, -8582.0, 1.0, 7.9);
    QVERIFY( almostEqual(Pii::innerProduct(m1, m2), -72092.4842) );
  }

  {
    PiiMatrix<double> m1(1,3, 8.4, -3.7, 0.002);
    PiiMatrix<double> m2(1,3, -8582.0, 1.0, 7.9);
    QVERIFY( almostEqual(Pii::innerProduct(m1, m2), -72092.4842) );
  }
  {
    PiiMatrix<double> m(2, 1, 1.0, -1.0);
    double dot = Pii::innerProduct(m.columnBegin(0),
                                   m.columnEnd(0),
                                   m.columnBegin(0),
                                   0.0);
    QCOMPARE(dot, 2.0);
  }
  {
    PiiMatrix<double> m (1, 2, 1.0, -1.0);
    double dot = Pii::innerProduct(m.rowBegin(0),
                                   m.rowEnd(0),
                                   m.rowBegin(0),
                                   0.0);
    QCOMPARE(dot, 2.0);
  }
}


void TestPiiMath::divided()
{
  {
    PiiMatrix<int> m1(2,2, 4, 8, 9, -84);
    PiiMatrix<int> m2(2,2, 2, 8, 5, 13);
    QVERIFY(Pii::equals(Pii::divided(m1, m2), PiiMatrix<int>(2,2, 2, 1, 1, -6)));
  }

  //'divided' with matrices with different size should throw exception.
  try
    {
      PiiMatrix<double> m1(1, 2, 5, 3);
      PiiMatrix<double> m2(2,2, 7, 0, -9, 500);
      Pii::divided(m1, m2);
      QFAIL("Matrix division should fail with incompatible sizes.");
    }
  catch(PiiException&)
    {
    }

  {
    PiiMatrix<double> m1(1,3, 1.0, 2.0, 3.0);
    PiiMatrix<double> m2(1,3, -10.0, 20.0, 30.0);
    QVERIFY(Pii::almostEqual(Pii::divided(m1, m2), PiiMatrix<double>(1,3, -0.1, 0.1, 0.1), tol));
  }
}


void TestPiiMath::distance()
{
  {
    int* p = 0;
    QCOMPARE(Pii::distanceN(p, 0, p, 0.0), 0.0);
    int a[3] = {5, -7, 321};
    int b[3] = {8, -3, 7};
    QVERIFY(almostEqual(Pii::distanceN(a,3,b,0.0), Pii::sqrt(98621.0)) );
  }

  {
    PiiMatrix<double> m1(1,1, 4.0);
    PiiMatrix<double> m2(1,2, 1.0, 2.0);
    PiiMatrix<double> result(Pii::distance(m1, m2, Pii::Horizontally));
    // m1 and m2 are not equal in size
    QVERIFY(result.isEmpty());
  }

  {
    PiiMatrix<double> m1(3,1, 1.5, 2.0, 2.5);
    PiiMatrix<double> m2(3,1, -1.5, -2.0, -2.5);
    QVERIFY(Pii::almostEqualRel(Pii::distanceN(m1.rowBegin(0), 3, m2.rowBegin(0), 0.0), Pii::sqrt(50.0)));
  }
}


void TestPiiMath::determinant()
{
  {
    PiiMatrix<double> mat(3,3,
                          M_PI , M_E , M_SQRT2,
                          2.0  , 3.0 ,    4.0 ,
                          8.0  , 9.0 , -400.0);
    QCOMPARE( Pii::determinant(mat), M_PI * (3.0*(-400.0) - 4.0*9.0) - M_E*(2.0*(-400.0) - 4.0*8.0) + M_SQRT2 * (2.0*9.0-3.0*8.0) ) ;
  }

  {
    PiiMatrix<double> mat(2,2,
                          M_PI ,   M_E ,
                          M_SQRT2, M_PI);
    QCOMPARE( Pii::determinant(mat), M_PI * M_PI - M_E * M_SQRT2 ) ;
  }

  {
    PiiMatrix<double> mat(5,5, 4.0, 3.0 , 2.0, 9.0 , 1.0,
                          1.0, 3.0 , 5.0, -1.0, 2.5,
                          7.0, 9.0 , 8.0, 1.0 , 4.0,
                          7.0, 56.0, 2.0, 3.0 , 1.0,
                          0.0, 4.0 , 7.0, 1.0,  4.0);
    QVERIFY( almostEqual(Pii::determinant(mat), -5143.5) );
  }

  {
    PiiMatrix<int> mat(3,3,
                       2   ,   -15,    4   ,
                       71  ,    13, -15641,
                       0   ,  9999, 71347);
    QCOMPARE( Pii::determinant(mat), 393468011 );
  }

  {
    PiiMatrix<float> mat(1,2, 1.0, 2.0);
    try
      {
        Pii::determinant(mat); //Should cause exception to be thrown.
        QVERIFY(false);
      }
    catch(...)
      {
        QVERIFY(true);
      }
  }

  {
    PiiMatrix<double> matD(3,3, 4.0, 8.0, -7.2, 85.3, -6.31, 7.4, 12.3, 4.1, 11.1);
    QVERIFY(almostEqual(Pii::determinant(matD), -10324.8736) );
  }
}


void TestPiiMath::crossProduct()
{
  {
    bool bExceptionThrown = false;
    PiiMatrix<float> mat(1,2, 1.0, 2.0);

    try
      {
        Pii::crossProduct(mat, mat); //Should cause exception to be thrown.
      }
    catch(...)
      {
        bExceptionThrown = true;
      }
    QVERIFY( bExceptionThrown == true );
  }

  PiiMatrix<float> m0(3,1, 1.0, 2.0, 3.0);
  QVERIFY( Pii::almostEqual(PiiMatrix<float>(3,1, 0.0, 0.0, 0.0), Pii::crossProduct(m0, m0), tolF) );
  PiiMatrix<float> m1(3,1, 2.0, 1.3, 5.4);
  QVERIFY( Pii::almostEqual(PiiMatrix<float>(3,1, 6.9, 0.6, -2.7), Pii::crossProduct(m0, m1), tolF) );
}


void TestPiiMath::covariance()
{
  PiiMatrix<float> m(3,4, 1.0, 2.0, 3.0,  4.0,
                     -4.0, 8.0, 7.0, -4.0,
                     8.0, 4.0, 5.0, -9.0);
  const PiiMatrix<double> mMeanExpected(1,4, 5.0/3.0, 14.0/3.0, 5.0, -3.0);
  // Expecting sum( (ai - <a>)(bi - <b>) ) / (measurementNumber(==3)-1)
  PiiMatrix<double> mCovExpected(4,4, 218.0/9.0 , -64.0/9.0 , -10.0/3.0 , -37.0/3.0   ,
                                 -64.0/9.0 , 56.0/9.0  ,  4.0      ,      -6.0   ,
                                 -10.0/3.0 , 4.0       ,  8.0/3.0  ,   -16.0/3.0 ,
                                 -37.0/3.0 , -6.0      , -16.0/3.0 ,    86.0/3.0 );
  // mCovExpected is written assuming 3 as divisor, but as divisor in covariance was changed
  // from n to n-1, multiplying it with 3.0/2.0 to get the new expected result.
  mCovExpected *= 3.0/2.0;
  PiiMatrix<double> mMean;
  Pii::covariance(m, &mMean);
  QVERIFY( Pii::almostEqual(mCovExpected, Pii::covariance(m), tol) );
  QVERIFY( Pii::almostEqual(mMeanExpected, mMean, tol) );
}


void TestPiiMath::conj()
{
  QVERIFY(Pii::equals(matItest, Pii::conj(matItest)));

  PiiMatrix<double> mD(2,2, 1.0, -3.0, 4.0, 8456.79210);
  QVERIFY(Pii::equals(mD, Pii::conj(mD)));

  PiiMatrix<std::complex<double> > mC(1,2);
  mC(0,0) = std::complex<double>(2, 1);
  mC(0,1) = std::complex<double>(5, -7);
  PiiMatrix<std::complex<double> > mCExpected(1, 2);
  mCExpected(0,0) = std::complex<double>(2, -1);
  mCExpected(0,1) = std::complex<double>(5, 7);
  QVERIFY(Pii::equals(Pii::conj(mC), mCExpected));
}


void TestPiiMath::almostEqualTest()
{
  const float tolf = 1e-3F;
  PiiMatrix<float> m0(2,1, 1.0, 1.0);
  PiiMatrix<float> m1(2,1, 1.0+tolf*0.999, 1.0);
  QVERIFY( Pii::almostEqual(m0, m1, tolf) );
  QVERIFY( !Pii::almostEqual(m0, m1, tolf/2) );

  PiiMatrix<double> mat1(1,1, 1.0), mat2(1,1, 1.01);
  QVERIFY( Pii::almostEqual(mat1, mat2, 0.1) );
}

void TestPiiMath::almostEqualRel()
{
  QVERIFY( Pii::almostEqualRel(0.0f, Pii::Numeric<float>::tolerance()) == true );
  QVERIFY( Pii::almostEqualRel(0.0f, 1.00001f*Pii::Numeric<float>::tolerance()) == false );
  QVERIFY( Pii::almostEqualRel(0.0, Pii::Numeric<double>::tolerance()) == true );
  QVERIFY( Pii::almostEqualRel(0.0, 1.00001*Pii::Numeric<double>::tolerance()) == false );

  QVERIFY( Pii::almostEqualRel(0.0, 1e-50) == true );
  QVERIFY( Pii::almostEqualRel(1e-45, 1e-50) == false );
  QVERIFY( Pii::almostEqualRel(1e15, 1e15 - 1) == true );
  QVERIFY( Pii::almostEqualRel(1e15, 1e15 - 1e10) == false );
  QVERIFY( Pii::almostEqualRel(1e15, 1e15-1, 1e-16) == false );
  QVERIFY( Pii::almostEqualRel(1e15, 1e15-1, 1.00001e-15) == true );

  QVERIFY( Pii::almostEqualRel(1e10 + 1, 1e10, 1e-10) == true );
  QVERIFY( Pii::almostEqualRel(1e10 + 1, 1e10, 0.9999e-10) == false );

  QVERIFY( Pii::almostEqualRel(5.0, 4.9999) == false );
  QVERIFY( Pii::almostEqualRel(5.0, 4.999999999999) == true );
}

void TestPiiMath::hypotenuse()
{
  QCOMPARE(Pii::hypotenuse(3.0, 4.0), 5.0);
  QCOMPARE(Pii::hypotenuse(1.0f, 1.0f), float(M_SQRT2));
}

void TestPiiMath::realAndImag()
{
  using std::complex;
  PiiMatrix<complex<float> > mat(2,2);
  mat(0,0) = complex<float>(1,2); mat(0,1) = complex<float>(3,4);
  mat(1,0) = complex<float>(5,6); mat(1,1) = complex<float>(7,8);
  QCOMPARE(Pii::real(std::complex<float>(1.3F, 2.0F)), 1.3f);
  QCOMPARE(Pii::imag(std::complex<float>(1.3F, 2.0F)), 2.0f);
  QCOMPARE(Pii::real(std::complex<double>(1.3, 2.0)), 1.3);
  QCOMPARE(Pii::imag(std::complex<double>(1.3, 2.0)), 2.0);
  QCOMPARE(Pii::real(5.0), 5.0);
  QCOMPARE(Pii::imag(5.0), 0.0);

  QVERIFY(Pii::equals(Pii::real(mat), PiiMatrix<float>(2,2,
                                                           1.0,3.0,
                                                           5.0,7.0)));

  QVERIFY(Pii::equals(Pii::imag(mat), PiiMatrix<float>(2,2,
                                                           2.0,4.0,
                                                           6.0,8.0)));

  QVERIFY(Pii::equals(Pii::real(PiiMatrix<int>(1,1,1)), PiiMatrix<int>(1,1,1)));
  QVERIFY(Pii::equals(Pii::imag(PiiMatrix<int>(1,1,1)), PiiMatrix<int>(1,1)));
}

void TestPiiMath::Abs()
{
  char a = -128;
  int b = Pii::abs(a);
  b = std::abs(a);
  b = abs(int(a));
  b = std::abs(-2147483647 - 1);
  Q_UNUSED(b);

  QVERIFY(Pii::equals(matItest.mapped(Pii::Abs<int>()), matItestAbs));

  PiiMatrix<std::complex<double> > m(1,2);
  m(0,0) = std::complex<double>(3,4);
  m(0,1) = std::complex<double>(0,4);
  m.map( Pii::Abs<std::complex<double> >() );
  QCOMPARE( m(0,0), std::complex<double>(5,0) );
  QCOMPARE( m(0,1), std::complex<double>(4,0) );
}

void TestPiiMath::IeeeFloat()
{
  {
    Pii::IeeeFloat<float> f(-1);
    QCOMPARE(f.rawSign(), 1u);
    QCOMPARE(f.sign(), -1);
    QCOMPARE(f.exponent(), 0);
    f.setSign(1);
    f.addExponent(2);
    QCOMPARE(f.value(), 4.0f);
  }
  {
    Pii::IeeeFloat<double> f(5);
    QCOMPARE(f.rawSign(), 0ull);
    QCOMPARE(f.sign(), 1ll);
    QCOMPARE(f.exponent(), 2ll);
    f.setRawSign(1);
    f.addExponent(2);
    QCOMPARE(f.value(), -20.0);
  }
  {
    Pii::IeeeFloat<double> f(16);
    f.addExponent(-4);
    QCOMPARE(f.value(), 1.0);
  }
}

void TestPiiMath::AbsDiff()
{
  QVERIFY(Pii::equals(matItest.mapped(Pii::AbsDiff<int>(), 5), PiiMatrix<int>(2,3, 4, 10546770, 189806, 5769, 1651647, 6)));
}


void TestPiiMath::AbsPow()
{
  {
    PiiMatrix<int> m(1,2, 2, -3);
    m.map(Pii::AbsPow<double, double>(), 3.0);
    QVERIFY(Pii::equals(m, PiiMatrix<int>(1,2, 8, 27)) );
  }

  {
    PiiMatrix<std::complex<double> > m(1,1);
    m(0,0) = std::complex<double>(3,-4);
    m.map(Pii::AbsPow<std::complex<double> >(), 2.0);
    QCOMPARE( m(0,0), std::complex<double>(25.0) );
  }
}


void TestPiiMath::AbsSum()
{
  PiiMatrix<int> m(1,2, 2, -3);
  m.map(Pii::AbsSum<int>(), -3);
  QVERIFY(Pii::equals(m, PiiMatrix<int>(1,2, 5, 6)));
}


void TestPiiMath::Atan2()
{
  PiiMatrix<double> m(1,2, 2.0, -3.0);
  m.map(Pii::Atan2<double>(), 7);
  QVERIFY(Pii::almostEqual(m, PiiMatrix<double>(1,2, atan(2.0/7.0), atan(-3.0/7.0)), tol));
}


void TestPiiMath::Hypotenuse()
{
  PiiMatrix<double> m(1,2, 20000.0, -3.0);
  m.map(Pii::Hypotenuse<double>(), 4.0);
  QVERIFY(Pii::equals(m, PiiMatrix<double>(1,2, ::sqrt(400000016.0), 5.0)));
}


void TestPiiMath::Log()
{
  PiiMatrix<double> m(1,2, M_E, 7.38905609893);
  m.map(Pii::Log<double>());
  QVERIFY(Pii::almostEqual(m, PiiMatrix<double>(1,2, 1.0, 2.0), tol));
}


void TestPiiMath::Max()
{
  PiiMatrix<double> m(1,3, M_E, 3*M_E, 10*M_PI);
  m.map(Pii::Max<double>(), 10);
  QVERIFY(Pii::almostEqual(m, PiiMatrix<double>(1,3, 10.0, 10.0, 10*M_PI), tol));
}

void TestPiiMath::Min()
{
  PiiMatrix<double> m(1,3, M_E, 3*M_E, 10*M_PI);
  m.map(Pii::Min<double>(), M_PI);
  QVERIFY(Pii::almostEqual(m, PiiMatrix<double>(1,3, M_E, M_PI, M_PI), tol));
}


void TestPiiMath::Pow()
{
  {
    PiiMatrix<int> m(1,2, 2, 3);
    m.map(Pii::Pow<double>(), 1.5);
    QVERIFY(Pii::equals(m, PiiMatrix<int>(1,2, 2, 5)));
  }

  {
    PiiMatrix<double> m(1,2, 2.0, 3.0);
    m.map(Pii::Pow<double>(), 1.5);
    QVERIFY(Pii::equals(m, PiiMatrix<double>(1,2, ::pow(2.0, 1.5), ::pow(3.0, 1.5))));
  }
}


void TestPiiMath::Round()
{
  PiiMatrix<double> m(1,2, 2.5, -3.4);
  m.map(Pii::Round<double>());
  QVERIFY(Pii::equals(m, PiiMatrix<double>(1,2, 3.0, -3.0)));
}


void TestPiiMath::Sqrt()
{
  PiiMatrix<int> m(1,2, 9, 16);
  m.map(Pii::Sqrt<double>());
  QVERIFY(Pii::equals(m, PiiMatrix<int>(1,2, 3, 4)));
}


void TestPiiMath::Square()
{
  PiiMatrix<int> m(1,2, 3, 4);
  m.map(Pii::Square<double>());
  QVERIFY(Pii::equals(m, PiiMatrix<int>(1,2, 9, 16)));
}

void TestPiiMath::Mean()
{
  PiiMatrix<int> mat(1,5, 1,2,3,4,5);
  QCOMPARE(Pii::forEach(mat.begin(), mat.end(), Pii::Mean<int,float>()).mean(),
           3.0f);
}

void TestPiiMath::Exp()
{
  const PiiMatrix<double> matd(1,2, 0.0, 1.0);
  const PiiMatrix<float> matf(matd);
  const PiiMatrix<int> mati(matd);
  const PiiMatrix<double> result(1,2, 1.0, M_E);
  QVERIFY(Pii::almostEqual(matf.mapped(Pii::Exp<float>()), result, 1e-6));
  QVERIFY(Pii::almostEqual(matd.mapped(Pii::Exp<double>()), result, 1e-6));
}

void TestPiiMath::Sin()
{
  const PiiMatrix<double> matd(1,2, 0.0, M_PI/2);
  const PiiMatrix<float> matf(matd);
  const PiiMatrix<double> result(1,2, 0.0, 1.0);
  QVERIFY(Pii::almostEqual(matf.mapped(Pii::Sin<float>()), PiiMatrix<float>(result), 1e-6));
  QVERIFY(Pii::almostEqual(matd.mapped(Pii::Sin<double>()), result, 1e-6));
}

void TestPiiMath::Cos()
{
  const PiiMatrix<double> matd(1,2, 0.0, M_PI/2);
  const PiiMatrix<float> matf(matd);
  const PiiMatrix<double> result(1,2, 1.0, 0.0);
  QVERIFY(Pii::almostEqual(matf.mapped(Pii::Cos<float>()), PiiMatrix<float>(result), 1e-6));
  QVERIFY(Pii::almostEqual(matd.mapped(Pii::Cos<double>()), result, 1e-6));
}

void TestPiiMath::Tan()
{
  const PiiMatrix<double> matd(1,2, 0.0, M_PI/4);
  const PiiMatrix<float> matf(matd);
  const PiiMatrix<double> result(1,2, 0.0, 1.0);
  QVERIFY(Pii::almostEqual(matf.mapped(Pii::Tan<float>()), PiiMatrix<float>(result), 1e-6));
  QVERIFY(Pii::almostEqual(matd.mapped(Pii::Tan<double>()), result, 1e-6));
}

void TestPiiMath::eigen()
{
#if 0
  const bool bDoAssertPrints = false;

#define DO_EIGEN_ASSERTS(T, Real, tolerance)                            \
  {                                                                     \
    const PiiMatrix<Real>& D = eigendata.blockDiagonalEigenvalMatrix(); \
    const PiiMatrix<Real>& V = eigendata.eigenvectors();                \
    const PiiMatrix<Real>& eR = eigendata.eigenvaluesR();               \
    const PiiMatrix<Real>& eI = eigendata.eigenvaluesI();               \
    if(bDoAssertPrints == true)                                         \
      {                                                                 \
        PrintMatrix(mat);                                               \
        PrintMatrix(eR);                                                \
        PrintMatrix(eI);                                                \
        PrintMatrix(V);                                                 \
        PrintMatrix(D);                                                 \
        PrintMatrix(((PiiMatrix<Real>)mat)*V);                          \
        PrintMatrix(V*D);                                               \
      }                                                                 \
    if( Pii::isSymmetric(mat) )                                     \
      QVERIFY( Pii::isOrthogonalLike(V) );                          \
    QVERIFY( Pii::almostEqual(((PiiMatrix<Real>)mat)*V, V*D, tolerance) ); \
    if(!eRealExpected.isEmpty()) QVERIFY(Pii::almostEqual(eR, eRealExpected, tolerance)); \
    if(!eImagExpected.isEmpty()) QVERIFY(Pii::almostEqual(eI, eImagExpected, tolerance)); \
  }

  //3x3 matrix with complex eigenvalues.
  {
    PiiMatrix<double> mat(3,3, 2.0, 3.0 , 4.0,
                          5.0,  8.0 , -4.0,
                          1.0,  3.0 , -2.0);
    const PiiMatrix<double> eRealExpected(1,3, 9.56531116178, -0.782655580887, -0.782655580887);
    const PiiMatrix<double> eImagExpected(1,3, 0.0, 1.83306807541, -1.83306807541);

    Pii::EigenSystem<double, double> eigendata;
    Pii::eigensolve(mat, eigendata);
    Pii::EigenSystem<double, double> eigendata2 = Pii::eigensolve(mat);
    for(int i = eigendata.eigenvalues().columns(); i--;)
      {
        QVERIFY( eigendata.eigenvalues()(0,i).real() == eigendata.eigenvaluesR()(0,i) );
        QVERIFY( eigendata.eigenvalues()(0,i).imag() == eigendata.eigenvaluesI()(0,i) );
      }
    QVERIFY(Pii::equals(eigendata.eigenvalues(), eigendata2.eigenvalues()));
    QVERIFY(Pii::equals(eigendata.eigenvaluesR(), eigendata2.eigenvaluesR()));
    QVERIFY(Pii::equals(eigendata.eigenvaluesI(), eigendata2.eigenvaluesI()));
    QVERIFY(Pii::equals(eigendata.eigenvectors(), eigendata2.eigenvectors()));
    QVERIFY(Pii::equals(eigendata.blockDiagonalEigenvalMatrix(), eigendata2.blockDiagonalEigenvalMatrix()));

    DO_EIGEN_ASSERTS(double, double, tol);
  }

  //1x1 matrix
  {
    PiiMatrix<double> mat(1,1, 1.0);
    const PiiMatrix<double> eRealExpected(1,1, 1.0);
    const PiiMatrix<double> eImagExpected(1,1, 0.0);
    Pii::EigenSystem<double, double> eigendata;
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(double, double, tol);
  }

  //2x2 symmetric int matrix
  {
    PiiMatrix<int> mat(2,2, 2, 0, 0, 2);
    Pii::EigenSystem<int, double> eigendata;
    const PiiMatrix<double> eRealExpected(1,2, 2.0, 2.0);
    const PiiMatrix<double> eImagExpected(1,2, 0.0, 0.0);
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(int, double, tol);
  }

  //Symmetric 2x2
  {
    PiiMatrix<float> mat(2,2, 3.0, 8.0, 8.0, -12.5);
    Pii::EigenSystem<float, float> eigendata;
    const PiiMatrix<float> eRealExpected(1,2, -15.888334705, 6.38833470497);
    const PiiMatrix<float> eImagExpected(1,2, 0.0, 0.0);
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(float, float, tolF);
  }

  Pii::EigenSystem<double, double> eigendata;

  //Symmetric 3x3
  {
    PiiMatrix<double> mat(3,3, 6.3, 5.0  , 6.5,
                          5.0, 7.0  , 158.0,
                          6.5, 158.0, 65.0);
    const PiiMatrix<double> eRealExpected(1,3, -124.640127257, 5.94864966337, 196.991477594);
    const PiiMatrix<double> eImagExpected(1,3, 0.0, 0.0, 0.0);
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(double, double, 1e-9);
  }


  //Singular non symmetric 3x3
  {
    PiiMatrix<double> mat(3,3, 1.0, 0.0 , 0.0,
                          4.0, 0.0 , 0.0,
                          0.0, 1.0 , 0.0);
    const PiiMatrix<double> eRealExpected(1,3, 0.0, 0.0, 1.0);
    const PiiMatrix<double> eImagExpected(1,3, 0.0, 0.0, 0.0);
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(double, double, 1e-7);
  }

  //Singular symmetric 3x3
  {
    PiiMatrix<double> mat(3,3, 1.0, 1.0 , 0.0,
                          1.0, 0.0 , 0.0,
                          0.0, 0.0 , 0.0);
    const PiiMatrix<double> eRealExpected(1,3, -0.61803398875, 0.0, 1.61803398875);
    const PiiMatrix<double> eImagExpected(1,3, 0.0, 0.0, 0.0);
    eigendata = Pii::eigensolve(mat);
    DO_EIGEN_ASSERTS(double, double, 1e-7);
  }

  //3x3 matrix with complex eigenvalues.
  {
    PiiMatrix<double> mat(3,3, 2.0, 3.0 , 4.0,
                          5.0,  8.0 , -4.0,
                          1.0,  3.0 , -2.0);
    const PiiMatrix<double> eRealExpected(1,3, 9.56531116178, -0.782655580887, -0.782655580887);
    const PiiMatrix<double> eImagExpected(1,3, 0.0, 1.83306807541, -1.83306807541);
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(double, double, tol);
  }

  //Right upper corner triangular 3x3 matrix
  {
    PiiMatrix<double> mat(3,3, 5.0, -6.0 , 2.0,
                          0.0,  7.0 , 8.0,
                          0.0,  0.0 , 10.0);
    PiiMatrix<double> eR(1,3), eI(1,3);
    const PiiMatrix<double> eRealExpected(1,3, 5.0, 7.0, 10.0);
    const PiiMatrix<double> eImagExpected(1,3, 0.0, 0.0, 0.0);
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(double, double, tol);
  }

  //Diagonal NxN matrix.
  {
    const int size = 30;
    PiiMatrix<double> mat(size, size);
    PiiMatrix<double> eR(1,size), eI(1,size), eRealExpected(1,size), eImagExpected(1,size), eVectors;
    for(int i = 0; i<size; mat(i,i) = i+1, eRealExpected(0,i) = i+1, eImagExpected(0,i) = 0.0, ++i) {}

    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(double, double, tol);
  }

  //Symmetric NxN matrix.
  {
    const int size = 30;
    PiiMatrix<double> mat(size, size);
    PiiMatrix<double> eRealExpected, eImagExpected;
    for(int i = 0; i<size; ++i)
      {
        for(int j = i; j<size; ++j)
          mat(i,j) = mat(j,i) = i+1;
      }
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(double, double, tol);
  }

  //Miscellaneous tests
  {
    PiiMatrix<double> mat(30,30);
    PiiMatrix<double> eRealExpected, eImagExpected;
    Pii::randomizeMatrix(mat, -1000.0, 1000.0);
    Pii::eigensolve(mat, eigendata);
    DO_EIGEN_ASSERTS(double, double, tol);
  }
#endif
}

void TestPiiMath::isSquare()
{
  QVERIFY( Pii::isSquare(PiiMatrix<double>(1,2)) == false );
  QVERIFY( Pii::isSquare(PiiMatrix<float>(4,2)) == false );
  QVERIFY( Pii::isSquare(PiiMatrix<int>(4,4)) == true );
  QVERIFY( Pii::isSquare(PiiMatrix<bool>(50,50)) == true );
}


void TestPiiMath::isOrthogonalLike()
{
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<double>(1,2, 1.0, 0.0)) == true );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<double>(2,3, 1.0, 0.0, 0.0,
                                                       0.0, 1.0, 0.0)) == true );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<double>(2,3, 1.0, 0.0, 0.0,
                                                       0.0, 1.0001, 0.0)) == false );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<double>(2,3, 1.0, 0.0, 0.0,
                                                       1.0, 0.0, 0.0)) == false );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<double>(3,2, 0.6, -0.8,
                                                       0.8,  0.6,
                                                       0.0,  0.0)) == true );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<double>(3,2, 1.0, 0.0,
                                                       0.1, 1.0,
                                                       0.0, 0.0)) == false );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<int>(4,4, 1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1)) == true );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<float>(4,4, 0.0, 0.0, 1.0, 0.0,
                                                      0.0, 1.0, 0.0, 0.0,
                                                      1.0, 0.0, 0.0, 0.0,
                                                      0.0, 0.0, 0.0, 1.0)) == true );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<double>(2,2,  3.0/5.0, 4.0/5.0,
                                                       -4.0/5.0, 3.0/5.0)) == true );
  QVERIFY( Pii::isOrthogonalLike(PiiMatrix<double>(2,2,  3.0/5.0, 4.0/5.0,
                                                       -4.0/5.0, 3.00001/5.0)) == false );

}

void TestPiiMath::rank()
{
  PiiMatrix<double> matrix(4,4);
  matrix = 1;
  QCOMPARE(Pii::rank(matrix), 1);
  QCOMPARE(Pii::rank(PiiMatrix<double>(3,3,
                                       -1.0, 0.0, 1.0,
                                       -2.0, 0.0, 2.0,
                                       -1.0, 0.0, 1.0)),
           1);
  QCOMPARE(Pii::rank(PiiMatrix<double>(2,2,
                                       1.0, 0.0,
                                       0.0, 1.0)),
           2);
  QCOMPARE(Pii::rank(PiiMatrix<double>(3,6,
                                       0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                                       0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                                       0.0, 0.0, 0.0, 0.0, 0.0, 1.0)),
           3);
  QCOMPARE(Pii::rank(PiiMatrix<double>(6,3,
                                       0.0, 0.0, 0.0,
                                       0.0, 0.0, 0.0,
                                       0.0, 0.0, 0.0,
                                       1.0, 0.0, 0.0,
                                       0.0, 1.0, 0.0,
                                       0.0, 0.0, 1.0)),
           3);

  for (int i=0; i<5; ++i)
    {
      PiiMatrix<double> vector(Pii::uniformRandomMatrix(int(Pii::uniformRandom()*10)+2,
                                                        1,
                                                        -100, 100));
      QCOMPARE(Pii::rank(vector * Pii::transpose(vector)), 1);
    }
}

void TestPiiMath::svd()
{
#if 0
  try{
  PiiMatrix<double> U,D,V;

#define DO_SVD_ASSERTS(RealType, tolerance)                             \
  {                                                                     \
    PrintMatrix(U);                                                 \
    PrintMatrix(D);                                                 \
    PrintMatrix(V);                                                 \
    /*Verify that rows/columns in V and U are orthogonal*/              \
    QVERIFY( Pii::isOrthogonalLike(V, tolerance) == true );         \
    QVERIFY( Pii::isOrthogonalLike(U, tolerance) == true );         \
    /*Create diagonal matrix from singular values*/                     \
    PiiMatrix<RealType> dMatrix(D.columns(),D.columns());               \
    for(int i = dMatrix.columns(); i--; ) dMatrix(i,i) = D(i);          \
    /*Verify that singular values are in descending order */            \
    for(int i = dMatrix.columns()-1; i--;) QVERIFY( D(i) >= D(i+1) );   \
    /* Verify that equation mat = U*D*tranpose(V) holds. */             \
    QVERIFY( Pii::almostEqual(mat, U*dMatrix*Pii::transpose(V), tolerance) ); \
    /* Verify that singular values are correct*/                        \
    PiiMatrix<RealType> eigenVals;                                      \
    if(mat.rows() > mat.columns())                                      \
      eigenVals = Pii::eigensolve(Pii::transpose(mat)*mat).eigenvaluesR(); \
    else                                                                \
      eigenVals = Pii::eigensolve(mat*Pii::transpose(mat)).eigenvaluesR(); \
    /*PiiMatrix<RealType> eigenVals(1, D.columns());*/                  \
    /*Pii::eigenvalues( (mat.rows() > mat.columns()) ? Pii::transpose(mat)*mat : mat*Pii::transpose(mat), eigenVals.row(0));*/ \
    std::sort(eigenVals.row(0), eigenVals.row(0)+eigenVals.columns(), std::greater<RealType>()); \
    eigenVals = Pii::sqrt(eigenVals);                               \
    /*PrintMatrix(D);*/                                                 \
    /*PrintMatrix(eigenVals);*/                                         \
    QVERIFY( Pii::almostEqual(D, eigenVals, tolerance) );           \
  }


  /*
  {
    PiiMatrix<double> mat(1,1, 2.0);
    D = Pii::svd(mat, &U, &V);
    D = Pii::svd<double>(mat, 0, &V);
    D = Pii::svd<double>(mat, &U, &V);
    D = Pii::svd<double>(mat, &U, 0);
    D = Pii::svd<double>(mat, 0, &V);
    D = Pii::svd<double>(mat, 0, 0);
    D = Pii::svd<double>(mat, &U, 0);
    D = Pii::svd<double>(mat, 0, 0);
    DO_SVD_ASSERTS(double, tol);
    }*/

  {
    PiiMatrix<double> mat(3,1, 2.0, 5.0, 4.0);
    D = Pii::svd(mat, &U, &V);
    DO_SVD_ASSERTS(double, tol);
  }

  {
    PiiMatrix<double> mat(3,3, 2.0, 5.0 , 4.0,
                          8.2, 3.2 , 1.1,
                          -0.2, 77.0, 9.1);
    D = Pii::svd(mat, &U, &V);
    DO_SVD_ASSERTS(double, tol);
  }

  {
    PiiMatrix<double> mat(3,3, 1.0, 0.0, 0.0,
                          0.0, 1.0, 0.0,
                          0.0, 0.0, 0.0);
    D = Pii::svd(mat, &U, &V);
    DO_SVD_ASSERTS(double, tol);
  }

  {
    PiiMatrix<double> mat(4,3,  -5.3,  96.3,  444.1,
                          -98.0, -22.0,    0.0,
                          5.0,  77.3,   55.0,
                          63.3,  0.01,    1.4);
    D = Pii::svd(mat, &U, &V);
    DO_SVD_ASSERTS(double, tol);
  }

  {
    PiiMatrix<double> mat(3,4,  -5.3,  96.3,  444.1,
                          -98.0, -22.0,    0.0,
                          5.0,  77.3,   55.0,
                          63.3,  0.01,    1.4);
    D = Pii::svd(mat, &U, &V);
    DO_SVD_ASSERTS(double, tol);
    PiiMatrix<double> U1, V1, D1;
    // Check that the results don't change even if interested
    // only in certain part of the SVD.
    Pii::svd<double>(mat, 0, &V1);
    QVERIFY(Pii::equals(V, V1));
    Pii::svd<double>(mat, &U1, 0);
    QVERIFY(Pii::equals(U, U1));
    D1 = Pii::svd<double>(mat, 0, 0);
    QVERIFY(Pii::equals(D, D1));
  }

  {
    PiiMatrix<float> U,D,V;
    PiiMatrix<float> mat(4,3,  -5.3,  96.3,  444.1,
                         -98.0, -22.0,    0.0,
                         5.0,  77.3,   55.0,
                         63.3,  0.01,    1.4);
    D = Pii::svd(mat, &U, &V);
    DO_SVD_ASSERTS(float, 1e-3f);
  }

  {
    Pii::seedRandom(100);
    PiiMatrix<double> mat(5, 82);
    Pii::randomizeMatrix(mat, -10.0, 10.0);
    D = Pii::svd(mat, &U, &V);
    DO_SVD_ASSERTS(double, tol);
  }

  {
    Pii::seedRandom(1000);
    PiiMatrix<double> mat(100, 10);
    Pii::randomizeMatrix(mat, -100000.0, 100000.0);
    D = Pii::svd(mat, &U, &V);
    DO_SVD_ASSERTS(double, 1e-9);

  }
  } catch (PiiException& ex)
    {
      qDebug("%s, %s", qPrintable(ex.message()), qPrintable(ex.location()));
    }
#endif
}


void TestPiiMath::pca()
{
#if 0
  {
    // Test PCA with simulated data from four 'coordinate cameras' looking at a harmonic oscillator.

    //Defines the amplitude vector of the oscillator.
    const PiiVector<double, 3> vecAmp(0.0, 0.0, 1.0);
    const double dAmplitude = Pii::sqrt(vecAmp.squaredLength());

    //Camera framerate
    const int iFramesPerSecond = 10;

    //Time step
    const double dTimeStep = 1.0/iFramesPerSecond;

    //Oscillator angular frequence
    const double dOmega = 2*M_PI*0.01;

    const double dInitPhase = 1;
    const double dStartTime = 0.0;         //
    const double dEndTime = 170.8;
    const int iMeasurementCount = int((dEndTime - dStartTime) * iFramesPerSecond);

    const bool bAddNoise = false;

    // Matrix where to store the data as column-per-dataframe.
    PiiMatrix<double> matDataColumns(8, iMeasurementCount);

    double t = dStartTime;
    for(int i = 0; i<iMeasurementCount; ++i, t += dTimeStep)
      {
        // Calculate oscillator position.
        // PiiVector<double, 3> r = vecAmp * sin(dOmega * t + dInitPhase);
        // HACK: On G++ 4.2.3, the above line seemed to do 'r = vecAmp'
        // i.e. somehow the factor wasn't taken into account.
        PiiVector<double, 3> r = vecAmp;
        r *= sin(dOmega * t + dInitPhase);

        //Add some noise to the 'measurement'
        if(bAddNoise)
          {
            r[0] += Pii::uniformRandom(-0.01*dAmplitude, 0.01*dAmplitude);
            r[1] += Pii::uniformRandom(-0.01*dAmplitude, 0.01*dAmplitude);
            r[2] += Pii::uniformRandom(-0.01*dAmplitude, 0.01*dAmplitude);
          }

        //Camera on x-axis
        matDataColumns(0,i) = r[1];
        matDataColumns(1,i) = r[2];

        //Camera on y-axis
        matDataColumns(2,i) = r[0];
        matDataColumns(3,i) = r[2];

        //Camera on z-axis
        matDataColumns(4,i) = r[1];
        matDataColumns(5,i) = r[0];

        //Second camera on x-axis
        matDataColumns(6,i) = 0.5*r[1];
        matDataColumns(7,i) = 0.5*r[2];

      }

    // For test purposes, tests PCA also with the same data but with
    // different orientation(rows instead of columns as new measurements).
    const PiiMatrix<double> matDataRows(Pii::transpose(matDataColumns));

    // Now the measurements should be ready in a (8, iMeasurementCount) and
    // (iMeasurementCount, 8) matrices.
    // Using PCA to determine the principal components.

    const Pii::PcaStruct<double> pcaDataColumns(Pii::pca(matDataColumns, Pii::Horizontally));
    const Pii::PcaStruct<double> pcaDataRows(Pii::pca(matDataRows, Pii::Vertically));

    // Verify that standard deviations are the same regardless of the data orientation.
    QVERIFY(Pii::almostEqual(pcaDataColumns.standardDevs(), pcaDataRows.standardDevs(), tol));

    // Verify that the first principal component is what it was in the first tests.
    QVERIFY( Pii::almostEqual(pcaDataColumns.principalComponents()(0,0,pcaDataColumns.principalComponents().rows(), 1),
                                  PiiMatrix<double>(8, 1, 0.0, -2.0/3.0, 0.0, -2.0/3.0, 0.0, 0.0, 0.0, -1.0/3.0), 100*tol) );

    // Verify that the projected data is the same regardless of the data orientation.
    const PiiMatrix<double> matProjectedDataColumn = Pii::transpose(pcaDataColumns.principalComponents()) * matDataColumns;
    const PiiMatrix<double> matProjectedDataRow = matDataRows * pcaDataRows.principalComponents();
    QVERIFY(Pii::almostEqual(matProjectedDataColumn, Pii::transpose(matProjectedDataRow), tol));

    // Calculate PCA directly with eigenvalues of covariance and compare the results with
    // data obtained above.
    {
      const PiiMatrix<double> matCovDataRows = Pii::covariance(matDataRows);
      Pii::EigenSystem<double, double> eigensystem = Pii::eigensolve(matCovDataRows);
      PiiMatrix<double> eigenvals = eigensystem.eigenvaluesR();

      // To make comparison possible, sort eigenvalues and take square root(variance -> standard deviation).
      std::sort(eigenvals.row(0), eigenvals.row(0) + eigenvals.columns(), std::greater<double>());
      eigenvals.map(Pii::unaryCompose(Pii::Sqrt<double>(), Pii::Abs<double>()));

      // Verify that got (almost) the same standard deviations with PCA and direct eigenvalue calculation.
      QVERIFY( Pii::almostEqual(eigenvals, pcaDataColumns.standardDevs(), 1e-5) );


      // Compare eigenvectors (for relevant axis only)
      const int numRelevantDimensions = pcaDataColumns.relevantDimensions();
      const int numRows = pcaDataColumns.principalComponents().rows();
      for(int i = 0; i<numRelevantDimensions; ++i)
        {
          // Check whether principal components are the same or the same except for opposite direction
          // as the eigenvectors. Due to different ordering, comparing first principal component to
          // the last eigenvector and so on.
          QVERIFY( Pii::almostEqual( pcaDataColumns.principalComponents()(0,i, numRows, 1),
                                         eigensystem.eigenvectors()(0, -i-1, numRows, 1), 1e-5 )
                   ||
                   Pii::almostEqual( pcaDataColumns.principalComponents()(0,i, numRows, 1),
                                         eigensystem.eigenvectors()(0, -i-1, numRows, 1) * -1.0, 1e-5 )
                   );
        }

      QVERIFY( Pii::isOrthogonalLike( eigensystem.eigenvectors()) );
      QVERIFY( Pii::isOrthogonalLike( pcaDataColumns.principalComponents()) );
    }
  }
#endif
}


void TestPiiMath::plu()
{
#if 0
  const bool bDoAssertPrints = false;
#define DO_PLU_ASSERTS()                                                \
  do                                                                    \
    {                                                                   \
      const bool isNonsingular = luData.isNonsingular();                \
      PiiMatrix<Real> vecB(mat.rows(), 1), matB(mat.rows(), 10);        \
      Pii::randomizeMatrix(vecB, Real(-5000.0), Real(5000.0)); \
      Pii::randomizeMatrix(matB, Real(-5000.0), Real(5000.0)); \
      const PiiMatrix<Real> P = luData.getP();                          \
      const PiiMatrix<Real> L = luData.getL();                          \
      const PiiMatrix<Real> U = luData.getU();                          \
      if(bDoAssertPrints)                                               \
        {                                                               \
          PrintMatrix(mat);                                             \
          PrintMatrix(L);                                               \
          PrintMatrix(U);                                               \
          PrintMatrix(P);                                               \
          PrintMatrix(luData.pivot());                                  \
          PrintMatrix(L * U);                                           \
          PrintMatrix(P * L * U);                                       \
          cout << "Pii::determinant() == " << Pii::determinant(mat) << endl; \
          cout << "luData.det() == " << luData.det() << endl;           \
          if(isNonsingular)                                             \
            {                                                           \
              /* Ax = b prints */                                       \
              PrintMatrix(vecB);                                        \
              PrintMatrix(luData.solve(vecB));                          \
              PrintMatrix(mat * luData.solve(vecB));                    \
              /* AX = B prints */                                       \
              PrintMatrix(matB);                                        \
              PrintMatrix(luData.solve(matB));                          \
              PrintMatrix(mat * luData.solve(matB));                    \
            }                                                           \
        }                                                               \
      QVERIFY( Pii::almostEqual(mat, P * L * U, Pii::Numeric<Real>::tolerance()) ); \
      QVERIFY( Pii::almostEqualRel(Pii::determinant(mat), luData.det(), Pii::Numeric<Real>::tolerance()) ); \
      if(isNonsingular)                                                 \
        {                                                               \
          QVERIFY( Pii::almostEqual(mat * luData.solve(vecB), vecB, 10*Pii::Numeric<Real>::tolerance()) ); \
          QVERIFY( Pii::almostEqual(mat * luData.solve(matB), matB, 10*Pii::Numeric<Real>::tolerance()) ); \
        }                                                               \
      QVERIFY( !isNonsingular == Pii::isSingular(mat) );            \
    }                                                                   \
  while(false)


  {
    PiiMatrix<int> matInt(3,3, 3,  2, 4,
                          2, -1, 4,
                          3,  2, 1);
    Pii::Plu<double> luData = Pii::plu(PiiMatrix<double>(matInt));
    const PiiMatrix<double> mat(matInt);
    typedef double Real;
    DO_PLU_ASSERTS();
  }

  { //Singular 10x10 matrix
    typedef double Real;
    PiiMatrix<Real> mat(10,10);
    for(int r = 10; r--;)
      {
        for(int c = 10; c--;)
          {
            mat(r,c) = M_PI*M_E*r;
          }
      }
    Pii::Plu<Real> luData = Pii::plu(mat);
    DO_PLU_ASSERTS();
  }

  { //50*50 random matrix.
    typedef double Real;
    PiiMatrix<Real> mat(50,50);
    Pii::randomizeMatrix(mat, -10.0, 10.0);
    Pii::Plu<Real> luData = Pii::plu(mat);
    DO_PLU_ASSERTS();
  }
  #endif
}

void TestPiiMath::numeric()
{
#define TEST_INTEGER_TYPE(type)                                         \
  QCOMPARE( Pii::Numeric<type>::minValue(), std::numeric_limits<type>::min() ); \
  QCOMPARE( Pii::Numeric<type>::maxValue(), std::numeric_limits<type>::max() ); \
  QCOMPARE( Pii::Numeric<type>::tolerance(), (type)0 );             \
  QCOMPARE( Pii::Numeric<unsigned type>::minValue(), std::numeric_limits<unsigned type>::min() ); \
  QCOMPARE( Pii::Numeric<unsigned type>::maxValue(), std::numeric_limits<unsigned type>::max() ); \
  QCOMPARE( Pii::Numeric<unsigned type>::tolerance(), (unsigned type)0 );

#define TEST_FLOATING_TYPE(type)                                        \
  QCOMPARE( Pii::Numeric<type>::minValue(), -std::numeric_limits<type>::max() ); \
  QCOMPARE( Pii::Numeric<type>::maxValue(), std::numeric_limits<type>::max() ); \
  QVERIFY( Pii::Numeric<type>::tolerance() != (type)0 );            \
  QVERIFY( Pii::Numeric<type>::tolerance() < (type)10e-5 );

  TEST_INTEGER_TYPE(char);
  TEST_INTEGER_TYPE(short);
  TEST_INTEGER_TYPE(int);
  TEST_INTEGER_TYPE(long);
  TEST_INTEGER_TYPE(long long);

  TEST_FLOATING_TYPE(float);
  TEST_FLOATING_TYPE(double);
  TEST_FLOATING_TYPE(long double);

#undef TEST_FLOAT_TYPE
#undef TEST_INTEGER_TYPES
}

void TestPiiMath::kthSmallest()
{
  {
    int array[31];
    int org[31];
    srand((unsigned)time(0));
    for(int i = 0; i < 31; ++i)
      {
        int val = rand()%30+1;
        //qDebug()<<"val: "<<val;
        org[i] = val;
        array[i] = val;
      }

    Pii::insertionSort<int>(org, 31);
    for(int p = 0; p < 30; ++p)
      {
        int k = p;
        int el = Pii::kthSmallest<int>(array,31,k);
        /*
          qDebug()<<" Original at " << k;
          if (org[p] != el)
          for(int i = 0; i < 30; ++i)
          qDebug() << array[i];
          qDebug()<<" Sorted:";
          if (org[p] != el)
          for(int i = 0; i < 30; ++i)
          qDebug() << org[i];
        */
        QCOMPARE(org[p], el);

      }
  }

  {
    int array[10] = {0,1,2,3,4,5,12,6,7,8};
    int orginal[10] =  {0,1,2,3,4,5,12,6,7,8};

    Pii::insertionSort<int>(orginal,10);

    int value = Pii::kthSmallest<int>(array,10,4);
    QCOMPARE(value, 4);


  }

}

void TestPiiMath::partition()
{
  {

    int array[10] = {1,4,0,5,2,3,7,6,8,9};
    int value = 3;
    int k = Pii::partition<int>(array, 10, 5);
    QCOMPARE(value, array[k]);

    //There exist property that all values which are 0..k-1 indices
    //must be below or equal as value

    for(int i = 0; i < k; ++i)
      {
        QVERIFY(array[i] <= value);
      }

    //There exist property that all value which are at indices k to end
    //must be equal or greater then value

    for(int i = k; i < 10; ++i)
      {
        QVERIFY(array[i] >= value);
      }


  }

}
void TestPiiMath::insertionSort()
{
  {
    int array[10] = {1,3,2,0,4,5,7,6,9,8};
    Pii::insertionSort<int>(array, 10);
    for(int i = 0; i < 10; ++i)
      {
        QCOMPARE(array[i],i);
      }
  }
}

void TestPiiMath::windowSum()
{
  float data[5] = { 1, 2, 3, 4, 5 };
  // Calculate moving average over three neighbors
  Pii::windowSum(data, data+5, 3, std::bind2nd(std::divides<float>(), 3));

  QCOMPARE(data[0], 1.0f);
  QCOMPARE(data[1], 2.0f);
  QCOMPARE(data[2], 3.0f);
  QCOMPARE(data[3], 4.0f);
  QCOMPARE(data[4], 3.0f);

  // Sum two neighbors
  Pii::windowSum(data, data+5, 2, Pii::Identity<float>());

  QCOMPARE(data[0], 3.0f);
  QCOMPARE(data[1], 5.0f);
  QCOMPARE(data[2], 7.0f);
  QCOMPARE(data[3], 7.0f);
  QCOMPARE(data[4], 3.0f);

  // Do not change anything
  Pii::windowSum(data, data+5, 1, Pii::Identity<float>());

  QCOMPARE(data[0], 3.0f);
  QCOMPARE(data[1], 5.0f);
  QCOMPARE(data[2], 7.0f);
  QCOMPARE(data[3], 7.0f);
  QCOMPARE(data[4], 3.0f);

  // Divide data[1] by five
  Pii::windowSum(data+1, data+2, 5, std::bind2nd(std::divides<float>(), 5));

  QCOMPARE(data[1], 1.0f);

  unsigned char data2[2] = { 128, 128 };
  // This overflows due to limited range of data type.
  Pii::windowSum(data2, data2+2, 2, std::bind2nd(std::divides<unsigned char>(), 2));

  QCOMPARE(data2[0], (unsigned char)0);
  QCOMPARE(data2[1], (unsigned char)64);

  data2[0] = data2[1] = 128;
  // Sum values as unsigned ints and cast the result to unsigned char
  Pii::windowSum(data2, data2+2, 2, Pii::unaryCompose(Pii::Cast<unsigned int, unsigned char>(),
                                                          std::bind2nd(std::divides<unsigned int>(), 2)));

  QCOMPARE(data2[0], (unsigned char)128);
  QCOMPARE(data2[1], (unsigned char)64);

  data2[0] = data2[1] = 128;
  Pii::movingAverage<unsigned int>(data2, data2+2, 2);
  QCOMPARE(data2[0], (unsigned char)128);
  QCOMPARE(data2[1], (unsigned char)64);
}

void TestPiiMath::findMinima()
{
  PiiMatrix<int> mat(3,3,
                     1,2,3,
                     4,5,6,
                     7,8,9);
  PiiHeap<PiiMatrixValue<int> > minima = Pii::findMinima(mat, 3);
  QCOMPARE(minima.size(), 3);
  for (int i=0; i<3; ++i)
    {
      QCOMPARE(minima[i].value, i+1);
      QCOMPARE(minima[i].row, 0);
      QCOMPARE(minima[i].column, i);
    }
}

void TestPiiMath::findMaxima()
{
  PiiMatrix<int> mat(3,3,
                     1,2,3,
                     4,5,6,
                     7,8,9);
  PiiHeap<PiiMatrixValue<int> > maxima = Pii::findMaxima(mat, 3);
  QCOMPARE(maxima.size(), 3);
  for (int i=0; i<3; ++i)
    {
      QCOMPARE(maxima[i].value, 9-i);
      QCOMPARE(maxima[i].row, 2);
      QCOMPARE(maxima[i].column, 2-i);
    }
}

void TestPiiMath::cumulativeSumAlong()
{
  PiiMatrix<int> mat(3,3,
                     1,2,3,
                     4,5,6,
                     7,8,9);
  QVERIFY(Pii::equals(Pii::cumulativeSum<int>(mat, Pii::Horizontally),
                      PiiMatrix<int>(3,3,
                                     1,3,6,
                                     4,9,15,
                                     7,15,24)));
  QVERIFY(Pii::equals(Pii::cumulativeSum<int>(mat, Pii::Vertically),
                      PiiMatrix<int>(3,3,
                                     1,2,3,
                                     5,7,9,
                                     12,15,18)));
}

void TestPiiMath::cumulativeSum()
{
  PiiMatrix<int> matrix(3, 3,
                        1, 2, 3,
                        2, 3, 4,
                        3, 4, 5);

  QVERIFY(Pii::equals(Pii::cumulativeSum<int>(matrix), PiiMatrix<int>(3,3,
                                                                      1,  3,  6,
                                                                      3,  8, 15,
                                                                      6, 15, 27)));
  QVERIFY(Pii::equals(Pii::cumulativeSum<int>(matrix,
                                              Pii::ZeroBorderCumulativeSum),
                      PiiMatrix<int>(4,4,
                                     0, 0,  0,  0,
                                     0, 1,  3,  6,
                                     0, 3,  8, 15,
                                     0, 6, 15, 27)));
  PiiMatrix<int> matSum(4,4);
  Pii::cumulativeSum(matrix, matSum, Pii::Identity<int>(), Pii::ZeroBorderCumulativeSum);
  QVERIFY(Pii::equals(matSum,
                      PiiMatrix<int>(4,4,
                                     0, 0,  0,  0,
                                     0, 1,  3,  6,
                                     0, 3,  8, 15,
                                     0, 6, 15, 27)));
}

void TestPiiMath::fastMovingAverage()
{
  PiiMatrix<int> matInput(3, 3,
                          1, 2, 3,
                          2, 3, 4,
                          3, 4, 5);
  PiiMatrix<int> matSum = Pii::cumulativeSum<int>(matInput, Pii::ZeroBorderCumulativeSum);
  PiiMatrix<float> matSmooth = Pii::fastMovingAverage<float>(matSum, 3);
  //PrintMatrix(matSmooth);

  QVERIFY(Pii::equals(matSmooth, PiiMatrix<float>(3, 3,
                                                  2.0, 2.5, 3.0,
                                                  2.5, 3.0, 3.5,
                                                  3.0, 3.5, 4.0)));

  static int aInput[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  static const int iCnt = sizeof(aInput)/sizeof(int);
  double aOutput[iCnt];
  Pii::fastMovingAverage(aInput, iCnt, aOutput, 4);
  double aResult0[] = { 1.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 9 };
  for (int i=0; i<iCnt; ++i)
    QCOMPARE(aOutput[i], aResult0[i]);

  Pii::fastMovingAverage(aInput, iCnt, aOutput, 5);
  double aResult1[] = { 1.5, 1.5, 3, 4, 5, 6, 7, 8.5, 8.5 };
  for (int i=0; i<iCnt; ++i)
    QCOMPARE(aOutput[i], aResult1[i]);
}

void TestPiiMath::angleDiff()
{
  QCOMPARE(Pii::angleDiff(0.0, 0.0), 0.0);
  QCOMPARE(Pii::angleDiff(M_PI, M_PI), 0.0);
  QCOMPARE(Pii::angleDiff(M_PI, 0.0), M_PI);
  QCOMPARE(Pii::angleDiff(0.0, M_PI), -M_PI);
  QCOMPARE(Pii::angleDiff(3*M_PI, M_PI), 0.0);
  QCOMPARE(Pii::angleDiff(3*M_PI, 2.5 * M_PI), 0.5*M_PI);
  QCOMPARE(Pii::angleDiff(0.0, -15*M_PI/8), -M_PI/8);
  QCOMPARE(Pii::angleDiff(0.0, 15*M_PI/8), M_PI/8);
  QCOMPARE(Pii::angleDiff(0.0, 4*M_PI), 0.0);
  QCOMPARE(Pii::angleDiff(M_PI/2, 3*M_PI/2), -M_PI);
}

void TestPiiMath::setIdentity()
{
  {
    PiiMatrix<int> mat(2,3);
    Pii::setIdentity(mat);
    QVERIFY(Pii::equals(mat, PiiMatrix<int>(2,3,
                                            1,0,0,
                                            0,1,0)));
  }
  {
    PiiMatrix<int> mat(3,2);
    Pii::setIdentity(mat);
    QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,2,
                                            1,0,
                                            0,1,
                                            0,0)));
  }
}

void TestPiiMath::setDiagonal()
{
  int diagonal[] = { 1, 2, 3 };
  {
    PiiMatrix<int> mat(2,3);
    Pii::setDiagonal(mat, diagonal);
    QVERIFY(Pii::equals(mat, PiiMatrix<int>(2,3,
                                            1,0,0,
                                            0,2,0)));
  }
  {
    PiiMatrix<int> mat(3,3);
    Pii::setDiagonal(mat, diagonal);
    QVERIFY(Pii::equals(mat, PiiMatrix<int>(3,3,
                                            1,0,0,
                                            0,2,0,
                                            0,0,3)));
  }
}

QTEST_MAIN(TestPiiMath)
