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

#include "TestPiiReadWriteLock.h"

#include <QtTest>
#include <PiiAsyncCall.h>
#include <PiiDelay.h>

TestPiiReadWriteLock::TestPiiReadWriteLock() :
  _iCounter(0), _bFailure(false)
{}

void TestPiiReadWriteLock::writer(int count)
{
  for (int i=0; i<count; ++i)
    {
      PiiWriteLocker lock(&_lock);
      ++_iCounter;
      PiiDelay::msleep(1);
    }
}

void TestPiiReadWriteLock::reader()
{
  int iPreviousValue = 0;
  for (int i=0; i<200; ++i)
    {
      PiiReadLocker lock(&_lock);
      if (iPreviousValue > _iCounter)
        _bFailure = true;
      iPreviousValue = _iCounter;
    }
}

void TestPiiReadWriteLock::threaded()
{
  QThread* pWriter1 = Pii::asyncCall(this, &TestPiiReadWriteLock::writer, 100);
  QThread* pWriter2 = Pii::asyncCall(this, &TestPiiReadWriteLock::writer, 200);
  QThread* pReader1 = Pii::asyncCall(this, &TestPiiReadWriteLock::reader);
  QThread* pReader2 = Pii::asyncCall(this, &TestPiiReadWriteLock::reader);
  QThread* pReader3 = Pii::asyncCall(this, &TestPiiReadWriteLock::reader);

  pWriter1->wait();
  pWriter2->wait();
  pReader1->wait();
  pReader2->wait();
  pReader3->wait();

  if (_bFailure)
    QFAIL("Numbers were read in wrong order.");
  QCOMPARE(_iCounter, 300);
}

void TestPiiReadWriteLock::recursive()
{
  PiiReadWriteLock lock(PiiReadWriteLock::Recursive);
  lock.lockForRead();
  lock.lockForWrite();
  lock.lockForRead();
  lock.unlockRead();
  lock.unlockWrite();
  lock.unlockRead();

  lock.lockForWrite();
  lock.lockForWrite();
  lock.lockForRead();
  lock.unlockRead();
  lock.unlockWrite();
  lock.unlockWrite();
}

QTEST_MAIN(TestPiiReadWriteLock)
