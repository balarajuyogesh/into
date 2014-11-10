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

#include "TestPiiRansac.h"

#include <PiiRansac.h>
#include <PiiRigidPlaneRansac.h>
#include <PiiCircleRansac.h>
#include <QtTest>
#include <PiiMatrixUtil.h>
#include <PiiTimer.h>
#include <iostream>

void TestPiiRansac::RigidPlaneRansac()
{
  PiiMatrix<double> matPoints1(Pii::uniformRandomMatrix(200, 2, -50, 50));
  PiiMatrix<double> matModel(1,4, 2.0, 1.0, 10.0, -20.0);
  PiiMatrix<double> matPoints2(PiiRigidPlaneRansac<double>::transform(matPoints1, matModel[0]));

  // Clean case, no outliers
  {
    PiiRigidPlaneRansac<int> ransac(matPoints1, matPoints2);
    ransac.setFittingThreshold(0.1);
    ransac.setSelectionProbability(0.999);
    ransac.findBestModel();
    QVERIFY(ransac.findBestModel());
    PiiMatrix<double> matEstModel(ransac.refineModel());
    //Pii::matlabPrint(std::cout, matEstModel);
    //Pii::matlabPrint(std::cout, ransac.refineModel());
    // Scale must be correct in 0.03 accuracy
    QVERIFY(Pii::abs(matModel(0) - matEstModel(0)) < 0.03);
    // Angle must be correct in 0.02 accuracy
    QVERIFY(Pii::abs(matModel(1) - matEstModel(1)) < 0.02);
    // 2 is enough for translation
    QVERIFY(Pii::abs(matModel(2) - matEstModel(2)) < 2);
    QVERIFY(Pii::abs(matModel(3) - matEstModel(3)) < 2);
  }

  // Add 50% outliers and some noise
  for (int i=0; i<matPoints2.rows(); ++i)
    {
      if (i % 2 == 0)
        {
          matPoints2(i,0) += Pii::uniformRandom(-50, 50);
          matPoints2(i,1) += Pii::uniformRandom(-50, 50);
        }
      else
        {
          matPoints2(i,0) += Pii::normalRandom();
          matPoints2(i,1) += Pii::normalRandom();
        }
    }

  // This should give essentially the same result
  {
    PiiRigidPlaneRansac<int> ransac(matPoints1, matPoints2);
    ransac.setFittingThreshold(2);
    ransac.setSelectionProbability(0.999);
    QVERIFY(ransac.findBestModel());
    PiiMatrix<double> matEstModel = ransac.refineModel();
    QVERIFY(!matEstModel.isEmpty());
    //Pii::matlabPrint(std::cout, ransac.bestModel());
    //Pii::matlabPrint(std::cout, ransac.refineModel());
    //Pii::matlabPrint(std::cout, ransac.transform(matPoints1,
    //ransac.refineModel()) - matPoints2);
    QVERIFY(Pii::abs(matModel(0) - matEstModel(0)) < 0.03);
    QVERIFY(Pii::abs(matModel(1) - matEstModel(1)) < 0.02);
    QVERIFY(Pii::abs(matModel(2) - matEstModel(2)) < 2);
    QVERIFY(Pii::abs(matModel(3) - matEstModel(3)) < 2);
  }

#if 0
  {
    PiiTimer timer;
    PiiRigidPlaneRansac<int> ransac(matPoints1, matPoints2);
    ransac.setFittingThreshold(1);
    for (int i=0; i<100; ++i)
      ransac.findBestModel();
    qDebug("Time: %d", timer.milliSeconds());
  }
#endif
}

void TestPiiRansac::CircleRansac()
{
  Pii::seedRandom();
  {
    // Unit circle
    PiiMatrix<int> matPoints(3, 2,
                             -1, 0,
                             0, 1,
                             1, 0);
    PiiCircleRansac<int> ransac(matPoints);
    QVERIFY(ransac.findBestModel());
    // The algorithm uses a fast sqrt approximation, so the model
    // isn't exactly correct.
    QVERIFY(Pii::max(Pii::abs(ransac.bestModel() - PiiMatrix<double>(1, 3, 0.0, 0.0, 1.0))) < 0.01);
  }
  {
    double dCx = Pii::uniformRandom(-10.0, 10.0);
    double dCy = Pii::uniformRandom(-10.0, 10.0);
    double dR = Pii::uniformRandom(10.0, 20.0);
    PiiMatrix<double> matPoints(0, 2);
    matPoints.reserve(256);
    for (double a = 0; a < 2 * M_PI; a += M_PI / 128)
      matPoints.appendRow(dCx + dR * cos(a) + Pii::uniformRandom(-1.0, 1.0),
                          dCy + dR * sin(a) + Pii::uniformRandom(-1.0, 1.0));
    PiiCircleRansac<double> ransac(matPoints);
    ransac.setFittingThreshold(1.5);
    QVERIFY(ransac.findBestModel());
    PiiMatrix<double> matModel(ransac.bestModel());
    double dCenterDiff1 = Pii::hypotenuse(matModel(0, 0) - dCx, matModel(0, 1) - dCy);
    double dRadiusDiff1 = Pii::abs(matModel(0, 2) - dR);

    QVERIFY(dCenterDiff1 < 1);
    QVERIFY(dRadiusDiff1 < 2);

    PiiMatrix<double> matRefined(ransac.refineModel());
    double dCenterDiff2 = Pii::hypotenuse(matRefined(0, 0) - dCx, matRefined(0, 1) - dCy);
    double dRadiusDiff2 = Pii::abs(matRefined(0, 2) - dR);

    QVERIFY(dCenterDiff2 < 0.2);
    QVERIFY(dRadiusDiff2 < 0.1);
  }
}

QTEST_MAIN(TestPiiRansac)
