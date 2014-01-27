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

#include "TestPiiMatrixDecompositions.h"
#include <PiiMatrixUtil.h>
#include <iostream>
#include <PiiHouseholderTransform.h>
#include <PiiQrDecomposition.h>
#include <PiiBdDecomposition.h>
#include <PiiSvDecomposition.h>
#include <QtTest>
#include <PiiRandom.h>

void TestPiiMatrixDecompositions::unpackReflectors()
{
  for (int iRows = 3; iRows < 5; ++iRows)
    {
      for (int iCols = 3; iCols < 5; ++iCols)
        {
          PiiMatrix<double> mat(iRows, iCols);
          Pii::sequence(mat.begin(), mat.end(), 1);

          unpackRowReflectors(mat, 0);
          unpackRowReflectors(mat, 1);
          unpackColumnReflectors(mat, 0);
          unpackColumnReflectors(mat, 1);
        }
    }
}

void TestPiiMatrixDecompositions::unpackRowReflectors(const PiiMatrix<double>& mat, int diagonal)
{
  PiiMatrix<double> matTransform(mat);
  int iVectors = qMin(mat.rows(), mat.columns()-diagonal);
  PiiMatrix<double> matTau(1, iVectors);
  for (int r=0; r<iVectors; ++r)
    {
      double dBeta;
      Pii::fillN(matTransform.rowBegin(r), r+diagonal, 0.0);
      Pii::householderTransform(matTransform.rowBegin(r)+r+diagonal,
                                mat.columns()-r-diagonal,
                                matTau[0] + r,
                                &dBeta);
      if (r < iVectors - 1)
        Pii::reflectRows((PiiMatrix<double>&)matTransform(r+1, r+diagonal, -1, -1),
                         matTransform.rowBegin(r)+r+diagonal,
                         matTau(0,r));
    }

  PiiMatrix<double> matT(mat.rows(), mat.rows()), matGram(iVectors, iVectors);
  Pii::unpackReflectors(Pii::Horizontally, matTransform, matTau[0], matT, matGram, diagonal);
  PiiMatrix<double> matP(Pii::transpose(matTransform) * Pii::transpose(matT) * matTransform);
  for (int d=0; d<matP.rows(); ++d)
    matP(d,d) += 1;
  PiiMatrix<double> matNulled = mat * Pii::transpose(matP);
  for (int r=0; r<iVectors; ++r)
    {
      for (int c=r+diagonal+1; c<matNulled.columns(); ++c)
        QVERIFY(Pii::almostEqualRel(matNulled(r,c), 0.0, 1e-10));
    }

}

void TestPiiMatrixDecompositions::unpackColumnReflectors(const PiiMatrix<double>& mat, int diagonal)
{
  PiiMatrix<double> matTransform(mat);
  int iVectors = qMin(mat.columns(), mat.rows()-diagonal);
  PiiMatrix<double> matTau(1, iVectors);
  for (int c=0; c<iVectors; ++c)
    {
      double dBeta;
      Pii::fillN(matTransform.columnBegin(c), c+diagonal, 0.0);
      Pii::householderTransform(matTransform.columnBegin(c)+c+diagonal,
                                mat.rows()-c-diagonal,
                                matTau[0] + c,
                                &dBeta);
      if (c < iVectors - 1)
        Pii::reflectColumns((PiiMatrix<double>&)matTransform(c+diagonal, c+1, -1, -1),
                            matTransform.columnBegin(c)+c+diagonal,
                            matTau(0,c));
    }

  PiiMatrix<double> matT(mat.columns(), mat.columns()), matGram(iVectors, iVectors);
  Pii::unpackReflectors(Pii::Vertically, matTransform, matTau[0], matT, matGram, diagonal);

  PiiMatrix<double> matQ(matTransform * matT * Pii::transpose(matTransform));
  for (int d=0; d<matQ.rows(); ++d)
    matQ(d,d) += 1;

  PiiMatrix<double> matNulled = Pii::transpose(matQ) * mat;
  for (int c=0; c<iVectors; ++c)
    {
      for (int r=c+diagonal+1; r<matNulled.rows(); ++r)
        QVERIFY(Pii::almostEqualRel(matNulled(r,c), 0.0, 1e-10));
    }
}

void TestPiiMatrixDecompositions::householderTransform()
{
  double dTau, dBeta;
  for (int i=0; i<30; ++i)
    {
      // Create a random size vector containing random numbers.
      int iLength = Pii::uniformRandom(2, 100);
      PiiMatrix<double> mat(Pii::uniformRandomMatrix(1, iLength, -1000, 1000));
      // Perform the Householder transform
      PiiMatrix<double> matTransform(mat);
      Pii::householderTransform(matTransform.rowBegin(0), iLength, &dTau, &dBeta);
      // Convert the transform vector to a transform matrix
      PiiMatrix<double> matH(PiiMatrix<double>::identity(iLength) - dTau * Pii::transpose(matTransform) * matTransform);
      // Reflect the input vector using the matrix
      PiiMatrix<double> matReflected = matH * Pii::transpose(mat);

      // Check that the output vector equals [ beta 0 ... 0 ]'
      QCOMPARE(matReflected(0,0), dBeta);
      for (int i=1; i<iLength; ++i)
        QVERIFY(Pii::almostEqualRel(matReflected(i,0), 0.0));

      // We should get the same result with the more efficient
      // reflectColumns() routine.
      matReflected = Pii::transpose(mat);
      Pii::reflectColumns(matReflected,
                          matTransform.rowBegin(0),
                          dTau);
      QCOMPARE(matReflected(0,0), dBeta);
      for (int i=1; i<iLength; ++i)
        QVERIFY(Pii::almostEqualRel(matReflected(i,0), 0.0));

      // ... and with the reflectRows() routine.
      Pii::reflectRows(mat, matTransform.rowBegin(0), dTau);
      QCOMPARE(mat(0,0), dBeta);
      for (int i=1; i<iLength; ++i)
        QVERIFY(Pii::almostEqualRel(mat(0,i), 0.0));
    }
}

void TestPiiMatrixDecompositions::qrDecompose()
{
  try
    {
      PiiMatrix<double> matA(3,2,
                             1.0, 0.0,
                             2.0, 0.0,
                             3.0, 0.0);
      PiiMatrix<double> matR;
      PiiMatrix<double> matQ = Pii::qrDecompose(matA, &matR);

      QVERIFY(Pii::almostEqual(matQ * matR, matA, 1e-4));
    }
  catch (PiiException& ex)
    {
      qWarning("%s", qPrintable(ex.location()));
      QFAIL(qPrintable(ex.message()));
    }

  try
    {
      // Simple 3-by-3 case with a known answer.
      PiiMatrix<double> matA(3, 3,
                             12.0, -51.0, 4.0,
                             6.0, 167.0, -68.0,
                             -4.0, 24.0, -41.0);
      PiiMatrix<double> matR;
      PiiMatrix<double> matQ = Pii::qrDecompose(matA, &matR);
      PiiMatrix<double> matTau;
      QVERIFY(Pii::almostEqual(matQ,
                               PiiMatrix<double>(3, 3,
                                                 -0.857143, 0.394286, 0.331429,
                                                 -0.428571, -0.902857, -0.0342857,
                                                 0.285714, -0.171429, 0.942857),
                               1e-5));

      // R must be upper triangle
      for (int r=1; r<matR.rows(); ++r)
        for (int c=0; c<r; ++c)
          QCOMPARE(matR(r,c), 0.0);

      // A = QR?
      //int r,c;
      //double dMax = Pii::max(Pii::matrix(Pii::abs(matA - matQ * matR)), &r, &c);
      //qDebug("%lg %d %d", dMax, r, c);
      QVERIFY(Pii::almostEqual(matQ * matR, matA, 1e-10));
    }
  catch (PiiException& ex)
    {
      qWarning("%s", qPrintable(ex.location()));
      QFAIL(qPrintable(ex.message()));
    }

  try
    {
      // Larger matrices
      for (int i=0; i<10; ++i)
        {
          PiiMatrix<double> matA(Pii::uniformRandomMatrix(30 + int(Pii::uniformRandom(0,100)),
                                                          30 + int(Pii::uniformRandom(0,100)),
                                                          -1e10, 1e10));
          //qDebug("%d: %dx%d", i, matA.rows(), matA.columns());

          PiiMatrix<double> matR;
          PiiMatrix<double> matQ = Pii::qrDecompose(matA, &matR);

          //int r,c;
          //double dMax = Pii::max(Pii::matrix(Pii::abs(matA - matQ * matR)), &r, &c);
          //qDebug("%lf %d %d", dMax, r, c);

          // R must be upper triangle
          for (int r=1; r<matR.rows(); ++r)
            for (int c=0; c<qMin(r,matR.columns()-1); ++c)
              QCOMPARE(matR(r,c), 0.0);

          // A = QR?
          QVERIFY(Pii::almostEqual(matQ * matR, matA, 1e-4));
        }
    }
  catch (PiiException& ex)
    {
      qWarning("%s", qPrintable(ex.location()));
      QFAIL(qPrintable(ex.message()));
    }
}

void TestPiiMatrixDecompositions::bdDecompose()
{
  for (int i=0; i<10; ++i)
    {
      PiiMatrix<double> matA(Pii::uniformRandomMatrix(3 + int(Pii::uniformRandom(0,40)),
                                                      3 + int(Pii::uniformRandom(0,40)),
                                                      -1e5, 1e5));

      PiiMatrix<double> matTauQ, matTauP, matDecomposed(matA);
      Pii::bdDecompose(matDecomposed, matTauQ, matTauP);
      PiiMatrix<double> matQ = Pii::bdUnpackQ(matDecomposed, matTauQ[0]);
      PiiMatrix<double> matP = Pii::bdUnpackP(matDecomposed, matTauP[0]);
      Pii::bdUnpackB(matDecomposed);
      QVERIFY(Pii::almostEqual(matA, matQ * matDecomposed * matP, 1e-8));
    }
}

void TestPiiMatrixDecompositions::svDecompose()
{
  {
    PiiMatrix<double,2,2> mat(1.0, 2.0,
                              -4.0, -1.0),
      copy(mat);

    PiiPlaneRotation<double> left, right;
    Pii::svDecompose(mat, left, right);
    left.rotateColumns(copy, 0, 1);
    right.rotateRows(copy, 0, 1);

    QVERIFY(Pii::almostEqualRel(copy(0,1), 0.0, 1e-15));
    QVERIFY(Pii::almostEqualRel(copy(1,0), 0.0, 1e-15));
  }

  {
    PiiMatrix<double> mat(3,3,
                          1.0, 1.0/2, 1.0/3,
                          1.0/2, 1.0/3, 1.0/4,
                          1.0/3, 1.0/4, 1.0/5);
    PiiMatrix<double> U,S,V;
    S = Pii::svDecompose(mat, &U, &V);

    //Pii::matlabPrint(std::cout, U);
    //Pii::matlabPrint(std::cout, S);
    //Pii::matlabPrint(std::cout, V);

    PiiMatrix<double> matS(3,3);
    Pii::setDiagonal(matS, S[0]);

    QVERIFY(Pii::almostEqual(mat, U*matS*Pii::transpose(V), 1e-15));
  }

  {
    PiiMatrix<double> mat(4,3,
                          1.0, -1.0, 0.0,
                          -2.0, -3.0, 1.0,
                          0.0, 1.0, 2.0,
                          -1.0, 1.0, -1.0);
    PiiMatrix<double> U,S,V;
    S = Pii::svDecompose(mat, &U, &V);

    //Pii::matlabPrint(std::cout, U);
    //Pii::matlabPrint(std::cout, S);
    //Pii::matlabPrint(std::cout, V);

    PiiMatrix<double> matS(3,3);
    Pii::setDiagonal(matS, S[0]);

    QVERIFY(Pii::almostEqual(mat, U*matS*Pii::transpose(V), 1e-7));
  }
}

QTEST_MAIN(TestPiiMatrixDecompositions)
