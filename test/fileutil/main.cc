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

#include "TestPiiFileUtil.h"

#include <QtTest>
#include <QFile>
#include <QFileInfo>

#include <PiiFileUtil.h>

void TestPiiFileUtil::createDirectory()
{
  QVERIFY(!QFileInfo(dirName()).exists());
  QVERIFY(Pii::createDirectory(dirName()));
  QVERIFY(QFileInfo(dirName()).exists());
  {
    QString strEmptyFile(dirName() + "emptyfile");
    QFile f(strEmptyFile);
    QVERIFY(f.open(QIODevice::WriteOnly));
    QVERIFY(QFileInfo(strEmptyFile).exists());
  }
  {
    QString strEmptyFile("empty/emptyfile");
    QFile f(strEmptyFile);
    QVERIFY(f.open(QIODevice::WriteOnly));
    QVERIFY(QFileInfo(strEmptyFile).exists());
  }
  {
    QString strEmptyFile("empty/path/emptyfile");
    QFile f(strEmptyFile);
    QVERIFY(f.open(QIODevice::WriteOnly));
    QVERIFY(QFileInfo(strEmptyFile).exists());
  }
}

void TestPiiFileUtil::deleteDirectory()
{
  QVERIFY(QFileInfo(dirName()).exists());
  QVERIFY(Pii::deleteDirectory(dirName()));
  QVERIFY(!QFileInfo(dirName()).exists());
  QVERIFY(Pii::deleteDirectory("empty"));
  QVERIFY(!QFileInfo("empty").exists());
}

QTEST_MAIN(TestPiiFileUtil)
