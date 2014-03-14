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

#include "TestPiiProbeInput.h"

#include <QtTest>
#include <PiiYdinTypes.h>
#include <PiiTimer.h>
#include <PiiAsyncCall.h>
#include <PiiDelay.h>

TestPiiProbeInput::TestPiiProbeInput() :
  _pProbe(new PiiProbeInput("")),
  _iCount(0)
{
  connect(_pProbe, SIGNAL(objectReceived(PiiVariant,PiiProbeInput*)),
          SLOT(count(PiiVariant,PiiProbeInput*)), Qt::DirectConnection);
  _pProbe->setParent(this);
}

void TestPiiProbeInput::tryToReceive()
{
  QVERIFY(_pProbe->tryToReceive(0, PiiVariant(1)));
  QCOMPARE(_iCount, 1);
  QCOMPARE(_varSaved.convertTo<int>(), 1);
}

void TestPiiProbeInput::savedObject()
{
  QCOMPARE(_pProbe->savedObject().convertTo<int>(), 1);
}

void TestPiiProbeInput::discardControlObjects()
{
  QVERIFY(_pProbe->tryToReceive(0, PiiYdin::createStartTag()));
  QCOMPARE(_varSaved.type(), uint(PiiYdin::SynchronizationTagType));
  QVERIFY(_pProbe->tryToReceive(0, PiiVariant(2)));
  QCOMPARE(_iCount, 3);
  _pProbe->setDiscardControlObjects(true);
  QVERIFY(_pProbe->tryToReceive(0, PiiYdin::createStartTag()));
  QCOMPARE(_iCount, 3);
  QCOMPARE(_varSaved.type(), uint(PiiVariant::IntType));
}

void TestPiiProbeInput::signalInterval()
{
  _iCount = 0;
  _pProbe->setSignalInterval(100);
  PiiTimer t;
  QThread* pThr1 = Pii::createAsyncCall(this, &TestPiiProbeInput::sendNumbers);
  QThread* pThr2 = Pii::createAsyncCall(this, &TestPiiProbeInput::sendNumbers);
  pThr1->start();
  pThr2->start();
  while (pThr2->isRunning() || pThr1->isRunning())
    QCoreApplication::processEvents();
  int iMs = t.milliseconds();
  delete pThr1;
  delete pThr2;
  QVERIFY(_iCount <= iMs/100);
  QVERIFY(_iCount >= iMs/100-1);
  //qDebug("2.000.000 objects, %d signals, %d ms", _iCount, iMs);
  _iCount = 0;
  // Resets the timer
  _pProbe->setSignalInterval(100);
  // Make sure no pending emission will screw us
  PiiDelay::msleep(110);
  _pProbe->tryToReceive(0, PiiVariant(0));
  _pProbe->tryToReceive(0, PiiVariant(1));
  _pProbe->tryToReceive(0, PiiVariant(2));
  QCOMPARE(_iCount, 1);
  _iCount = 0;
  PiiDelay::msleep(110);
  QCoreApplication::processEvents();
  QCOMPARE(_iCount, 1);
  QCOMPARE(_varSaved.convertTo<int>(), 2);
}

void TestPiiProbeInput::sendNumbers()
{
  for (int i=0; i<1000000; ++i)
    _pProbe->tryToReceive(0, PiiVariant(i));
}

void TestPiiProbeInput::count(const PiiVariant& obj, PiiProbeInput*)
{
  _varSaved = obj;
  ++_iCount;
}

QTEST_MAIN(TestPiiProbeInput)
