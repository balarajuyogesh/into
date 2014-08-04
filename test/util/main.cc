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

#include "TestPiiUtil.h"

#include <PiiUtil.h>
#include <string>
#include <QtTest>

void TestPiiUtil::decodeProperties()
{
  QVariantMap map = Pii::decodeProperties("value1 = 1\n"
                                          "value2 = \"123\"\n"
                                          "value3=abcdef \n"
                                          "value4=\"abc 123 \"");

  QCOMPARE(map["value1"].toInt(), 1);
  QCOMPARE(map["value2"].toInt(), 123);
  QCOMPARE(map["value3"].toString(), QString("abcdef"));
  QCOMPARE(map["value4"].toString(), QString("abc 123 "));

  map = Pii::decodeProperties("value1\\==1;value2 \\\\= 123;value3=ab\\cdef ; value4=abc\\\\ 123",
                              ';','=','\\', Pii::TrimPropertyName);

  QCOMPARE(map["value1="].toInt(), 1);
  QCOMPARE(map["value2 \\"].toInt(), 123);
  QCOMPARE(map["value3"].toString(), QString("abcdef "));
  QCOMPARE(map["value4"].toString(), QString("abc\\ 123"));
}

struct ArrayCopyTestClass
{
  ArrayCopyTestClass& operator=(const ArrayCopyTestClass& a) {val = a.val+1; return *this;}
  int val;
};

void TestPiiUtil::arrayCopy()
{
  {
    const std::size_t size = 5;
    const int arrayFrom[size] = {1,2,3,4,5};
    int arrayTo[size];
    QVERIFY( std::memcmp(arrayFrom, arrayTo, sizeof(arrayFrom)) != 0 );
    Pii::arrayCopy(arrayTo, arrayFrom, size);
    QVERIFY( std::memcmp(arrayFrom, arrayTo, sizeof(arrayFrom)) == 0 );
  }

  {
    const ArrayCopyTestClass lstFrom[3] = {{1},{1},{1}};
    ArrayCopyTestClass lstTo[3] = {{3},{3},{3}};
    QVERIFY( std::memcmp(lstFrom, lstTo, sizeof(lstFrom)) != 0 );
    Pii::arrayCopy(lstTo, lstFrom, 3);
    QVERIFY( std::memcmp(lstFrom, lstTo, sizeof(lstFrom)) != 0 );
    QVERIFY( lstFrom[0].val + 1 == lstTo[0].val );
    QVERIFY( lstFrom[1].val + 1 == lstTo[1].val );
    QVERIFY( lstFrom[2].val + 1 == lstTo[2].val );
  }
}

void TestPiiUtil::findNeighbors()
{
  QLinkedList<QPair<int,int> > lstPairs;
  lstPairs << qMakePair(0,1) << qMakePair(0,3) << qMakePair(0,4) << qMakePair(3,4) << qMakePair(3,5);
  lstPairs << qMakePair(2,6) << qMakePair(6,8) << qMakePair(7,8);

  QList<QList<int> > lstNeighbors(Pii::findNeighbors(lstPairs));
  QCOMPARE(lstNeighbors.size(), 2);
  QCOMPARE(lstNeighbors[0], QList<int>() << 0 << 1 << 3 << 4 << 5);
  QCOMPARE(lstNeighbors[1], QList<int>() << 2 << 6 << 7 << 8);
}

void TestPiiUtil::findDependencies()
{
  {
    QLinkedList<QPair<int,int> > lstPairs;
    lstPairs << qMakePair(0,1) << qMakePair(0,3) << qMakePair(0,4) << qMakePair(3,4) << qMakePair(3,5)
             << qMakePair(2,6) << qMakePair(6,8) << qMakePair(7,8);
    QList<QList<int> > lstDeps(Pii::findDependencies(lstPairs, Pii::AnyValidOrder));
    QCOMPARE(lstDeps.size(), 1);
    QCOMPARE(lstDeps[0], QList<int>() << 0 << 1 << 3 << 4 << 5 << 2 << 6 << 7 << 8);
  }
  {
    QLinkedList<QPair<int,int> > lstPairs;
    lstPairs << qMakePair(0,1) << qMakePair(0,3) << qMakePair(0,4) << qMakePair(3,4) << qMakePair(3,5)
             << qMakePair(2,6) << qMakePair(6,8) << qMakePair(7,8);
    QList<QList<int> > lstDeps(Pii::findDependencies(lstPairs, Pii::SortedLayeredOrder));
    QCOMPARE(lstDeps.size(), 3);
    QCOMPARE(lstDeps[0], QList<int>() << 0 << 2 << 7);
    QCOMPARE(lstDeps[1], QList<int>() << 1 << 3 << 6);
    QCOMPARE(lstDeps[2], QList<int>() << 4 << 5 << 8);
  }
  {
    QLinkedList<QPair<int,int> > lstPairs;
    lstPairs << qMakePair(0,1) << qMakePair(1,2) << qMakePair(2,0) // loop
             << qMakePair(3,4) << qMakePair(4,6) << qMakePair(6,5) << qMakePair(5,7)
             << qMakePair(3,5) << qMakePair(3,6) << qMakePair(3,7)
             << qMakePair(8,8); // loop
    QList<QList<int> > lstDeps(Pii::findDependencies(lstPairs, Pii::AnyValidOrder));
    QCOMPARE(lstDeps.size(), 1);
    QCOMPARE(lstDeps[0], QList<int>() << 3 << 4 << 6 << 5 << 7);
    QCOMPARE(lstPairs,
             (QLinkedList<QPair<int,int> >()
              << qMakePair(0,1) << qMakePair(1,2) << qMakePair(2,0) << qMakePair(8,8)));
  }
  {
    QLinkedList<QPair<int,int> > lstPairs;
    lstPairs << qMakePair(0,1) << qMakePair(1,2) << qMakePair(2,0) // loop
             << qMakePair(3,4) << qMakePair(4,6) << qMakePair(6,5) << qMakePair(5,7)
             << qMakePair(3,5) << qMakePair(3,6) << qMakePair(3,7)
             << qMakePair(8,8); // loop
    QList<QList<int> > lstDeps(Pii::findDependencies(lstPairs, Pii::AnyLayeredOrder));
    QCOMPARE(lstDeps.size(), 5);
    QCOMPARE(lstDeps[0], QList<int>() << 3);
    QCOMPARE(lstDeps[1], QList<int>() << 4);
    QCOMPARE(lstDeps[2], QList<int>() << 6);
    QCOMPARE(lstDeps[3], QList<int>() << 5);
    QCOMPARE(lstDeps[4], QList<int>() << 7);
    QCOMPARE(lstPairs,
             (QLinkedList<QPair<int,int> >()
              << qMakePair(0,1) << qMakePair(1,2) << qMakePair(2,0) << qMakePair(8,8)));
  }
  {
    QLinkedList<QPair<int,int> > lstPairs;
    lstPairs << qMakePair(6,7) << qMakePair(6,8);
    QList<QList<int> > lstDeps(Pii::findDependencies(lstPairs, Pii::AnyLayeredOrder));
    QCOMPARE(lstDeps.size(), 2);
    QCOMPARE(lstDeps[0], QList<int>() << 6);
    QCOMPARE(lstDeps[1].size(), 2);
    QVERIFY(lstDeps[1].contains(7));
    QVERIFY(lstDeps[1].contains(8));
  }
}

void TestPiiUtil::splitQuoted()
{
  QCOMPARE(Pii::splitQuoted("\"a,b,c\",d,e"), QStringList() << "a,b,c" << "d" << "e");
  QCOMPARE(Pii::splitQuoted(" \"a\", b , \"c"), QStringList() << "a" << " b " << "\"c");
  QCOMPARE(Pii::splitQuoted("1; ' 2 ' ;; '3'", ';', '\''), QStringList() << "1" << " 2 " << "" << "3");
  QCOMPARE(Pii::splitQuoted("/test/string;1", ';'), QStringList() << "/test/string" << "1");
}

QTEST_MAIN(TestPiiUtil)
