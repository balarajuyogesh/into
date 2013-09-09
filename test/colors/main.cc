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

#include <QtTest>
#include "TestPiiColors.h"
#include <PiiColors.h>
#include <PiiColor.h>
#include <QDebug>
#include <PiiMatrixUtil.h>

void TestPiiColors::sizeOf()
{
  QCOMPARE((unsigned int) sizeof(PiiColor<>), 3u);
  QCOMPARE((unsigned int) sizeof(PiiColor4<>), 4u);
}

void TestPiiColors::accessors()
{
  //Accessors for PiiColor
  {
    PiiColor<short> clr(1,2,3);
    QCOMPARE(clr.channels[0], clr.rgbB);
    QCOMPARE(clr.rgbaB, clr.rgbB);

    QCOMPARE(clr.channels[1], clr.rgbG);
    QCOMPARE(clr.rgbaG, clr.rgbG);

    QCOMPARE(clr.channels[2], clr.rgbR);
    QCOMPARE(clr.rgbaR, clr.rgbR);
  }

  //Accessors for PiiColor4
  {
    PiiColor4<short> clr2(1,2,3,4);
    QCOMPARE(clr2.channels[0], clr2.rgbaB);
    QCOMPARE(clr2.cmykY, clr2.rgbaB);

    QCOMPARE(clr2.channels[1], clr2.rgbaG);
    QCOMPARE(clr2.cmykM, clr2.rgbaG);

    QCOMPARE(clr2.channels[2], clr2.rgbaR);
    QCOMPARE(clr2.cmykC, clr2.rgbaR);

    QCOMPARE(clr2.channels[3], clr2.rgbaA);
    QCOMPARE(clr2.cmykK, clr2.rgbaA);
  }
}

void TestPiiColors::normalizedRgb()
{
  {
    PiiMatrix<PiiColor<> > clrMat(2,2);
    clrMat(0,0) = PiiColor<>(1,2,3); 
    clrMat(0,1) = PiiColor<>(0,0,0);
    clrMat(1,0) = PiiColor<>(10,10,10);
    clrMat(1,1) = PiiColor<>(10,0,0);

    PiiMatrix<unsigned char> red, green;

    PiiColors::normalizedRgb(clrMat, red, green);

    QVERIFY(Pii::equals(red, PiiMatrix<unsigned char>(2,2,
                                                      42,0,
                                                      85,255)));
    

    QVERIFY(Pii::equals(green, PiiMatrix<unsigned char>(2,2,
                                                        85,0,
                                                        85,0)));
  }
  {
    PiiMatrix<PiiColor4<int> > clrMat(2,2);
    clrMat(0,0) = PiiColor4<int>(1,2,3); 
    clrMat(0,1) = PiiColor4<int>(0,0,0);
    clrMat(1,0) = PiiColor4<int>(10,10,10);
    clrMat(1,1) = PiiColor4<int>(10,0,0);

    PiiMatrix<int> red, blue;

    PiiColors::normalizedRgb(clrMat, red, blue, 10, 0, 2);

    QVERIFY(Pii::equals(red, PiiMatrix<int>(2,2,
                                            1,0,
                                            3,10)));
    QVERIFY(Pii::equals(blue, PiiMatrix<int>(2,2,
                                             5,0,
                                             3,0)));
  }
  {
    PiiMatrix<PiiColor4<> > clrMat(2,2);
    clrMat = PiiColor4<>(100,100,100); 

    PiiMatrix<unsigned char> red, blue;

    PiiColors::normalizedRgb(clrMat, red, blue, 10, 0, 2);
    

    QVERIFY(Pii::equals(red, blue));

    QVERIFY(Pii::equals(blue, PiiMatrix<unsigned char>(2,2,
                                                       3,3,
                                                       3,3)));
  }

  {
    PiiMatrix<PiiColor4<> > clrMat(1,1);
    clrMat = PiiColor4<>(250,50,100); 

    PiiMatrix<unsigned char> green, blue;

    PiiColors::normalizedRgb(clrMat, green, blue, 15, 1, 2);

    QVERIFY(Pii::equals(green, PiiMatrix<unsigned char>(1,1,1)));
    QVERIFY(Pii::equals(blue, PiiMatrix<unsigned char>(1,1,3)));
  }
  {
    PiiMatrix<PiiColor4<> > clrMat(1,1);
    clrMat = PiiColor4<>(255,0,0); 

    PiiMatrix<unsigned char> red, green;

    PiiColors::normalizedRgb(clrMat, red, green, 15, 0, 1);

    QVERIFY(Pii::equals(red, PiiMatrix<unsigned char>(1,1,15)));
    QVERIFY(Pii::equals(green, PiiMatrix<unsigned char>(1,1,0)));
  }
}

void TestPiiColors::rgbToHsv()
{
  PiiMatrix<PiiColor4<> > rgbImg(2,2);
  rgbImg(0,0) = PiiColor4<>(0,0,255);
  PiiMatrix<PiiColor4<> > hsvImg = PiiColors::rgbToHsv(rgbImg);
  QCOMPARE(hsvImg(0,0).hsvH, uchar(171));
  QCOMPARE(hsvImg(0,0).hsvS, uchar(255));  
  QCOMPARE(hsvImg(0,0).hsvV, uchar(255));
}

void TestPiiColors::hsvToRgb()
{
  PiiMatrix<PiiColor4<> > hsvImg(2,2);
  hsvImg(0,0) = PiiColor4<>(171,128,255);
  PiiMatrix<PiiColor4<> > rgbImg = PiiColors::hsvToRgb(hsvImg);
  QCOMPARE(rgbImg(0,0).rgbR, uchar(128));
  QCOMPARE(rgbImg(0,0).rgbG, uchar(127));  
  QCOMPARE(rgbImg(0,0).rgbB, uchar(255));
}

void TestPiiColors::rgbToFromHsv()
{
  // Test accuracy over the whole RGB space
  for (int r=0; r<255; r+=16)
    for (int g=0; g<255; g+=16)
      for (int b=0; b<255; b+=16)
        {
          {
            PiiColor<float> rgb(r/255.0,g/255.0,b/255.0);
            PiiColor<float> hsv = PiiColors::rgbToHsv(rgb);
            PiiColor<float> rgb2(PiiColors::hsvToRgb(hsv));
            QVERIFY(Pii::abs(rgb.c0 - rgb2.c0) < 1e-6);
            QVERIFY(Pii::abs(rgb.c1 - rgb2.c1) < 1e-6);
            QVERIFY(Pii::abs(rgb.c2 - rgb2.c2) < 1e-6);
          }
          {
            PiiColor<short> rgb(r,g,b);
            PiiColor<short> hsv = PiiColors::rgbToHsv(rgb);
            PiiColor<short> rgb2(PiiColors::hsvToRgb(hsv));
            QVERIFY(Pii::abs(rgb.c0 - rgb2.c0) <= 2);
            QVERIFY(Pii::abs(rgb.c1 - rgb2.c1) <= 2);
            QVERIFY(Pii::abs(rgb.c2 - rgb2.c2) <= 2);
          }
          {
            PiiColor<> rgb(r,g,b);
            PiiColor<> hsv = PiiColors::rgbToHsv(rgb);
            PiiColor<> rgb2(PiiColors::hsvToRgb(hsv));
            QVERIFY(Pii::abs(rgb.c0 - rgb2.c0) <= 3);
            QVERIFY(Pii::abs(rgb.c1 - rgb2.c1) <= 3);
            QVERIFY(Pii::abs(rgb.c2 - rgb2.c2) <= 3);
          }         
        }
  
  // Does it work with a four-channel color?
  PiiColor<> rgb(1,2,3);
  PiiColor<> hsv = PiiColors::rgbToHsv(rgb);
  PiiColor4<> rgb4(rgb);
  PiiColor4<> hsv4 = PiiColors::rgbToHsv(rgb4);
  QCOMPARE(hsv.c0, hsv4.c0);
  QCOMPARE(hsv.c1, hsv4.c1);
  QCOMPARE(hsv.c2, hsv4.c2);
}

void TestPiiColors::correctGamma()
{
  for (int i=0; i<255; ++i)
    {
      unsigned char c1 = static_cast<unsigned char>(i);
      unsigned char c2 = PiiColors::correctGamma(PiiColors::correctGamma(c1, 1.0/2.2, 255), 2.2, 255);
      QVERIFY(Pii::abs(int(c1)-int(c2)) <= 1); // no more than rounding errors

      int i2 = PiiColors::correctGamma(PiiColors::correctGamma(i, 1.0/2.2, 255), 2.2, 255);
      QVERIFY(Pii::abs(i-i2) <= 1); // no more than rounding errors

      double d1 = double(i)/255.0;
      double d2 = PiiColors::correctGamma(PiiColors::correctGamma(d1, 1.0/2.2), 2.2);
      QCOMPARE(d1, d2);

      PiiColor<float> clr1(float(i)/255.0f);
      PiiColor<float> clr2 = PiiColors::correctGamma(PiiColors::correctGamma(clr1, 1.0/2.2), 2.2);
      QCOMPARE(clr1.c0, clr2.c0);
      QCOMPARE(clr1.c1, clr2.c1);
      QCOMPARE(clr1.c2, clr2.c2);
    }
}

void TestPiiColors::rgbToFromYpbpr()
{
  for (int r=0; r<256; r+=16)
    for (int g=0; g<256; g+=16)
      for (int b=0; b<256; b+=16)
        {
          PiiColor<float> rgb(r,g,b);
          PiiColor<float> ybr(PiiColors::rgbToYpbpr(rgb));
          PiiColor<float> rgb2(PiiColors::ypbprToRgb(ybr));
          /*qDebug("(%f %f %f) -> (%f %f %f) -> (%f %f %f)",
                 rgb.c0, rgb.c1, rgb.c2,
                 ybr.c0, ybr.c1, ybr.c2,
                 rgb2.c0, rgb2.c1, rgb2.c2);
          */
          QVERIFY(ybr.c0 <= 255.0f);
          QVERIFY(ybr.c0 >= 0.0f);
          QVERIFY(ybr.c1 >= -127.5f);
          QVERIFY(ybr.c1 <= 127.5f);
          QVERIFY(ybr.c2 >= -127.5f);
          QVERIFY(ybr.c2 <= 127.5f);
          QVERIFY(Pii::almostEqualRel(rgb.c0, rgb2.c0, 1e-5));
          QVERIFY(Pii::almostEqualRel(rgb.c1, rgb2.c1, 1e-5));
          QVERIFY(Pii::almostEqualRel(rgb.c2, rgb2.c2, 1e-5));
          // Test (255,255,255)
          if (r == 240 && g == 240 && b == 240)
            --r, --g, --b;
        }
}

void TestPiiColors::rgbToFromYcbcr()
{
  for (int r=0; r<256; r+=16)
    for (int g=0; g<256; g+=16)
      for (int b=0; b<256; b+=16)
        {
          PiiColor<> rgb(r,g,b);
          PiiColor<> ybr(PiiColors::rgbToYcbcr(rgb));
          PiiColor<> rgb2(PiiColors::ycbcrToRgb(ybr));

          if (Pii::abs(int(rgb.c0) - rgb2.c0) > 1 ||
              Pii::abs(int(rgb.c1) - rgb2.c1) > 1 ||
              Pii::abs(int(rgb.c2) - rgb2.c2) > 1)
            qDebug("(%d %d %d) -> (%d %d %d) -> (%d %d %d)",
                   rgb.c0, rgb.c1, rgb.c2,
                   ybr.c0, ybr.c1, ybr.c2,
                   rgb2.c0, rgb2.c1, rgb2.c2);
          
          QVERIFY(Pii::abs(int(rgb.c0) - rgb2.c0) < 2);
          QVERIFY(Pii::abs(int(rgb.c1) - rgb2.c1) < 2);
          QVERIFY(Pii::abs(int(rgb.c2) - rgb2.c2) < 2);
          // Test (255,255,255)
          if (r == 240 && g == 240 && b == 240)
            --r, --g, --b;
        }
}

void TestPiiColors::autocorrelogram()
{
  PiiMatrix<int> input1(4,4,
                        1,1,1,1,
                        1,0,0,1,
                        1,0,0,1,
                        1,1,1,1);
  PiiMatrix<int> input2(2,5,
                        1,1,0,0,0,
                        1,1,0,0,0);

  PiiMatrix<float> c1 = PiiColors::autocorrelogram(input1, 3, 2);
  PiiMatrix<float> c2 = PiiColors::autocorrelogram(input2, 4, 2);

  PiiMatrix<float> r1(1,6,
                      3.0/8*4, 2.0/3*4 + 3.0/5*8,
                      0.0, 2.0/5*4 + 4.0/6*8,
                      0.0, 7.0/7*4 + 4.0/4*8);
  PiiMatrix<float> r2(1,8,
                      3.0/5*2 + 1.0*4, 1.0*2 + 3.0/5*2,
                      2.0/4*2 + 1.0*2, 0.0,
                      0.0, 0.0,
                      0.0, 0.0);
  
  QVERIFY(Pii::almostEqual(c1, r1, 1e-6));
  QVERIFY(Pii::almostEqual(c2, r2, 1e-6));
  QVERIFY(Pii::almostEqual(PiiColors::autocorrelogram(Pii::matrix(Pii::transpose(input2)), 4), r2, 1e-6));
}

QTEST_MAIN(TestPiiColors)
