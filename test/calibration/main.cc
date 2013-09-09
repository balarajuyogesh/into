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

#include "TestPiiCalibration.h"

#include <QtTest>
#include <QDebug>
#include <PiiMatrix.h>
#include <PiiMatrixUtil.h>
#include <iostream>

#include <PiiCalibration.h>
#ifndef PII_NO_OPENCV
#  include <PiiCalibrationPointFinder.h>
#endif

void TestPiiCalibration::rotationVectors()
{
  QFETCH(double, extremum);
  QFETCH(double, step);
    
  for (double x = -extremum; x<=extremum; x+=step)
    for (double y = -extremum; y<=extremum; y+=step)
      for (double z = -extremum; z<=extremum; z+=step)
        {
          // Create a rotation vector
          PiiVector<double,3> v(x,y,z);
          if (sqrt(v.squaredLength()) > M_PI)
            continue;
          //qDebug("%lf %lf %lf", x, y, z);
          // Turn it into a rotation matrix
          PiiMatrix<double> rotationMatrix(PiiCalibration::rotationVectorToMatrix(v));
          // ... and back
          PiiVector<double,3> v2(PiiCalibration::rotationMatrixToVector(rotationMatrix));

          QCOMPARE(v2[0],v[0]);
          QCOMPARE(v2[1],v[1]);
          QCOMPARE(v2[2],v[2]);
        }
}

void TestPiiCalibration::rotationVectors_data()
{
  QTest::addColumn<double>("extremum");
  QTest::addColumn<double>("step");

  // Ensure the length of the vector won't exceed pi.
  QTest::newRow("Whole range") << 0.05 << sqrt(M_PI*M_PI/3);
  QTest::newRow("Degenerate cases") << 1.0 << 1.0;
  QTest::newRow("Pi base vectors") << M_PI << M_PI;;
}

void TestPiiCalibration::init()
{
  _bVerbose = false;

  //We write image-coordinates from stereo-calibration
  //example for left-camera:

  PiiMatrix<double> x_1(54,2,
                        513.82,  86.53,
                        514.26, 122.79,
                        513.89, 159.38,
                        513.11, 195.60,
                        511.84, 231.60,
                        510.37, 266.23,
                        477.64,  86.26,
                        478.01, 122.22,
                        477.89, 158.33,
                        477.42, 194.36,
                        476.68, 230.01,
                        475.28, 264.64,
                        441.64,  86.28,
                        442.12, 122.06,
                        442.07, 157.86,
                        441.75, 193.63,
                        441.26, 228.61,
                        440.52, 263.24,
                        406.47,  86.75,
                        406.63, 122.15,
                        406.79, 157.53,
                        406.77, 192.51,
                        406.60, 227.63,
                        406.21, 261.69,
                        371.71,  87.90,
                        372.21, 122.24,
                        372.40, 157.39,
                        372.55, 192.03,
                        372.66, 226.33,
                        372.69, 259.92,
                        338.28,  88.85,
                        338.61, 123.12,
                        338.91, 157.43,
                        339.26, 191.55,
                        339.53, 225.39,
                        340.03, 258.27,
                        305.48,  90.33,
                        306.06, 124.01,
                        306.53, 157.63,
                        307.14, 191.04,
                        307.56, 224.28,
                        308.48, 256.53,
                        274.40,  92.19,
                        274.73, 124.87,
                        275.27, 158.09,
                        275.84, 190.54,
                        276.89, 223.41,
                        277.61, 255.12,
                        244.43,  94.16,
                        244.89, 126.22,
                        245.37, 158.31,
                        246.36, 190.37,
                        247.38, 222.25,
                        248.85, 253.61);
                        


  PiiMatrix<double> x_2(54,2,
                        540.09, 133.10,
                        523.41, 181.12,
                        507.88, 224.17,
                        493.47, 262.32,
                        480.25, 296.04,
                        468.05, 325.85,
                        457.05, 352.42,
                        446.69, 376.16,
                        437.74, 396.77,
                        483.13, 120.09,
                        469.55, 169.33,
                        456.81, 213.56,
                        445.33, 252.41,
                        434.77, 286.79,
                        425.19, 317.41,
                        416.48, 344.54,
                        408.44, 368.60,
                        401.31, 389.79,
                        424.17, 107.81,
                        413.96, 157.75,
                        404.58, 202.77,
                        396.12, 242.29,
                        388.42, 277.44,
                        381.62, 308.39,
                        375.30, 336.07,
                        369.58, 360.49,
                        364.47, 382.32,
                        365.09,  96.78,
                        358.51, 146.97,
                        352.42, 192.15,
                        347.04, 232.27,
                        342.31, 267.74,
                        338.12, 299.17,
                        334.32, 327.14,
                        330.95, 352.28,
                        327.79, 374.27,
                        307.04,  86.70,
                        303.79, 136.93,
                        301.11, 182.17,
                        298.78, 222.33,
                        296.89, 258.09,
                        295.13, 289.79,
                        293.62, 318.11,
                        292.63, 343.37,
                        291.52, 365.89,
                        251.47,  78.19,
                        251.12, 128.04,
                        251.31, 172.58,
                        251.74, 212.84,
                        252.65, 248.54,
                        253.28, 280.44,
                        254.35, 308.87,
                        255.25, 334.46,
                        256.21, 357.18);
                        
                        
  
  PiiMatrix<double>x_3 (54,2,
                        603.74, 168.38,
                        595.50, 208.17,
                        585.40, 250.77,
                        573.65, 295.62,
                        559.88, 342.74,
                        544.75, 390.70,
                        562.22, 153.63,
                        552.34, 193.26,
                        541.28, 235.27,
                        528.20, 280.02,
                        513.80, 326.69,
                        497.79, 374.55,
                        519.73, 139.58,
                        508.48, 178.66,
                        495.90, 220.18,
                        481.98, 264.12,
                        466.55, 310.28,
                        449.84, 357.73,
                        477.04, 126.41,
                        464.50, 164.45,
                        450.69, 205.27,
                        435.60, 248.40,
                        419.44, 293.70,
                        402.14, 340.51,
                        434.68, 113.62,
                        421.02, 150.93,
                        406.10, 190.69,
                        389.93, 233.03,
                        373.03, 277.36,
                        355.32, 323.19,
                        393.30, 101.95,
                        378.47, 138.09,
                        362.63, 177.04,
                        345.81, 218.16,
                        327.98, 261.43,
                        309.91, 305.94,
                        352.99,  91.12,
                        337.17, 126.33,
                        320.53, 163.91,
                        303.20, 203.92,
                        284.98, 245.71,
                        266.58, 289.19,
                        314.02,  81.29,
                        297.54, 115.24,
                        280.24, 151.74,
                        262.53, 190.53,
                        244.14, 231.07,
                        225.44, 272.83,
                        277.24,  72.24,
                        260.04, 105.34,
                        242.44, 140.71,
                        224.18, 177.87,
                        205.89, 217.07,
                        187.27, 257.46);

  
  PiiMatrix<double> x_4(54,2,
                        514.57, 109.20,
                        517.53, 152.17,
                        519.88, 197.22,
                        521.39, 243.60,
                        522.17, 290.92,
                        521.99, 338.14,
                        470.27, 110.19,
                        472.43, 153.33,
                        474.01, 198.03,
                        474.92, 244.17,
                        475.36, 291.32,
                        475.05, 338.22,
                        425.85, 112.20,
                        427.01, 154.53,
                        427.90, 198.93,
                        428.50, 244.58,
                        428.61, 291.47,
                        428.26, 337.93,
                        382.42, 114.40,
                        382.67, 156.37,
                        382.76, 200.21,
                        382.70, 245.31,
                        382.56, 291.30,
                        382.24, 337.25,
                        340.16, 117.26,
                        339.49, 158.40,
                        338.92, 201.50,
                        338.57, 245.71,
                        338.14, 290.74,
                        337.55, 335.90,
                        299.35, 120.21,
                        298.14, 160.63,
                        297.10, 202.81,
                        296.13, 246.13,
                        295.26, 290.25,
                        294.71, 334.31,
                        260.36, 123.71,
                        258.51, 163.21,
                        256.93, 204.28,
                        255.45, 246.45,
                        254.64, 289.61,
                        253.86, 332.37,
                        223.29, 127.12,
                        220.91, 165.84,
                        218.84, 205.86,
                        217.26, 246.82,
                        216.11, 288.63,
                        215.30, 330.41,
                        188.57, 130.61,
                        185.59, 168.29,
                        183.34, 207.53,
                        181.28, 247.40,
                        180.13, 287.85,
                        179.35, 328.24);

  PiiMatrix<double> x_5(54,2,
                        436.27,  49.61,
                        449.06,  78.04,
                        462.65, 108.95,
                        477.01, 142.83,
                        492.51, 180.52,
                        508.61, 221.36,
                        525.29, 265.95,
                        542.39, 313.81,
                        559.32, 364.64,
                        397.82,  58.10,
                        408.59,  86.46,
                        420.46, 118.03,
                        433.23, 153.10,
                        446.80, 191.14,
                        461.25, 233.11,
                        476.27, 278.62,
                        491.81, 327.84,
                        507.68, 379.70,
                        358.44,  66.78,
                        367.55,  95.85,
                        377.41, 127.69,
                        388.28, 163.47,
                        399.87, 202.41,
                        412.35, 245.03,
                        425.53, 291.43,
                        439.58, 341.60,
                        453.72, 394.37,
                        319.06,  76.34,
                        326.22, 105.58,
                        334.06, 138.22,
                        342.82, 174.48,
                        352.20, 213.88,
                        362.78, 257.06,
                        373.91, 304.13,
                        385.92, 354.81,
                        398.26, 407.92,
                        279.42,  86.46,
                        284.71, 116.36,
                        290.80, 149.29,
                        297.42, 185.68,
                        304.83, 225.52,
                        312.92, 269.19,
                        322.19, 316.45,
                        332.18, 367.28,
                        342.89, 420.71,
                        241.05,  97.01,
                        244.36, 126.95,
                        248.14, 160.49,
                        253.02, 197.02,
                        258.25, 237.22,
                        264.46, 280.84,
                        271.67, 328.04,
                        279.51, 378.84,
                        288.59, 431.72);


  _lstLeftCameraImageCoordinates.append(x_1);
  _lstLeftCameraImageCoordinates.append(x_2);
  _lstLeftCameraImageCoordinates.append(x_3);
  _lstLeftCameraImageCoordinates.append(x_4);
  _lstLeftCameraImageCoordinates.append(x_5);

  
  //Corresponding world-coordinates
  
  PiiMatrix<double> X_1(54,3,
                        0.00, 240.00,0.00,
                        30.00, 240.00,0.00,
                        60.00, 240.00, 0.00,
                        90.00, 240.00,0.00,
                        120.00, 240.00,   0.00,
                        150.00, 240.00,   0.00,
                        0.00, 210.00,   0.00,
                        30.00, 210.00,   0.00,
                        60.00, 210.00,   0.00,
                        90.00, 210.00,   0.00,
                        120.00, 210.00,   0.00,
                        150.00, 210.00,   0.00,
                        0.00, 180.00,   0.00,
                        30.00, 180.00,   0.00,
                        60.00, 180.00,   0.00,
                        90.00, 180.00,   0.00,
                        120.00, 180.00,   0.00,
                        150.00, 180.00,   0.00,
                        0.00, 150.00,   0.00,
                        30.00, 150.00,   0.00,
                        60.00, 150.00,   0.00,
                        90.00, 150.00,   0.00,
                        120.00, 150.00,   0.00,
                        150.00, 150.00,   0.00,
                        0.00, 120.00,   0.00,
                        30.00, 120.00,   0.00,
                        60.00, 120.00,   0.00,
                        90.00, 120.00,   0.00,
                        120.00, 120.00,   0.00,
                        150.00, 120.00,   0.00,
                        0.00,  90.00,   0.00,
                        30.00,  90.00,   0.00,
                        60.00,  90.00,   0.00,
                        90.00,  90.00,   0.00,
                        120.00,  90.00,   0.00,
                        150.00,  90.00,   0.00,
                        0.00,  60.00,  0.00,
                        30.00,  60.00,   0.00,
                        60.00,  60.00,   0.00,
                        90.00,  60.00,   0.00,
                        120.00,  60.00,   0.00,
                        150.00,  60.00,   0.00,
                        0.00,  30.00,   0.00,
                        30.00,  30.00,  0.00,
                        60.00,  30.00,   0.00,
                        90.00,  30.00,   0.00,
                        120.00,  30.00,   0.00,
                        150.00,  30.00,   0.00,
                        0.00,   0.00,   0.00,
                        30.00,   0.00,   0.00,
                        60.00,   0.00,   0.00,
                        90.00,   0.00,   0.00,
                        120.00,   0.00,   0.00,
                        150.00,   0.00,   0.00);

  
  PiiMatrix<double> X_2(54,3,
                        0.00,150.00,0.00,
                        30.00,150.00,0.00,
                        60.00,150.00,0.00,
                        90.00,150.00,0.00,
                        120.00,150.00,0.00,
                        150.00,150.00,0.00,
                        180.00,150.00,0.00,
                        210.00,150.00,0.00,
                        240.00,150.00,0.00,
                        0.00,120.00,0.00,
                        30.00,120.00,0.00,
                        60.00,120.00,0.00,
                        90.00,120.00,0.00,
                        120.00,120.00,0.00,
                        150.00,120.00,0.00,
                        180.00,120.00,0.00,
                        210.00,120.00,0.00,
                        240.00,120.00,0.00,
                        0.00,90.00,0.00,
                        30.00,90.00,0.00,
                        60.00,90.00,0.00,
                        90.00,90.00,0.00,
                        120.00,90.00,0.00,
                        150.00,90.00,0.00,
                        180.00,90.00,0.00,
                        210.00,90.00,0.00,
                        240.00,90.00,0.00,
                        0.00,60.00,0.00,
                        30.00,60.00,0.00,
                        60.00,60.00,0.00,
                        90.00,60.00,0.00,
                        120.00,60.00,0.00,
                        150.00,60.00,0.00,
                        180.00,60.00,0.00,
                        210.00,60.00,0.00,
                        240.00,60.00,0.00,
                        0.00,30.00,0.00,
                        30.00,30.00,0.00,
                        60.00,30.00,0.00,
                        90.00,30.00,0.00,
                        120.00,30.00,0.00,
                        150.00,30.00,0.00,
                        180.00,30.00,0.00,
                        210.00,30.00,0.00,
                        240.00,30.00,0.00,
                        0.00,0.00,0.00,
                        30.00,0.00,0.00,
                        60.00,0.00,0.00,
                        90.00,0.00,0.00,
                        120.00,0.00,0.00,
                        150.00,0.00,0.00,
                        180.00,0.00,0.00,
                        210.00,0.00,0.00,
                        240.00,0.00,0.00);
  
  PiiMatrix<double> X_3(54,3,
                        0.00,240.00,0.00,
                        30.00,240.00,0.00,
                        60.00,240.00,0.00,
                        90.00,240.00,0.00,
                        120.00,240.00,0.00,
                        150.00,240.00,0.00,
                        0.00,210.00,0.00,
                        30.00,210.00,0.00,
                        60.00,210.00,0.00,
                        90.00,210.00,0.00,
                        120.00,210.00,0.00,
                        150.00,210.00,0.00,
                        0.00,180.00,0.00,
                        30.00,180.00,0.00,
                        60.00,180.00,0.00,
                        90.00,180.00,0.00,
                        120.00,180.00,0.00,
                        150.00,180.00,0.00,
                        0.00,150.00,0.00,
                        30.00,150.00,0.00,
                        60.00,150.00,0.00,
                        90.00,150.00,0.00,
                        120.00,150.00,0.00,
                        150.00,150.00,0.00,
                        0.00,120.00,0.00,
                        30.00,120.00,0.00,
                        60.00,120.00,0.00,
                        90.00,120.00,0.00,
                        120.00,120.00,0.00,
                        150.00,120.00,0.00,
                        0.00,90.00,0.00,
                        30.00,90.00,0.00,
                        60.00,90.00,0.00,
                        90.00,90.00,0.00,
                        120.00,90.00,0.00,
                        150.00,90.00,0.00,
                        0.00,60.00,0.00,
                        30.00,60.00,0.00,
                        60.00,60.00,0.00,
                        90.00,60.00,0.00,
                        120.00,60.00,0.00,
                        150.00,60.00,0.00,
                        0.00,30.00,0.00,
                        30.00,30.00,0.00,
                        60.00,30.00,0.00,
                        90.00,30.00,0.00,
                        120.00,30.00,0.00,
                        150.00,30.00,0.00,
                        0.00,0.00,0.00,
                        30.00,0.00,0.00,
                        60.00,0.00,0.00,
                        90.00,0.00,0.00,
                        120.00,0.00,0.00,
                        150.00,0.00,0.00);
  
  
  PiiMatrix<double> X_4(54,3,
                        0.00,240.00,0.00,
                        30.00,240.00,0.00,
                        60.00,240.00,0.00,
                        90.00,240.00,0.00,
                        120.00,240.00,0.00,
                        150.00,240.00,0.00,
                        0.00,210.00,0.00,
                        30.00,210.00,0.00,
                        60.00,210.00,0.00,
                        90.00,210.00,0.00,
                        120.00,210.00,0.00,
                        150.00,210.00,0.00,
                        0.00,180.00,0.00,
                        30.00,180.00,0.00,
                        60.00,180.00,0.00,
                        90.00,180.00,0.00,
                        120.00,180.00,0.00,
                        150.00,180.00,0.00,
                        0.00,150.00,0.00,
                        30.00,150.00,0.00,
                        60.00,150.00,0.00,
                        90.00,150.00,0.00,
                        120.00,150.00,0.00,
                        150.00,150.00,0.00,
                        0.00,120.00,0.00,
                        30.00,120.00,0.00,
                        60.00,120.00,0.00,
                        90.00,120.00,0.00,
                        120.00,120.00,0.00,
                        150.00,120.00,0.00,
                        0.00,90.00,0.00,
                        30.00,90.00,0.00,
                        60.00,90.00,0.00,
                        90.00,90.00,0.00,
                        120.00,90.00,0.00,
                        150.00,90.00,0.00,
                        0.00,60.00,0.00,
                        30.00,60.00,0.00,
                        60.00,60.00,0.00,
                        90.00,60.00,0.00,
                        120.00,60.00,0.00,
                        150.00,60.00,0.00,
                        0.00,30.00,0.00,
                        30.00,30.00,0.00,
                        60.00,30.00,0.00,
                        90.00,30.00,0.00,
                        120.00,30.00,0.00,
                        150.00,30.00,0.00,
                        0.00,0.00,0.00,
                        30.00,0.00,0.00,
                        60.00,0.00,0.00,
                        90.00,0.00,0.00,
                        120.00,0.00,0.00,
                        150.00,0.00,0.00);
  
  
  PiiMatrix<double> X_5(54,3,
                        0.00,150.00,0.00,
                        30.00,150.00,0.00,
                        60.00,150.00,0.00,
                        90.00,150.00,0.00,
                        120.00,150.00,0.00,
                        150.00,150.00,0.00,
                        180.00,150.00,0.00,
                        210.00,150.00,0.00,
                        240.00,150.00,0.00,
                        0.00,120.00,0.00,
                        30.00,120.00,0.00,
                        60.00,120.00,0.00,
                        90.00,120.00,0.00,
                        120.00,120.00,0.00,
                        150.00,120.00,0.00,
                        180.00,120.00,0.00,
                        210.00,120.00,0.00,
                        240.00,120.00,0.00,
                        0.00,90.00,0.00,
                        30.00,90.00,0.00,
                        60.00,90.00,0.00,
                        90.00,90.00,0.00,
                        120.00,90.00,0.00,
                        150.00,90.00,0.00,
                        180.00,90.00,0.00,
                        210.00,90.00,0.00,
                        240.00,90.00,0.00,
                        0.00,60.00,0.00,
                        30.00,60.00,0.00,
                        60.00,60.00,0.00,
                        90.00,60.00,0.00,
                        120.00,60.00,0.00,
                        150.00,60.00,0.00,
                        180.00,60.00,0.00,
                        210.00,60.00,0.00,
                        240.00,60.00,0.00,
                        0.00,30.00,0.00,
                        30.00,30.00,0.00,
                        60.00,30.00,0.00,
                        90.00,30.00,0.00,
                        120.00,30.00,0.00,
                        150.00,30.00,0.00,
                        180.00,30.00,0.00,
                        210.00,30.00,0.00,
                        240.00,30.00,0.00,
                        0.00,0.00,0.00,
                        30.00,0.00,0.00,
                        60.00,0.00,0.00,
                        90.00,0.00,0.00,
                        120.00,0.00,0.00,
                        150.00,0.00,0.00,
                        180.00,0.00,0.00,
                        210.00,0.00,0.00,
                        240.00,0.00,0.00);


  _lstLeftCameraWorldCoordinates.append(X_1);
  _lstLeftCameraWorldCoordinates.append(X_2);
  _lstLeftCameraWorldCoordinates.append(X_3);
  _lstLeftCameraWorldCoordinates.append(X_4);
  _lstLeftCameraWorldCoordinates.append(X_5);

}

#ifdef PII_NO_OPENCV
#  if QT_VERSION >= 0x050000
#    define SKIPTEST QSKIP("OpenCV disabled")
#  else
#    define SKIPTEST QSKIP("OpenCV disabled", SkipAll)
#  endif
#endif

void TestPiiCalibration::calibrateCameras()
{
#ifdef PII_NO_OPENCV
  SKIPTEST;
#else
   /*     calibrateCamera(const QList<PiiMatrix<double> >& worldPoints,
         const QList<PiiMatrix<double> >& imagePoints,
         CameraParameters& intrinsic,
         QList<RelativePosition>* extrinsic = 0,
         CalibrationOptions options = EstimateIntrinsic)
;

  */
 
  
  PiiCalibration::CameraParameters intrinsic(640,480);
  QList<PiiCalibration::RelativePosition> extrinsic;
 
  try
    {
      calibrateCamera(_lstLeftCameraWorldCoordinates, _lstLeftCameraImageCoordinates, intrinsic, &extrinsic);
    }
  catch (PiiCalibrationException& ob)
    {
      QFAIL(qPrintable(ob.message()));
    }

  _bVerbose = false;
  if (_bVerbose)
    {
      qDebug()<<"Camera intrinsic-parameters: ";
      qDebug()<<"focalLength x : "<<intrinsic.focalLength.x<<" y: "
              <<intrinsic.focalLength.y;
      qDebug()<<"Principal point x: "<<intrinsic.center.x<<" y:"
              <<intrinsic.center.y;
      qDebug()<<"k1: "<<intrinsic.k1;
      qDebug()<<"k2: "<<intrinsic.k2;
      qDebug()<<"p1: "<<intrinsic.p1;
      qDebug()<<"p2: "<<intrinsic.p2;
    }

  QVERIFY(Pii::abs(intrinsic.focalLength.x-533.664) < 0.001);
  QVERIFY(Pii::abs(intrinsic.focalLength.y - 533.835) < 0.001);
  QVERIFY(Pii::abs(intrinsic.center.x-339.248) < 0.001);
  QVERIFY(Pii::abs(intrinsic.center.y-235.23) < 0.001);
  QVERIFY(Pii::abs(intrinsic.k1 + 0.288583) < 0.001); // -0.288583
  QVERIFY(Pii::abs(intrinsic.k2-0.101168) < 0.001);
  QVERIFY(Pii::abs(intrinsic.p1 -0.00187418 ) < 0.001);
  QVERIFY(Pii::abs(intrinsic.p2 +0.000668739) < 0.001); // -0.00668739

  _bVerbose = false;
  
  int size = extrinsic.size();
  QCOMPARE(size,5);
  for(int i = 0; i  < size; ++i)
    {
      if (_bVerbose)
        {
          qDebug()<<"RotationMatrix : i  "<<i;
          Pii::matlabPrint(std::cout,extrinsic[i].rotationMatrix());
          qDebug()<<"TranslationMatrix : i "<<i;
          Pii::matlabPrint(std::cout,extrinsic[i].translationMatrix());
        }
      QVERIFY(extrinsic[i].rotationMatrix().rows() > 0);
      QVERIFY(extrinsic[i].translationMatrix().columns() > 0);
      
    }
#endif
}


void TestPiiCalibration::calculateCameraPosition()
{
#ifdef PII_NO_OPENCV
  SKIPTEST;
#else
  /* Point arrangement
   *
   * 2   1
   *     0
   * 3   4
   *
   * First five points are real, others are faulty measurements.
   */
  PiiMatrix<double> worldPoints(11,3,
                                10.0, 0.0, 0.0,
                                10.0, 10.0, 0.0,
                                -10.0, 10.0, 0.0,
                                -10.0, -10.0, 0.0,
                                10.0, -10.0, 0.0,
                                5.0, 20.0, 1.0, // erroneous points start here
                                -25.0, -20.0, 0.0,
                                -15.0, 10.0, 0.0,
                                -2.5, 11.0, 0.1,
                                0.0, 9.0, 0.0,
                                7.0, 8.7, 0.0,
                                0.0, 0.0, 0.5);


  PiiVector<double,3> rotation(-1.0, 0.01, -0.01);
  // Rotate enough to flip y coordinates. This must be done to match
  // the image coordinate system.
  rotation *= 2;
  PiiVector<double,3> translation(0.0, 0.0, 50.0);
  PiiCalibration::RelativePosition extrinsic(rotation,translation);

  PiiCalibration::CameraParameters intrinsic(400,400);
  intrinsic.focalLength = PiiPoint<double>(600, 600);
  
  PiiMatrix<double> imagePoints = PiiCalibration::cameraToPixelCoordinates(PiiCalibration::worldToCameraCoordinates(worldPoints, extrinsic), intrinsic);

  //Pii::matlabPrint(std::cout, imagePoints(0,0,5,-1));

  PiiCalibrationPointFinder finder;
  PiiCalibration::RelativePosition calculatedPosition = finder.calculateCameraPosition(worldPoints(0,0,5,3), imagePoints, intrinsic);

  //Pii::matlabPrint(std::cout, finder.selectedPoints());
  QCOMPARE(calculatedPosition.translation[0] + 1, translation[0] + 1);
  QCOMPARE(calculatedPosition.translation[1] + 1, translation[1] + 1);
  QCOMPARE(calculatedPosition.translation[2], translation[2]);
  QCOMPARE(calculatedPosition.rotation[0], rotation[0]);
  QCOMPARE(calculatedPosition.rotation[1], rotation[1]);
  QCOMPARE(calculatedPosition.rotation[2], rotation[2]);

  // Test 2: we know the intrinsic parameters
  intrinsic.focalLength = PiiPoint<double>(624.59, 626.17);
  intrinsic.center = PiiPoint<double>(301.08, 244.22);
  intrinsic.k1 = -0.3;
  intrinsic.k2 = 0.62;
  intrinsic.p1 = 0.0021;
  intrinsic.p2 = 0.0017;

  // We know the image points
  PiiMatrix<double> imagePoints2(5,2,
                                 107.0,448.0,
                                 415.0,442.0,
                                 363.0,373.0,
                                 157.0,375.0,
                                 136.0,404.0);

  // And we know the world points
  PiiMatrix<double> worldPoints2(5,3,
                                 0.0,0.0,0.0,
                                 128.6,0.0,0.0,
                                 128.7,126.7,0.0,
                                 0.0,126.6,0.0,
                                 0.0,65.9,0.0);
  worldPoints2 *= 10;


  try
    {
      PiiCalibration::RelativePosition calculatedPosition2 = finder.calculateCameraPosition(worldPoints2, imagePoints2, intrinsic);
      PiiCalibration::RelativePosition calculatedPosition3 = PiiCalibration::calculateCameraPosition(worldPoints2, imagePoints2, intrinsic);

      /*
      qDebug("calculatedPosition2.translation[0]: %lf", calculatedPosition2.translation[0]);
      qDebug("calculatedPosition2.translation[1]: %lf", calculatedPosition2.translation[1]);
      qDebug("calculatedPosition2.translation[2]: %lf", calculatedPosition2.translation[2]);
      qDebug("calculatedPosition2.rotation[0]: %lf", calculatedPosition2.rotation[0]);
      qDebug("calculatedPosition2.rotation[1]: %lf", calculatedPosition2.rotation[1]);
      qDebug("calculatedPosition2.rotation[2]: %lf", calculatedPosition2.rotation[2]);
      */

      // We should find the same position with both methods
      QCOMPARE(calculatedPosition2.translation[0], calculatedPosition3.translation[0]);
      QCOMPARE(calculatedPosition2.translation[1], calculatedPosition3.translation[1]);
      QCOMPARE(calculatedPosition2.translation[2], calculatedPosition3.translation[2]);
      QCOMPARE(calculatedPosition2.rotation[0], calculatedPosition3.rotation[0]);
      QCOMPARE(calculatedPosition2.rotation[1], calculatedPosition3.rotation[1]);
      QCOMPARE(calculatedPosition2.rotation[2], calculatedPosition3.rotation[2]);
      // Maximum pixel error is two
      QVERIFY(Pii::max(Pii::abs(imagePoints2-finder.selectedPoints())) < 2);
    }
  catch (PiiCalibrationException& ex)
    {
      QFAIL(qPrintable(ex.message()));
    }
#endif
}

void TestPiiCalibration::worldToCameraCoordinates()
{
#ifdef PII_NO_OPENCV
  SKIPTEST;
#else
  _bVerbose = false;

  bool verbose = false;
  PiiCalibration::CameraParameters intrinsic(640,480);
  QList<PiiCalibration::RelativePosition> extrinsic;
 
  try
    {
      calibrateCamera(_lstLeftCameraWorldCoordinates, _lstLeftCameraImageCoordinates, intrinsic, &extrinsic);
    }
  catch (PiiCalibrationException& ob)
    {
      QFAIL(qPrintable(ob.message()));
    }

  QVERIFY(extrinsic.size() > 0);
  PiiMatrix<double> worldCoordinates = _lstLeftCameraWorldCoordinates[0];
  PiiMatrix<double> imageCoordinates = _lstLeftCameraImageCoordinates[0];
  QVERIFY(worldCoordinates.rows() > 0);
  QVERIFY(imageCoordinates.rows() > 0);
  
  PiiMatrix<double> res = PiiCalibration::worldToCameraCoordinates(worldCoordinates,extrinsic[0]);
  QVERIFY(res.rows() > 0 );
  
  if (verbose)
    {
      qDebug()<<"Result: ";
      Pii::matlabPrint(std::cout, res);
    }

  PiiMatrix<double> dat = PiiCalibration::cameraToWorldCoordinates(res, extrinsic[0]);
  if (verbose)
    {
      qDebug()<<"Change res back to world";
      Pii::matlabPrint(std::cout,dat);
      qDebug()<<"Right ones..";
      Pii::matlabPrint(std::cout,worldCoordinates);
    }

  QCOMPARE(dat.rows(), worldCoordinates.rows());
  for(int i = 0; i < dat.rows(); ++i)
    {
      for(int j = 0; j < dat.columns(); ++j)
        QVERIFY(Pii::abs(dat(i,j)-worldCoordinates(i,j)) < 0.001);
    }
#endif
}


void TestPiiCalibration::cameraToWorldCoordinates()
{
#ifdef PII_NO_OPENCV
  SKIPTEST;
#else
  _bVerbose = false;
  
  bool verbose = false;

  PiiCalibration::CameraParameters intrinsic(640,480);
  QList<PiiCalibration::RelativePosition> extrinsic;
 
  try
    {
      calibrateCamera(_lstLeftCameraWorldCoordinates, _lstLeftCameraImageCoordinates, intrinsic, &extrinsic);
    }
  catch (PiiCalibrationException& ob)
    {
      QFAIL(qPrintable(ob.message()));
    }

  PiiMatrix<double> testData(2,3, 1.0,2.0,3.0, 4.0,5.0,6.0);
  QVERIFY(extrinsic.size() > 0);
  PiiMatrix<double> res = PiiCalibration::cameraToWorldCoordinates(testData,extrinsic[0]);


  
  if (verbose)
    {
      qDebug()<<"Transformed random data";
      Pii::matlabPrint(std::cout,res);
    }  
#endif
}


void TestPiiCalibration::unDistort()
{
#ifdef PII_NO_OPENCV
  SKIPTEST;
#else
  _bVerbose = false;
  
  bool verbose = false;

  PiiCalibration::CameraParameters intrinsic(640,480);
  QList<PiiCalibration::RelativePosition> extrinsic;
 
  try
    {
      calibrateCamera(_lstLeftCameraWorldCoordinates, _lstLeftCameraImageCoordinates, intrinsic, &extrinsic);
    }
  catch (PiiCalibrationException& ob)
    {
      QFAIL(qPrintable(ob.message()));
    }

  //Then we use undistort-function to get normalized coordinates

  PiiMatrix<double> testNormalizedCameraCoordinates;

  if (verbose)
    {
      qDebug()<<"Pixel-coordinates:";
      Pii::matlabPrint(std::cout,_lstLeftCameraImageCoordinates[0]);
      qDebug()<<"Intrinsic : ";
      qDebug()<<"center: "<<intrinsic.center.x<<" "<<intrinsic.center.y<<" " <<intrinsic.focalLength.x<<" "<<
        intrinsic.focalLength.y << intrinsic.k1 << intrinsic.k2 << intrinsic.p1 << intrinsic.p2;
    }
  
  try
    {
      testNormalizedCameraCoordinates = undistort(_lstLeftCameraImageCoordinates[0],intrinsic);
    }
  catch (PiiCalibrationException& ob)
    {
      QFAIL(qPrintable(ob.message()));
    }

  
  if (verbose)
    {
      qDebug()<<"Normalized coordinates";
      Pii::matlabPrint(std::cout,testNormalizedCameraCoordinates);
    }

  PiiMatrix<double> cameraCoord = PiiCalibration::worldToCameraCoordinates(_lstLeftCameraWorldCoordinates[0],extrinsic[0]);


  
  for(int i = 0; i<cameraCoord.rows(); ++i)
    {
      for(int j = 0; j  < cameraCoord.columns()-1; ++j)
        {
          cameraCoord(i,j) = cameraCoord(i,j)/cameraCoord(i,2);
        }
    }
  
  QCOMPARE(cameraCoord.rows(),testNormalizedCameraCoordinates.rows()); 

  for(int i = 0; i < cameraCoord.rows(); ++i)
    {
      QVERIFY(Pii::abs(cameraCoord(i,0) - testNormalizedCameraCoordinates(i,0)) < 0.001);
      QVERIFY(Pii::abs(cameraCoord(i,1) - testNormalizedCameraCoordinates(i,1)) < 0.001);
    }
#endif  
}

void TestPiiCalibration::normalizedToPixelCoordinates()
{
  PiiCalibration::CameraParameters intrinsic;
  intrinsic.focalLength.x = intrinsic.focalLength.y = 1000;
  intrinsic.center.x = intrinsic.center.y = 256;
  intrinsic.k1 = -0.2;
  PiiMatrix<double> matCorners(4,2,
                               0.5, 0.5,
                               0.5, 511.5,
                               511.5, 511.5,
                               511.5, 0.5);
  PiiMatrix<double> matUndistortedCorners(undistort(matCorners, intrinsic));
  //QDebug d = qDebug();
  //Pii::matlabPrint(d, matUndistortedCorners);
  PiiMatrix<double> matMinCorners(Pii::min(matUndistortedCorners, Pii::Vertically));
  PiiMatrix<double> matMaxCorners(Pii::max(matUndistortedCorners, Pii::Vertically));
  double dXStep = (matMaxCorners(0) - matMinCorners(0)) / 10;
  double dYStep = (matMaxCorners(1) - matMinCorners(1)) / 10;
  for (double dY = matMinCorners(1); dY <= matMaxCorners(1); dY += dYStep)
    for (double dX = matMinCorners(0); dX <= matMaxCorners(0); dX += dXStep)
      {
        double dNewX, dNewY;
        PiiCalibration::normalizedToPixelCoordinates(intrinsic, dX, dY, &dNewX, &dNewY);
        //qDebug("(%lf,%lf) -> (%lf, %lf)", dX, dY, dNewX, dNewY);
      }
}


QTEST_MAIN(TestPiiCalibration) 

