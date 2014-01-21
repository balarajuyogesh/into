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

#include "TestPiiDsp.h"

#include <PiiDsp.h>
#include <PiiFft.h>
#include <PiiMatrixUtil.h>
#include <QtTest>
#include <iostream>

void TestPiiDsp::fftShift()
{
  PiiMatrix<int> mat(2,2,1,2,3,4);
  QVERIFY(Pii::equals(PiiDsp::fftShift(mat), PiiMatrix<int>(2,2,4,3,2,1)));
  mat = PiiDsp::fftShift(mat);
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(2,2,4,3,2,1)));
  PiiMatrix<int> mat2(3,3,
                      1,2,3,
                      4,5,6,
                      7,8,9);
  QVERIFY(Pii::equals(PiiDsp::fftShift(PiiDsp::fftShift(mat2), true), mat2));
}

void TestPiiDsp::fft()
{
  {
    PiiFft<double> fft;
    PiiMatrix<double> input(2, 2,
                            1.0, 2.0,
                            8.0, 7.0);

    // Base 2
    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input))),
                             input,
                             1e-100));
  }
  {
    PiiFft<double> fft;
    PiiMatrix<double> input(4, 4,
                            1.0, 2.0, 3.0, 4.0,
                            8.0, 7.0, 6.0, 5.0,
                            -1.0, 2.0, -4.0, -8.0,
                            -9.0, -6.0, -3.0, 0.0);

    // Base 4
    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input))),
                                 input,
                                 1e-100));
    // Base 8
    PiiMatrix<double> input2(Pii::replicate(input, 2, 2));
    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input2))),
                                 input2,
                                 1e-10));
  }
  {
    PiiFft<float> fft;
    PiiMatrix<float> input(4, 4,
                           1.0, 2.0, 3.0, 4.0,
                           8.0, 7.0, 6.0, 5.0,
                           -1.0, 2.0, -4.0, -8.0,
                           -9.0, -6.0, -3.0, 0.0);

    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input))),
                                 input,
                                 1e-50));
  }

  // Input size not power of two -> less accurate but still close
  {
    PiiFft<double> fft;
    PiiMatrix<double> input(3, 3,
                            1.0, 2.0, 3.0,
                            8.0, 7.0, 6.0,
                            -1.0, 2.0, -4.0);

    // Base 3
    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input))),
                                 input,
                                 1e-10));
  }
  {
    PiiFft<double> fft;
    // Base 5
    PiiMatrix<double> input(5, 5,
                            1.0, 2.0, 3.0, 4.0, 5.0,
                            8.0, 7.0, 6.0, 5.0, 4.0,
                            -1.0, 2.0, -4.0, -8.0, -10.0,
                            -9.0, -6.0, -3.0, 0.0, 3.0,
                            0.0, 1.0, 0.0, 1.0, 0.0);

    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input))),
                                 input,
                                 1e-10));

    // Base 10
    PiiMatrix<double> input2(Pii::replicate(input, 2, 2));
    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input2))),
                                 input2,
                                 1e-10));

    // Mixed radix (20 horizontally, 24 vertically)
    PiiMatrix<double> input3(Pii::replicate(input2, 2, 2));
    input3.insertRow(-1, input3[0]);
    input3.insertRow(-1, input3[1]);
    input3.insertRow(-1, input3[0]);
    input3.insertRow(-1, input3[1]);
    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input3))),
                                 input3,
                                 1e-10));

    // Prime dimensions (17 horizontally, 11 vertically)
    PiiMatrix<double> input4(Pii::concatenate(input2, input2(0,0,-1,7), Pii::Horizontally));
    input3.appendRow();
    QVERIFY(Pii::almostEqual(Pii::real(fft.inverseFft(fft.forwardFft(input4))),
                                 input4,
                                 1e-10));
  }
}

void TestPiiDsp::findPeaks()
{
  try
    {
  static const double aData[] =
    {
      0.013847,0.021728,-3.3047e-05,0.007574,-0.0011177,0.016625,0.016011,-0.0087548,0.016061,0.0063633,
      0.013048,0.015558,0.008482,0.012087,0.008915,0.017723,0.0085345,0.0098406,0.026092,0.028401,
      0.042356,0.056874,0.071651,0.067498,0.097932,0.11384,0.12701,0.16499,0.1977,0.23231,
      0.27074,0.28541,0.33066,0.36477,0.40885,0.44215,0.48502,0.52362,0.55842,0.59356,
      0.61752,0.64178,0.66344,0.68202,0.68171,0.68012,0.66607,0.66144,0.65943,0.65267,
      0.62509,0.58907,0.54501,0.52902,0.45374,0.44262,0.40546,0.36241,0.32562,0.29254,
      0.26114,0.21882,0.19902,0.18793,0.15449,0.12188,0.12061,0.099323,0.098386,0.074357,
      0.063264,0.074124,0.0713,0.084628,0.097838,0.13621,0.12855,0.13852,0.1702,0.1887,
      0.22549,0.26122,0.2937,0.30256,0.34945,0.38166,0.41059,0.44273,0.44965,0.48388,
      0.5146,0.52207,0.5581,0.56439,0.56583,0.57388,0.5638,0.56415,0.55678,0.54675,
      0.53086,0.49973,0.46572,0.4433,0.41096,0.38204,0.34343,0.30134,0.27841,0.24914,
      0.20575,0.19913,0.17932,0.15833,0.13169,0.15288,0.10693,0.12144,0.11435,0.12779,
      0.11867,0.16983,0.19942,0.21158,0.25559,0.29049,0.35399,0.40873,0.45854,0.5322,
      0.63446,0.70681,0.79481,0.87689,0.98606,1.0894,1.1346,1.2593,1.3355,1.4188,
      1.4931,1.548,1.6098,1.6268,1.6405,1.6627,1.6333,1.6159,1.5822,1.5408,
      1.4957,1.4051,1.3273,1.2597,1.1697,1.0558,0.98152,0.88354,0.78564,0.68152,
      0.59897,0.5199,0.46939,0.38916,0.33969,0.27573,0.22791,0.18554,0.14768,0.1368,
      0.10097,0.092758,0.059241,0.052236,0.039065,0.017366,0.019644,0.0093199,0.020892,0.013309,
      -0.0014534,0.0048911,0.0080175,-0.0074851,0.0042732,8.6256e-05,0.0010314,-0.0027769,-0.0032541,-0.015478,
      0.011456,-0.01045,0.0071717,-0.021191,-0.0084446,-0.020149,0.016979,0.0072331,-0.015741,-0.010742
    };

  PiiMatrix<double> matData(1,200, (void*)aData, Pii::RetainOwnership);

  QList<PiiDsp::Peak> lstPeaks = PiiDsp::findPeaks(matData,
                                                   0.12, // levelThreshold
                                                   1e-3, // slopeThreshold
                                                   5,    // smoothWidth
                                                   25);  // windowWidth

  QCOMPARE(lstPeaks.size(), 3);

  QCOMPARE(lstPeaks[0].dataIndex, 44);
  QVERIFY(Pii::abs(lstPeaks[0].position - 45) < 1);
  QVERIFY(Pii::abs(lstPeaks[0].height - 0.68) < 1e-2);

  QCOMPARE(lstPeaks[1].dataIndex, 95);
  QVERIFY(Pii::abs(lstPeaks[1].position - 95) < 1);
  QVERIFY(Pii::abs(lstPeaks[1].height - 0.57) < 1e-2);

  QCOMPARE(lstPeaks[2].dataIndex, 145);
  QVERIFY(Pii::abs(lstPeaks[2].position - 144) < 1);
  QVERIFY(Pii::abs(lstPeaks[2].height - 1.65) < 1e-2);

  double dStep = 0.2;

  PiiMatrix<double> matX(1,200);
  for (int i=0; i<200; ++i)
    matX(0,i) = dStep * i;

  lstPeaks = PiiDsp::findPeaks(Pii::concatenate(matX, matData, Pii::Vertically),
                               0.12, // levelThreshold
                               1e-3, // slopeThreshold
                               5,    // smoothWidth
                               25);  // windowWidth

  QCOMPARE(lstPeaks.size(), 3);

  QCOMPARE(lstPeaks[0].dataIndex, 44);
  QVERIFY(Pii::abs(lstPeaks[0].position - 45 * dStep) < 1);
  QVERIFY(Pii::abs(lstPeaks[0].height - 0.68) < 1e-2);

  QCOMPARE(lstPeaks[1].dataIndex, 95);
  QVERIFY(Pii::abs(lstPeaks[1].position - 95 * dStep) < 1);
  QVERIFY(Pii::abs(lstPeaks[1].height - 0.57) < 1e-2);

  QCOMPARE(lstPeaks[2].dataIndex, 145);
  QVERIFY(Pii::abs(lstPeaks[2].position - 144 * dStep) < 1);
  QVERIFY(Pii::abs(lstPeaks[2].height - 1.65) < 1e-2);

  lstPeaks = PiiDsp::findPeaks(Pii::concatenate(matData, matData, Pii::Horizontally),
                               0.12, // levelThreshold
                               1e-3, // slopeThreshold
                               5,    // smoothWidth
                               25);  // windowWidth

  QCOMPARE(lstPeaks.size(), 6);
    }
  catch (PiiException& ex)
    {
      piiDebug(ex.location() + " " + ex.message());
    }
}


void TestPiiDsp::correlation()
{
  PiiMatrix<int> aint(1,4, 1,2,3,4);
  PiiMatrix<int> bint(1,2, 1,2);
  {
    PiiMatrix<int> result = PiiDsp::correlation<int>(aint,bint);
    QVERIFY(Pii::equals(result, PiiMatrix<int>(1,5, 2, 5, 8, 11, 4)));
  }
  {
    PiiMatrix<int> result = PiiDsp::correlation<int>(aint, bint, PiiDsp::FilterValidPart);
    QVERIFY(Pii::equals(result, PiiMatrix<int>(1,3, 5, 8, 11)));
  }
  {
    PiiMatrix<int> result = PiiDsp::correlation<int>(aint, bint, PiiDsp::FilterOriginalSize);
    QVERIFY(Pii::equals(result, PiiMatrix<int>(1,4, 5, 8, 11, 4)));
  }
  {
    PiiMatrix<double> a(aint);
    PiiMatrix<double> b(bint);
    PiiMatrix<double> result = PiiDsp::correlation<double>(a,b);
    QVERIFY(Pii::equals(result, PiiMatrix<double>(1,5, 2.0, 5.0, 8.0, 11.0, 4.0)));
  }

  {
    PiiMatrix<float> a(aint);
    PiiMatrix<float> b(bint);
    PiiMatrix<float> result = PiiDsp::correlation<float>(a,b);
    QVERIFY(Pii::equals(result, PiiMatrix<float>(1,5, 2.0, 5.0, 8.0, 11.0, 4.0)));
  }
  {
    PiiMatrix<int> a(2,3,
                     1,2,3,
                     4,5,6);
    PiiMatrix<int> b(2,2,
                     1,2,
                     3,4);

    QVERIFY(Pii::equals(PiiDsp::correlation<int>(a,b), PiiMatrix<int>(3,4,
                                                                      4,11,18,9,
                                                                      18,37,47,21,
                                                                      8,14,17,6)));
  }
}

void TestPiiDsp::normalizedCorrelation()
{
  PiiMatrix<int> a(4, 4,
                   1, 2, 3, 4,
                   5, 6, 7, 8,
                   1, 2, 3, 4,
                   5, 6, 7, 8);
  PiiMatrix<int> b(2, 2,
                   1, 2,
                   3, 4);
  PiiMatrix<double> matResult = PiiDsp::normalizedCorrelation<double>(a, b, PiiDsp::FilterValidPart);
  QVERIFY(Pii::equals(matResult,
                      PiiMatrix<double>(3, 3,
                                        44.0/14, 54.0/18, 64.0/22,
                                        28.0/14, 38.0/18, 48.0/22,
                                        44.0/14, 54.0/18, 64.0/22)));
}

void TestPiiDsp::convolution()
{
  {
    PiiMatrix<int> a(2,3,
                     1,2,3,
                     4,5,6);
    PiiMatrix<int> b(2,2,
                     1,2,
                     3,4);
    PiiMatrix<int> c(2,2,
                     4,3,
                     2,1);

    QVERIFY(Pii::equals(PiiDsp::convolution<int>(a,b), PiiMatrix<int>(3,4,
                                                                      1,4,7,6,
                                                                      7,23,33,24,
                                                                      12,31,38,24)));
  }
}

void TestPiiDsp::fastCorrelation()
{
  PiiMatrix<double> a(6,6,
                      1.0, 2.0, 3.0, 4.0, 0.0, 0.0,
                      4.0, 3.0, 2.0, 1.0, 0.0, 0.0,
                      2.0, 3.0, 4.0, 5.0, 0.0, 0.0,
                      5.0, 4.0, 3.0, 2.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  PiiMatrix<double> b(6,6,
                      0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 2.0, 3.0, 4.0,
                      0.0, 0.0, 4.0, 3.0, 2.0, 1.0,
                      0.0, 0.0, 2.0, 3.0, 4.0, 5.0,
                      0.0, 0.0, 5.0, 4.0, 3.0, 2.0,
                      0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  // Find the translation of b wrt a
  PiiMatrixValue<double> peak(PiiDsp::findTranslation(b, a));
  // One row down (positive)
  QCOMPARE(peak.row, 1);
  // Two columns right (also positive)
  QCOMPARE(peak.column, 2);

  peak = PiiDsp::findTranslation<double>(b(0,1,5,5), a(0,0,5,5));
  QCOMPARE(peak.row, 1);
  QCOMPARE(peak.column, 1);
}

QTEST_MAIN(TestPiiDsp)
