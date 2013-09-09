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

#include "TestPiiIterators.h"

#include <PiiIterator.h>
#include <PiiAlgorithm.h>
#include <QtTest>

void TestPiiIterators::unaryFunctionIterator()
{
  int values[] = { -2, -1, 0, 1, 2 };
  Pii::copy(Pii::unaryFunctionIterator(values, std::bind2nd(std::plus<int>(), 2)),
            Pii::unaryFunctionIterator(values + 5, std::bind2nd(std::plus<int>(), 2)),
            values);
  for (int i=0; i<5; ++i)
    QCOMPARE(values[i], i);

  bool result[5];
  Pii::copyN(Pii::unaryFunctionIterator(values, std::bind2nd(std::greater<int>(), 2)), 5, result);
  QCOMPARE(result[0], false);
  QCOMPARE(result[1], false);
  QCOMPARE(result[2], false);
  QCOMPARE(result[3], true);
  QCOMPARE(result[4], true);
}

void TestPiiIterators::binaryFunctionIterator()
{
  int values1[] = { -2, -1, 0, 1, 2 };
  int values2[] = { -2, -1, 0, 1, 2 };

  Pii::copyN(Pii::binaryFunctionIterator(values1, values2, std::minus<int>()), 5, values1);
  for (int i=0; i<5; ++i)
    QCOMPARE(values1[i], 0);

  bool result[5];
  Pii::copyN(Pii::binaryFunctionIterator(values1, values2, std::greater_equal<int>()), 5, result);
  QCOMPARE(result[0], true);
  QCOMPARE(result[1], true);
  QCOMPARE(result[2], true);
  QCOMPARE(result[3], false);
  QCOMPARE(result[4], false);
}

void TestPiiIterators::filteredIterator()
{
  int values[] = { -2, -1, 0, 1, 2 };
  bool filter[] = { 1, 0, 1, 0, 1 };
  int result[4];
  result[3] = 314159265;
  Pii::copy(Pii::filteredIterator(values, values + 5, filter),
            Pii::filteredIterator(values, values + 5, values + 5, filter + 5),
            result);
  QCOMPARE(result[0], -2);
  QCOMPARE(result[1], 0);
  QCOMPARE(result[2], 2);
  QCOMPARE(result[3], 314159265);
}

QTEST_MAIN(TestPiiIterators)
