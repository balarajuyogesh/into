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

#include "TestPiiTimer.h"


#include <QtTest>

#include <PiiTimer.h>
#include <PiiDelay.h>
#include <QTime>

void TestPiiTimer::copy()
{
  PiiTimer tmr;
  PiiTimer tmr2(tmr);
  qint64 t2 = tmr2.microseconds();
  QVERIFY(tmr.microseconds() >= t2);
}

void TestPiiTimer::elapsedTime()
{
  QTime t;
  t.start();
  for (int i=0; i<30; i++)
  {
    PiiTimer tmr1;
    PiiDelay::msleep(5);
    //qDebug("A%i %i %i",i, (int)tmr1.microseconds(),t.restart());
  }

  PiiTimer tmr2;
  for (int i=0; i<30; i++)
  {
    PiiDelay::msleep(10);
    //qDebug("B%i: %i %i", i, (int)tmr2.restart(), t.restart());
  }

  PiiTimer tmr;
  PiiDelay::msleep(50);
   qint64 iUsecs = tmr.microseconds();
   //qDebug("iUsecs == %i", (int)iUsecs);
  QVERIFY(iUsecs > qint64(49000));
  QVERIFY(iUsecs < qint64(60000));
  tmr.stop();
  QCOMPARE(tmr.milliseconds(), qint64(0));
}

QTEST_MAIN(TestPiiTimer)
