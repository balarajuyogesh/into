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

#include "TestPiiHoughTransformOperation.h"

#include <QtTest>
#include <PiiMatrixUtil.h>

void TestPiiHoughTransformOperation::initTestCase()
{
  QVERIFY(createOperation("piitransforms", "PiiHoughTransformOperation"));
}

void TestPiiHoughTransformOperation::process()
{
  operation()->setProperty("threadCount", 0);
  operation()->setProperty("maxPeakCount", 9);
  operation()->setProperty("minPeakDistance", 20);

  PiiMatrix<uchar> matInput(500,500);

  // Horizontal lines, strongest on top
  for (int r=1; r<=9; ++r)
    matInput(r*50,0,1,-1) = 255;

  QVERIFY(start(ExpectFail));

  QVERIFY(connectInput("image"));

  QVERIFY(start());

  QVERIFY(sendObject("image", matInput));

  PiiMatrix<int> matEndPoints = outputValue("coordinates", PiiMatrix<int>());
  QCOMPARE(matEndPoints.rows(), 9);

  // Sort lines in ascending order based on y coordinage
  Pii::sortRows(matEndPoints, std::less<int>(), 1);

  for (int r=0; r<9; ++r)
    {
      QCOMPARE(matEndPoints(r,0), 0);
      QVERIFY(qAbs(matEndPoints(r,1) - (r+1)*50) <= 1);
      QCOMPARE(matEndPoints(r,2), 499);
      QCOMPARE(matEndPoints(r,3), matEndPoints(r,1));
    }

  matInput = 0;
  // Vertical lines
  for (int c=1; c<=9; ++c)
    matInput(0,c*50,-1,1) = 255;

  QVERIFY(sendObject("image", matInput));
  matEndPoints = outputValue("coordinates", PiiMatrix<int>());
  QCOMPARE(matEndPoints.rows(), 9);

  // Sort lines in ascending order based on x coordinage
  Pii::sortRows(matEndPoints, std::less<int>(), 0);

  for (int r=0; r<9; ++r)
    {
      QVERIFY(qAbs(matEndPoints(r,0) - (r+1)*50) <= 1);
      QCOMPARE(matEndPoints(r,1), 0);
      QCOMPARE(matEndPoints(r,2), matEndPoints(r,0));
      QCOMPARE(matEndPoints(r,3), 499);
    }

  matInput = 0;
  // Diagonals
  for (int i=0; i<500; ++i)
    matInput(i,i) = matInput(i,499-i) = 255;

  operation()->setProperty("maxPeakCount", 2);

  QVERIFY(sendObject("image", matInput));
  matEndPoints = outputValue("coordinates", PiiMatrix<int>());
  QCOMPARE(matEndPoints.rows(), 2);

  Pii::sortRows(matEndPoints, std::less<int>(), 1);

  QVERIFY(qAbs(matEndPoints(0,0) - 0) <= 1);
  QVERIFY(qAbs(matEndPoints(0,1) - 0) <= 1);
  QVERIFY(qAbs(matEndPoints(0,2) - 499) <= 1);
  QVERIFY(qAbs(matEndPoints(0,3) - 499) <= 1);

  QVERIFY(qAbs(matEndPoints(1,0) - 0) <= 1);
  QVERIFY(qAbs(matEndPoints(1,1) - 499) <= 1);
  QVERIFY(qAbs(matEndPoints(1,2) - 499) <= 1);
  QVERIFY(qAbs(matEndPoints(1,3) - 0) <= 1);
}

QTEST_MAIN(TestPiiHoughTransformOperation)
