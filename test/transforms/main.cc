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

#include "TestPiiTransforms.h"
#include "TestPiiHoughTransformOperation.h"
#include <PiiTransforms.h>
#include <PiiHoughTransform.h>
#include <QtTest>
#include <iostream>

#include <PiiImageFileReader.h>
#include <PiiImage.h>

#include <QDebug>
#include <QFile>

void TestPiiTransforms::linearHough()
{
  {
    PiiMatrix<int> img(5,5,
                       0,0,0,0,1,
                       0,0,0,1,0,
                       0,0,1,0,0,
                       0,1,0,0,0,
                       1,0,0,0,0);

    double distanceResolution = 0.5;
    double angleResolution = 45.0;
    PiiMatrix<int> result(PiiHoughTransform(angleResolution, distanceResolution).transform<int>(img));

    int r,c;
    Pii::max(result, &r, &c);

    QCOMPARE(r, result.rows()/2); // line crosses origin
    QCOMPARE(c, 1); // 45 degrees
    //Pii::matlabPrint(std::cout, result);
  }
  {
    PiiMatrix<int> img(5,5,
                       1,0,0,0,0,
                       0,1,0,0,0,
                       0,0,1,0,0,
                       0,0,0,1,0,
                       0,0,0,0,1);

    double distanceResolution = 1.0;
    double angleResolution = 45.0;
    PiiMatrix<int> result(PiiHoughTransform(angleResolution,distanceResolution).transform<int>(img));

    int r,c;
    Pii::max(result, &r, &c);

    //qDebug()<<"dist : "<<(r-rows)*distanceResolution<<" angle: "<<c*angleResolution;

    QCOMPARE(r, result.rows()/2); // line crosses origin
    QCOMPARE(c, 3); // 135 degrees
  }
  {
    PiiMatrix<int> img(5,5,
                       0,1,0,0,0,
                       0,0,1,0,0,
                       0,0,0,1,0,
                       0,0,0,0,1,
                       0,0,0,0,0);
    double distanceResolution = 1.0;
    double angleResolution = 45.0;
    PiiMatrix<int> result(PiiHoughTransform(angleResolution, distanceResolution).transform<int>(img));
    int r,c;
    Pii::max(result, &r, &c);
    //qDebug()<<"dist : "<<r*distanceResolution<<" angle: "<<c*angleResolution;

    QCOMPARE(r, result.rows()/2 - 1); // minimum distance to origin is ~1 pixel (negative d)
    QCOMPARE(c, 3); // 135 degrees
  }

  {
    PiiMatrix<int> img(5,5,
                       0,0,0,1,0,
                       0,0,0,1,0,
                       0,0,0,1,0,
                       0,0,0,1,0,
                       0,0,0,1,0);
    double distanceResolution = 1.0;
    double angleResolution = 45.0;
    PiiMatrix<int> result(PiiHoughTransform(angleResolution, distanceResolution).transform<int>(img));
    //Pii::matlabPrint(std::cout, result);
    int rows = result.rows()/2;
    int r,c;
    Pii::max(result, &r, &c);
    //qDebug()<<"dist : "<<r*distanceResolution<<" angle should be 0 or 90 (0): "<<c*angleResolution;

    QCOMPARE(1.0,(r-rows)*distanceResolution); // d is 1
    QCOMPARE(0.0,c*angleResolution); // 0 degrees
  }


  {
    PiiMatrix<int> img(5,5,
                       1,1,1,1,1,
                       0,0,0,0,0,
                       0,0,0,0,0,
                       0,0,0,0,0,
                       0,0,0,0,0);

    double distanceResolution = 1.0;
    double angleResolution = 45.0;
    PiiMatrix<int> result(PiiHoughTransform(angleResolution, distanceResolution).transform<int>(img));

    int rows = result.rows()/2;
    int r,c;
    Pii::max(result, &r, &c);


    QCOMPARE(-2.0,(r-rows)*distanceResolution); // d is -2
    QCOMPARE(90.0,c*angleResolution); // 90 degrees
  }

  {
    PiiMatrix<int> img(7,7,
                       1,1,1,1,1,1,1,
                       0,0,0,0,0,0,0,
                       1,0,0,0,0,0,0,
                       0,1,0,0,0,0,0,
                       0,0,1,0,0,0,0,
                       0,0,0,1,0,0,0,
                       0,0,0,0,1,0,0);
    double distanceResolution = 1.0;
    double angleResolution = 45.0;
    PiiMatrix<int> result(PiiHoughTransform(angleResolution, distanceResolution).transform<int>(img));

    int r,c;
    Pii::max(result, &r, &c);
    //qDebug()<<"highest dist : "<<r*distanceResolution<<" angle: "<<c*angleResolution;

    int rows = result.rows()/2;
    QCOMPARE(-3.0,(r-rows)*distanceResolution); // d is -3
    QCOMPARE(90.0,c*angleResolution); // 90 degrees


    // Suppress highest peak
    result(r,c) = 0;

    // Second strongest line
    Pii::max(result, &r, &c);
    //qDebug()<<"next highest dist : "<<r*distanceResolution<<" angle: "<<c*angleResolution;
    QCOMPARE(1.0,(r-rows)*distanceResolution); //d is 1
    QCOMPARE(135.0, c*angleResolution);
  }

  {
    PiiMatrix<int> img(4,4,
                       0,0,0,1,
                       0,0,1,0,
                       0,1,0,0,
                       1,0,0,0);
    double distanceResolution = 1.0;
    double angleResolution = 45.0;
    PiiMatrix<int> result(PiiHoughTransform(angleResolution, distanceResolution).transform<int>(img));

    //QCOMPARE(result.rows(), 5);
    //QCOMPARE(result.columns(), 4);
    //Pii::matlabPrint(std::cout, result);
    int r,c;
    Pii::max(result, &r, &c);
    //qDebug()<<"dist : "<<r*distanceResolution<<" angle: "<<c*angleResolution;

    QCOMPARE(r,2); // d is 0
    QCOMPARE(c,1); // 45 degrees
  }
  /*
  {

    PiiMatrix<int> img(12,12,
                       1,1,1,1,1,1,1,1,1,1,1,1,
                       0,0,0,0,0,0,0,0,0,0,0,0,
                       1,0,0,0,0,0,0,0,0,0,0,0,
                       0,1,0,0,0,0,0,0,0,1,1,1,
                       0,0,1,0,0,0,0,0,0,1,1,1,
                       0,0,0,1,0,0,0,1,1,1,1,1,
                       0,0,0,0,1,0,0,0,0,1,1,1,
                       0,0,0,0,0,1,0,0,0,1,1,1,
                       0,0,0,0,0,0,1,0,0,0,0,0,
                       0,0,0,0,0,0,0,1,0,0,0,0,
                       0,0,0,0,0,0,0,0,1,0,0,0,
                       0,0,0,0,0,0,0,0,0,1,0,0);
    double distanceResolution = 1.0;
    double angleResolution = 10.0;
    PiiMatrix<int> result(PiiTransforms::linearHough<int>(img, std::bind2nd(std::greater<int>(), 0), angleResolution,
                                                          distanceResolution));
    //PiiMatrix<int> result(PiiTransforms::linearHough<int, std::greater<int> >(img, 0, 10));

    int r,c;
    int high=Pii::max(result, &r, &c);
    //qDebug()<<"Isoin: "<<high<<" r: "<<r*distanceResolution <<" angle :"<<c*angleResolution;
   // QCOMPARE(result.rows(), 9);
   // QCOMPARE(result.columns(), 8);

   // QCOMPARE(r,1); // d is -3
   // QCOMPARE(c,4); // 90 degrees

    QDebug d = qDebug();
    //Pii::matlabPrint(std::cout, result);

    // Suppress highest peak
    result(r,c) = 0;

    // Second strongest line
    high = Pii::max(result, &r, &c);
    //qDebug()<<"toiseksi Isoin: "<<high;
    //QCOMPARE(r,5); // d is 1
    //QCOMPARE(c,6); // 135 degrees


  }
  */
}

void TestPiiTransforms::circularHough()
{
  PiiMatrix<int> matImg(9,9,
                        0,0,0,0,0,0,0,0,0,
                        0,0,0,1,1,1,0,0,0,
                        0,0,1,0,0,0,1,0,0,
                        0,1,0,0,0,0,0,1,0,
                        0,1,0,0,0,0,0,1,0,
                        0,1,0,0,0,0,0,1,0,
                        0,0,1,0,0,0,1,0,0,
                        0,0,0,1,1,1,0,0,0,
                        0,0,0,0,0,0,0,0,0);

  // Max gradient is detected just outside of the circle -> radius is
  // 4, not 3
  QList<PiiMatrix<int> > lstMatrices(PiiTransforms::circularHough(matImg,
                                                                  PiiTransforms::ThresholdSelector(1.0),
                                                                  4, 4));
  QCOMPARE(lstMatrices.size(), 1);
  QCOMPARE(lstMatrices[0].rows(), 9);
  QCOMPARE(lstMatrices[0].columns(), 9);
  int r,c;
  Pii::max(lstMatrices[0], &r, &c);
  QCOMPARE(r, 4);
  QCOMPARE(c, 4);

  lstMatrices = PiiTransforms::circularHough(matImg,
                                             PiiTransforms::ThresholdSelector(1.0),
                                             4, 4, 1,
                                             PiiTransforms::PositiveGradient);

  QCOMPARE(lstMatrices.size(), 1);
  QCOMPARE(lstMatrices[0].rows(), 9);
  QCOMPARE(lstMatrices[0].columns(), 9);
  Pii::max(lstMatrices[0], &r, &c);
  QCOMPARE(r, 4);
  QCOMPARE(c, 4);
}

void TestPiiHoughTransformOperation::initTestCase()
{
  QVERIFY(createOperation("piitransforms", "PiiHoughTransformOperation"));
}

void TestPiiHoughTransformOperation::hough()
{
  QString strImage =  "data/colorframe0.jpeg";

  if (!QFile::exists(strImage))
    QSKIP(qPrintable(QString("Couldn't find the test image '%1'").arg(strImage))
#if QT_VERSION < 0x050000
          , SkipAll
#endif
          );

  QVERIFY(start(ExpectFail));

  operation()->setProperty("angleResolution", 1.0);
  operation()->setProperty("distanceResolution", 1.0);
  operation()->setProperty("results", 2);

  connectAllInputs();

  QVERIFY(start());

  try
    {
      //Read image.
      PiiMatrix<unsigned char> image = PiiImageFileReader::readGrayImage(strImage)->toMatrix();

      //Then do filtering and edge detection
      PiiMatrix<double> filtered = PiiImage::filter<double>(image,
                                                            PiiImage::makeFilter<double>(PiiImage::GaussianFilter,7),
                                                            Pii::ExtendZeros);

      PiiMatrix<int> img = PiiImage::detectEdges(filtered);
      bool bEmpty = true;
      for(int r = 0; r < img.rows(); ++r)
        for(int c =0; c < img.columns(); ++c)
          if(img(r,c) != 0)
            bEmpty = false;

      QVERIFY(!bEmpty);
      QVERIFY(sendObject("image", img));
    }
  catch(PiiException& ob)
    {
      qDebug()<<"Exception : "<<ob.message();
      QVERIFY(false);
    }

   QVERIFY(hasOutputValue("objects"));
   PiiMatrix<int> objects = outputValue<PiiMatrix<int> >("objects", PiiMatrix<int>());
   QVERIFY(objects.rows() != 0);
   QVERIFY(objects.columns() != 0);

   QVERIFY(hasOutputValue("coordinates"));
   PiiMatrix<double> coordinates = outputValue<PiiMatrix<double> >("coordinates", PiiMatrix<double>());
   QVERIFY(coordinates.rows() != 0);
   QVERIFY(coordinates.columns() != 0);

   stop();
}

int main(int argc, char *argv[])
{
  //Contains all transform-namespace functions.
  TestPiiTransforms transform;
  QTest::qExec(&transform,argc, argv);

  //Contains all Hough-operation functions.
  TestPiiHoughTransformOperation op;
  QTest::qExec(&op, argc, argv);


  return 0;
}
