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

#include "TestPiiThreadSafeTimer.h"

#include <QtTest>

#include <PiiThreadSafeTimer.h>
#include <PiiAsyncCall.h>
#include <PiiDelay.h>

TestPiiThreadSafeTimer::TestPiiThreadSafeTimer() :
  _bStopperRunning(false),
  _iCount(0)
{}

void TestPiiThreadSafeTimer::oneThread()
{
  PiiThreadSafeTimer timer;
  connect(&timer, SIGNAL(timeout()), this, SLOT(count()), Qt::DirectConnection);

  // 10 ms per round, 100 ms -> should send about 10 signals
  timer.start(10);
  QVERIFY(timer.remainingTime() > 0);
  QVERIFY(timer.remainingTime() <= 10);
  PiiDelay::msleep(100);
  QVERIFY(_iCount > 8);
  QVERIFY(_iCount < 12);
  QVERIFY(timer.isActive());
  // A stopped timer should not change value.
  timer.stop();
  QCOMPARE(timer.remainingTime(), -1);
  QVERIFY(!timer.isActive());
  int iCurrentCount = _iCount;
  PiiDelay::msleep(100);
  QCOMPARE(_iCount, iCurrentCount);

  _iCount = 0;
  timer.setSingleShot(true);
  timer.start(10);
  PiiDelay::msleep(50);
  QCOMPARE(_iCount, 1);
  QVERIFY(!timer.isActive());

  _iCount = 0;
  timer.setInterval(1000);
  timer.setSingleShot(false);
  timer.start();
  QVERIFY(timer.isActive());
  // Stop immediately
  timer.stop();
  QVERIFY(!timer.isActive());
  QCOMPARE(_iCount, 0);

  _iCount = 0;
  // Start ...
  timer.start(100);
  PiiDelay::msleep(10);
  QVERIFY(timer.isActive());
  // ... but change interval before timeout.
  timer.setInterval(10);
  timer.setSingleShot(true);
  PiiDelay::msleep(30);
  QCOMPARE(_iCount, 1);
  QVERIFY(!timer.isActive());
}

void TestPiiThreadSafeTimer::twoThreads()
{
  PiiThreadSafeTimer timer;
  connect(&timer, SIGNAL(timeout()), this, SLOT(count()), Qt::DirectConnection);
  _iCount = 0;
  timer.start(100);
  _bStopperRunning = true;
  // This thread always stops the timer before it fires.
  QThread* pThread = Pii::asyncCall(this, &TestPiiThreadSafeTimer::stopTimer, &timer);
  for (int i=0; i<20; ++i)
    {
      timer.start(10);
      PiiDelay::msleep(20);
    }
  _bStopperRunning = false;
  pThread->wait();
  PiiDelay::msleep(20);
  QVERIFY(!timer.isActive());
  QCOMPARE(_iCount, 1);
}

void TestPiiThreadSafeTimer::stopTimer(PiiThreadSafeTimer* timer)
{
  while (_bStopperRunning)
    {
      timer->stop();
      PiiDelay::msleep(5);
    }
  timer->start(10);
  timer->setSingleShot(true);
}

void TestPiiThreadSafeTimer::count()
{
  ++_iCount;
}

QTEST_MAIN(TestPiiThreadSafeTimer)
