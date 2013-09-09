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

QTEST_MAIN(TestPiiRansac)
