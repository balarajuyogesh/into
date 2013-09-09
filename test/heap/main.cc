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

#include "TestPiiHeap.h"


#include <PiiHeap.h>
#include <QtTest>

void TestPiiHeap::append()
{
  PiiHeap<int,4> heap;
  for (int i=0; i<10; ++i)
    heap.append(i);

  /* The binary tree:
   *
   *       9
   *    8     5
   *  6   7  1  4
   * 0 3 2
   */
  int correct[] = { 9, 8, 5, 6, 7, 1, 4, 0, 3, 2 };
  for (int i=0; i<10; ++i)
    QCOMPARE(heap[i], correct[i]);
}

void TestPiiHeap::replace()
{
  PiiHeap<int> heap;
  for (int i=0; i<10; ++i)
    heap.append(i);

  /*       9
   *    8     5
   *  6   7  1  4
   * 0 3 2
   */
  heap.replace(9, 9);
  /*       9
   *    9     5
   *  6   8  1  4
   * 0 3 7
   */
  heap.replace(0, 1);
  /*       9
   *    8    5
   *  6   7  1  4
   * 0 3 1
   */
  
  int correct[] = { 9, 8, 5, 6, 7, 1, 4, 0, 3, 1 };
  for (int i=0; i<10; ++i)
    QCOMPARE(heap[i], correct[i]);
}

void TestPiiHeap::remove()
{
  PiiHeap<int, 4> heap;
  heap.append(3);
  heap.append(2);
  heap.append(1);
  heap.append(0);
  QCOMPARE(heap[2], 1);
  QCOMPARE(heap.take(2), 1);
  QCOMPARE(heap.size(), 3);
  QCOMPARE(heap[2], 0);
  heap.remove(0);
  QCOMPARE(heap[0], 2);
  QCOMPARE(heap[1], 0);
  QCOMPARE(heap.size(),2);
}

void TestPiiHeap::fill()
{
  PiiHeap<int, 10> heap1;
  PiiHeap<int, 5> heap2;
  heap1.fill(10,0);
  heap2.fill(10,0);
  for (int i=0; i<10; ++i)
    {
      QCOMPARE(heap1[i],0);
      QCOMPARE(heap1[i],0);
    }
}

void TestPiiHeap::sort()
{
  PiiHeap<double, 8> heap;
  for (int i=0; i<8; ++i)
    heap.append(i+1);
  heap.sort();
  QVERIFY(heap.isInverse());
  for (int i=0; i<8; ++i)
    QCOMPARE(heap[i], double(i+1));
  heap.sort();
  QVERIFY(!heap.isInverse());
  for (int i=0; i<8; ++i)
    QCOMPARE(heap[7-i], double(i+1));
}

void TestPiiHeap::sorted()
{
  PiiHeap<double, 8> heap1;
  for (int i=0; i<8; ++i)
    heap1.append(i+1);
  PiiHeap<double, 8> heap2 = heap1.sorted();
  heap1.sort();
  for (int i=0; i<8; ++i)
    QCOMPARE(heap1[i], heap2[i]);
  QCOMPARE(heap1.isInverse(), heap2.isInverse());
}

void TestPiiHeap::put()
{
  PiiHeap<int, 5> heap;
  heap.fill(5, 10);
  for (int i=0; i<20; ++i)
    heap.put(i);
  QCOMPARE(heap.size(), 5);
  heap.sort();
  for (int i=0; i<5; ++i)
    QCOMPARE(heap[i], i);
}

QTEST_MAIN(TestPiiHeap)
