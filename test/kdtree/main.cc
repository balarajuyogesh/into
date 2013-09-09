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
#include "TestPiiKdTree.h"

#include <QtTest>

void TestPiiKdTree::initTestCase()
{
  PiiMatrix<int> matTrainingData(8, 2,
                                 -3, -1,
                                 -1, -2,
                                 -2, 1,
                                 -1, 2,
                                 1, -1,
                                 3, -2,
                                 2, 1,
                                 2, 3);
  
  _pTree = new PiiKdTree<PiiMatrix<int> >(matTrainingData);
  //_pTree->print(std::cout);
}

void TestPiiKdTree::findClosestMatch()
{
  PiiMatrix<int> matTestingData(3, 2,
                                0, 2,
                                7, -1,
                                -1, -1);

  QCOMPARE(_pTree->findClosestMatch(matTestingData[0]), 3);
  QCOMPARE(_pTree->findClosestMatch(matTestingData[1]), 5);
  QCOMPARE(_pTree->findClosestMatch(matTestingData[2]), 1);

  QCOMPARE(_pTree->findClosestMatch(matTestingData[0], 8), 3);
  QCOMPARE(_pTree->findClosestMatch(matTestingData[1], 8), 5);
  QCOMPARE(_pTree->findClosestMatch(matTestingData[2], 8), 1);
}

void TestPiiKdTree::findClosestMatches()
{
  PiiMatrix<int> matTestingData(3, 2,
                                0, -2,
                                -1, 5,
                                1, 1);
  int aMatches[3][3] = { { 1, 4, 5 },
                         { 3, 7, 2 },
                         { 6, 4, 3 } };

  for (int i=0; i<matTestingData.rows(); ++i)
    {
      PiiClassification::MatchList lstMatches = _pTree->findClosestMatches(matTestingData[i], 3);
      QCOMPARE(lstMatches.size(), 3);
      for (int j=0; j<3; ++j)
        QCOMPARE(lstMatches[j].second, aMatches[i][j]);
      
      lstMatches = _pTree->findClosestMatches(matTestingData[i], 3, 8);
      QCOMPARE(lstMatches.size(), 3);
      for (int j=0; j<3; ++j)
        QCOMPARE(lstMatches[j].second, aMatches[i][j]);
    }
}

void TestPiiKdTree::cleanupTestCase()
{
  delete _pTree;
}

QTEST_MAIN(TestPiiKdTree)
