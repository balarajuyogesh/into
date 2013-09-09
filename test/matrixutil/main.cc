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

#include <PiiMatrixUtil.h>

#include "TestPiiMatrixUtil.h"
#include <QtTest>
#include <PiiMath.h>
#include <QTextStream>
#include <QDebug>
#include <iostream>

void TestPiiMatrixUtil::columnToList()
{
  //Basic 
  {
    PiiMatrix<int> mat(2,2, 1,2, 3,4);
    QList<int> lst = Pii::columnToList<QList<int> >(mat, 0);
    for(int i = 0; i < lst.size(); ++i)
      QCOMPARE(mat(i,0), lst[i]);
     

    lst = Pii::columnToList<QList<int> > (mat,1);
    for(int i = 0; i < lst.size(); ++i)
      QCOMPARE(mat(i,1),lst[i]);

    PiiMatrix<char> matChar(2,3, 'a','p','i',
                                 'n','a','t');

    QList<char> lstChar = Pii::columnToList<QList<char> > (matChar,2);
    for(int i = 0; i < lst.size(); ++i)
      QCOMPARE(matChar(i,2), lstChar[i]);
  }

  //What happens if matrix is freed
  {
    PiiMatrix<int>* pMat = new PiiMatrix<int>(2,2,1,2,3,4);
    QList<int> lst = Pii::columnToList<QList<int> >(*pMat, 0);
    delete pMat;
    pMat = 0;
    QCOMPARE(1, lst[0]);
    QCOMPARE(3, lst[1]);

    //And how about some evil-pointers;
    int i = 42;
    int j = 41;
    int k = 40;
    int l = 39;

    int* pI = &i;
    int* pJ = &j;
    int* pK = &k;
    int* pL = &l;
    
    PiiMatrix<int*>* pMatEvil = new PiiMatrix<int*>(2,2, pI, pJ, pK, pL);
    QList<int*> lstPointers = Pii::columnToList<QList<int*> >(*pMatEvil, 0);
    delete pMatEvil;
    pMatEvil = 0;
    pI = 0;
    pJ = 0;
    pK = 0;
    pL = 0;
    QCOMPARE(42, *lstPointers[0]);
    QCOMPARE(40, *lstPointers[1]);
  }
}

void TestPiiMatrixUtil::concatenate()
{
  PiiMatrix<int> mat1(1,2,
                      1,2);
  
  PiiMatrix<int> mat2(3,2,
                      3,4,
                      5,6,
                      7,8);
  
  QVERIFY(Pii::equals(Pii::concatenate(mat1, mat2, Pii::Vertically),
                      PiiMatrix<int>(4,2,
                                     1,2,
                                     3,4,
                                     5,6,
                                     7,8)));
  QVERIFY(Pii::equals(Pii::concatenate(Pii::transpose(mat1),
                                       Pii::transpose(mat2),
                                       Pii::Horizontally),
                      PiiMatrix<int>(2,4,
                                     1,3,5,7,
                                     2,4,6,8)));
}
void TestPiiMatrixUtil::extend()
{
  {
    PiiMatrix<int> mat(5,5,
                       1, 2, 3, 4, 5,
                       2, 3, 4, 5, 6,
                       3, 4, 5, 6, 7,
                       4, 5, 6, 7, 8,
                       5, 6, 7, 8, 9);

    QVERIFY(Pii::equals(Pii::extend(mat, 2, Pii::ExtendSymmetric),
                        PiiMatrix<int>(9, 9,
                                       3, 2, 2, 3, 4, 5, 6, 6, 5,
                                       2, 1, 1, 2, 3, 4, 5, 5, 4,
                                       2, 1, 1, 2, 3, 4, 5, 5, 4,
                                       3, 2, 2, 3, 4, 5, 6, 6, 5,
                                       4, 3, 3, 4, 5, 6, 7, 7, 6,
                                       5, 4, 4, 5, 6, 7, 8, 8, 7,
                                       6, 5, 5, 6, 7, 8, 9, 9, 8,
                                       6, 5, 5, 6, 7, 8, 9, 9, 8,
                                       5, 4, 4, 5, 6, 7, 8, 8, 7)));

    QVERIFY(Pii::equals(Pii::extend(mat, 2, Pii::ExtendPeriodic),
                        PiiMatrix<int>(9, 9,
                                       7, 8, 4, 5, 6, 7, 8, 4, 5,
                                       8, 9, 5, 6, 7, 8, 9, 5, 6,
                                       4, 5, 1, 2, 3, 4, 5, 1, 2,
                                       5, 6, 2, 3, 4, 5, 6, 2, 3,
                                       6, 7, 3, 4, 5, 6, 7, 3, 4,
                                       7, 8, 4, 5, 6, 7, 8, 4, 5,
                                       8, 9, 5, 6, 7, 8, 9, 5, 6,
                                       4, 5, 1, 2, 3, 4, 5, 1, 2,
                                       5, 6, 2, 3, 4, 5, 6, 2, 3)));

    QVERIFY(Pii::equals(Pii::extend(mat, 2, Pii::ExtendReplicate),
                        (PiiMatrix<int>(9, 9,
                                        1, 1, 1, 2, 3, 4, 5, 5, 5,
                                        1, 1, 1, 2, 3, 4, 5, 5, 5,
                                        1, 1, 1, 2, 3, 4, 5, 5, 5,
                                        2, 2, 2, 3, 4, 5, 6, 6, 6,
                                        3, 3, 3, 4, 5, 6, 7, 7, 7,
                                        4, 4, 4, 5, 6, 7, 8, 8, 8,
                                        5, 5, 5, 6, 7, 8, 9, 9, 9,
                                        5, 5, 5, 6, 7, 8, 9, 9, 9,
                                        5, 5, 5, 6, 7, 8, 9, 9, 9))));
  }

  {
    PiiMatrix<int> mat(2, 2,
                       1, 2,
                       2, 3);

    QVERIFY(Pii::equals(Pii::extend(mat, 3, Pii::ExtendSymmetric), 
            PiiMatrix<int>(8, 8,
                           3, 3, 2, 2, 3, 3, 2, 2,
                           3, 3, 2, 2, 3, 3, 2, 2,
                           2, 2, 1, 1, 2, 2, 1, 1,
                           2, 2, 1, 1, 2, 2, 1, 1,
                           3, 3, 2, 2, 3, 3, 2, 2,
                           3, 3, 2, 2, 3, 3, 2, 2,
                           2, 2, 1, 1, 2, 2, 1, 1,
                           2, 2, 1, 1, 2, 2, 1, 1)));
    
    QVERIFY(Pii::equals(Pii::extend(mat, 3, Pii::ExtendPeriodic), 
                        PiiMatrix<int>(8, 8,
                                       3, 2, 3, 2, 3, 2, 3, 2,
                                       2, 1, 2, 1, 2, 1, 2, 1,
                                       3, 2, 3, 2, 3, 2, 3, 2,
                                       2, 1, 2, 1, 2, 1, 2, 1,
                                       3, 2, 3, 2, 3, 2, 3, 2,
                                       2, 1, 2, 1, 2, 1, 2, 1,
                                       3, 2, 3, 2, 3, 2, 3, 2,
                                       2, 1, 2, 1, 2, 1, 2, 1)));
    
    QVERIFY(Pii::equals(Pii::extend(mat, 3, Pii::ExtendReplicate), 
                        PiiMatrix<int>(8, 8,
                                       1, 1, 1, 1, 2, 2, 2, 2,
                                       1, 1, 1, 1, 2, 2, 2, 2,
                                       1, 1, 1, 1, 2, 2, 2, 2,
                                       1, 1, 1, 1, 2, 2, 2, 2,
                                       2, 2, 2, 2, 3, 3, 3, 3,
                                       2, 2, 2, 2, 3, 3, 3, 3,
                                       2, 2, 2, 2, 3, 3, 3, 3,
                                       2, 2, 2, 2, 3, 3, 3, 3)));
  }
}
void TestPiiMatrixUtil::findFirst()
{
  //Basic
  {
    PiiMatrix<int> mat(2,2,
                       1,2,
                       3,4);
    int r,c;
    Pii::findFirst<std::equal_to<int> >(mat,5,r,c);
    QCOMPARE(r, -1);
    QCOMPARE(c, -1);

    Pii::findFirst<std::equal_to<int> >(mat,2,r,c);
    QCOMPARE(r,0);
    QCOMPARE(c,1);
   
  }

}
void TestPiiMatrixUtil::findLast()
{
  //Basic
  {
    PiiMatrix<int> mat(2,2,
                       1,2,
                       3,2);
    int r,c;
    Pii::findLast<std::equal_to<int> >(mat,5,r,c);
    QCOMPARE(r, -1);
    QCOMPARE(c, -1);

    Pii::findLast<std::equal_to<int> >(mat,2,r,c);
    QCOMPARE(r,1);
    QCOMPARE(c,1);
   
  }
}

void TestPiiMatrixUtil::listToMatrix()
{
  //Basic
  {
    QList<double> lst = QList<double>() << 1.0 << 2.0;
    //Default Pii::Vertically
    PiiMatrix<double> mat = Pii::listToMatrix<double>(lst);
    QCOMPARE(mat.rows(),1);
    QCOMPARE(mat.columns(),2);
    QCOMPARE(mat(0,0),1.0);
    QCOMPARE(mat(0,1),2.0);

    lst << 3.0 << 4.0;
    PiiMatrix<double> matSec = Pii::listToMatrix<double>(lst, Pii::Vertically);
    QCOMPARE(matSec.rows(), 4);
    QCOMPARE(matSec.columns(),1);
    QVERIFY(Pii::equals(matSec, PiiMatrix<double>(4,1, 1.0, 2.0, 3.0, 4.0)));
    
  }
}

void TestPiiMatrixUtil::matlabPrint()
{
  PiiMatrix<double> mat(4,4,
                        1.0, 2.3, 5.4, 8.0,
                        2.3, 5.4, 8.0, 1.0,
                        5.4, 8.0, 1.0, 2.3,
                        8.0, 1.0, 2.3, 5.4);
  QString data;
  QTextStream stream(&data);
  Pii::matlabPrint(stream, mat);

  QVERIFY(data ==
          "[ 1 2.3 5.4 8; ...\n"
          "  2.3 5.4 8 1; ...\n"
          "  5.4 8 1 2.3; ...\n"
          "  8 1 2.3 5.4 ];\n");
}

void TestPiiMatrixUtil::matlabParse()
{
  PiiMatrix<double> mat(Pii::matlabParse("1.0 2.3 5.4, 8;...\n2.3, 5.4, 8, 1.0;5.4 8 1.0 2.3\n;\n8,1.0,2.3,5.4"));

  QVERIFY(Pii::equals(mat, PiiMatrix<double>(4,4,
                                             1.0, 2.3, 5.4, 8.0,
                                             2.3, 5.4, 8.0, 1.0,
                                             5.4, 8.0, 1.0, 2.3,
                                             8.0, 1.0, 2.3, 5.4)));

  QVERIFY(Pii::equals(Pii::matlabParse("[]"), PiiMatrix<double>()));
  QVERIFY(Pii::equals(Pii::matlabParse("[ ; ]"), PiiMatrix<double>()));
}

void TestPiiMatrixUtil::printMatrix()
{
  //Basic
  {
    PiiMatrix<double> mat(4,4,
                          1.0, 2.3, 5.4, 8.0,
                          2.3, 5.4, 8.0, 1.0,
                          5.4, 8.0, 1.0, 2.3,
                          8.0, 1.0, 2.3, 5.4);
    QString data;
    QTextStream stream(&data);
    Pii::printMatrix(stream, mat, ", ", "\n"); 
    QVERIFY(data == "1, 2.3, 5.4, 8\n"
                    "2.3, 5.4, 8, 1\n"
                    "5.4, 8, 1, 2.3\n"
                    "8, 1, 2.3, 5.4");

  }  
}

void TestPiiMatrixUtil::replicate()
{
  //Basic
  {
    PiiMatrix<int> mat(1,1,1);
                       
    int vertical = 2;
    int horizontal = 3;
    PiiMatrix<int> mat2(Pii::replicate(mat,vertical,horizontal));
    QCOMPARE(mat2.rows(), vertical*mat.rows());
    QCOMPARE(mat2.columns(), horizontal*mat.columns());
  }

  //More basic
  {
    PiiMatrix<int> mat(2,2,
                       1,0,
                       0,1);
    int vertical = 2;
    int horizontal = 3;
    PiiMatrix<int> mat2(Pii::replicate(mat,vertical,horizontal));
    QCOMPARE(mat2.rows(), vertical*mat.rows());
    QCOMPARE(mat2.columns(), horizontal*mat.columns());

    for(int r = 0; r < mat2.rows(); ++r)
      {
        for(int c = 0; c < mat2.columns(); ++c)
          {
            if(!(r % 2)) //even
              {
                if(!(c % 2))
                  QCOMPARE(mat2(r,c),1);
                else
                  QCOMPARE(mat2(r,c),0);
              }
            else //odd
              {
                if(!(c % 2))
                  QCOMPARE(mat2(r,c),0);
                else
                  QCOMPARE(mat2(r,c),1);
              }
          }
      } 
  }
}

void TestPiiMatrixUtil::rowToList()
{
  //Basic 
  {
    PiiMatrix<int> mat(2,2, 1,2, 3,4);
    QList<int> lst = Pii::rowToList<QList<int> >(mat, 0);
    for(int i = 0; i < lst.size(); ++i)
      QCOMPARE(mat(0,i), lst[i]);
    
    
    lst = Pii::rowToList<QList<int> > (mat,1);
    for(int i = 0; i < lst.size(); ++i)
      QCOMPARE(mat(1,i),lst[i]);
    
    PiiMatrix<char> matChar(2,3, 'a','p','i',
                                 'n','a','t');
    
    QList<char> lstChar = Pii::rowToList<QList<char> > (matChar,1);
    for(int i = 0; i < lst.size(); ++i)
      QCOMPARE(matChar(1,i), lstChar[i]);
  }
}

void TestPiiMatrixUtil::sortRows()
{
  {
    PiiMatrix<int> mat(3,3,
                       -1,2,3,
                       -2,1,3,
                       3,0,3);
    Pii::sortRows(mat, std::greater<int>());
    QVERIFY(Pii::equals(mat,
                        PiiMatrix<int>(3,3,
                                       3,0,3,
                                       -1,2,3,
                                       -2,1,3)));
    Pii::sortRows(mat);
    QVERIFY(Pii::equals(mat,
                        PiiMatrix<int>(3,3,
                                       -2,1,3,
                                       -1,2,3,
                                       3,0,3)));
    Pii::sortRows(mat);
    QVERIFY(Pii::equals(mat,
                        PiiMatrix<int>(3,3,
                                       -2,1,3,
                                       -1,2,3,
                                       3,0,3))); 
    Pii::sortRows(mat,1);
    QVERIFY(Pii::equals(mat,
                        PiiMatrix<int>(3,3,
                                       3,0,3,
                                       -2,1,3,
                                       -1,2,3)));
  }
  {
    const PiiMatrix<int> mat(3,3,
                             -1,2,3,
                             -2,1,3,
                             3,0,3);
    QVERIFY(Pii::equals(Pii::sortedRows(mat, 1),
                        PiiMatrix<int>(3,3,
                                       3,0,3,
                                       -2,1,3,
                                       -1,2,3)));
  }
  {
    QVERIFY(Pii::equals(Pii::sortedRows(PiiMatrix<float>()), PiiMatrix<float>()));
  }
  {
    QVERIFY(Pii::equals(Pii::sortedRows(PiiMatrix<int>(7,1, 7, 6, 5, 4, 3, 2, 1)),
                        PiiMatrix<int>(7,1, 1, 2, 3, 4, 5, 6, 7)));
  }
}

void TestPiiMatrixUtil::valueAt()
{
  // Basic
  {
    PiiMatrix<int> mat(3,3,
                       1,1,2,
                       2,1,3,
                       2,1,4);
    QCOMPARE(Pii::valueAt(mat,0.5,0.5), 1.25f);
    QCOMPARE(Pii::valueAt(mat,1.0,0.5), 1.5f);
    QCOMPARE(Pii::valueAt(mat,1.0,1.5), 2.0f);
    QCOMPARE(Pii::valueAt(mat,1.5,2.0), 3.5f); 
  }
  {
    PiiMatrix<int> mat(3,3,
                       1,1,2,
                       2,1,3,
                       2,1,4);

    // Here we just check how corners are handled (do there happen any
    // segmentation fail)

    QVERIFY(Pii::valueAt(mat, 2.0, 2.0));
    QVERIFY(Pii::valueAt(mat, 0.0, 0.0));
    QVERIFY(Pii::valueAt(mat, 0.0, 2.0));
    QVERIFY(Pii::valueAt(mat, 2.0, 0.0));
  }
}
 
void TestPiiMatrixUtil::flip()
{
  {
    PiiMatrix<int> mat(4,2,
                       1,2,
                       2,3,
                       3,4,
                       4,5);
    PiiMatrix<int> mat2(4,2,
                        4,5,
                        3,4,
                        2,3,
                        1,2);

    QVERIFY(Pii::equals(Pii::flipped(mat, Pii::Vertically), mat2));
    Pii::flip(mat2, Pii::Vertically);
    QVERIFY(Pii::equals(mat2, mat));
  }
  {
    PiiMatrix<int> mat(2,4,
                       1,2,3,4,
                       4,3,2,-1);
    PiiMatrix<int> mat2(2,4,
                        4,3,2,1,
                        -1,2,3,4);

    QVERIFY(Pii::equals(Pii::flipped(mat, Pii::Horizontally), mat2));
    Pii::flip(mat2, Pii::Horizontally);
    QVERIFY(Pii::equals(mat2, mat));
  }
  {
    QVERIFY(Pii::equals(Pii::flipped(PiiMatrix<int>(2,4,
                                                    1,2,3,4,
                                                    4,3,2,-1),
                                     Pii::Horizontally | Pii::Vertically),
                        PiiMatrix<int>(2,4,
                                       -1,2,3,4,
                                       4,3,2,1)));
    QVERIFY(Pii::equals(Pii::flipped(PiiMatrix<int>(1,4, 1,2,3,4),
                                     Pii::Horizontally | Pii::Vertically),
                        PiiMatrix<int>(1,4, 4,3,2,1)));
    QVERIFY(Pii::equals(Pii::flipped(PiiMatrix<int>(4,1, 1,2,3,4),
                                     Pii::Horizontally | Pii::Vertically),
                        PiiMatrix<int>(4,1, 4,3,2,1)));
    PiiMatrix<int> mat(1,4, 1,2,3,4);
    Pii::flip(mat, Pii::Horizontally | Pii::Vertically);
    QVERIFY(Pii::equals(mat, PiiMatrix<int>(1,4, 4,3,2,1)));
    PiiMatrix<int> mat2(4,1, 1,2,3,4);
    Pii::flip(mat2, Pii::Horizontally | Pii::Vertically);
    QVERIFY(Pii::equals(mat2, PiiMatrix<int>(4,1, 4,3,2,1)));
  }
}

QTEST_MAIN(TestPiiMatrixUtil)
