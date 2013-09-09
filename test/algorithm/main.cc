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

#include "TestPiiAlgorithm.h"

#include <PiiAlgorithm.h>
#include <PiiMatrix.h>
#include <QtTest>

void TestPiiAlgorithm::swap()
{
  PiiMatrix<int> mat(2,3,
                     1,2,3,
                     4,5,6);
  Pii::swap(mat.rowBegin(0), mat.rowEnd(0), mat.rowBegin(1));
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(2,3,
                                          4,5,6,
                                          1,2,3)));
  
  Pii::swap(mat.columnBegin(0), mat.columnEnd(0), mat.columnBegin(2));
  QVERIFY(Pii::equals(mat, PiiMatrix<int>(2,3,
                                          6,5,4,
                                          3,2,1)));
}

QTEST_MAIN(TestPiiAlgorithm)
