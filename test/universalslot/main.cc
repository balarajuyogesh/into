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

#include "TestPiiUniversalSlot.h"

#include <QtTest>

#include <PiiUniversalSlot.h>

class TestSlot : public PiiUniversalSlot
{
public:
  TestSlot() : bVoidCalled(false), iSum(0) {}

  bool bVoidCalled;
  int iSum;
  QString strValue;

protected:
  bool invokeSlot(int id, void** args)
  {
    switch (id)
      {
      case 0: bVoidCalled = true; break;
      case 1:
      case 2: iSum += *reinterpret_cast<int*>(args[1]); break;
      case 3: strValue = *reinterpret_cast<QString*>(args[1]); break;
      default: return false;
      }
    return true;
  }
};

void TestPiiUniversalSlot::invokeSlot()
{
  TestSlot testSlot;
  QCOMPARE(testSlot.dynamicConnect(this, "test()"), 0);
  QCOMPARE(testSlot.dynamicConnect(this, "test(int)"), 1);
  QCOMPARE(testSlot.dynamicConnect(this, SIGNAL(test(int)), SLOT(test(int))), 1);
  QCOMPARE(testSlot.dynamicConnect(this, "test(int)", "test2(int)"), 2);
  QCOMPARE(testSlot.dynamicConnect(this, "test(QString)"), 3);

  emit test();
  QVERIFY(testSlot.bVoidCalled);
  emit test(2);
  QCOMPARE(testSlot.iSum, 3*2);
  emit test("abc");
  QCOMPARE(testSlot.strValue, QString("abc"));

  QVERIFY(testSlot.dynamicDisconnect(this, "test()"));
  testSlot.bVoidCalled = false;
  emit test();
  QVERIFY(!testSlot.bVoidCalled);

  QVERIFY(testSlot.dynamicDisconnect(this, SIGNAL(test(int))));
  testSlot.iSum = 0;
  emit test(1);
  QCOMPARE(testSlot.iSum, 1);
}

QTEST_MAIN(TestPiiUniversalSlot)
