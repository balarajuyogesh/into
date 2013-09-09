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

#include "TestPiiDatabaseWriter.h"

#include <QtTest>
#include <PiiDelay.h>

void TestPiiDatabaseWriter::initTestCase()
{
  QVERIFY(createOperation("piidatabase", "PiiDatabaseWriter"));
}

void TestPiiDatabaseWriter::process()
{
  QString strFileName("test.csv");
  QFile file(strFileName);
  QVERIFY(!file.exists() || file.remove());

  operation()->setProperty("columnNames", QStringList() << "test1" << "test2");
  operation()->setProperty("databaseUri", "csv://");
  operation()->setProperty("databaseName", strFileName);

  QVERIFY(start(ExpectFail));

  QVERIFY(connectInput("input0"));
  QVERIFY(connectInput("input1"));

  QVERIFY(start());

  QVERIFY(sendObject("input0", QString("\"abc\"")));
  QVERIFY(sendObject("input1", 123));

  QVERIFY(stop());

  QVERIFY(file.open(QIODevice::ReadOnly));
  QString strCsv(file.readAll());
  file.close();

  QCOMPARE(strCsv, QString("\"\"\"abc\"\"\",\"123\"\n"));
}

QTEST_MAIN(TestPiiDatabaseWriter)
