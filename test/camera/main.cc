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

#include "TestPiiCamera.h"

#include <PiiCamera.h>
#include <PiiBayerConverter.h>
#include <PiiColor.h>

#include <QtTest>

void TestPiiCamera::bayerToRgb()
{
  PiiMatrix<unsigned char> test(4,4,
                                2,4,6,8,
                                8,6,4,2,
                                1,2,3,4,
                                5,6,7,8);

  PiiMatrix<PiiColor4<> > rgb(PiiCamera::bayerToRgb(test,PiiCamera::RggbDecoder<>(),
                PiiCamera::RgbPixel<>()));

  QCOMPARE(rgb.rows(), 4);
  QCOMPARE(rgb.columns(), 4);

  //qDebug() << rgb(3,0).rgbR << rgb(3,0).rgbG << rgb(3,0).rgbB;
  QVERIFY(rgb(0,0) == PiiColor4<>(2,6,6));
  QVERIFY(rgb(0,1) == PiiColor4<>(4,4,6));
  QVERIFY(rgb(0,2) == PiiColor4<>(6,5,4));
  QVERIFY(rgb(0,3) == PiiColor4<>(6,8,2));

  QVERIFY(rgb(1,0) == PiiColor4<>(1,8,6));
  QVERIFY(rgb(1,1) == PiiColor4<>(3,4,6));
  QVERIFY(rgb(1,2) == PiiColor4<>(4,4,4));
  QVERIFY(rgb(1,3) == PiiColor4<>(4,5,2));

  QVERIFY(rgb(2,0) == PiiColor4<>(1,5,6));
  QVERIFY(rgb(2,1) == PiiColor4<>(2,2,6));
  QVERIFY(rgb(2,2) == PiiColor4<>(3,4,5));
  QVERIFY(rgb(2,3) == PiiColor4<>(3,4,5));

  QVERIFY(rgb(3,0) == PiiColor4<>(1,5,6));
  QVERIFY(rgb(3,1) == PiiColor4<>(2,4,6));
  QVERIFY(rgb(3,2) == PiiColor4<>(3,7,7));
  QVERIFY(rgb(3,3) == PiiColor4<>(3,5,8));

  PiiMatrix<int> red(PiiCamera::bayerToRgb(test,
                                           PiiCamera::RggbDecoder<>(),
                                           PiiCamera::RedPixel<int>()));
  QVERIFY(Pii::equals(red, PiiMatrix<int>(4,4,
                                          2,4,6,6,
                                          1,3,4,4,
                                          1,2,3,3,
                                          1,2,3,3)));

  PiiMatrix<int> green(PiiCamera::bayerToRgb(test,
                                             PiiCamera::RggbDecoder<>(),
                                             PiiCamera::GreenPixel<int>()));
  QVERIFY(Pii::equals(green, PiiMatrix<int>(4,4,
                                            6,4,5,8,
                                            8,4,4,5,
                                            5,2,4,4,
                                            5,4,7,5)));

  PiiMatrix<int> blue(PiiCamera::bayerToRgb(test,
                                            PiiCamera::RggbDecoder<>(),
                                            PiiCamera::BluePixel<int>()));
  QVERIFY(Pii::equals(blue, PiiMatrix<int>(4,4,
                                           6,6,4,2,
                                           6,6,4,2,
                                           6,6,5,5,
                                           6,6,7,8)));

  PiiMatrix<int> gray(PiiCamera::bayerToRgb(test,
                                            PiiCamera::RggbDecoder<>(),
                                            PiiCamera::GrayPixel<int>()));
  QVERIFY(Pii::equals(gray, (red + green + blue)/3));
}

#if 0
void TestPiiCamera::bayerToRgbSpeed()
{
  PiiMatrix<unsigned char> test(1024,1024);
  QTime t;
  t.start();
  for (int i=0; i<10; ++i)
    PiiMatrix<PiiColor4<> > rgb(PiiCamera::bayerToRgb(test,PiiCamera::RggbDecoder<>(),
                  PiiCamera::RgbPixel<>()));
  qDebug("Full rgb decoding speed: %lf Mb/s", 10000.0/t.elapsed());

  t.restart();
  for (int i=0; i<10; ++i)
    PiiMatrix<int> blue(PiiCamera::bayerToRgb(test,
                                              PiiCamera::RggbDecoder<>(),
                                              PiiCamera::BluePixel<int>()));
  qDebug("Blue channel only decoding speed: %lf Mb/s", 10000.0/t.elapsed());
}
#endif

QTEST_MAIN(TestPiiCamera)
