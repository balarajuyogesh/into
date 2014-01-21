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

#include "TestPiiFeatureCombiner.h"

#include <QtTest>
#include <PiiDelay.h>

void TestPiiFeatureCombiner::initTestCase()
{
  QVERIFY(createOperation("piiclassification", "PiiFeatureCombiner"));
}

void TestPiiFeatureCombiner::process()
{
  operation()->setProperty("dynamicInputCount", 3);
  operation()->setProperty("learningBatchSize", 3);
  operation()->setProperty("distanceMeasures",
                           QStringList() <<
                           "PiiAbsDiffDistance" <<
                           "PiiGeometricDistance" <<
                           "PiiHistogramIntersection");

  /* Send the following vectors
   *
   * [1 2] [3] [4 5 6]
   * [2 3] [4] [5 6 7]
   * [3 4] [5] [6 7 8]
   *
   * Pairwise distances between vectors
   *
   * Pair  | AbsDiff  Geometr  HistInt
   *-------+--------------------------
   * 0,1   |  2        1        -15
   * 0,2   |  4        2        -15
   * 1,2   |  2        1        -18
   * ------+--------------------------
   * u     |  2.67     1.33     -16
   * var   |  0.89     0.22     2.0
   * scale |  1.125    4.5      0.5
   */

  QVERIFY(start(ExpectFail));

  QVERIFY(connectInput("features0"));
  QVERIFY(connectInput("features1"));
  QVERIFY(connectInput("features2"));

  QVERIFY(start());

  // First set of vectors
  QVERIFY(sendObject("features0", PiiMatrix<int>(1,2, 1,2)));
  QVERIFY(sendObject("features1", 3.0));
  QVERIFY(sendObject("features2", PiiMatrix<int>(1,3, 4,5,6)));
  QVERIFY(Pii::equals(outputValue("features", PiiMatrix<double>()),PiiMatrix<double>(1,6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0)));
  QVERIFY(Pii::equals(outputValue("boundaries", PiiMatrix<int>()),PiiMatrix<int>(1,3, 2,3,6)));

  // Second set of vectors
  QVERIFY(sendObject("features0", PiiMatrix<int>(1,2, 2,3)));
  QVERIFY(sendObject("features1", 4.0));
  QVERIFY(sendObject("features2", PiiMatrix<int>(1,3, 5,6,7)));
  QVERIFY(Pii::equals(outputValue("features", PiiMatrix<double>()),PiiMatrix<double>(1,6, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0)));

  // Third set of vectors
  QVERIFY(sendObject("features0", PiiMatrix<int>(1,2, 3,4)));
  QVERIFY(sendObject("features1", 5.0));
  QVERIFY(sendObject("features2", PiiMatrix<int>(1,3, 6,7,8)));
  QVERIFY(Pii::equals(outputValue("features", PiiMatrix<double>()),PiiMatrix<double>(1,6, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0)));

  QMetaObject::invokeMethod(operation(), "startLearningThread", Qt::DirectConnection);

  QTime time;
  time.start();
  while (operation()->property("learningThreadRunning").toBool())
    {
      PiiDelay::msleep(10);
      QCoreApplication::processEvents();
      if (time.elapsed() > 3000) // shouldn't take more than 3 seconds in any case
        QFAIL("Learning distance variances took way too long.");
    }
  QList<double> lstDistanceWeights = Pii::variantsToList<double>(operation()->property("distanceWeights").toList());
  QCOMPARE(lstDistanceWeights.size(), 3);
  QCOMPARE(lstDistanceWeights[0], 1.125);
  QCOMPARE(lstDistanceWeights[1], 4.5);
  QCOMPARE(lstDistanceWeights[2], 0.5);
}

QTEST_MAIN(TestPiiFeatureCombiner)
