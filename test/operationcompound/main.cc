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

#include "TestPiiOperationCompound.h"

#include <PiiGenericTextOutputArchive.h>
#include <PiiGenericTextInputArchive.h>

#include <QtTest>

#include "TestOperation.h"

void TestPiiOperationCompound::initTestCase()
{
  _compound.createInputProxy("input");
  _compound.createOutputProxy("output");
  _compound.inputProxy("input")->output()->connectInput(_compound.outputProxy("output")->input());

  setOperation(&_compound);
}

void TestPiiOperationCompound::serialize()
{
  QByteArray array;
  QBuffer buffer(&array);

  try
    {
      buffer.open(QIODevice::ReadWrite);
      PiiGenericTextOutputArchive oa(&buffer);
      oa << &_compound;
    }
  catch (PiiSerializationException& ex)
    {
      QFAIL(("Serialization error: " + ex.message() + ". Additional info: " + ex.info()).toUtf8().constData());
    }

  //qDebug() << array;

  try
    {
      buffer.seek(0);
      PiiGenericTextInputArchive ia(&buffer);
      PiiOperationCompound* pCompound;
      ia >> pCompound;
      QCOMPARE(pCompound->outputProxy("output")->input()->connectedOutput(),
               pCompound->inputProxy("input")->output());
      delete pCompound;
    }
  catch (PiiSerializationException& ex)
    {
      QFAIL(("Serialization error: " + ex.message() + ". Additional info: " + ex.info()).toUtf8().constData());
    }
}

void TestPiiOperationCompound::clone()
{
  PiiOperationCompound* pCompound = _compound.clone();
  QVERIFY(pCompound->input("input") != 0);
  QVERIFY(pCompound->output("output") != 0);
  QCOMPARE(pCompound->outputProxy("output")->input()->connectedOutput(),
           pCompound->inputProxy("input")->output());
  delete pCompound;
}

void TestPiiOperationCompound::socketData()
{
  PiiOperationCompound* pCompound = _compound.clone();
  TestOperation* pTest = new TestOperation;
  pTest->connectOutput("output", pCompound, "input");
  QCOMPARE(pTest->socketData(pTest->output("output"), 1).toInt(), 1);
  QCOMPARE(pCompound->socketData(pCompound->output("output"), 1).toInt(), 1);
  delete pCompound;
  delete pTest;
}

void TestPiiOperationCompound::fullName()
{
  PiiOperationCompound* pCompound1 = _compound.clone();
  PiiOperationCompound* pCompound2 = _compound.clone();
  pCompound1->setObjectName("c1");
  pCompound2->setObjectName("c2");
  TestOperation* pTest = new TestOperation;
  pTest->setObjectName("test");
  pCompound2->addOperation(pTest);
  pCompound1->addOperation(pCompound2);

  QCOMPARE(pTest->fullName(), QString("c2.test"));
  QCOMPARE(pTest->output("output")->fullName(), QString("c2.test.output"));
  QCOMPARE(pTest->input("input")->fullName(), QString("c2.test.input"));
  delete pCompound1;
}

void TestPiiOperationCompound::disabledOperations()
{
  PiiOperationCompound* pCompound = new PiiOperationCompound;
  setOperation(pCompound);
  /*    ,- b
   * a -      ,- d
   *    `- c -
   *          `- e - f
   */

  TestOperation* a = new TestOperation;
  TestOperation* b = new TestOperation;
  TestOperation* c = new TestOperation;
  TestOperation* d = new TestOperation;
  TestOperation* e = new TestOperation;
  TestOperation* f = new TestOperation;
  pCompound->addOperation(a);
  pCompound->addOperation(b);
  pCompound->addOperation(c);
  pCompound->addOperation(d);
  pCompound->addOperation(e);
  pCompound->addOperation(f);

  a->connectOutput("output", b, "input");
  a->connectOutput("output", c, "input");
  c->connectOutput("output", d, "input");
  c->connectOutput("output", e, "input");
  e->connectOutput("output", f, "input");

  pCompound->exposeInput(a->input("input"));

  QVERIFY(start());
  QVERIFY(stop());

  QCOMPARE(a->activityMode(), PiiOperation::Enabled);
  QCOMPARE(b->activityMode(), PiiOperation::Enabled);
  QCOMPARE(c->activityMode(), PiiOperation::Enabled);
  QCOMPARE(d->activityMode(), PiiOperation::Enabled);
  QCOMPARE(e->activityMode(), PiiOperation::Enabled);
  QCOMPARE(f->activityMode(), PiiOperation::Enabled);

  c->setActivityMode(PiiOperation::Disabled);
  QCOMPARE(c->activityMode(), PiiOperation::Disabled);
  try
    {
      pCompound->check(true);
    }
  catch (PiiException& ex)
    {
      QFAIL(qPrintable(ex.message()));
    }

  QCOMPARE(a->activityMode(), PiiOperation::Enabled);
  QCOMPARE(b->activityMode(), PiiOperation::Enabled);
  QCOMPARE(c->activityMode(), PiiOperation::Disabled);
  QCOMPARE(d->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(e->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(f->activityMode(), PiiOperation::TemporarilyDisabled);

  QVERIFY(start());
  QCOMPARE(pCompound->state(), PiiOperation::Running);
  QVERIFY(stop());
  QCOMPARE(pCompound->state(), PiiOperation::Stopped);

  // Move c, d and e to a nested compound
  PiiOperationCompound* pCompound2 = new PiiOperationCompound;
  pCompound2->addOperation(c);
  pCompound2->addOperation(d);
  pCompound2->addOperation(e);

  c->connectOutput("output", d, "input");
  c->connectOutput("output", e, "input");
  pCompound2->createInputProxy("input");
  pCompound2->createOutputProxy("output");
  pCompound2->inputProxy("input")->output()->connectInput(c->input("input"));
  e->connectOutput("output", pCompound2->outputProxy("output")->input());

  pCompound->addOperation(pCompound2);
  a->connectOutput("output", pCompound2, "input");
  pCompound2->connectOutput("output", f, "input");

  c->setActivityMode(PiiOperation::Enabled);
  e->setActivityMode(PiiOperation::Disabled);

  try
    {
      pCompound->check(true);
    }
  catch (PiiException& ex)
    {
      QFAIL(qPrintable(ex.message()));
    }

  QCOMPARE(a->activityMode(), PiiOperation::Enabled);
  QCOMPARE(b->activityMode(), PiiOperation::Enabled);
  QCOMPARE(c->activityMode(), PiiOperation::Enabled);
  QCOMPARE(d->activityMode(), PiiOperation::Enabled);
  QCOMPARE(e->activityMode(), PiiOperation::Disabled);
  QCOMPARE(f->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(pCompound2->activityMode(), PiiOperation::Enabled);

  QVERIFY(start());
  QCOMPARE(pCompound->state(), PiiOperation::Running);
  QVERIFY(stop());
  QCOMPARE(pCompound->state(), PiiOperation::Stopped);

  e->setActivityMode(PiiOperation::Enabled);
  a->bFail = true;
  QVERIFY(start(IgnoreFail));
  QVERIFY(stop());

  QCOMPARE(a->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(b->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(c->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(d->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(e->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(f->activityMode(), PiiOperation::TemporarilyDisabled);
  QCOMPARE(pCompound2->activityMode(), PiiOperation::TemporarilyDisabled);

  pCompound2->setActivityMode(PiiOperation::Disabled);
  QCOMPARE(c->activityMode(), PiiOperation::Disabled);
  QCOMPARE(d->activityMode(), PiiOperation::Disabled);
  QCOMPARE(e->activityMode(), PiiOperation::Disabled);
}

void TestPiiOperationCompound::cleanupTestCase()
{
  setOperation(0);
}

QTEST_MAIN(TestPiiOperationCompound)
