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


#ifndef _TESTPIICALIBRATION_H_
#define _TESTPIICALIBRATION_H_

#include <QObject>
#include <PiiMatrix.h>

class TestPiiCalibration :  public QObject
{
  Q_OBJECT

public:

  TestPiiCalibration() { init(); }
  ~TestPiiCalibration() {}

  void init();

private slots:
  void calculateCameraPosition();
  void calibrateCameras();
  void worldToCameraCoordinates();
  void cameraToWorldCoordinates();
  void unDistort();

  void rotationVectors();
  void rotationVectors_data();
  void normalizedToPixelCoordinates();

private:
  bool _bVerbose;
  QList<PiiMatrix<double> > _lstLeftCameraWorldCoordinates;
  QList<PiiMatrix<double> > _lstLeftCameraImageCoordinates;
};

#endif
