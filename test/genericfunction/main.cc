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

#include "TestPiiGenericFunction.h"

#include <QtTest>

#include <PiiGenericFunction.h>

void TestPiiGenericFunction::noArguments()
{
  int iResult = -1;
  void* args[] = { &iResult };
  PiiGenericFunction* pFunc = Pii::genericFunction(this, &TestPiiGenericFunction::test0);
  pFunc->call(args);
  delete pFunc;
  QCOMPARE(iResult, 0);
}

void TestPiiGenericFunction::oneArgument()
{
  int iResult = -1, iArg = 5;
  void* args[] = { &iResult, &iArg };
  PiiGenericFunction* pFunc = Pii::genericFunction(this, &TestPiiGenericFunction::test1);
  pFunc->call(args);
  delete pFunc;
  QCOMPARE(iResult, 1);
  QCOMPARE(_iArg, 5);
}


void TestPiiGenericFunction::threeArguments()
{
  char cResult = 0;
  double dArg = -1.41;
  unsigned int uiArg = 666;
  QString strArg("abc");
  void* args[] = { &cResult, &dArg, &uiArg, &strArg };
  PiiGenericFunction* pFunc = Pii::genericFunction(this, &TestPiiGenericFunction::test3);
  pFunc->call(args);
  delete pFunc;
  QCOMPARE(cResult, 'a');
  QCOMPARE(_strArg, QString("abc"));
  QCOMPARE(_dArg, -1.41);
  QCOMPARE(_uiArg, 666u);
}

void TestPiiGenericFunction::voidFunc()
{
  void* args[] = {};
  _bVoidCalled = false;
  PiiGenericFunction* pFunc = Pii::genericFunction(this, &TestPiiGenericFunction::testVoid);
  pFunc->call(args);
  delete pFunc;
  QVERIFY(_bVoidCalled);
}

void TestPiiGenericFunction::constFunc()
{
  int a = 1, b = 2, c = 0;
  void* args[] = { &c, &a, &b };
  PiiGenericFunction* pFunc = Pii::genericFunction(this, &TestPiiGenericFunction::plus);
  pFunc->call(args);
  delete pFunc;
  QCOMPARE(c,3);
}

QTEST_MAIN(TestPiiGenericFunction)
