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

#include "TestPiiStereoTriangulator.h"

#include <PiiStereoTriangulator.h>
#include <PiiCalibration.h>
#include <PiiMatrixUtil.h>
#include <iostream>
#include <QtTest>

void TestPiiStereoTriangulator::calculate3DPoints()
{
  // Arbitrarily chosen camera parameters and positions. Three cameras
  PiiCalibration::CameraParameters intrinsic1(400,400);
  intrinsic1.focalLength = PiiPoint<double>(100,100);
  PiiCalibration::CameraParameters intrinsic2 = intrinsic1;
  PiiCalibration::CameraParameters intrinsic3 = intrinsic1;
  PiiCalibration::RelativePosition extrinsic1(PiiVector<double,3>(0.1, 0.1, 0.1),
                                              PiiVector<double,3>(50.0, 50.0, 50.0));
  PiiCalibration::RelativePosition extrinsic2(PiiVector<double,3>(-0.1, 0.1, 0.1),
                                              PiiVector<double,3>(-50.0, 50.0, 50.0));
  PiiCalibration::RelativePosition extrinsic3(PiiVector<double,3>(0.1, -0.1, 0.1),
                                              PiiVector<double,3>(50.0, -50.0, 50.0));

  PiiMatrix<double> originalWorldPoints(5,3,
                                        0.0, 0.0, 0.0,
                                        10.0, 0.0, 0.0,
                                        0.0, 10.0, 0.0,
                                        0.0, 0.0, 10.0,
                                        10.0, 0.0, 10.0);

  // Project original world points to image plane
  PiiMatrix<double> pixel1 = PiiCalibration::worldToPixelCoordinates(originalWorldPoints, extrinsic1, intrinsic1);
  PiiMatrix<double> pixel2 = PiiCalibration::worldToPixelCoordinates(originalWorldPoints, extrinsic2, intrinsic2);
  PiiMatrix<double> pixel3 = PiiCalibration::worldToPixelCoordinates(originalWorldPoints, extrinsic3, intrinsic3);

  // Add the first two cameras
  PiiStereoTriangulator triangulator;
  triangulator.addCamera(intrinsic1, extrinsic1);
  triangulator.addCamera(intrinsic2, extrinsic2);

  // Find the 3D points with two cameras
  PiiMatrix<double> calculated3DPoints = triangulator.calculate3DPoints(QList<PiiMatrix<double> >() <<
                                                                        pixel1 << pixel2);

  // Convert from camera coordinates to world coordinates
  PiiMatrix<double> calculatedWorldPoints = PiiCalibration::cameraToWorldCoordinates(calculated3DPoints,
                                                                                     extrinsic1);
  //Pii::matlabPrint(std::cout, calculatedWorldPoints);

  // Triangulated points should match the original ones very well
  QVERIFY(Pii::almostEqual(originalWorldPoints, calculatedWorldPoints, 1e-10));

  // Add third camera
  triangulator.addCamera(intrinsic3, extrinsic3);

  // Find the 3D points with three cameras
  PiiMatrix<double> calculatedWorldPoints2 =
    PiiCalibration::cameraToWorldCoordinates(triangulator.calculate3DPoints(QList<PiiMatrix<double> >() <<
                                                                            pixel1 << pixel2 << pixel3),
                                             extrinsic1);

  // Should be pretty close to the known positions
  QVERIFY(Pii::almostEqual(originalWorldPoints, calculatedWorldPoints2, 1e-10));

  // And should be better than with two cameras
  QVERIFY(Pii::sum<double>(Pii::abs(calculatedWorldPoints2 - originalWorldPoints)) <
          Pii::sum<double>(Pii::abs(calculatedWorldPoints - originalWorldPoints)));
}

QTEST_MAIN(TestPiiStereoTriangulator)
