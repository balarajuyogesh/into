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

#include "TestPiiLbp.h"

#include <PiiLbp.h>
#include <PiiMath.h>
#include <PiiTypeTraits.h>
#include <QtTest>

void TestPiiLbp::basicLbp()
{
  basicLbp<unsigned char>();
  basicLbp<short>();
  basicLbp<int>();
  basicLbp<float>();
  basicLbp<double>();
}

void TestPiiLbp::genericLbp()
{
  genericLbp<unsigned char>();
  genericLbp<short>();
  genericLbp<int>();
  genericLbp<float>();
  genericLbp<double>();
}

void TestPiiLbp::thresholdedLbp()
{
  thresholdedLbp<unsigned char>();
  thresholdedLbp<short>();
  thresholdedLbp<int>();
  thresholdedLbp<float>();
  thresholdedLbp<double>();
}

template <class T> PiiMatrix<T> TestPiiLbp::createRandomImage()
{
  PiiMatrix<T> image(256, 256);
  for (int r=0; r<image.rows(); ++r)
    for (int c=0; c<image.columns(); ++c)
      image(r,c) = (T)rand();
  return image;
}

template <class T> void TestPiiLbp::basicLbp()
{
  PiiMatrix<int> test(5, 5,
                      1, 2, 3, 4, 5,
                      5, 4, 3, 2, 1,
                      6, 6, 6, 6, 6,
                      2, 3, 4, 5, 6,
                      8, 4, 2, 1, 0);
  PiiMatrix<T> testT(test);

  PiiMatrix<int> histogram = PiiLbp::basicLbp<PiiLbp::Histogram>(testT);
  /*  240, 242, 254,
      0,     0,   0,
      111,  15,  15
  */

  PiiMatrix<T> large(createRandomImage<T>());
  PiiMatrix<int> largeHist = PiiLbp::basicLbp<PiiLbp::Histogram>(large);
  PiiMatrix<int> scaledLarge = PiiLbp::basicLbp<PiiLbp::Histogram>(Pii::matrix(PiiMatrix<float>(large) * 2));

  QCOMPARE(histogram(240), 1);
  QCOMPARE(histogram(242), 1);
  QCOMPARE(histogram(254), 1);
  QCOMPARE(histogram(111), 1);
  QCOMPARE(histogram(15), 2);
  QCOMPARE(histogram(0), 3);
  QCOMPARE(Pii::sum<int>(histogram), 9);
  // LBP is invariant to illumination level. Multiplication by two
  // makes no difference.
  QVERIFY(Pii::equals(largeHist, scaledLarge));
}

template <class T> void TestPiiLbp::genericLbp()
{
  PiiMatrix<int> test1(3, 3,
                       20, 10, 9,
                       10, 10, 10,
                       10,  9, 11);
  /* Interpolated:
   * 15   10  9.5
   * 10   10  10
   * 9.8   9  10.3
   *
   * binary:  10001000
   * decimal: 136
   */

  PiiMatrix<int> test2(3, 3,
                       9,  20,  0,
                       20, 10, 20,
                       10,  1, 10);
  /* Interpolated:
   * 13.6 20  9.1
   * 20   10  20
   * 10.2  1  10.2
   *
   * binary:  10111101
   * decimal: 189
   */

  PiiMatrix<int> test3(5, 5,
                       1, 2, 3, 4, 5,
                       5, 4, 3, 2, 1,
                       6, 6, 4, 6, 6,
                       2, 3, 4, 5, 6,
                       8, 4, 2, 1, 0);
  /* Sampling order:
   *   5 4 3
   * 7 6   2 1
   * 8       0
   * 9 a   e f
   *   b c d
   *
   * binary:  1100000110000001
   * decimal: 49537
   */

  PiiMatrix<T> testT(test1);
  PiiMatrix<T> testT2(test2);
  PiiMatrix<T> testT3(test3);

  PiiLbp lbp(8, 1, PiiLbp::Standard, Pii::LinearInterpolation);
  PiiLbp lbp2(16, 2);

  PiiMatrix<int> hist1 = lbp.genericLbp<PiiLbp::Histogram>(testT);
  PiiMatrix<int> hist2 = lbp.genericLbp<PiiLbp::Histogram>(testT2);
  PiiMatrix<int> hist3 = lbp2.genericLbp<PiiLbp::Histogram>(testT3);

  QCOMPARE(hist1(136), 1);
  QCOMPARE(Pii::sum<int>(hist1), 1);
  QCOMPARE(hist2(189), 1);
  QCOMPARE(Pii::sum<int>(hist2), 1);
  QCOMPARE(hist3(49537), 1);
  QCOMPARE(Pii::sum<int>(hist3), 1);
}

template <class T> void TestPiiLbp::thresholdedLbp()
{
  typedef typename Pii::Larger<T,unsigned int>::Type TT;
  PiiMatrix<T> matTest = PiiMatrix<int>(3, 3,
                                        3, 2, 1,
                                        4, 4, 0,
                                        5, 6, 7);

  PiiLbp lbp(8, 1, PiiLbp::Standard, Pii::NearestNeighborInterpolation);
  for (int i=-4; i<4; ++i)
    {
      PiiMatrix<int> matResult(lbp.genericLbp<PiiLbp::Image>(matTest,
                                                             PiiImage::DefaultRoi(),
                                                             std::bind2nd(std::plus<TT>(), i)));
      QCOMPARE(matResult.rows(), 1);
      QCOMPARE(matResult.columns(), 1);
      QCOMPARE(matResult(0,0), 0xff - (1 << (i+5)) + 1);
    }
}

QTEST_MAIN(TestPiiLbp)
