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

#include <iostream>

#include <QtTest>
#include "TestPiiImage.h"

#include <PiiHistogram.h>
#include <PiiMatrixUtil.h>
#include <PiiImage.h>
#include <PiiMath.h>
#include <PiiMorphology.h>
#include <PiiBoundaryFinder.h>
#include <PiiLabeling.h>
#include <PiiFunctional.h>
#include <PiiObjectProperty.h>
#include <PiiMaskGenerator.h>
#include <PiiColor.h>
#include <PiiImageDistortions.h>

#include <functional>

TestPiiImage::TestPiiImage() : _matThreshold(3,3,
                                             1,2,3,
                                             4,5,6,
                                             7,8,9)
{
}

void TestPiiImage::bottomHat()
{
  {
    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    PiiMatrix<int> mask(3,3,
                        1,1,1,
                        1,1,1,
                        1,1,1);

    //BottomHat is operation where first image goes through dilate
    //operation. After dilate operation result image is eroded and
    //subtracted with orginal image
    //Output after dilate:
    // 1,1,1,1,1,1,1,1,
    // 1,1,1,1,1,1,1,1,
    // 1,1,1,1,1,1,1,1,
    // 0,1,1,1,1,1,1,1,
    // 0,1,1,1,1,1,1,1,
    // 0,1,1,1,1,1,1,1,
    // 0,1,1,1,1,1,1,1,
    // 0,1,1,1,1,0,0,0
    //Output after erode:
    // 0 0 0 0 0 0 0 0
    // 0 1 1 1 1 1 1 0
    // 0 0 1 1 1 1 1 0
    // 0 0 1 1 1 1 1 0
    // 0 0 1 1 1 1 1 0
    // 0 0 1 1 1 1 1 0
    // 0 0 1 1 0 0 0 0
    // 0 0 0 0 0 0 0 0
    // After these operations result image is subtracted with orginal
    // image (note that borders are not calculated!)
    // Output after bottomHat
    // 0 0 0 0 0 0 0 0
    // 0 0 0 0 0 0 0 0
    // 0 0 1 0 0 0 0 0
    // 0 0 1 0 0 0 0 0
    // 0 0 0 0 0 0 0 0
    // 0 0 1 0 0 0 0 0
    // 0 0 0 0 0 0 0 0
    // 0 0 0 0 0 0 0 0

    PiiMatrix<int> result(PiiImage::bottomHat(source,mask));

    //Pii::matlabPrint(std::cout, result);
    QVERIFY(Pii::equals(result, PiiMatrix<int>(8,8,
                                               0,0,0,0,0,0,0,0,
                                               0,0,0,0,0,0,0,0,
                                               0,0,1,0,0,0,0,0,
                                               0,0,1,0,0,0,0,0,
                                               0,0,0,0,0,0,0,0,
                                               0,0,1,0,0,0,0,0,
                                               0,0,0,0,0,0,0,0,
                                               0,0,0,0,0,0,0,0)));


  }
}
void TestPiiImage::calculateDirection()
{
  const bool bDoAssertPrints = false;
#define DO_CALCULATE_DIRECTION_ASSERTS()                        \
  if(bDoAssertPrints)                                           \
    {                                                           \
      std::cout << "length = " << length << std::endl;          \
      std::cout << "width  = " << width << std::endl;           \
      Pii::printMatrix(std::cout, matEigenVectors, " ", "\n");  \
      std::cout << std::endl;                                   \
    }                                                           \
  /*Make sure eigenvectors are orthogonal*/                     \
  QVERIFY(Pii::isOrthogonalLike(matEigenVectors));

  {
    PiiMatrix<int> source(8,8,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,1,0,0,0);

    double width = 0.0, length = 0.0;
    const int label = 1;

    PiiMatrix<double> matEigenVectors =  PiiImage::calculateDirection(source, label, &length, &width);
    DO_CALCULATE_DIRECTION_ASSERTS();
    QVERIFY(length > width);
    // Expecting most prominent orientation to be either (0,1) or (0,-1).
    QVERIFY( Pii::almostEqualRel(matEigenVectors(0,0), 0.0) );
    QVERIFY( Pii::almostEqualRel(Pii::abs(matEigenVectors(0,1)), 1.0) );
  }

  {
    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,1,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    double width = 0.0, length = 0.0;
    const int label = 1;

    PiiMatrix<double> matEigenVectors =  PiiImage::calculateDirection(source, label, &length, &width);
    DO_CALCULATE_DIRECTION_ASSERTS();
    QVERIFY(length > width);
    // Expecting most prominent orientation to be either (1,0) or (-1,0).
    QVERIFY( Pii::almostEqualRel(Pii::abs(matEigenVectors(0,0)), 1.0) );
    QVERIFY( Pii::almostEqualRel(matEigenVectors(0,1), 0.0) );

  }

  {

    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          0,0,1,1,1,1,0,0,
                          0,0,1,1,1,1,0,0,
                          0,0,1,1,1,1,0,0,
                          0,0,1,1,1,1,0,0,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    double width = 0.0, length = 0.0;
    const int label = 1;

    PiiMatrix<double> matEigenVectors =  PiiImage::calculateDirection(source, label, &length, &width);
    DO_CALCULATE_DIRECTION_ASSERTS();
    // Expecting length == width (at least almost).
    QVERIFY( Pii::almostEqualRel(length, width) );
    // Expecting eigenvectors (+-1, 0) or (0,+-1)
    QVERIFY( Pii::almostEqualRel(Pii::abs(matEigenVectors(0,0)), 1.0) || Pii::almostEqualRel(Pii::abs(matEigenVectors(0,1)), 1.0) );
    QVERIFY( Pii::almostEqualRel(Pii::abs(matEigenVectors(1,0)), 1.0) || Pii::almostEqualRel(Pii::abs(matEigenVectors(1,1)), 1.0) );


  }

  {
    PiiMatrix<int> source(8,8,
                          2,2,2,2,2,2,2,0,
                          2,2,2,2,2,2,0,0,
                          2,2,2,2,2,0,0,2,
                          2,2,2,2,0,0,2,2,
                          2,2,2,0,0,2,2,2,
                          2,2,0,0,2,2,2,2,
                          2,0,0,2,2,2,2,2,
                          0,0,2,2,2,2,2,2);

    double width = 0.0, length = 0.0;
    const int label = 0;

    PiiMatrix<double> matEigenVectors =  PiiImage::calculateDirection(source, label, &length, &width);

    DO_CALCULATE_DIRECTION_ASSERTS();
    QVERIFY(length > width);
    QVERIFY( Pii::almostEqualRel(Pii::abs(matEigenVectors(0,0)), 1.0/::sqrt(2.0)) );
    QVERIFY( Pii::almostEqualRel(Pii::abs(matEigenVectors(0,1)), 1.0/::sqrt(2.0)) );
  }

  {

    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,1,1,
                          0,0,0,0,1,1,0,0,
                          0,0,1,1,0,0,0,0,
                          1,1,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    double width = 0.0, length = 0.0;
    const int label = 1;

    PiiMatrix<double> matEigenVectors =  PiiImage::calculateDirection(source, label, &length, &width);
    DO_CALCULATE_DIRECTION_ASSERTS();
    QVERIFY(length > width);
    QVERIFY( Pii::abs(matEigenVectors(0,0)) > Pii::abs(matEigenVectors(0,1)) );
  }

  {
    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,
                          1,1,0,0,0,0,0,0,
                          0,0,1,1,0,0,0,0,
                          0,0,0,0,1,1,0,0,
                          0,0,0,0,0,0,1,1,
                          0,0,0,0,0,0,0,0);

    double width = 0.0, length = 0.0;
    const int label = 1;

    PiiMatrix<double> matEigenVectors =  PiiImage::calculateDirection(source, label, &length, &width);
    DO_CALCULATE_DIRECTION_ASSERTS();
    QVERIFY(length > width);
    QVERIFY( Pii::abs(matEigenVectors(0,0)) > Pii::abs(matEigenVectors(0,1)) );
  }

  {
    PiiMatrix<int> source(8,8,
                          0,0,1,0,0,0,0,0,
                          0,0,0,1,0,0,0,0,
                          0,0,0,1,0,0,0,0,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,1,0,0,0,
                          0,0,0,0,0,1,0,0,
                          0,0,0,0,0,1,0,0,
                          0,0,0,0,0,0,1,0);

    double width = 0.0, length = 0.0;
    const int label = 1;

    PiiMatrix<double> matEigenVectors =  PiiImage::calculateDirection(source, label, &length, &width);
    DO_CALCULATE_DIRECTION_ASSERTS();
    QVERIFY(length > width);
    QVERIFY( Pii::abs(matEigenVectors(0,0)) < Pii::abs(matEigenVectors(0,1)) );
  }

  {
    PiiMatrix<int> source(1,1,1);

    double width = 0.0, length = 0.0;
    const int label = 1;

    PiiMatrix<double> matEigenVectors =  PiiImage::calculateDirection(source, label, &length, &width);
  }

}
void TestPiiImage::calculateProperties()
{
  {
    //8-Connected labeled image
    PiiMatrix<int> source(10,10,
                          0,0,0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,0,2,2,
                          0,0,0,1,0,0,1,0,2,2,
                          0,0,0,1,0,0,1,0,2,2,
                          0,0,1,1,0,0,1,0,2,2,
                          0,0,0,1,1,1,1,0,0,0,
                          0,0,1,1,0,0,0,0,0,0,
                          0,0,0,0,0,3,0,0,0,0,
                          4,4,0,0,0,0,0,0,0,0,
                          4,4,0,0,0,0,0,0,0,0);

    int nbLabels = 4;  // Labels 1,2,3,4
    PiiMatrix<int> areas;
    PiiMatrix<int> centroids;
    PiiMatrix<int> bbox;
    PiiImage::calculateProperties(source, nbLabels, areas, centroids, bbox);

    QCOMPARE(centroids.rows(), 4);
    QCOMPARE(centroids.columns(),2);
    QCOMPARE(areas.rows(),4);
    QCOMPARE(areas.columns(),1);
    QCOMPARE(bbox.rows(),4);
    QCOMPARE(bbox.columns(),4);

    QCOMPARE(areas(0),20);
    QCOMPARE(areas(1),8);
    QCOMPARE(areas(2),1);
    QCOMPARE(areas(3),4);

    //Note! that in centroids matrix first column is column position
    //starting from index zero. Second column is row position starting
    //from index zero.

    QVERIFY(Pii::equals(centroids,PiiMatrix<int>(4,2,
                                                 4,3,
                                                 9,3,
                                                 5,7,
                                                 1,9)));

    QVERIFY(Pii::equals(bbox,PiiMatrix<int>(4,4,
                                            0,1,7,6,
                                            8,1,2,4,
                                            5,7,1,1,
                                            0,8,2,2)));



  }
}

void TestPiiImage::sweepLine()
{
  {

    PiiMatrix<int> source(10,10,
                          0,0,0,0,0,0,0,0,0,0,
                          0,0,1,0,0,0,0,0,0,0,
                          0,0,0,1,0,0,0,2,0,0,
                          0,0,0,0,1,0,2,0,0,0,
                          0,0,0,0,0,1,0,0,0,0,
                          0,0,0,0,2,0,1,0,0,0,
                          0,0,0,2,0,0,0,1,0,0,
                          0,0,0,0,0,0,0,0,10,0,
                          0,0,0,0,0,0,0,0,0,10,
                          0,0,0,0,0,0,0,0,0,0);

    PiiMatrix<double> coordinates(1,4,
                                  2.0,1.0,7.0,6.0);




    PiiMatrix<double> result = PiiImage::sweepLine(source,
                                                   coordinates,
                                                   PiiImage::SweepCollectorFunction<double,int>(),
                                                   3).matCoordinates;

    QVERIFY(result.rows() != 0);
    QVERIFY(result.columns() != 0);


  }


}

void TestPiiImage::close()
{
  {
    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    PiiMatrix<int> mask(3,3,
                        1,1,1,
                        1,1,1,
                        1,1,1);

    PiiMatrix<int> result(PiiImage::close(source, mask));
    QVERIFY(Pii::equals(result,PiiMatrix<int>(8,8,
                                              0,0,0,0,0,0,0,0,
                                              0,1,1,1,1,1,1,0,
                                              0,0,1,1,1,1,1,0,
                                              0,0,1,1,1,1,1,0,
                                              0,0,1,1,1,1,1,0,
                                              0,0,1,1,1,1,1,0,
                                              0,0,1,1,0,0,0,0,
                                              0,0,0,0,0,0,0,0)));

  }
}

void TestPiiImage::open()
{
  {
    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    PiiMatrix<int> mask(3,3,
                        1,1,1,
                        1,1,1,
                        1,1,1);

    PiiMatrix<int> result(PiiImage::open(source, mask));
    QVERIFY(Pii::equals(result,PiiMatrix<int>(8,8,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,1,1,1,1,0,
                                              0,0,0,1,1,1,1,0,
                                              0,0,0,1,1,1,1,0,
                                              0,0,0,1,1,1,1,0,
                                              0,0,0,1,1,1,1,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0)));

  }
}

void TestPiiImage::createMask()
{
  { //5x5
    PiiMatrix<int> expectedRec(5,5);
    expectedRec = 1;
    PiiMatrix<int> expectedEll(5,5);
    expectedEll = 1;
    expectedEll(0,0) = 0;
    expectedEll(0,4) = 0;
    expectedEll(4,0) = 0;
    expectedEll(4,4) = 0;
    PiiMatrix<int> expectedDia(5,5, 0, 0, 1, 0, 0,
                               0, 1, 1, 1, 0,
                               1, 1, 1, 1, 1,
                               0, 1, 1, 1, 0,
                               0, 0, 1, 0, 0);
    PiiMatrix<int> mask(5,5);
    PiiImage::createMask(PiiImage::RectangularMask, mask);
    QVERIFY(Pii::equals(expectedRec,mask));
    QVERIFY(Pii::equals(expectedRec,PiiImage::createMask(PiiImage::RectangularMask, 5, 5)));

    mask = 0;
    PiiImage::createMask(PiiImage::EllipticalMask, mask);
    QVERIFY(Pii::equals(mask,expectedEll));
    QVERIFY(Pii::equals(expectedEll,PiiImage::createMask(PiiImage::EllipticalMask, 5, 5)));

    mask = 0;
    PiiImage::createMask(PiiImage::DiamondMask, mask);
    QVERIFY(Pii::equals(expectedDia,mask));
    QVERIFY(Pii::equals(expectedDia,PiiImage::createMask(PiiImage::DiamondMask, 5, 5)));
  }

  /* Some masks for visual inspection.
     {
     Pii::printMatrix(std::cout, PiiImage::createMask(PiiImage::RectangularMask, 29, 29), " ", "\n");
     std::cout << std::endl;
     Pii::printMatrix(std::cout, PiiImage::createMask(PiiImage::EllipticalMask, 49, 29), " ", "\n");
     std::cout << std::endl;
     Pii::printMatrix(std::cout, PiiImage::createMask(PiiImage::DiamondMask, 29, 39), " ", "\n");
     }
  */
}

void TestPiiImage::erode()
{
  {
    PiiMatrix<int> source(6,6,
                          0,0,0,1,0,0,
                          0,1,1,1,1,0,
                          0,1,1,1,1,1,
                          0,1,1,1,1,0,
                          0,1,1,1,1,0,
                          0,0,0,0,0,0);
    PiiMatrix<bool> mask(3,3);
    mask = true;
    PiiMatrix<int> result(PiiImage::erode(source, mask));
    QVERIFY(Pii::equals(result,PiiMatrix<int>(6,6,
                                              0,0,0,0,0,0,
                                              0,0,0,0,0,0,
                                              0,0,1,1,0,0,
                                              0,0,1,1,0,0,
                                              0,0,0,0,0,0,
                                              0,0,0,0,0,0)));


  }

  {
    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    PiiMatrix<int> mask(3,3,
                        1,1,1,
                        1,1,1,
                        1,1,1);

    PiiMatrix<int> result = PiiImage::erode(source, mask);
    QVERIFY(Pii::equals(result,PiiMatrix<int>(8,8,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,1,1,0,0,
                                              0,0,0,0,1,1,0,0,
                                              0,0,0,0,1,1,0,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0)));
  }

  {

    PiiMatrix<int> source(8,8,
                          1,1,0,0,0,0,0,1,
                          1,1,1,1,1,1,1,0,
                          1,1,0,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,0,0,0,0,
                          1,0,0,0,0,0,0,0);

    PiiMatrix<int> mask(3,3,
                        1,1,1,
                        1,1,1,
                        1,1,1);

    PiiMatrix<int> result = PiiImage::erode(source, mask,true);
    QVERIFY(Pii::equals(result,PiiMatrix<int>(8,8,
                                              1,0,0,0,0,0,0,0,
                                              1,0,0,0,0,0,0,0,
                                              0,0,0,0,1,1,0,0,
                                              0,0,0,0,1,1,0,0,
                                              0,0,0,0,1,1,0,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0)));



  }

  {
    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,1,
                          0,0,0,1,1,1,1,1,
                          0,0,0,1,1,1,1,1,
                          0,0,1,1,1,1,1,1,
                          0,0,0,1,1,1,1,1,
                          0,0,1,1,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    PiiMatrix<int> mask(3,4,
                        1,1,1,1,
                        1,1,1,1,
                        1,1,1,1); // origin (1,2)

    PiiMatrix<int> result = PiiImage::erode(source, mask,false);
    //Pii::matlabPrint(std::cout, result);

    QVERIFY(Pii::equals(result,PiiMatrix<int>(8,8,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,1,1,0,
                                              0,0,0,0,0,1,1,0,
                                              0,0,0,0,0,1,1,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0)));





    mask = PiiMatrix<int>(2,4,
                          1,1,1,1,
                          1,1,1,1); //
                                    //
                                    //if first element is
                                    //0,0 then origin is (1,2)


    result = PiiImage::erode(source, mask,false);
    QVERIFY(Pii::equals(result,PiiMatrix<int>(8,8,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,1,1,0,
                                              0,0,0,0,0,1,1,0,
                                              0,0,0,0,0,1,1,0,
                                              0,0,0,0,0,1,1,0,
                                              0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0)));
  }



}


void TestPiiImage::dilate()
{
  {

    PiiMatrix<bool> mask(3,3);
    mask = true;
    PiiMatrix<int> source(6,6,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,1,1,0,0,
                          0,0,1,1,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0);

    PiiMatrix<int> result = PiiImage::dilate(source, mask);
    QVERIFY(Pii::equals(result,PiiMatrix<int>(6,6,
                                              0,0,0,0,0,0,
                                              0,1,1,1,1,0,
                                              0,1,1,1,1,0,
                                              0,1,1,1,1,0,
                                              0,1,1,1,1,0,
                                              0,0,0,0,0,0)));
  }
  {


    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,1,1,1,0,
                          0,0,0,1,1,1,1,0,
                          0,0,1,1,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    PiiMatrix<int> mask(3,3,
                        1,1,1,
                        1,1,1,
                        1,1,1);

    PiiMatrix<int> result = PiiImage::dilate(source, mask);
    //Pii::matlabPrint(std::cout,result);
    QCOMPARE(result.rows(), source.rows());
    QCOMPARE(result.columns(), source.columns());
    QVERIFY(Pii::equals(result,PiiMatrix<int>(8,8,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,1,1,1,
                                              0,1,1,1,1,1,1,1,
                                              0,1,1,1,1,1,1,1,
                                              0,1,1,1,1,1,1,1,
                                              0,1,1,1,1,1,1,1,
                                              0,1,1,1,1,0,0,0)));


  }
  {


    PiiMatrix<int> source(8,8,
                          0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,1,
                          0,0,0,1,1,1,1,1,
                          0,0,0,1,1,1,1,1,
                          0,0,1,1,1,1,1,1,
                          0,0,0,1,1,1,1,1,
                          0,0,1,1,0,0,0,0,
                          0,0,0,0,0,0,0,0);

    PiiMatrix<int> mask(3,4,
                        1,1,1,1,
                        1,1,1,1,
                        1,1,1,1); // origin (1,2)

    PiiMatrix<int> result = PiiImage::dilate(source, mask);
    // Pii::matlabPrint(std::cout, result);
    QVERIFY(Pii::equals(result,PiiMatrix<int>(8,8,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,1,1,1,
                                              1,1,1,1,1,0,0,0)));

  }
}
void TestPiiImage::scaleLinearInterpolation()
{
  PiiMatrix<int> input(3,3,
                       1,2,3,
                       4,5,6,
                       7,8,9);
  PiiMatrix<int> temp(5,5,
                      2,3,4,5,6,
                      5,6,7,8,9,
                      8,9,10,11,12,
                      11,12,13,14,15,
                      14,15,16,17,18);

  QCOMPARE(Pii::sum<float>(PiiImage::scale(PiiMatrix<float>(input), 5,5) * 2 - temp), 0.0f);

  PiiMatrix<int> input2(4,4,
                        0,2,1,3,
                        2,0,3,1,
                        3,1,2,0,
                        1,3,0,2);
  QVERIFY(Pii::equals(PiiImage::scale(input2, 0.5),PiiMatrix<int>(2,2, 1, 2, 2, 1)));
}

void TestPiiImage::scaleNearestNeighborInterpolation()
{
  PiiMatrix<int> input(4,4,
                       1,2,3,4,
                       5,6,7,8,
                       9,10,11,12,
                       13,14,15,16);

  QVERIFY(Pii::equals(PiiImage::scale(input, 0.5, Pii::NearestNeighborInterpolation),PiiMatrix<int>(2,2,
                                                                                                    1,3,
                                                                                                    9,11)));

  QVERIFY(Pii::equals(PiiImage::scale(input, 4,2, Pii::NearestNeighborInterpolation),PiiMatrix<int>(4,2,
                                                                                                    1,3,
                                                                                                    5,7,
                                                                                                    9,11,
                                                                                                    13,15)));

  QVERIFY(Pii::equals(PiiImage::scale(input, 2,4, Pii::NearestNeighborInterpolation),PiiMatrix<int>(2,4,
                                                                                                    1,2,3,4,
                                                                                                    9,10,11,12)));

  QVERIFY(Pii::equals(PiiImage::scale(input, 8,8, Pii::NearestNeighborInterpolation),PiiMatrix<int>(8,8,
                                                                                                    1,1,2,2,3,3,4,4,
                                                                                                    1,1,2,2,3,3,4,4,
                                                                                                    5,5,6,6,7,7,8,8,
                                                                                                    5,5,6,6,7,7,8,8,
                                                                                                    9,9,10,10,11,11,12,12,
                                                                                                    9,9,10,10,11,11,12,12,
                                                                                                    13,13,14,14,15,15,16,16,
                                                                                                    13,13,14,14,15,15,16,16)));

  QVERIFY(Pii::equals(PiiImage::scale(input, 8,2, Pii::NearestNeighborInterpolation),PiiMatrix<int>(8,2,
                                                                                                    1,3,
                                                                                                    1,3,
                                                                                                    5,7,
                                                                                                    5,7,
                                                                                                    9,11,
                                                                                                    9,11,
                                                                                                    13,15,
                                                                                                    13,15)));

  QVERIFY(Pii::equals(PiiImage::scale(input, 2,8, Pii::NearestNeighborInterpolation),PiiMatrix<int>(2,8,
                                                                                                    1,1,2,2,3,3,4,4,
                                                                                                    9,9,10,10,11,11,12,12)));


  QVERIFY(Pii::equals(PiiImage::scale(input, 4,4, Pii::NearestNeighborInterpolation),input));
}

void TestPiiImage::scaleColor()
{
  PiiMatrix<int> input(4,4,
                       0x01010101,0x02020202,0x03030303,0x04040404,
                       0x05050505,0x06060606,0x07070707,0x08080808,
                       0x09090909,0x10101010,0x11111111,0x12121212,
                       0x13131313,0x14141414,0x15151515,0x16161616);
  PiiMatrix<int> result(2,2,
                        0x01010101,0x03030303,
                        0x09090909,0x11111111);

  PiiMatrix<PiiColor4<unsigned char> >* pInput = reinterpret_cast<PiiMatrix<PiiColor4<unsigned char> >*>(&input);
  PiiMatrix<PiiColor4<unsigned char> >* pResult = reinterpret_cast<PiiMatrix<PiiColor4<unsigned char> >*>(&result);

  QVERIFY(Pii::equals(PiiImage::scale(*pInput, 0.5, Pii::NearestNeighborInterpolation),*pResult));

  PiiMatrix<int> input2(4,4,
                        0x00000000,0x02020204,0x01010501,0x03030303,
                        0x02020204,0x00000000,0x03030303,0x01010501,
                        0x03030303,0x01030101,0x02020202,0x02000000,
                        0x01030101,0x03030303,0x02000000,0x02020202);
  PiiMatrix<int> result2(2,2,
                         0x01010102,0x02020402,
                         0x02030202,0x02010101);

  PiiMatrix<PiiColor4<unsigned char> >* pInput2 = reinterpret_cast<PiiMatrix<PiiColor4<unsigned char> >*>(&input2);
  PiiMatrix<PiiColor4<unsigned char> >* pResult2 = reinterpret_cast<PiiMatrix<PiiColor4<unsigned char> >*>(&result2);

  QVERIFY(Pii::equals(PiiImage::scale(*pInput2, 0.5),*pResult2));
}

void TestPiiImage::rotate()
{
  PiiMatrix<int> mat(3,3,
                     1,2,3,
                     4,5,6,
                     7,8,9);

  PiiMatrix<int> matNotSquare(3,5,
                              1,2,3,4,5,
                              6,7,8,9,10,
                              11,12,13,14,15);

  QVERIFY(Pii::equals(PiiImage::rotate(mat, 0.0),mat));

  QVERIFY(Pii::equals(PiiImage::rotate(mat, -M_PI/2),PiiMatrix<int>(3,3,
                                                                    3,6,9,
                                                                    2,5,8,
                                                                    1,4,7)));

  QVERIFY(Pii::equals(PiiImage::rotate(matNotSquare,3*M_PI/2),PiiMatrix<int>(5,3,
                                                                             5,10,15,
                                                                             4,9,14,
                                                                             3,8,13,
                                                                             2,7,12,
                                                                             1,6,11)));



  QVERIFY(Pii::equals(PiiImage::rotate(mat, -M_PI),PiiMatrix<int>(3,3,
                                                                  9,8,7,
                                                                  6,5,4,
                                                                  3,2,1)));



  QVERIFY(Pii::equals(PiiImage::rotate(mat, -3.0*M_PI/2),PiiMatrix<int>(3,3,
                                                                        7,4,1,
                                                                        8,5,2,
                                                                        9,6,3)));

  PiiMatrix<int> mat2(3,2,
                      1,2,
                      4,5,
                      7,8);

  QVERIFY(Pii::equals(PiiImage::rotate(mat2, 0.0),mat2));

  QVERIFY(Pii::equals(PiiImage::rotate(mat2, -M_PI/2),PiiMatrix<int>(2,3,
                                                                     2,5,8,
                                                                     1,4,7)));

  QVERIFY(Pii::equals(PiiImage::rotate(mat2, M_PI),PiiMatrix<int>(3,2,
                                                                  8,7,
                                                                  5,4,
                                                                  2,1)));

  QVERIFY(Pii::equals(PiiImage::rotate(mat2, M_PI/2),PiiMatrix<int>(2,3,
                                                                    7,4,1,
                                                                    8,5,2)));
}

void TestPiiImage::colorChannel()
{
  PiiMatrix<PiiColor4<> > img(4,4);
  PiiColor4<> clr(1,2,3,0);
  img = clr;
  PiiMatrix<unsigned char> ch0 = PiiImage::colorChannel(img, 0);
  PiiMatrix<unsigned char> ch1 = PiiImage::colorChannel(img, 1);
  PiiMatrix<unsigned char> ch2 = PiiImage::colorChannel(img, 2);
  QCOMPARE(ch0(0,0), static_cast<unsigned char>(1));
  QCOMPARE(ch1(1,2), static_cast<unsigned char>(2));
  QCOMPARE(ch2(3,3), static_cast<unsigned char>(3));
}

void TestPiiImage::setColorChannel()
{
  PiiMatrix<PiiColor4<> > img(4,4);
  PiiMatrix<unsigned char> ch0(4,4);
  PiiMatrix<unsigned char> ch1(4,4);
  ch0 = 1;
  ch1 = 2;
  PiiImage::setColorChannel(img,0,ch0);
  PiiImage::setColorChannel(img,1,ch1);
  PiiImage::setColorChannel(img,2,3);
  QCOMPARE(img(0,0).channel(0), static_cast<unsigned char>(1));
  QCOMPARE(img(1,2).channel(1), static_cast<unsigned char>(2));
  QCOMPARE(img(3,3).channel(2), static_cast<unsigned char>(3));

  PiiMatrix<PiiColor4<> > img2(4,4);
  PiiImage::setColorChannel(img2,2,PiiImage::colorChannel(img,2));
  for (int c=2; c--; )
    PiiImage::setColorChannel(img2,c,PiiImage::colorChannel(img,c));

  QVERIFY(Pii::equals(img,img2));
}

void TestPiiImage::hysteresisThreshold()
{
  PiiMatrix<int> source(8,8,
                        0,0,0,0,0,0,0,0,
                        0,1,2,3,4,5,5,0,
                        0,1,1,2,2,2,5,0,
                        0,0,0,0,0,0,2,0,
                        0,6,6,0,1,1,2,0,
                        0,6,6,0,1,3,1,0,
                        0,2,6,1,1,1,5,0,
                        0,0,0,0,0,0,0,0);
  QVERIFY(Pii::equals(PiiImage::hysteresisThreshold(source, 2, 5),
                      PiiMatrix<int>(8,8,
                                     0,0,0,0,0,0,0,0,
                                     0,0,1,1,1,1,1,0,
                                     0,0,0,1,1,1,1,0,
                                     0,0,0,0,0,0,1,0,
                                     0,1,1,0,0,0,1,0,
                                     0,1,1,0,0,1,0,0,
                                     0,1,1,0,0,0,1,0,
                                     0,0,0,0,0,0,0,0)));

}

void TestPiiImage::detectEdges()
{
  PiiMatrix<int> source(8,8,
                        0,0,0,0,0,0,0,0,
                        0,5,4,3,3,4,5,0,
                        0,5,5,5,5,5,4,0,
                        0,3,5,5,5,5,3,0,
                        0,5,5,5,5,5,2,0,
                        0,5,5,5,5,5,4,0,
                        0,5,5,5,5,5,5,0,
                        0,0,0,0,0,0,0,0);
  QVERIFY(Pii::equals(PiiImage::detectEdges(source, 0, 20, 10),
                      PiiMatrix<int>(8,8,
                                     0,0,0,0,0,0,0,0,
                                     0,1,1,1,1,1,1,0,
                                     0,1,0,0,0,0,1,0,
                                     0,1,0,0,0,0,1,0,
                                     0,1,0,0,0,0,1,0,
                                     0,1,0,0,0,0,1,0,
                                     0,1,1,1,1,1,1,0,
                                     0,0,0,0,0,0,0,0)));

}

template <class TernaryFunction, class T>
PiiMatrix<typename TernaryFunction::result_type> TestPiiImage::apply(const PiiMatrix<T>& mat,
                                                                     TernaryFunction func,
                                                                     typename TernaryFunction::second_argument_type p1,
                                                                     typename TernaryFunction::third_argument_type p2)
{
  typedef typename TernaryFunction::result_type R;
  PiiMatrix<R> result(PiiMatrix<T>::uninitialized(mat.rows(), mat.columns()));
  for (int r=0; r<mat.rows(); ++r)
    for (int c=0; c<mat.columns(); ++c)
      result(r,c) = func(mat(r,c), p1, p2);
  return result;
}

void TestPiiImage::adaptiveThreshold()
{
  using namespace PiiImage;

  const PiiMatrix<int> source(3,3,
                              1,2,3,
                              4,5,6,
                              7,8,9);

  PiiMatrix<int> result = apply(source,
                                meanStdThresholdFunction(InverseThresholdFunction<int>(1),
                                                         Pii::Identity<int>(),
                                                         0),
                                5, 0);

  QVERIFY(Pii::equals(result,PiiMatrix<int>(3,3,
                                            1,1,1,
                                            1,0,0,
                                            0,0,0)));

  // mean =
  // 3.0 3.5 4.0
  // 4.5 5.0 5.5
  // 6.0 6.5 7.0

  result = PiiImage::adaptiveThreshold(source,
                                       ThresholdFunction<double,int>(255),
                                       1.0,
                                       0.0,
                                       3,3);
  QVERIFY(Pii::equals(result, PiiMatrix<int>(3,3,
                                             0,0,0,
                                             0,255,255,
                                             255,255,255)));

  result = PiiImage::adaptiveThresholdVar(source,
                                          meanStdThresholdFunction(InverseThresholdFunction<double,int>(),
                                                                   Pii::Identity<int>(),
                                                                   -0.5),
                                          3);

  // std =
  // 1.5811    1.7078    1.5811
  // 2.5000    2.5820    2.5000
  // 1.5811    1.7078    1.5811

  // mean - 0.5*std =
  // 2.2094    2.6461    3.2094
  // 3.2500    3.7090    4.2500
  // 5.2094    5.6461    6.2094

  QVERIFY(Pii::equals(result, PiiMatrix<int>(3,3,
                                             1,1,1,
                                             0,0,0,
                                             0,0,0)));
  const PiiMatrix<bool> matMask(3,3,
                                1,1,1,
                                1,1,0,
                                1,0,0);
  result = PiiImage::adaptiveThreshold(source,
                                       matMask,
                                       std::greater<float>(),
                                       3, 3);

  // mean =
  // 3.0 3.0 4.0
  // 3.8 3.7 N/A
  // 3.8 N/A N/A

  QVERIFY(Pii::equals(result, PiiMatrix<int>(3,3,
                                             0,0,0,
                                             1,1,0,
                                             1,0,0)));

}

void TestPiiImage::suppressNonMaxima()
{
  PiiMatrix<int> source(8,8,
                        0,0,0,0,0,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,1,1,1,1,0,0,
                        0,1,1,0,0,1,1,0,
                        0,1,1,0,0,1,1,0,
                        0,0,1,1,1,1,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,0,0,0,0,0);

  PiiMatrix<int> gradientX = PiiImage::filter<int>(source, PiiImage::SobelXFilter);
  PiiMatrix<int> gradientY = PiiImage::filter<int>(source, PiiImage::SobelYFilter);
  QVERIFY(Pii::equals(PiiImage::suppressNonMaxima(PiiImage::gradientMagnitude(gradientX, gradientY),
                                                  PiiImage::gradientDirection(gradientX, gradientY),
                                                  PiiImage::RadiansToPoints<float>())
                      ,PiiMatrix<int>(8,8,
                                      0,0,0,0,0,0,0,0,
                                      0,0,6,6,6,6,0,0,
                                      0,6,0,0,0,0,6,0,
                                      0,6,0,6,6,0,6,0,
                                      0,6,0,6,6,0,6,0,
                                      0,6,0,0,0,0,6,0,
                                      0,0,6,6,6,6,0,0,
                                      0,0,0,0,0,0,0,0)));

  PiiMatrix<int> source2(5,5,
                         1,2,3,0,0,
                         1,2,3,0,0,
                         1,2,3,0,0,
                         1,2,3,0,0,
                         1,2,3,0,0);

  gradientX = PiiImage::filter<int>(source2, PiiImage::SobelXFilter);
  gradientY = PiiImage::filter<int>(source2, PiiImage::SobelYFilter);

  QVERIFY(Pii::equals((PiiImage::suppressNonMaxima(PiiImage::gradientMagnitude(gradientX, gradientY),
                                                   PiiImage::gradientDirection(gradientX, gradientY),
                                                   PiiImage::RadiansToPoints<float>()) > 0), PiiMatrix<int>(5,5,
                                                                                                            0,0,0,1,0,
                                                                                                            0,0,0,1,0,
                                                                                                            0,0,0,1,0,
                                                                                                            0,0,0,1,0,
                                                                                                            0,0,0,1,0)));

  gradientX = PiiImage::filter<int>(Pii::matrix(Pii::transpose(source2)), PiiImage::SobelXFilter);
  gradientY = PiiImage::filter<int>(Pii::matrix(Pii::transpose(source2)), PiiImage::SobelYFilter);

  /*Pii::matlabPrint(std::cout, PiiImage::suppressNonMaxima(PiiImage::gradientMagnitude(gradientX, gradientY),
    PiiImage::gradientDirection(gradientX, gradientY),
    PiiImage::RadiansToPoints<float>()));
  */
  QVERIFY(Pii::equals((PiiImage::suppressNonMaxima(PiiImage::gradientMagnitude(gradientX, gradientY),
                                                   PiiImage::gradientDirection(gradientX, gradientY),
                                                   PiiImage::RadiansToPoints<float>()) > 0) ,PiiMatrix<int>(5,5,
                                                                                                            0,0,0,0,0,
                                                                                                            0,0,0,0,0,
                                                                                                            0,0,0,0,0,
                                                                                                            1,1,1,1,1,
                                                                                                            0,0,0,0,0)));
}

void TestPiiImage::medianFilter()
{
  PiiMatrix<double> mat(5,5,
                        1.0, 2.0, 3.0, 4.0, 5.0,
                        1.0, 2.0, 3.0, 4.0, 5.0,
                        1.0, 2.0, 3.0, 4.0, 5.0,
                        1.0, 2.0, 3.0, 4.0, 5.0,
                        1.0, 2.0, 3.0, 4.0, 5.0);
  PiiMatrix<double> matOutput(5,5);

  QVERIFY(Pii::equals(PiiImage::medianFilter(mat, 5, 5),
                      PiiMatrix<double>(5,5,
                                        0.0, 0.0, 1.0, 0.0, 0.0,
                                        0.0, 1.0, 2.0, 2.0, 0.0,
                                        1.0, 2.0, 3.0, 3.0, 3.0,
                                        0.0, 1.0, 2.0, 2.0, 0.0,
                                        0.0, 0.0, 1.0, 0.0, 0.0)));
  PiiImage::medianFilter(mat, 5, 5, matOutput, PiiImage::zeroBorder(mat));
  QVERIFY(Pii::equals(matOutput,
                      PiiMatrix<double>(5,5,
                                        0.0, 0.0, 1.0, 0.0, 0.0,
                                        0.0, 1.0, 2.0, 2.0, 0.0,
                                        1.0, 2.0, 3.0, 3.0, 3.0,
                                        0.0, 1.0, 2.0, 2.0, 0.0,
                                        0.0, 0.0, 1.0, 0.0, 0.0)));
  PiiImage::medianFilter(mat, 3, 3, matOutput, PiiImage::zeroBorder(mat));

  QVERIFY(Pii::equals(matOutput,
                      PiiMatrix<double>(5,5,
                                        0.0, 1.0, 2.0, 3.0, 0.0,
                                        1.0, 2.0, 3.0, 4.0, 4.0,
                                        1.0, 2.0, 3.0, 4.0, 4.0,
                                        1.0, 2.0, 3.0, 4.0, 4.0,
                                        0.0, 1.0, 2.0, 3.0, 0.0)));

  QVERIFY(Pii::equals(PiiImage::medianFilter(mat, 5, 5, Pii::ExtendNot),
                      PiiMatrix<double>(1,1,3.0)));

  PiiMatrix<int> mat2(3,3,
                      0, 1, -1,
                      3, 4, -5,
                      0, 0, 0);
  QVERIFY(Pii::equals(PiiImage::medianFilter(mat2, 3, 3, Pii::ExtendNot),
                      PiiMatrix<int>(1,1,0)));

  PiiMatrix<PiiColor<> > clrImg(2, 3);
  clrImg(0, 0) = PiiColor<>(0, 1, 2);
  clrImg(0, 1) = PiiColor<>(1, 2, 3);
  clrImg(0, 2) = PiiColor<>(2, 3, 4);
  clrImg(1, 0) = PiiColor<>(3, 4, 5);
  clrImg(1, 1) = PiiColor<>(4, 5, 6);
  clrImg(1, 2) = PiiColor<>(5, 6, 7);

  PiiMatrix<PiiColor<> > matClrOut(2, 3);
  PiiImage::medianFilter(clrImg, 1, 3, matClrOut, PiiImage::replicateBorder(clrImg));
  QCOMPARE(matClrOut(0, 0), PiiColor<>(0, 1, 2));
  QCOMPARE(matClrOut(0, 1), PiiColor<>(1, 2, 3));
  QCOMPARE(matClrOut(0, 2), PiiColor<>(2, 3, 4));
  QCOMPARE(matClrOut(1, 0), PiiColor<>(3, 4, 5));
  QCOMPARE(matClrOut(1, 1), PiiColor<>(4, 5, 6));
  QCOMPARE(matClrOut(1, 2), PiiColor<>(5, 6, 7));
}

void TestPiiImage::backProject()
{
  {
    PiiMatrix<unsigned char> img(4,4,
                                 0,1,2,3,
                                 3,2,1,0,
                                 1,0,3,2,
                                 3,1,2,0);
    PiiMatrix<int> model(1,4, 1,2,4,8);

    QVERIFY(Pii::equals(PiiImage::backProject(img, model),PiiMatrix<int>(4,4,
                                                                         1,2,4,8,
                                                                         8,4,2,1,
                                                                         2,1,8,4,
                                                                         8,2,4,1)));
  }
  {
    PiiMatrix<unsigned char> ch1(4,4,
                                 0,0,0,0,
                                 1,1,1,1,
                                 2,2,2,2,
                                 3,3,3,3);
    PiiMatrix<unsigned char> ch2(4,4,
                                 0,1,2,3,
                                 0,1,2,3,
                                 0,1,2,3,
                                 0,1,2,3);
    PiiMatrix<unsigned char> model(4,4,
                                   1,2,3,4,
                                   5,6,7,8,
                                   9,10,11,12,
                                   13,14,15,16);

    QVERIFY(Pii::equals(PiiImage::backProject(ch1, ch2, model),model));
  }
}

void TestPiiImage::equalize()
{
  PiiMatrix<int> img(4,4,
                     0,0,0,0,
                     1,1,1,1,
                     2,2,2,2,
                     3,3,3,3);

  QVERIFY(Pii::equals(PiiImage::equalize(img,9),PiiMatrix<int>(4,4,
                                                               2,2,2,2,
                                                               4,4,4,4,
                                                               7,7,7,7,
                                                               8,8,8,8)));
  QVERIFY(Pii::equals(PiiImage::equalize(img),PiiMatrix<int>(4,4,
                                                             1,1,1,1,
                                                             2,2,2,2,
                                                             3,3,3,3,
                                                             3,3,3,3)));
  img += 14;
  QVERIFY(Pii::equals(PiiImage::equalize(img,32),PiiMatrix<int>(4,4,
                                                                8,8,8,8,
                                                                16,16,16,16,
                                                                24,24,24,24,
                                                                31,31,31,31)));
}
void TestPiiImage::histogram()
{
  //Testing basic functionality of PiiHistogram-class
  PiiMatrix<int> image(4, 4,
                       0, 1, 2, 3,
                       0, 1, 2, 3,
                       0, 1, 2, 3,
                       0, 1, 2, 3);

  PiiMatrix<int> hist(PiiImage::histogram(image));
  QCOMPARE(hist.rows(),1);
  QCOMPARE(hist.columns(),4);
  for(int r = 0; r < hist.rows(); ++r)
    {
      for(int c = 0; c < hist.columns(); ++c)
        {
          QCOMPARE(hist(r,c), 4);
        }
    }


}
void TestPiiImage::cumulative()
{
  //Testing basic functionality of PiiHistogram-class

  PiiMatrix<int> hist(1,4, 4,4,4,4);
  PiiMatrix<int> cum(PiiImage::cumulative(hist));
  QCOMPARE(cum.rows(),1);
  QCOMPARE(cum.columns(),4);
  for(int r = 0; r < cum.rows(); ++r)
    {
      for(int c =0; c < cum.columns(); ++c)
        {
          if(c == 0)
            QCOMPARE(cum(r,c),4);
          else
            {
              int val = c*4 + 4;
              QCOMPARE(cum(r,c), val);
            }
        }
    }


}
void TestPiiImage::normalize()
{
  PiiMatrix<int> hist(1,4,4,4,4,4);
  PiiMatrix<double> normalized(PiiImage::normalize<double>(hist));
  QCOMPARE(normalized.rows(), 1);
  QCOMPARE(normalized.columns(),4);
  for(int r = 0; r < normalized.rows(); ++r)
    {
      for(int c = 0;  c < normalized.columns(); ++c)
        QCOMPARE(normalized(r,c), 0.25);
    }
}

void TestPiiImage::percentile()
{
  PiiMatrix<int> hist(1,4, 4,4,4,4);
  PiiMatrix<int> cum(PiiImage::cumulative(hist));
  QCOMPARE(PiiImage::percentile(cum,0),0);
  QCOMPARE(PiiImage::percentile(cum,4),0);
  QCOMPARE(PiiImage::percentile(cum,5),1);
  QCOMPARE(PiiImage::percentile(cum,12),2);
  QCOMPARE(PiiImage::percentile(cum,13),3);
  QCOMPARE(PiiImage::percentile(cum,16),3);
  QCOMPARE(PiiImage::percentile(cum,17),-1);
}

void TestPiiImage::hitAndMiss()
{
  PiiMatrix<int> source(6,6,
                        0,0,0,1,0,0,
                        0,1,1,1,1,0,
                        0,1,1,1,1,1,
                        0,1,1,1,1,0,
                        0,1,1,1,1,0,
                        0,0,0,0,0,0);
  // Look for lower left corner
  PiiMatrix<int> mask(3,3,
                      0,1,1,
                      0,1,1,
                      0,0,0);
  // We don't care about all values
  PiiMatrix<int> significance(3,3,
                              0,1,0,
                              1,1,1,
                              1,1,0);
  PiiMatrix<int> result = PiiImage::hitAndMiss(source, mask, significance);

  // Find the upper right "corner"
  result += PiiImage::hitAndMiss(source,
                                 PiiMatrix<int>(3,3,
                                                0,0,0,
                                                1,1,0,
                                                1,1,0),
                                 PiiMatrix<int>(3,3,
                                                0,1,1,
                                                1,1,1,
                                                0,1,0));

  QVERIFY(Pii::equals(result,PiiMatrix<int>(6,6,
                                            0,0,0,0,0,0,
                                            0,0,0,0,1,0,
                                            0,0,0,0,0,0,
                                            0,0,0,0,0,0,
                                            0,1,0,0,0,0,
                                            0,0,0,0,0,0)));

}

void TestPiiImage::labelImage()
{
  PiiMatrix<int> mat(8,8,
                     1,1,1,0,0,0,0,0,
                     0,1,0,1,1,1,0,1,
                     0,1,0,1,0,1,0,1,
                     0,0,1,1,0,1,1,1,
                     0,1,1,0,1,0,0,1,
                     0,0,1,0,0,0,1,1,
                     0,0,1,1,1,1,0,0,
                     1,0,0,0,0,1,1,1);

  int count = 0;
  PiiMatrix<int> result = PiiImage::labelImage(mat, &count);

  PiiMatrix<int> labels(8,8,
                        1,1,1,0,0,0,0,0,
                        0,1,0,2,2,2,0,2,
                        0,1,0,2,0,2,0,2,
                        0,0,2,2,0,2,2,2,
                        0,2,2,0,3,0,0,2,
                        0,0,2,0,0,0,2,2,
                        0,0,2,2,2,2,0,0,
                        4,0,0,0,0,2,2,2);

  QVERIFY(Pii::equals(result,labels));
  QCOMPARE(count,4);

  PiiMatrix<int> mat2(10,10,
                      1,0,1,0,1,0,1,0,1,0,
                      0,1,0,1,0,1,0,1,0,1,
                      1,0,1,0,1,0,1,0,1,0,
                      0,1,0,1,0,1,0,1,0,1,
                      1,0,1,0,1,0,1,0,1,0,
                      0,1,0,1,0,1,0,1,0,1,
                      1,0,1,0,1,0,1,0,1,0,
                      0,1,0,1,0,1,0,1,0,1,
                      1,0,1,0,1,0,1,0,1,0,
                      0,1,0,1,0,1,0,1,0,1);

  result = PiiImage::labelImage(mat2, &count);
  int index = 0;
  for (int r=0; r<result.rows(); r++)
    for (int c=0; c<result.columns(); c++)
      {
        if ((r&1) == (c&1))
          QCOMPARE(result(r,c), ++index);
        else
          QCOMPARE(result(r,c), 0);
      }
  QCOMPARE(count, 50);

  result = PiiImage::labelLargerThan(mat, 5, &count);
  PiiMatrix<int> correct(labels);
  correct(correct != 2) = 0;
  correct(correct == 2) = 1;
  QVERIFY(Pii::equals(result,correct));
  QCOMPARE(count, 1);

  result = PiiImage::labelLargerThan(mat2, 1, &count);
  QVERIFY(Pii::equals(result,PiiMatrix<int>(10,10)));
  QCOMPARE(count,0);

  PiiMatrix<int> mat3(10,10,
                      1,0,5,0,1,0,1,0,1,0,
                      0,5,0,1,0,1,0,1,0,1,
                      5,0,1,0,1,0,6,0,1,0,
                      0,1,0,1,0,6,0,1,0,1,
                      1,0,1,0,6,0,1,0,1,0,
                      0,1,0,6,0,1,0,1,0,2,
                      1,0,6,0,1,0,1,0,2,0,
                      0,6,0,1,0,1,0,2,0,1,
                      1,0,1,0,1,0,2,0,1,0,
                      0,1,0,1,0,2,0,1,0,1);

  int labelCount = 0;
  PiiMatrix<int> labelMat = PiiImage::labelImage(mat3,
                                                 std::bind2nd(std::greater_equal<int>(), 5),
                                                 PiiImage::DefaultLabelingLimiter(),
                                                 &labelCount);

  QCOMPARE(labelCount, 9);

  PiiMatrix<int> mat4(10,10,
                      1,0,5,0,1,0,1,0,0,0,
                      0,5,0,0,0,1,0,1,0,0,
                      5,0,0,0,1,0,3,0,1,0,
                      0,0,0,0,0,3,0,1,0,1,
                      0,0,0,0,6,0,1,0,1,0,
                      0,0,0,6,0,1,0,1,0,0,
                      0,0,3,3,1,0,1,0,0,0,
                      0,3,0,3,0,0,0,0,0,0,
                      0,0,0,0,0,0,2,0,0,0,
                      0,0,0,1,0,2,0,1,0,0);

  labelMat = PiiImage::labelImage(mat4,
                                  std::bind2nd(std::not_equal_to<int>(), 0),
                                  Pii::YesFunction<bool>(),
                                  PiiImage::Connect8);

  //Pii::matlabPrint(std::cout, labelMat);
  QVERIFY(Pii::equals(labelMat,PiiMatrix<int>(10,10,
                                              1,0,1,0,2,0,2,0,0,0,
                                              0,1,0,0,0,2,0,2,0,0,
                                              1,0,0,0,2,0,2,0,2,0,
                                              0,0,0,0,0,2,0,2,0,2,
                                              0,0,0,0,2,0,2,0,2,0,
                                              0,0,0,2,0,2,0,2,0,0,
                                              0,0,2,2,2,0,2,0,0,0,
                                              0,2,0,2,0,0,0,0,0,0,
                                              0,0,0,0,0,0,3,0,0,0,
                                              0,0,0,4,0,3,0,3,0,0)));

  labelMat = PiiImage::labelImage(mat4,
                                  std::bind2nd(std::not_equal_to<int>(), 0),
                                  std::bind2nd(std::greater<int>(), 5),
                                  PiiImage::Connect8,
                                  0);
  QVERIFY(Pii::equals(labelMat,PiiMatrix<int>(10,10,
                                              0,0,0,0,1,0,1,0,0,0,
                                              0,0,0,0,0,1,0,1,0,0,
                                              0,0,0,0,1,0,1,0,1,0,
                                              0,0,0,0,0,1,0,1,0,1,
                                              0,0,0,0,1,0,1,0,1,0,
                                              0,0,0,1,0,1,0,1,0,0,
                                              0,0,1,1,1,0,1,0,0,0,
                                              0,1,0,1,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0,0,0,
                                              0,0,0,0,0,0,0,0,0,0)));

  PiiMatrix<int> mat5(5,8,
                      0,0,0,1,1,0,1,1,
                      0,0,1,1,0,1,1,0,
                      0,1,1,0,1,1,0,0,
                      1,1,0,1,1,0,0,0,
                      0,1,1,1,0,0,0,0);
  QVERIFY(Pii::equals(PiiImage::labelImage(mat5), mat5));

  PiiMatrix<int> mat6(1,1);
  mat6 = 1;
  QVERIFY(Pii::equals(PiiImage::labelImage(mat6), mat6));

  PiiMatrix<int> mat7(3,3,
                      0,1,0,
                      1,1,1,
                      0,1,0);
  QVERIFY(Pii::equals(PiiImage::labelImage(mat7), mat7));

  PiiMatrix<int> mat8(10,10,
                      2,2,0,1,1,0,4,4,0,3,
                      2,2,0,1,1,0,4,4,0,3,
                      0,0,0,1,1,0,4,4,0,3,
                      1,1,1,1,1,0,4,4,0,3,
                      1,1,1,1,1,0,4,4,0,3,
                      0,0,0,0,0,0,4,4,0,3,
                      4,4,4,4,4,4,4,4,0,3,
                      4,4,4,4,4,4,4,4,0,3,
                      0,0,0,0,0,0,0,0,0,3,
                      3,3,3,3,3,3,3,3,3,3);
  PiiMatrix<int> matResult(mat8);
  matResult(matResult == 2) = 0;
  matResult(matResult == 4) = 2;

  // Find three largest objects
  QVERIFY(Pii::equals(PiiImage::labelImage(mat8,
                                           std::bind2nd(std::greater<int>(), 0),
                                           PiiImage::ObjectSizeLimiter(-3)),
                      matResult));

  // Test that the correct objects are found with any parameter value
  // (don't care about actual label values here)
  QVERIFY(Pii::equals(PiiImage::labelImage(mat8,
                                           std::bind2nd(std::greater<int>(), 0),
                                           PiiImage::ObjectSizeLimiter(-4)) > 0,
                      mat8 > 0));
  QVERIFY(Pii::equals(PiiImage::labelImage(mat8,
                                           std::bind2nd(std::greater<int>(), 0),
                                           PiiImage::ObjectSizeLimiter(-999)) > 0,
                      mat8 > 0));
  QVERIFY(Pii::equals(PiiImage::labelImage(mat8,
                                           std::bind2nd(std::greater<int>(), 0),
                                           PiiImage::ObjectSizeLimiter(0)) > 0,
                      mat8 > 0));
  QVERIFY(Pii::equals(PiiImage::labelImage(mat8,
                                           std::bind2nd(std::greater<int>(), 0),
                                           PiiImage::ObjectSizeLimiter(-1)) > 0,
                      mat8 > 3));
  QVERIFY(Pii::equals(PiiImage::labelImage(mat8,
                                           std::bind2nd(std::greater<int>(), 0),
                                           PiiImage::ObjectSizeLimiter(-2)) > 0,
                      mat8 > 2));
}

void TestPiiImage::labelLargerThan()
{
  PiiMatrix<int> source(6,5,
                        0,0,1,0,0,
                        0,1,1,0,0,
                        1,1,0,1,1,
                        1,0,1,1,1,
                        0,1,0,0,0,
                        1,1,0,1,1);
  PiiMatrix<int> result(PiiImage::labelLargerThan(source, 5));
  QVERIFY(Pii::equals(result,
                      PiiMatrix<int>(6,5,
                                     0,0,1,0,0,
                                     0,1,1,0,0,
                                     1,1,0,0,0,
                                     1,0,0,0,0,
                                     0,0,0,0,0,
                                     0,0,0,0,0)));

  result = PiiImage::labelLargerThan(source, 2);

  QVERIFY(Pii::equals(result,
                      PiiMatrix<int>(6,5,
                                     0,0,1,0,0,
                                     0,1,1,0,0,
                                     1,1,0,2,2,
                                     1,0,2,2,2,
                                     0,3,0,0,0,
                                     3,3,0,0,0)));

  source = PiiMatrix<int>(6,5,
                          0,0,1,0,0,
                          0,1,1,0,0,
                          1,1,0,1,1,
                          1,0,1,1,1,
                          0,1,1,0,0,
                          1,1,0,1,1);
  result = PiiImage::labelLargerThan(source, 0);
  QVERIFY(Pii::equals(result,
                      PiiMatrix<int>(6,5,
                                     0,0,0,0,0,
                                     0,0,0,0,0,
                                     0,0,0,1,1,
                                     0,0,1,1,1,
                                     0,1,1,0,0,
                                     1,1,0,0,0)));
}

void TestPiiImage::thin()
{
  PiiMatrix<int> source(6,6,
                        0,0,0,0,0,0,
                        0,1,1,1,0,0,
                        0,1,1,1,1,0,
                        0,1,1,1,1,0,
                        0,0,1,1,1,0,
                        0,0,0,0,0,0);
  PiiMatrix<int> result(PiiImage::thin(source));
  QVERIFY(Pii::equals(result,PiiMatrix<int>(6,6,
                                            0,0,0,0,0,0,
                                            0,0,1,0,0,0,
                                            0,0,1,1,0,0,
                                            0,0,1,1,0,0,
                                            0,0,0,0,1,0,
                                            0,0,0,0,0,0)));

  result = PiiImage::thin(source, -1);
  QVERIFY(Pii::equals(result,PiiMatrix<int>(6,6,
                                            0,0,0,0,0,0,
                                            0,0,1,0,0,0,
                                            0,0,1,0,0,0,
                                            0,0,0,1,0,0,
                                            0,0,0,0,1,0,
                                            0,0,0,0,0,0)));
}

void TestPiiImage::border()
{
  PiiMatrix<int> source(6,6,
                        0,0,0,0,0,0,
                        0,1,1,1,0,0,
                        0,1,1,1,1,0,
                        0,1,1,1,1,0,
                        0,0,1,1,1,0,
                        0,0,0,0,0,0);
  PiiMatrix<int> result(PiiImage::border(source));
  QVERIFY(Pii::equals(result,PiiMatrix<int>(6,6,
                                            0,0,0,0,0,0,
                                            0,1,1,1,0,0,
                                            0,1,0,0,1,0,
                                            0,1,0,0,1,0,
                                            0,0,1,1,1,0,
                                            0,0,0,0,0,0)));

  result = PiiImage::shrink(source);
  QVERIFY(Pii::equals(result,PiiMatrix<int>(6,6,
                                            0,0,0,0,0,0,
                                            0,0,0,0,0,0,
                                            0,0,1,1,0,0,
                                            0,0,1,1,0,0,
                                            0,0,0,0,0,0,
                                            0,0,0,0,0,0)));

  result = PiiImage::shrink(source, 2);
  // Should be all zeros
  QVERIFY(Pii::equals(result,PiiMatrix<int>(6,6)));
}

void TestPiiImage::findBoundary()
{
  {
    PiiMatrix<int> objects(8,8,
                           0,0,1,0,0,0,0,0,
                           0,0,1,0,0,0,0,0,
                           1,1,1,0,0,0,0,0,
                           0,0,1,1,1,1,1,0,
                           0,0,1,1,1,1,0,0,
                           0,0,1,0,1,1,0,0,
                           0,1,0,0,0,0,0,0,
                           1,0,1,0,0,0,0,0);

    PiiMatrix<int> boundary(25,2,
                            2,7,
                            1,6,
                            0,7,
                            1,6,
                            2,5,
                            2,4,
                            2,3,
                            1,2,
                            0,2,
                            1,2,
                            2,1,
                            2,0,
                            2,1,
                            2,2,
                            3,3,
                            4,3,
                            5,3,
                            6,3,
                            5,4,
                            5,5,
                            4,5,
                            3,4,
                            2,5,
                            1,6,
                            2,7);

    PiiMatrix<int> extracted = PiiBoundaryFinder::findBoundary<int>(objects, 1);

    QVERIFY(Pii::equals(extracted,boundary));
  }

  {
    PiiMatrix<int> objects(6,6,
                           0,0,1,0,0,0,
                           0,1,0,1,0,0,
                           1,0,1,0,1,0,
                           0,1,0,1,0,1,
                           0,0,0,1,0,1,
                           0,0,0,0,1,0);

    PiiMatrix<int> boundary(13,2,
                            4,5,
                            3,4,
                            3,3,
                            2,2,
                            1,3,
                            0,2,
                            1,1,
                            2,0,
                            3,1,
                            4,2,
                            5,3,
                            5,4,
                            4,5);

    PiiMatrix<int> extracted = PiiBoundaryFinder::findBoundary<int>(objects, 1);

    QVERIFY(Pii::equals(extracted,boundary));
  }
  {
    //PiiBoundaryFinder::findBoundary(PiiMatrix<int>(1,1,1), 1),PiiMatrix<int>(1,2);
  }
}

void TestPiiImage::findNextBoundary()
{
  {
    PiiMatrix<int> objects(9,9,
                           1,1,1,1,1,1,1,1,1,
                           1,0,0,0,0,0,0,0,1,
                           1,0,1,1,1,1,1,0,1,
                           1,0,1,0,0,0,1,0,1,
                           1,1,1,0,1,0,1,0,1,
                           1,0,1,0,0,0,1,0,1,
                           1,0,1,1,1,1,1,0,1,
                           1,0,0,0,0,0,0,0,1,
                           1,1,1,1,1,1,1,1,1);

    PiiMatrix<int> result;

    PiiMatrix<unsigned char> mask(9,9);
    PiiBoundaryFinder finder(9, 9, &mask);

    result = finder.findNextBoundary<int>(objects, std::bind2nd(std::greater<int>(), 0));
    QCOMPARE(result(0,0), 8);
    QCOMPARE(result(0,1), 8);

    result = finder.findNextBoundary<int>(objects, std::bind2nd(std::greater<int>(), 0));
    QCOMPARE(result(0,0), 0);
    QCOMPARE(result(0,1), 7);

    result = finder.findNextBoundary<int>(objects, std::bind2nd(std::greater<int>(), 0));
    QCOMPARE(result(0,0), 2);
    QCOMPARE(result(0,1), 5);

    result = finder.findNextBoundary<int>(objects, std::bind2nd(std::greater<int>(), 0));
    QCOMPARE(result(0,0), 4);
    QCOMPARE(result(0,1), 4);

    result = finder.findNextBoundary<int>(objects, std::bind2nd(std::greater<int>(), 0));
    QVERIFY(result.isEmpty());

    //QDebug d = qDebug();
    //Pii::matlabPrint(d, mask);

    QVERIFY(Pii::equals(mask,
                        PiiMatrix<unsigned char>(9,9,
                                                 2,3,3,3,3,3,3,3,1,
                                                 3,0,0,0,0,0,0,0,3,
                                                 3,0,2,3,3,3,1,0,3,
                                                 3,0,3,0,0,0,3,0,3,
                                                 2,0,1,0,3,0,3,0,3,
                                                 3,0,3,0,0,0,3,0,3,
                                                 3,0,2,3,3,3,1,0,3,
                                                 3,0,0,0,0,0,0,0,3,
                                                 2,3,3,3,3,3,3,3,1)));
  }
  {
    PiiMatrix<int> objects(9,9,
                           1,1,1,1,1,1,1,1,1,
                           1,0,0,0,0,0,0,0,1,
                           1,0,1,1,1,1,1,1,1,
                           1,0,1,0,0,0,1,1,1,
                           1,0,1,1,1,0,1,1,1,
                           1,0,1,0,0,0,1,1,1,
                           1,0,1,1,1,1,1,1,1,
                           1,0,0,0,0,0,0,0,1,
                           1,1,1,1,1,1,1,0,0);

    PiiBoundaryFinder finder(9, 9);
    PiiMatrix<int> result;

    result = finder.findNextBoundary<int>(objects, std::bind2nd(std::greater<int>(), 0));
    QCOMPARE(result(0,0), 6);
    QCOMPARE(result(0,1), 8);

    result = finder.findNextBoundary<int>(objects, std::bind2nd(std::greater<int>(), 0));
    QCOMPARE(result(0,0), 2);
    QCOMPARE(result(0,1), 5);

    result = finder.findNextBoundary<int>(objects, std::bind2nd(std::greater<int>(), 0));
    QVERIFY(result.isEmpty());

    //QDebug d = qDebug();
    //Pii::matlabPrint(d, finder.boundaryMask());

    QVERIFY(Pii::equals(finder.boundaryMask(),
                        PiiMatrix<unsigned char>(9,9,
                                                 2,3,3,3,3,3,3,3,1,
                                                 3,0,0,0,0,0,0,0,3,
                                                 3,0,2,3,3,3,1,1,1,
                                                 3,0,3,0,0,0,2,0,1,
                                                 3,0,2,3,1,0,2,0,1,
                                                 3,0,3,0,0,0,2,0,1,
                                                 3,0,2,3,3,3,2,2,1,
                                                 3,0,0,0,0,0,0,0,3,
                                                 2,3,3,3,3,3,1,0,0)));
  }
  {
    PiiMatrix<int> objects(6,8,
                           1,0,1,1,0,1,0,1,
                           0,1,0,0,1,0,1,0,
                           1,0,0,0,0,1,0,1,
                           0,1,0,0,1,0,1,0,
                           1,0,1,1,0,1,0,1,
                           0,0,0,0,0,0,0,0);

    PiiBoundaryFinder finder(6, 8);
    finder.findNextBoundary<int>(objects, std::bind2nd(std::not_equal_to<int>(), 0));
    //QDebug d = qDebug();
    //Pii::matlabPrint(d, finder.boundaryMask());
    QVERIFY(Pii::equals(finder.boundaryMask(),PiiMatrix<unsigned char>(6,8,
                                                                       3,0,2,1,0,3,0,3,
                                                                       0,2,0,0,0,0,1,0,
                                                                       2,0,0,0,0,0,0,1,
                                                                       0,2,0,0,0,0,1,0,
                                                                       3,0,2,1,0,3,0,3,
                                                                       0,0,0,0,0,0,0,0)));
  }
  {
    PiiMatrix<int> objects(4,3,
                           0,1,0,
                           1,0,1,
                           0,1,0,
                           1,0,1);

    PiiBoundaryFinder finder(4, 3);
    finder.findNextBoundary<int>(objects, std::bind2nd(std::not_equal_to<int>(), 0));
    QVERIFY(Pii::equals(finder.boundaryMask(),PiiMatrix<unsigned char>(4,3,
                                                                       0,3,0,
                                                                       2,0,1,
                                                                       0,3,0,
                                                                       3,0,3)));
  }
  {
    PiiMatrix<int> objects(3,4,
                           0,1,0,1,
                           1,0,1,0,
                           0,1,0,1);

    PiiBoundaryFinder finder(3, 4);
    finder.findNextBoundary<int>(objects, std::bind2nd(std::not_equal_to<int>(), 0));
    QVERIFY(Pii::equals(finder.boundaryMask(),PiiMatrix<unsigned char>(3,4,
                                                                       0,3,0,3,
                                                                       2,0,1,0,
                                                                       0,3,0,3)));
  }
  {
    PiiMatrix<int> objects(3,3,
                           1,0,1,
                           0,1,0,
                           1,0,1);

    PiiBoundaryFinder finder(3, 3);
    finder.findNextBoundary<int>(objects, std::bind2nd(std::not_equal_to<int>(), 0));
    QVERIFY(Pii::equals(finder.boundaryMask(),PiiMatrix<unsigned char>(3,3,
                                                                       3,0,3,
                                                                       0,3,0,
                                                                       3,0,3)));
  }
  {
    PiiMatrix<int> objects(3,3,
                           0,1,0,
                           1,0,1,
                           0,1,0);

    PiiBoundaryFinder finder(3, 3);
    finder.findNextBoundary<int>(objects, std::bind2nd(std::not_equal_to<int>(), 0));
    QVERIFY(Pii::equals(finder.boundaryMask(),PiiMatrix<unsigned char>(3,3,
                                                                       0,3,0,
                                                                       2,0,1,
                                                                       0,3,0)));
  }
  {
    PiiMatrix<int> objects(4,5,
                           1,1,1,1,1,
                           1,0,0,0,1,
                           1,0,1,0,1,
                           1,1,0,1,1);

    PiiBoundaryFinder finder(4, 5);
    finder.findNextBoundary<int>(objects, std::bind2nd(std::not_equal_to<int>(), 0));
    finder.findNextBoundary<int>(objects, std::bind2nd(std::not_equal_to<int>(), 0));
    QVERIFY(Pii::equals(finder.boundaryMask(),PiiMatrix<unsigned char>(4,5,
                                                                       2,3,3,3,1,
                                                                       3,0,0,0,3,
                                                                       3,0,3,0,3,
                                                                       2,1,0,2,1)));
  }
}

void TestPiiImage::findBoundaries()
{
  PiiMatrix<int> objects(8,8,
                         0,1,2,0,1,1,0,0,
                         0,2,2,0,1,1,0,0,
                         0,2,2,0,0,0,0,0,
                         0,2,2,1,1,1,1,0,
                         0,2,2,0,0,0,1,0,
                         0,2,2,2,2,3,1,0,
                         0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0);

  // Everything other than zero is an object
  QList<PiiMatrix<int> > coordinates(
    PiiBoundaryFinder::findBoundaries<int>(objects,
                                           std::bind2nd(std::not_equal_to<int>(), 0)));
  QCOMPARE(coordinates.size(), 3);

  QVERIFY(Pii::equals(coordinates[0],PiiMatrix<int>(20,2,
                                                    6,5,
                                                    5,5,
                                                    4,5,
                                                    3,5,
                                                    2,5,
                                                    1,5,
                                                    1,4,
                                                    1,3,
                                                    1,2,
                                                    1,1,
                                                    1,0,
                                                    2,0,
                                                    2,1,
                                                    2,2,
                                                    3,3,
                                                    4,3,
                                                    5,3,
                                                    6,3,
                                                    6,4,
                                                    6,5)));
  QVERIFY(Pii::equals(coordinates[1],PiiMatrix<int>(9,2,
                                                    2,4,
                                                    3,5,
                                                    4,5,
                                                    5,5,
                                                    6,4,
                                                    5,3,
                                                    4,3,
                                                    3,3,
                                                    2,4)));
  QVERIFY(Pii::equals(coordinates[2],PiiMatrix<int>(5,2,
                                                    5,1,
                                                    4,1,
                                                    4,0,
                                                    5,0,
                                                    5,1)));

  // Repeat with a boundary mask
  PiiMatrix<unsigned char> mask;
  PiiBoundaryFinder::findBoundaries<int>(objects, std::bind2nd(std::greater_equal<int>(), 1), &mask);
  QVERIFY(Pii::equals(mask, PiiMatrix<unsigned char>(8,8,
                                                     0,2,1,0,2,1,0,0,
                                                     0,2,1,0,2,1,0,0,
                                                     0,2,1,0,0,0,0,0,
                                                     0,2,0,3,3,3,1,0,
                                                     0,2,1,0,0,0,3,0,
                                                     0,2,2,3,3,3,1,0,
                                                     0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0)));
}

void TestPiiImage::threshold()
{
  QVERIFY(Pii::equals(PiiImage::threshold(_matThreshold, 5),
                      PiiMatrix<int>(3,3,
                                     0,0,0,
                                     0,1,1,
                                     1,1,1)));
}

void TestPiiImage::inverseThreshold()
{
  QVERIFY(Pii::equals(PiiImage::inverseThreshold(_matThreshold, 5),
                      PiiMatrix<int>(3,3,
                                     1,1,1,
                                     1,0,0,
                                     0,0,0)));
}

void TestPiiImage::twoLevelThreshold()
{
  QVERIFY(Pii::equals(_matThreshold.mapped(PiiImage::TwoLevelThresholdFunction<int>(2,6)),
                      PiiMatrix<int>(3,3,
                                     0,1,1,
                                     1,1,1,
                                     0,0,0)));
}

void TestPiiImage::inverseTwoLevelThreshold()
{
  QVERIFY(Pii::equals(_matThreshold.mapped(PiiImage::InverseTwoLevelThresholdFunction<int>(2,6)),
                      PiiMatrix<int>(3,3,
                                     1,0,0,
                                     0,0,0,
                                     1,1,1)));
}

void TestPiiImage::cut()
{
  QVERIFY(Pii::equals(PiiImage::cut(_matThreshold, 3),
                      PiiMatrix<int>(3,3,
                                     1,2,3,
                                     3,3,3,
                                     3,3,3)));
}

void TestPiiImage::inverseCut()
{
  QVERIFY(Pii::equals(PiiImage::inverseCut(_matThreshold, 3),
                      PiiMatrix<int>(3,3,
                                     3,3,3,
                                     4,5,6,
                                     7,8,9)));
}

void TestPiiImage::zeroAbove()
{
  {
    PiiMatrix<int> a(3,3, 8, 4, 31,   4, 0, 3,  15,8, 8);
    QVERIFY(Pii::equals(PiiImage::zeroAbove(a, 8),PiiMatrix<int>(3,3, 0,4,0, 4,0,3 ,0,0,0)));
  }

  {
    PiiMatrix<float> a(2,3, 8.0, 4.0, 31.0,   4.0, 7.9999, 1.0);
    QVERIFY(Pii::equals(PiiImage::zeroAbove(a, 7.99999f),PiiMatrix<float>(2,3, 0.0, 4.0, 0.0,  4.0, 7.9999, 1.0)));
  }
}


void TestPiiImage::zeroBelow()
{
  {
    PiiMatrix<int> a(3,3, 8, 4, 31,   4, 0, 3,  15,8, 8);
    QVERIFY(Pii::equals(PiiImage::zeroBelow(a, 8),PiiMatrix<int>(3,3, 8,0,31, 0,0,0, 15,8,8)) );
  }

  {
    PiiMatrix<double> a(2,3, 8.0, 4.0, 31.0,   4.0, 7.9999, 1.0);
    QVERIFY(Pii::equals(PiiImage::zeroBelow(a, 7.99999),PiiMatrix<double>(2,3, 8.0, 0.0, 31.0,  0.0, 0.0, 0.0)) );
  }
}

void TestPiiImage::separateFilter()
{
  {
    PiiMatrix<double> filter = PiiImage::makeFilter<double>(PiiImage::GaussianFilter);
    PiiMatrix<double> horz, vert;
    QVERIFY(PiiImage::separateFilter(filter, horz, vert));
    QVERIFY(Pii::almostEqual(filter, vert*horz, 1e-10));
  }
  for (int i=PiiImage::SobelXFilter; i<=PiiImage::PrewittYFilter; ++i)
    {
      PiiMatrix<int> filter = PiiImage::makeFilter<double>(static_cast<PiiImage::PrebuiltFilterType>(i));
      PiiMatrix<int> horz, vert;
      QVERIFY(PiiImage::separateFilter(filter, horz, vert));
      QVERIFY(Pii::equals(filter,vert*horz));
    }
  {
    PiiMatrix<double> filter = PiiImage::makeFilter<double>(PiiImage::UniformFilter);
    PiiMatrix<double> horz, vert;
    QVERIFY(PiiImage::separateFilter(filter, horz, vert));
    QVERIFY(Pii::equals(filter,vert*horz));
  }
  {
    // This cannot be factored (rank 2)
    PiiMatrix<int> filter(3,3,
                          1,2,1,
                          2,0,2,
                          1,2,1);
    PiiMatrix<int> horz, vert;
    QVERIFY(!PiiImage::separateFilter(filter, horz, vert));
  }
}

void TestPiiImage::filter()
{
  PiiMatrix<int> input(3,3,
                       1,2,3,
                       4,5,6,
                       7,8,9);
  PiiMatrix<int> filter(3,3);
  filter = 1;

  QVERIFY(Pii::equals(PiiImage::filter<int>(input, filter),PiiMatrix<int>(3,3,
                                                                          21,27,33,
                                                                          39,45,51,
                                                                          57,63,69)));
  PiiMatrix<int> matResult(3,3);
  PiiImage::filter(input, filter, Pii::Identity<int>(), matResult, PiiImage::replicateBorder(input));
  QVERIFY(Pii::equals(matResult, PiiMatrix<int>(3,3,
                                                21,27,33,
                                                39,45,51,
                                                57,63,69)));

  QVERIFY(Pii::equals(PiiImage::filter<int>(input, filter, Pii::ExtendZeros),PiiMatrix<int>(3,3,
                                                                                            12,21,16,
                                                                                            27,45,33,
                                                                                            24,39,28)));

  PiiImage::filter(input, filter, Pii::Identity<int>(), matResult, PiiImage::zeroBorder(input));
  QVERIFY(Pii::equals(matResult, PiiMatrix<int>(3,3,
                                                12,21,16,
                                                27,45,33,
                                                24,39,28)));

  QVERIFY(Pii::equals(PiiImage::filter<int>(input, filter, Pii::ExtendSymmetric),PiiMatrix<int>(3,3,
                                                                                                21,27,33,
                                                                                                39,45,51,
                                                                                                57,63,69)));
  PiiImage::filter(input, filter, Pii::Identity<int>(), matResult, PiiImage::reflectBorder(input));
  QVERIFY(Pii::equals(matResult, PiiMatrix<int>(3,3,
                                                21,27,33,
                                                39,45,51,
                                                57,63,69)));

  QVERIFY(Pii::equals(PiiImage::filter<int>(input, filter, Pii::ExtendPeriodic),PiiMatrix<int>(3,3,
                                                                                               45,45,45,
                                                                                               45,45,45,
                                                                                               45,45,45)));
  PiiImage::filter(input, filter, Pii::Identity<int>(), matResult, PiiImage::periodicBorder(input));
  QVERIFY(Pii::equals(matResult, PiiMatrix<int>(3,3,
                                                45,45,45,
                                                45,45,45,
                                                45,45,45)));

  QVERIFY(Pii::equals(PiiImage::filter<int>(input, filter, Pii::ExtendNot),PiiMatrix<int>(1,1,45)));

  PiiMatrix<int> matResult2(1,1);
  PiiImage::filter(input, filter, Pii::Identity<int>(), matResult2);
  QCOMPARE(matResult2(0,0), 45);

  filter = PiiImage::makeFilter<int>(PiiImage::SobelXFilter);
  {
    PiiMatrix<int> h,v;
    PiiImage::separateFilter(filter, h, v);
    QVERIFY(Pii::equals(PiiImage::filter<int>(input, filter),PiiImage::filter<int>(input, h, v)));
    QVERIFY(Pii::equals(PiiImage::filter<int>(input, filter),PiiImage::filter<int>(input, PiiImage::SobelXFilter)));
  }
  {
    PiiMatrix<double> dFilter = PiiImage::makeFilter<double>(PiiImage::GaussianFilter);
    PiiMatrix<double> h,v;
    PiiImage::separateFilter(dFilter, h, v);
    QVERIFY(Pii::almostEqual(PiiImage::filter<double>(input, dFilter),
                             PiiImage::filter<double>(input, h, v),
                             1e-6));
  }
}

void TestPiiImage::intFilter()
{
  PiiMatrix<double> filter(PiiImage::makeFilter<double>(PiiImage::GaussianFilter, 3));

  PiiMatrix<int> input(5,5);
  QVERIFY(Pii::equals(PiiImage::intFilter(input, filter),input));

  input = 255;
  filter = PiiImage::makeFilter<double>(PiiImage::GaussianFilter, 5);
  QVERIFY(Pii::equals(PiiImage::intFilter(input, filter),input));

  PiiMatrix<int> image(5,5,
                       0,1,2,3,4,
                       1,2,3,4,5,
                       2,3,4,5,6,
                       3,4,5,6,7,
                       4,5,6,7,8);

  PiiMatrix<int> reference(Pii::round<int>(PiiImage::filter<double>(image, filter)));
  // Int filter should give the same result as rounded double-valued
  // filtering.
  QVERIFY(Pii::equals(PiiImage::intFilter(image, filter),reference));

  PiiMatrix<double> h,v;
  PiiImage::separateFilter(filter, h, v);
  // And so should the separated version
  QVERIFY(Pii::equals(PiiImage::intFilter(image, h, v),reference));
}

void TestPiiImage::unwarpCylinder()
{
  PiiMatrix<int> matSource(11,11);
  //qDebug("d = 1000, f = 1500");
  PiiImage::unwarpCylinder(matSource, 1000, 1500);
  //qDebug("d = 1000, f = 150");
  PiiImage::unwarpCylinder(matSource, 1000, 150);
  //qDebug("d = 1, f = INF");
  PiiImage::unwarpCylinder(matSource, 1, 1e100);
}

void TestPiiImage::crop()
{
  PiiMatrix<int> image(5, 5,
                       0, 0, 1, 0, 0,
                       0, 0, 2, 0, 0,
                       0, 0, 3, 0, 0,
                       0, 0, 4, 0, 0,
                       0, 0, 5, 0, 0);

  // Rotate 90 degrees clockwise around origin and translate 3 steps
  // to the right and one down.
  PiiMatrix<float> matTransform(3, 3,
                                0.0, -1.0, 3.0,
                                1.0, 0.0, 1.0,
                                0.0, 0.0, 1.0);

  PiiMatrix<int> matCropped = PiiImage::crop(image,
                                             0, 0, 3, 3,
                                             matTransform);

  QVERIFY(Pii::equals(matCropped, PiiMatrix<int>(3, 3,
                                                 0, 0, 0,
                                                 2, 3, 4,
                                                 0, 0, 0)));
}

void TestPiiImage::xorMatch()
{
  PiiMatrix<uchar> matA(5,5,
                        0,0,0,0,0,
                        0,1,1,0,0,
                        0,1,1,0,0,
                        0,1,1,1,0,
                        0,0,0,0,0);
  PiiMatrix<uchar> matB(3,3,
                        1,1,0,
                        1,1,0,
                        1,1,1);

  QCOMPARE(PiiImage::xorMatch(matA, matB), 1.0);

  matB(2,2) = 0;
  QCOMPARE(PiiImage::xorMatch(matA, matB), 1.0 - 1.0/9);

  matB = matB(0,0,-1,2);
  QCOMPARE(PiiImage::xorMatch(matA, matB), 1.0);
}

void TestPiiImage::maxFilter()
{
  PiiMatrix<uchar> img(7, 8,
                       5,4,3,2,1,0,2,1,
                       5,6,5,4,3,2,1,0,
                       4,4,6,5,4,4,2,1,
                       7,5,5,5,5,6,0,0,
                       0,0,0,0,0,0,0,1,
                       0,0,0,0,0,0,0,0,
                       0,0,1,0,0,0,0,0);
  PiiMatrix<uchar> res(7, 8,
                       6,6,6,5,4,3,2,2,
                       6,6,6,6,5,4,4,2,
                       7,7,6,6,6,6,6,2,
                       7,7,6,6,6,6,6,2,
                       7,7,5,5,6,6,6,1,
                       0,1,1,1,0,0,1,1,
                       0,1,1,1,0,0,0,0);
  PiiMatrix<uchar> res2(7, 8,
                        6,6,6,5,4,3,2,2,
                        6,6,6,6,5,4,4,2,
                        7,7,6,6,6,6,6,2,
                        7,7,6,6,6,6,6,2,
                        7,7,6,6,6,6,6,2,
                        7,7,5,5,6,6,6,1,
                        0,1,1,1,0,0,1,1);

  QVERIFY(Pii::equals(PiiImage::maxFilter(img,3), res));
  QVERIFY(Pii::equals(PiiImage::maxFilter(img,4,3), res2));
}

void TestPiiImage::minFilter()
{
  PiiMatrix<uchar> img(7, 8,
                       5,4,3,2,1,0,2,1,
                       5,6,5,4,3,2,1,0,
                       4,4,6,5,4,4,2,1,
                       7,5,5,5,5,6,2,2,
                       3,4,4,3,2,2,8,8,
                       0,2,8,8,2,2,8,8,
                       0,8,4,4,2,2,8,8);
  PiiMatrix<uchar> res(7, 8,
                       3,2,1,0,0,0,0,0,
                       3,2,1,0,0,0,0,0,
                       4,4,3,2,1,0,0,0,
                       3,3,2,2,2,1,1,1,
                       0,0,0,2,2,2,2,2,
                       0,0,0,2,2,2,2,2,
                       0,0,0,2,2,2,2,2);

  QVERIFY(Pii::equals(PiiImage::minFilter(img,3,5), res));
}

struct GradientPicker
{
  GradientPicker(QList<QPair<int, int> >* coords) :
    _plstCoords(coords)
  {}

  void operator() (int y, int x, int yGrad, int /*xGrad*/)
  {
    if (yGrad < 0)
      _plstCoords->append(qMakePair(y, x));
  }

  QList<QPair<int, int> >* _plstCoords;
};

void TestPiiImage::fastGradient()
{
  PiiMatrix<uchar> img(4, 5,
                       1, 2, 3, 4, 5,
                       5, 4, 3, 2, 1,
                       2, 3, 4, 5, 6,
                       4, 3, 2, 1, 0);
  {
    PiiMatrix<uint> res(PiiMatrix<uint>::uninitialized(4, 5));
    PiiImage::fastGradient(img, PiiImage::PackedGradientFunction<uint>(), res);
    QCOMPARE(res(1, 1), uint(((-1 - 4 - 3 + 2 + 6 + 4) << 16) | (-1 - 10 - 2 + 3 + 6 + 4)));
    QCOMPARE(res(1, 2), uint(((-2 - 6 - 4 + 3 + 8 + 5) << 16) | (-2 - 8 - 3 + 4 + 4 + 5)));
    QCOMPARE(res(1, 3), uint(((-3 - 8 - 5 + 4 + 10 + 6) << 16) | (-3 - 6 - 4 + 5 + 2 + 6)));
    QCOMPARE(res(2, 1), uint(((-5 - 8 - 3 + 4 + 6 + 2) << 16) | (-5 - 4 - 4 + 3 + 8 + 2)));
    QCOMPARE(res(2, 2), uint(((-4 - 6 - 2 + 3 + 4 + 1) << 16) | (-4 - 6 - 3 + 2 + 10 + 1)));
    QCOMPARE(res(2, 3), uint(((-3 - 4 - 1 + 2 + 2 + 0) << 16) | (-3 - 8 - 2 + 1 + 12 + 0)));
  }
  {
    QList<QPair<int,int > > lstPicked;
    // 4,0  4,0  4,0
    // -4,0  -4,0  -4,0
    PiiImage::fastGradient(img, GradientPicker(&lstPicked));
    QCOMPARE(lstPicked, (QList<QPair<int, int > >() << qMakePair(2, 1) << qMakePair(2, 2) << qMakePair(2, 3)));
  }
}

QTEST_MAIN(TestPiiImage)
