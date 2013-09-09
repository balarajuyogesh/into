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

#include "TestPiiFileSystemScanner.h"

#include <QtTest>

const char* TestPiiFileSystemScanner::aFolders[] =
  {
    "root",
    "root/a",
    "root/a/d",
    "root/b",
    "root/c"
  };

const char* TestPiiFileSystemScanner::aFiles[] =
  {
    "root/test1.txt",
    "root/a/test2.txt",
    "root/a/test3.txt",
    "root/b/test5.txt",
    "root/c/foobar.txt",
    "root/c/test6.txt",
    "root/c/test7.txt",
    "root/c/test8.tex",
    "root/a/d/test4.txt"
  };

void TestPiiFileSystemScanner::initTestCase()
{
  QVERIFY(createOperation("piibase", "PiiFileSystemScanner"));
  operation()->setProperty("threadCount", 0);

  for (unsigned i=0; i<sizeof(aFolders)/sizeof(aFolders[0]); ++i)
    QVERIFY(QDir(".").mkdir(aFolders[i]));

  // "touch" each file
  for (unsigned i=0; i<sizeof(aFiles)/sizeof(aFiles[0]); ++i)
    {
      QFile file(aFiles[i]);
      QVERIFY(file.open(QIODevice::WriteOnly));
      file.close();
    }
}

void TestPiiFileSystemScanner::cleanupTestCase()
{
  for (unsigned i=0; i<sizeof(aFiles)/sizeof(aFiles[0]); ++i)
    QFile(aFiles[i]).remove();
  for (unsigned i=sizeof(aFolders)/sizeof(aFolders[0]); i--; )
    QDir(".").rmdir(aFolders[i]);
}

void TestPiiFileSystemScanner::process()
{
  operation()->setProperty("paths", QStringList() << "root");
  operation()->setProperty("nameFilters", QStringList() << "test*.txt");
  operation()->setProperty("maxDepth", 2);
  operation()->setProperty("sortFlags", "Name");

  QVERIFY(connectInput("trigger"));

  QVERIFY(start());

  QVERIFY(sendObject("trigger", true));
  QCOMPARE(outputValue("filename", QString("")), QString("root/test1.txt"));
  QVERIFY(sendObject("trigger", true));
  QCOMPARE(outputValue("filename", QString("")), QString("root/a/test2.txt"));
  QVERIFY(sendObject("trigger", true));
  QCOMPARE(outputValue("filename", QString("")), QString("root/a/test3.txt"));
  QVERIFY(sendObject("trigger", true));
  QCOMPARE(outputValue("filename", QString("")), QString("root/b/test5.txt"));
  QVERIFY(sendObject("trigger", true));
  QCOMPARE(outputValue("filename", QString("")), QString("root/c/test6.txt"));
  QVERIFY(sendObject("trigger", true));
  QCOMPARE(outputValue("filename", QString("")), QString("root/c/test7.txt"));
  QVERIFY(sendObject("trigger", true));
  QCOMPARE(outputValue("filename", QString("")), QString("root/test1.txt"));

  QVERIFY(stop());

  disconnectInput("trigger");
  QVERIFY(connectInput("path"));

  operation()->setProperty("nameFilters", QStringList());
  operation()->setProperty("maxDepth", 3);
  
  connect(this, SIGNAL(objectReceived(QString,PiiVariant)), this, SLOT(capture(QString,PiiVariant)));

  QVERIFY(start());
  QVERIFY(sendObject("path", QString("root")));

  int iFileCount = sizeof(aFiles)/sizeof(aFiles[0]);
  QCOMPARE(iFileCount, _lstFileNames.size());
  for (int i=0; i<iFileCount; ++i)
    QCOMPARE(QString(aFiles[i]), _lstFileNames[i]);
}

void TestPiiFileSystemScanner::capture(const QString&, const PiiVariant& obj)
{
  if (obj.type() != PiiYdin::QStringType)
    return;
  _lstFileNames << obj.valueAs<QString>();
}


QTEST_MAIN(TestPiiFileSystemScanner)
