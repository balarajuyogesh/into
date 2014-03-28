/* This file is part of Into.
 * Copyright (C) Intopii 2014.
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

#include "TestPiiFunctionOperation.h"

#include <QtTest>

double sum(double a, int b) { return a + b; }
void sum2(double a, int b, int* c) { *c = a + b; }

void SumTestOperation::checkSockets()
{
  QCOMPARE(socketAt<0>(), outputAt(0));
  QCOMPARE(socketAt<1>(), inputAt(0));
  QCOMPARE(socketAt<2>(), inputAt(1));
  QCOMPARE(socketAt<0>(), output("sum"));
  QCOMPARE(socketAt<1>(), input("a"));
  QCOMPARE(socketAt<2>(), input("b"));
}

void Sum2TestOperation::checkSockets()
{
  QCOMPARE(socketAt<0>(), inputAt(0));
  QCOMPARE(socketAt<1>(), inputAt(1));
  QCOMPARE(socketAt<2>(), outputAt(0));
  QCOMPARE(socketAt<0>(), input("a"));
  QCOMPARE(socketAt<1>(), input("b"));
  QCOMPARE(socketAt<2>(), output("sum"));
}

void TestPiiFunctionOperation::initTestCase()
{
#ifndef PII_CXX11
  QSKIP("C++11 support required.");
#endif
}

#ifdef PII_CXX11

void TestPiiFunctionOperation::cleanup()
{
  stop();
  PiiOperationTest::cleanup();
  delete operation();
  setOperation(0);
}

void TestPiiFunctionOperation::sumOperation()
{
  setOperation(new SumTestOperation);

  QVERIFY(start(ExpectFail));

  QVERIFY(connectInput("a"));
  QVERIFY(connectInput("b"));

  QVERIFY(start());

  QVERIFY(sendObject("a", 1.0));
  QVERIFY(sendObject("b", 2));
  QCOMPARE(outputValue("sum", 0.0), 3.0);
}

void TestPiiFunctionOperation::sum2Operation()
{
  setOperation(new Sum2TestOperation);

  QVERIFY(start(ExpectFail));

  QVERIFY(connectInput("a"));
  QVERIFY(connectInput("b"));

  QVERIFY(start());

  QVERIFY(sendObject("a", 1.0));
  QVERIFY(sendObject("b", 2));
  QCOMPARE(outputValue("sum", 0), 3);
}

void TestPiiFunctionOperation::setDefaultValue()
{
  SumTestOperation* pOp = new SumTestOperation;
  setOperation(pOp);
  pOp->setDefault();

  QVERIFY(connectInput("a"));
  QVERIFY(start());
  QVERIFY(sendObject("a", 20.0));
  QCOMPARE(outputValue("sum", 0.0), 30.0);

  QVERIFY(stop());
  pOp->unsetDefault();

  QVERIFY(start(ExpectFail));
  QVERIFY(connectInput("b"));

  QVERIFY(start());

  QVERIFY(sendObject("a", 1.0));
  QVERIFY(sendObject("b", 2));
  QCOMPARE(outputValue("sum", 0.0), 3.0);
}

void TestPiiFunctionOperation::socketAt()
{
  SumTestOperation s1;
  Sum2TestOperation s2;
  s1.checkSockets();
  s2.checkSockets();
}

#endif

QTEST_MAIN(TestPiiFunctionOperation)
