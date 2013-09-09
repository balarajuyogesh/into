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

#ifndef _TESTPIIMATH_H
#define _TESTPIIMATH_H

#include <QObject>

class TestPiiMath : public QObject
{
  Q_OBJECT

private slots:
  void gaussian();
  void median();
  void round();
  void floor();
  void ceil();
  void divide();
  void atan2();
  void fastAtan2();
  void normalize();
  void combinations();
  void permutations();
  void transformRows();
  void transformColumns();
  void movingAverage();
  void diff();
  void centralDiff();
  void sum();
  void sqrt();
  void distanceAlong();
  void pow();
  void fitPolynomial();
  void min();
  void max();
  void invert();
  void matrixArithmetic();
  void var();
  void std();
  void squaredDistance();
  void square();
  void pseudoInverse();
  void multiplyTransposed();
  void pivot();
  void norm();
  void multiply();
  void multiplied();
  void minMax();
  void mean();
  void isSingular();
  void isDiagonal();
  void isAntiSymmetric();
  void isSymmetric();
  void inverse();
  void gcd();
  void find();
  void factorial();
  void innerProduct();
  void divided();
  void distance();
  void determinant();
  void crossProduct();
  void covariance();
  void conj();
  void almostEqualTest();
  void almostEqualRel();
  void forEach();
  void forEachIf();
  void mapIf();
  void eigen();
  void hypotenuse();
  void realAndImag();
  void isSquare();
  void isOrthogonalLike();
  void rank();
  void svd();
  void pca();
  void plu();
  void numeric();
  void angleDiff();
  
  void IeeeFloat();
  void Abs();
  void AbsDiff();
  void AbsPow();
  void AbsSum();
  void Atan2();
  void Hypotenuse();
  void Log();
  void Max();
  void Min();
  void Pow();
  void Round();
  void Sqrt();
  void Square();
  void Mean();
  void Exp();
  void Sin();
  void Cos();
  void Tan();
  void kthSmallest();
  void partition();
  void insertionSort();

  void windowSum();
  void findMinima();
  void findMaxima();
  void cumulativeSumAlong();
  void cumulativeSum();
  void fastMovingAverage();

  void setIdentity();
  void setDiagonal();
};

#endif //_TESTPIIMATH_H
