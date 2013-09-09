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

#include "TestPiiMatching.h"

#include <PiiMatching.h>
#include <PiiMath.h>
#include <PiiMatrixUtil.h>

#include <iostream>
#include <QtTest>
#include <QDebug>

void TestPiiMatching::boundaryDirections()
{
  
  {
    PiiMatrix<int> points(5,2,
                          0,0,
                          10,0,
                          10,10,
                          0,10,
                          0,0);
    QVector<double> lstExpectedAnglesInDegree = QVector<double>() << -45 << 45 << 135 << -135;
    QVector<double> lstCalculatedAngles = PiiMatching::boundaryDirections(points);
    QCOMPARE(lstExpectedAnglesInDegree.size(), lstCalculatedAngles.size());
    for (int i=0; i<lstCalculatedAngles.size(); i++)
      QVERIFY(Pii::abs(lstCalculatedAngles[i] / M_PI * 180.0 - lstExpectedAnglesInDegree[i]) <= 1);
  }

  {
    PiiMatrix<int> points(5,2,
                          0,0,
                          10,-10,
                          20,0,
                          10,10,
                          0,0);
    QVector<double> lstExpectedAnglesInDegree = QVector<double>() << -90 << 0 << 90 << 180;
    QVector<double> lstCalculatedAngles = PiiMatching::boundaryDirections(points);
    QCOMPARE(lstExpectedAnglesInDegree.size(), lstCalculatedAngles.size());
    
    for (int i=0; i<lstCalculatedAngles.size(); i++)
      QVERIFY(Pii::abs(lstCalculatedAngles[i] / M_PI * 180.0 - lstExpectedAnglesInDegree[i]) <= 1);
  }

}

void TestPiiMatching::shapeContextDescriptor()
{
  {
    PiiMatrix<int> matPoints(0,2);
    //distance ~50
    for (double alpha=M_PI/6; alpha < 5*M_PI/6; alpha += M_PI/6)
      matPoints.appendRow(Pii::round<int>(50*cos(alpha)), Pii::round<int>(50*sin(alpha)));
    for (double alpha=7*M_PI/6; alpha < 12*M_PI/6; alpha += M_PI/6)
      matPoints.appendRow(Pii::round<int>(50*cos(alpha)), Pii::round<int>(50*sin(alpha)));
    //distance ~ 100
    for (double alpha=M_PI/6; alpha < 5*M_PI/6; alpha += M_PI/6)
      matPoints.appendRow(Pii::round<int>(100*cos(alpha)), Pii::round<int>(100*sin(alpha)));
    for (double alpha=7*M_PI/6; alpha < 12*M_PI/6; alpha += M_PI/6)
      matPoints.appendRow(Pii::round<int>(100*cos(alpha)), Pii::round<int>(100*sin(alpha)));
    matPoints.appendRow(matPoints[0]); // duplicate first point

    PiiMatrix<int> matKeyPoints(2,2);
    QVector<double> vecDistances = QVector<double>() << 4000 << INFINITY;
    PiiMatrix<float> matFeatures = PiiMatching::shapeContextDescriptor(matPoints,
                                                                       matKeyPoints,
                                                                       2,
                                                                       vecDistances);

    //Pii::printMatrix(std::cout, matFeatures, " ", "\n");
    //std::cout << std::endl;
  }
}

QTEST_MAIN(TestPiiMatching)
