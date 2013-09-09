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
#include <PiiQImage.h>
#include <PiiQImageUtil.h>
#include <PiiYdinTypes.h>
#include <PiiColor.h>
#include "TestPiiQImage.h"
#include <iostream>

uchar rawData[] = { 'A', 'B', 'C', 'D',
                    'E', 'F', 'G', 'H',
                    'I', 'J', 'K', 'L',
                    'M', 'N', 'O', 'P',
                    'Q', 'R', 'S', 'T',
                    'U', 'V', 'W', 'X',
                    'Y', 'Z' };

void TestPiiQImage::imageToMatrix()
{
  {
    // Color image
    QImage image(rawData, 2, 3, QImage::Format_RGB32);
    PiiMatrix<PiiColor4<> > matrix = PiiColorQImage::create(image)->toMatrix();
    QCOMPARE(matrix(0,0).rgbB, (unsigned char)'A');
    QCOMPARE(matrix(0,0).rgbG, (unsigned char)'B');
    QCOMPARE(matrix(0,0).rgbR, (unsigned char)'C');
    QCOMPARE(matrix(2,1).rgbB, (unsigned char)'U');
    QCOMPARE(matrix(2,1).rgbG, (unsigned char)'V');
    QCOMPARE(matrix(2,1).rgbR, (unsigned char)'W');
    matrix(0,0) = 0xff;
    QCOMPARE(image.pixel(0,0), 0xffffffff);
    image.setPixel(0,0, 'A' | 'B' << 8 | 'C' << 16 | 'D' << 24);
    QCOMPARE(matrix(0,0).rgbB, (unsigned char)'A');
    QCOMPARE(matrix(0,0).rgbG, (unsigned char)'B');
    QCOMPARE(matrix(0,0).rgbR, (unsigned char)'C');
  }
  {
    // 8-bit image
    QImage image(rawData, 2, 2, QImage::Format_Indexed8);
    PiiMatrix<unsigned char> matrix = PiiGrayQImage::create(image)->toMatrix();
    QCOMPARE(matrix(0,0), (unsigned char)'A');
    QCOMPARE(matrix(1,1), (unsigned char)'F');
  }
  {
    // 8-bit image with own data
    QImage image(9, 9, QImage::Format_Indexed8);
    PiiMatrix<unsigned char> matrix = PiiGrayQImage::create(image)->toMatrix();
    matrix(1,1,4,4) = '5';
    QCOMPARE(image.scanLine(4)[4], (unsigned char)'5');
  }
}

void TestPiiQImage::matrixToImage()
{
  {
    // Aligned 8-bit matrix
    PiiMatrix<unsigned char> matrix(4, 4, rawData);
    PiiGrayQImage *pImage = PiiGrayQImage::create(matrix);
    QCOMPARE(pImage->scanLine(0)[0], (unsigned char)'A');
    QCOMPARE(pImage->scanLine(3)[3], (unsigned char)'P');
    pImage->scanLine(0)[1] = '1'; // changes global array
    QCOMPARE(matrix(0,1), (unsigned char)'1');
    pImage->scanLine(0)[1] = 'B'; // restore old value
  }
  {
    // Aligned 8-bit matrix with non-aligned width
    PiiMatrix<unsigned char> matrix(4, 3, rawData, Pii::RetainOwnership, 4);
    PiiGrayQImage *pImage = PiiGrayQImage::create(matrix);
    QCOMPARE(pImage->scanLine(0)[0], (unsigned char)'A');
    QCOMPARE(pImage->scanLine(3)[2], (unsigned char)'O');
    pImage->scanLine(0)[1] = '2'; // changes global array
    QCOMPARE(matrix(0,1), (unsigned char)'2');
    pImage->scanLine(0)[1] = 'B'; // restore old value
  }
  {
    // Color matrix 
    PiiMatrix<PiiColor4<unsigned char> > matrix(3, 2, static_cast<void*>(rawData), Pii::RetainOwnership);
    PiiColorQImage *pImage = PiiColorQImage::create(matrix);
    QCOMPARE(qRed(((QRgb*)pImage->scanLine(0))[0]), (int)'C');
    QCOMPARE(qBlue(((QRgb*)pImage->scanLine(2))[1]), (int)'U');
    pImage->scanLine(0)[1] = '1';
    QCOMPARE(matrix(0,0).rgbG, (unsigned char)'1');
    QCOMPARE(qGreen(((QRgb*)pImage->scanLine(0))[0]), (int)'1');

    PiiMatrix<PiiColor4<> > matrix2 = pImage->toMatrix();
    QCOMPARE(matrix2(0,0).rgbB, (unsigned char)'A');

  }
}

QTEST_MAIN(TestPiiQImage)
