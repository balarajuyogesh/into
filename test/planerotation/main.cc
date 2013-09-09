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

#include "TestPiiPlaneRotation.h"

#include <PiiPlaneRotation.h>
#include <PiiMatrixUtil.h>
#include <QtTest>

#include <iostream>

void TestPiiPlaneRotation::givensRotation()
{
  PiiMatrix<double> matA(3, 3,
                         6.0, 5.0, 0.0,
                         5.0, 1.0, 3.0,
                         0.0, 4.0, 3.0);

  double dR;
  PiiPlaneRotation<double> rotation(Pii::givensRotation(6.0, 5.0, &dR));
  QCOMPARE(dR, 7.810249675906654);
  QCOMPARE(rotation.c, 0.7682212795973759);
  QCOMPARE(rotation.s, 0.6401843996644799);
  // This should null out the five
  rotation.rotateColumns(matA, 0, 1);
  QCOMPARE(matA(1,0), 0.0);
}

void TestPiiPlaneRotation::jacobiRotation()
{
  PiiMatrix<double,2,2> matA(1.0, 2.0,
                             2.0, 5.0);
  
  PiiPlaneRotation<double> rotation(Pii::jacobiRotation(1.0, 2.0, 5.0));

  // Two-sided rotation should null out the twos
  Pii::transpose(rotation).rotateColumns(matA, 0, 1);
  rotation.rotateRows(matA, 0, 1);
  QVERIFY(Pii::almostEqualRel(matA(0,1), 0.0, 1e-15));
  QVERIFY(Pii::almostEqualRel(matA(1,0), 0.0, 1e-15));
}

QTEST_MAIN(TestPiiPlaneRotation)
