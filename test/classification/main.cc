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

#include "TestPiiClassification.h"

#include <PiiClassification.h>
#include <PiiSquaredGeometricDistance.h>
#include <PiiGeometricDistance.h>
#include <QtTest>

#include <PiiMatrixUtil.h>
#include <iostream>

void TestPiiClassification::kMeans()
{
  PiiMatrix<double> matSamples(6,2,
                               -1.0,-0.5,
                               -1.0,0.0,
                               -1.0,0.5,
                               1.0,-0.5,
                               1.0,0.0,
                               1.0,0.5);
  
  /*PiiMatrix<double> centroids = PiiClassification::kMeans(samples, 2,
                                                          PiiGeometricDistance<const double*>());
  Pii::matlabPrint(std::cout, centroids);
  QCOMPARE(Pii::abs(centroids(0,0)), 1.0);
  QCOMPARE(centroids(0,1) + 1, 1.0);
  QCOMPARE(Pii::abs(centroids(1,0)), 1.0);
  QCOMPARE(centroids(1,1) + 1, 1.0);
  QVERIFY(centroids(0,0) != centroids(1,0));
  */
  
  PiiMatrix<double> centroids = PiiClassification::kMeans(matSamples, 6, PiiSquaredGeometricDistance<const double*>());
  QVERIFY(centroids.isEmpty());
}

void TestPiiClassification::calculateDistanceMatrix()
{
  /*PiiMatrix<double> matPoints(5,2, 124.0,474.0,
                                     113.0,460.0,
                                     122.0,448.0,
                                     121.0,448.0,
                                     351.0,447.0);


  PiiMatrix<double> matDistances = PiiClassification::calculateDistanceMatrix(matPoints,
  PiiSquaredGeometricDistance<double>());

  Pii::matlabPrint(std::cout,matDistances);
  */

  PiiMatrix<double> matSecPoints(3,2, 1.0, 1.0,
                                      2.0, 2.0,
                                      3.0, 3.0);

  PiiMatrix<double> matDistancesSec = PiiClassification::calculateDistanceMatrix(matSecPoints,
                                                                                 PiiSquaredGeometricDistance<const double*>(),
                                                                                 true,true);

  QCOMPARE(matDistancesSec.rows(),3);
  QCOMPARE(matDistancesSec.columns(),3);
  //Diagonal should be zero!
  for(int r = 0; r < matDistancesSec.rows(); ++r)
    for(int c = 0; c < matDistancesSec.columns(); ++c)
      if(r == c)
        QVERIFY(matDistancesSec(r,c) == 0.0);

  QCOMPARE(matDistancesSec(0,1),2.0);
  QCOMPARE(matDistancesSec(0,2),8.0);
  QCOMPARE(matDistancesSec(1,0),2.0);
  QCOMPARE(matDistancesSec(1,2),2.0);
  QCOMPARE(matDistancesSec(2,0),8.0);
  QCOMPARE(matDistancesSec(2,1),2.0);
}

void TestPiiClassification::countLabels()
{
  QVector<double> labels;
  labels << 1 << 2 << 0 << 0 << 1 << 4;
  QList<QPair<double,int> > counts(PiiClassification::countLabels(labels));
  QCOMPARE(counts.size(), 4);
  QCOMPARE(counts[0].first, 0.0);
  QCOMPARE(counts[0].second, 2);
  QCOMPARE(counts[1].first, 1.0);
  QCOMPARE(counts[1].second, 2);
  QCOMPARE(counts[2].first, 2.0);
  QCOMPARE(counts[2].second, 1);
  QCOMPARE(counts[3].first, 4.0);
  QCOMPARE(counts[3].second, 1);
}

QTEST_MAIN(TestPiiClassification)
