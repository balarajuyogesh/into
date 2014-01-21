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

#include "TestPiiPisoOperation.h"

#include <PiiEngine.h>
#include <QtTest>

void TestPiiPisoOperation::initTestCase()
{
  QVERIFY(createOperation("piiflowcontrol", "PiiPisoOperation"));
  operation()->setProperty("groupSize", 3);
  operation()->setProperty("dynamicInputCount", 5);
}

void TestPiiPisoOperation::groupedMode()
{
  QVERIFY(start(ExpectFail));

  connectAllInputs();

  operation()->setProperty("defaultValues",
                           QVariant::fromValue(PiiVariantList()
                                               << PiiVariant()
                                               << PiiVariant()
                                               << PiiVariant(5)));

  QVERIFY(start());

  QVERIFY(sendObject("input0", 0));
  QVERIFY(!hasOutputValue());
  QVERIFY(sendObject("input3", 3));
  QVERIFY(!hasOutputValue());
  QVERIFY(sendObject("input2", 2));
  QVERIFY(!hasOutputValue());
  // This should finally send something
  QVERIFY(sendObject("group1 input1", 4)); // same as input4
  ensureOutput(1,3,4,5);

  // So should this
  QVERIFY(sendObject("input1", 1));
  ensureOutput(0,0,1,2);

  stop();
}

void TestPiiPisoOperation::ensureOutput(int group, int v0, int v1, int v2)
{
  QVERIFY(hasOutputValue("index"));
  QVERIFY(hasOutputValue("output0"));
  QVERIFY(hasOutputValue("output1"));
  QVERIFY(hasOutputValue("output2"));
  QCOMPARE(outputValue("index", -1), group);
  QCOMPARE(outputValue("output0", -1), v0);
  QCOMPARE(outputValue("output1", -1), v1);
  QCOMPARE(outputValue("output2", -1), v2);
}

void TestPiiPisoOperation::nonGroupedMode()
{
  operation()->setProperty("groupSize", 1);

  disconnectAllInputs();
  QVERIFY(connectInput("input0"));
  QVERIFY(connectInput("input4"));
  QVERIFY(!connectInput("input5"));

  QVERIFY(start());

  QVERIFY(!sendObject("input1", 0));

  QVERIFY(sendObject("input0", 100));
  ensureOutput(0, 100);
  QVERIFY(sendObject("input4", 1000));
  ensureOutput(4, 1000);

  stop();
}

void TestPiiPisoOperation::ensureOutput(int group, int val)
{
  QVERIFY(hasOutputValue("index"));
  QVERIFY(hasOutputValue("output0"));
  QCOMPARE(outputValue("index", -1), group);
  QCOMPARE(outputValue("output0", -1), val);
}

QTEST_MAIN(TestPiiPisoOperation)
