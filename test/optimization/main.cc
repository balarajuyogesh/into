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

#include "TestPiiOptimization.h"


#include <PiiOptimization.h>
#include <QtTest>
#include "TestFunction.h"
#include "TestMarquardtFunction.h"

#include <QDebug>
#include <PiiMatrix.h>
#include <PiiMatrixUtil.h>
#include <PiiRandom.h>
#include <iostream>

void TestPiiOptimization::bfgsMinimize()
{
  TestFunction func;
  PiiMatrix<double> initialParams(1,1, 3.243);
  double epsG = 10e-5;
  double epsF = 10e-5;
  double epsX = 10e-5;
  int maxIterations = 100;
  PiiMatrix<double> results(1,1, 1.0);
  results = PiiOptimization::bfgsMinimize(&func,initialParams,epsG,epsF,epsX,maxIterations);
  QCOMPARE(results(0,0),0.0);
}

void TestPiiOptimization::lmMinimize()
{
  TestMarquardtFunction func;

  PiiMatrix<double> initialParams(1,3, 1.0, -1.0, 2.0);
  PiiMatrix<double> result = PiiOptimization::lmMinimize(&func, initialParams);

  PiiMatrix<double> residuals(1,3);
  func.residualValues(result[0], residuals[0]);

  QCOMPARE(result(0),2.0);
  QCOMPARE(result(1),-5.0);
  QCOMPARE(result(2),4.0);

  QCOMPARE(residuals(0)+1, 1.0);
  QCOMPARE(residuals(1)+1, 1.0);
  QCOMPARE(residuals(2)+1, 1.0);

  // Repeat with precalculated jacobian
  func.setHasJacobian(true);
  result = PiiOptimization::lmMinimize(&func, initialParams);
  func.residualValues(result[0], residuals[0]);

  QCOMPARE(result(0),2.0);
  QCOMPARE(result(1),-5.0);
  QCOMPARE(result(2),4.0);

  QCOMPARE(residuals(0)+1, 1.0);
  QCOMPARE(residuals(1)+1, 1.0);
  QCOMPARE(residuals(2)+1, 1.0);
}

void TestPiiOptimization::assign()
{
  PiiMatrix<int> matCost(4, 4,
                         1, 0, 3, 4,
                         2, 1, 5, 4,
                         3, 3, 3, 3,
                         1, 2, 1, 3);
  PiiMatrix<int> matSolution;
  int iCost = PiiOptimization::assign(matCost, &matSolution);
  //Pii::matlabPrint(std::cout, matSolution);
  QCOMPARE(matSolution(0,0), 1);
  QCOMPARE(matSolution(0,1), 0);
  QCOMPARE(matSolution(0,2), 3);
  QCOMPARE(matSolution(0,3), 2);
  QCOMPARE(iCost, 6);
}


QTEST_MAIN(TestPiiOptimization)

