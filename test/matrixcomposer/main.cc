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

#include "TestPiiMatrixComposer.h"
#include <QtTest>

void TestPiiMatrixComposer::initTestCase()
{
  QVERIFY(createOperation("piibase", "PiiMatrixComposer"));
}

void TestPiiMatrixComposer::process()
{
  operation()->setProperty("dynamicInputCount", 4);
  operation()->setProperty("defaultValues", QVariantList() << 1.0 << 2.0);

  QVERIFY(start(ExpectFail));

  QVERIFY(connectInput("input1"));
  QVERIFY(connectInput("input3"));

  QVERIFY(start());

  QVERIFY(sendObject("input1", 5));
  QVERIFY(!hasOutputValue());
  QVERIFY(sendObject("input3", 6));

  QVERIFY(Pii::equals(outputValue("output", PiiMatrix<int>()),PiiMatrix<int>(1,4, 1, 5, 0, 6)));
}


QTEST_MAIN(TestPiiMatrixComposer)
