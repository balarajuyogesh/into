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

#include "TestPiiStringFormatter.h"
#include <QtTest>

void TestPiiStringFormatter::initTestCase()
{
  QVERIFY(createOperation("piibase", "PiiStringFormatter"));
}

void TestPiiStringFormatter::process()
{
  QVERIFY(start(ExpectFail));

  QCOMPARE(operation()->inputCount(), 1);

  operation()->setProperty("format", "FooBar");
  QCOMPARE(operation()->inputCount(), 1);

  operation()->setProperty("format", "%0 %3 %0");
  QCOMPARE(operation()->inputCount(), 2);

  connectAllInputs();
  QVERIFY(start());

  QVERIFY(sendObject("input0", 1));
  QVERIFY(!hasOutputValue());
  QVERIFY(sendObject("input1", 3.5));

  QCOMPARE(outputValue("output", QString()), QString("1 3.5 1"));

  QVERIFY(sendObject("input0", "abc"));
  QVERIFY(sendObject("input1", true));
  
  QCOMPARE(outputValue("output", QString()), QString("abc true abc"));
}

QTEST_MAIN(TestPiiStringFormatter)
