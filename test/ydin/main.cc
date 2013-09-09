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

#include "TestPiiYdin.h"

#include <PiiYdin.h>
#include <PiiPlugin.h>
#include <QtTest>

class Interface1
{
public:
  virtual ~Interface1() {}
  virtual int test1() const = 0;
};

PII_SERIALIZATION_NAME(Interface1);

class Interface2
{
public:
  virtual ~Interface2() {}
  virtual int test2() const = 0;
};

PII_SERIALIZATION_NAME(Interface2);

class Base1
{
public:
  virtual ~Base1() {}

private:
  char padding[5];
};

PII_SERIALIZATION_NAME(Base1);

class Inherited1 : public Base1, public Interface1
{
public:
  int test1() const { return 1; }
};

class Inherited2 : public Inherited1, public Interface2
{
public:
  int test2() const { return 2; }
};

PII_IMPLEMENT_PLUGIN(TestPlugin);

PII_REGISTER_CLASS(Inherited1, Base1);
PII_REGISTER_CLASS(Inherited2, Inherited1);

PII_BEGIN_STATEMENTS(TestPlugin)
  PII_REGISTER_SUPERCLASS(Inherited1, Interface1)
  PII_REGISTER_SUPERCLASS(Inherited2, Interface2)
PII_END_STATEMENTS

void TestPiiYdin::initTestCase()
{
  //PiiYdin::resourceDatabase()->dump();
}

void TestPiiYdin::createResource()
{
  {
    Base1* pObj = PiiYdin::createResource<Base1>("Inherited2");
    QVERIFY(pObj != 0);
    //qDebug("%p", pObj);
    delete pObj;
  }
  {
    Interface1* pObj = PiiYdin::createResource<Interface1>("Inherited2");
    QVERIFY(pObj != 0);
    //qDebug("%p", pObj);
    QCOMPARE(pObj->test1(), 1);
    delete pObj;
  }
  {
    Interface2* pObj = PiiYdin::createResource<Interface2>("Inherited2");
    QVERIFY(pObj != 0);
    //qDebug("%p", pObj);
    QCOMPARE(pObj->test2(), 2);
    delete pObj;
  }
  
}

QTEST_MAIN(TestPiiYdin)
