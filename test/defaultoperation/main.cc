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

#include "TestPiiDefaultOperation.h"

#include <QtTest>

#include <PiiYdinUtil.h>

CounterOperation::CounterOperation() :
  _iProp1(0),
  _dProp2(2.0),
  _iProp3(0),
  _iProp4(0)
{
  setObjectName("counter");
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output0"));
  addSocket(new PiiOutputSocket("output1"));
}

void CounterOperation::process()
{
  int iValue = readInput().valueAs<int>();
  outputAt(0)->emitObject(iValue);
  outputAt(1)->emitObject(iValue*2);
}

BufferOperation::BufferOperation()
{
  setObjectName("buffer");
  addSocket(new PiiInputSocket("input0"));
  addSocket(new PiiInputSocket("input1"));
}

void BufferOperation::process()
{
  lstData << qMakePair(inputAt(0)->firstObject().valueAs<int>(),
                       inputAt(1)->firstObject().valueAs<int>());
}

void TestPiiDefaultOperation::initTestCase()
{
  try
    {
      PiiEngine::loadPlugins(QStringList() << "piibase");
    }
  catch (PiiException& ex)
    {
      QFAIL(qPrintable(ex.message()));
    }

  PiiOperation* pGenerator = _engine.createOperation("PiiSequenceGenerator", "generator");
  pGenerator->setProperty("sequenceEnd", sequenceLength - 1);
  pGenerator->setProperty("repeatCount", 1);
  pGenerator->setProperty("integerOutput", true);

  _pCounter = new CounterOperation;
  _engine.addOperation(_pCounter);

  _pBuffer = new BufferOperation;
  _engine.addOperation(_pBuffer);

  QVERIFY(_engine.connectOutput("generator.output", "counter.input"));
  QVERIFY(_engine.connectOutput("counter.output0", "buffer.input0"));
  QVERIFY(_engine.connectOutput("counter.output1", "buffer.input1"));
}

void TestPiiDefaultOperation::metaProperty()
{
  QCOMPARE(_pCounter->metaProperty("prop1", "min").toInt(), 0);
  QCOMPARE(_pCounter->metaProperty("prop2", "min").toDouble(), 0.5);
  QCOMPARE(_pCounter->metaProperty("prop2", "max").toDouble(), 6.0);
  QCOMPARE(_pCounter->metaProperty("prop2", "step").toDouble(), 1.5);
  QCOMPARE(_pCounter->metaProperties("prop3")["max"].toInt(), 255);
  QCOMPARE(_pCounter->metaProperty("prop4", "min").toInt(), -10);
  QCOMPARE(_pCounter->metaProperty("prop4", "step").toInt(), 2);
  QCOMPARE(_pCounter->metaProperty("prop4", "max").toInt(), 10);

  _pCounter->setProperty("prop1", -1);
  QCOMPARE(_pCounter->prop1(), 0);

  _pCounter->setProperty("prop1", 1);
  QCOMPARE(_pCounter->prop1(), 1);

  _pCounter->setProperty("prop2", 0);
  QCOMPARE(_pCounter->prop2(), 0.5);

  _pCounter->setProperty("prop2", 1.0);
  QCOMPARE(_pCounter->prop2(), 0.5);

  _pCounter->setProperty("prop2", 1.5);
  QCOMPARE(_pCounter->prop2(), 0.5);

  _pCounter->setProperty("prop2", 2.0000001);
  QCOMPARE(_pCounter->prop2(), 2.0);

  _pCounter->setProperty("prop2", 7);
  QCOMPARE(_pCounter->prop2(), 6.0);

  _pCounter->setProperty("prop3", 256);
  QCOMPARE(_pCounter->prop3(), 255);

  _pCounter->setProperty("prop3", -256);
  QCOMPARE(_pCounter->prop3(), -256);

  _pCounter->setProperty("prop4", -11);
  QCOMPARE(_pCounter->prop4(), -10);

  _pCounter->setProperty("prop4", 11);
  QCOMPARE(_pCounter->prop4(), 10);

  _pCounter->setProperty("prop4", 1);
  QCOMPARE(_pCounter->prop4(), 0);
}

void TestPiiDefaultOperation::process()
{
  QFETCH(int, threadCount);

  _pBuffer->lstData.clear();
  _pCounter->setProperty("threadCount", threadCount);
  try
    {
      _engine.execute();
    }
  catch (PiiException& ex)
    {
      QFAIL(qPrintable(ex.message()));
    }

  QVERIFY(_engine.wait(PiiOperation::Stopped, 500));
  //_engine.wait(PiiOperation::Stopped, 3000);
  //PiiYdin::dumpOperation(&_engine);

  QCOMPARE(_pBuffer->lstData.size(), int(sequenceLength));
  QList<QPair<int,int> > lstData(_pBuffer->lstData);
  for (int i=0; i<sequenceLength; ++i)
    {
      QCOMPARE(lstData[i].first, i);
      QCOMPARE(lstData[i].second, i*2);
    }
}

void TestPiiDefaultOperation::process_data()
{
  QTest::addColumn<int>("threadCount");

  for (int i=0; i<=6; ++i)
    QTest::newRow(qPrintable(QString::number(i))) << i;
}

QTEST_MAIN(TestPiiDefaultOperation)
