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

#ifndef _TESTPIIREMOTEOBJECT_H
#define _TESTPIIREMOTEOBJECT_H

#include <QObject>
#include <QThread>

#include <PiiVariant.h>
#include <PiiHttpServer.h>
#include <PiiQObjectServer.h>
#include <PiiRemoteQObject.h>

class ServerObject : public QObject
{
  Q_OBJECT

  Q_PROPERTY(int number READ number WRITE setNumber);
  Q_PROPERTY(double floatingPoint READ floatingPoint WRITE setFloatingPoint);
  Q_PROPERTY(PiiVariant variant READ variant WRITE setVariant);
  Q_PROPERTY(int readOnly READ readOnly);

public:
  ServerObject() :
    iNumber(0),
    dNumber(3.14159),
    bTest1Called(false),
    bFloatingPointCalled(false),
    bSetFloatingPointCalled(false),
    bNumberCalled(false),
    iTest1Value(0),
    pCallingThread(0)
  {}

  int readOnly() const { return -1; }

  void setNumber(int number)
  {
    pCallingThread = QThread::currentThread();
    iNumber = number;
    emit numberChanged(number);
  }
  int number() const
  {
    pCallingThread = QThread::currentThread();
    bNumberCalled = true;
    return iNumber;
  }
  double floatingPoint() const
  {
    pCallingThread = QThread::currentThread();
    bFloatingPointCalled = true;
    return dNumber;
  }
  void setFloatingPoint(double value)
  {
    pCallingThread = QThread::currentThread();
    Q_UNUSED(value); // no effect
    bSetFloatingPointCalled = true;
  }
  void setVariant(const PiiVariant& var)
  {
    pCallingThread = QThread::currentThread();
    varValue = var;
    emit variantChanged(var);
  }
  PiiVariant variant() const
  {
    pCallingThread = QThread::currentThread();
    return varValue;
  }

  int iNumber;
  double dNumber;
  mutable bool bTest1Called, bFloatingPointCalled, bSetFloatingPointCalled, bNumberCalled;
  int iTest1Value;
  PiiVariant varValue;
  mutable QThread* pCallingThread;

public slots:
  void test1()
  {
    pCallingThread = QThread::currentThread();
    bTest1Called = true;
  }
  void test1(int value)
  {
    pCallingThread = QThread::currentThread();
    iTest1Value = value;
  }
  QString test2()
  {
    pCallingThread = QThread::currentThread();
    return "test2";
  }
  QString test2(const QString& str)
  {
    pCallingThread = QThread::currentThread();
    return str;
  }
  int plus(int a, int b)
  {
    pCallingThread = QThread::currentThread();
    return a+b;
  }
  void thrower(int type);
signals:
  void numberChanged(int);
  void variantChanged(const PiiVariant&);
};

class ServerObject2 : public ServerObject
{
  Q_OBJECT
  Q_CLASSINFO("functionSafetyLevel", "test1():0 test2():AccessConcurrently");
  Q_CLASSINFO("propertySafetyLevel", "number:0 floatingPoint:AccessPropertyConcurrently");
};

class TestPiiRemoteObject : public QObject
{
  Q_OBJECT

public:
  TestPiiRemoteObject();

public slots:
  void storeNumber(int value) { _iNumber = value; }
  void storeVariant(const PiiVariant& var) { _variant = var; }

private slots:
  void initTestCase();
  void safetyLevels();
  void properties();
  void functionSignatures();
  void remoteSlots();
  void remoteSignals();
  void functionCalls();
  void cleanupTestCase();
  void exceptions();
  void singleThreaded();
  void propertyCache();

signals:
  void test1();
  void test1(int value);

private:
  void serverThread();

  QThread* _pServerThread;
  PiiHttpServer* _pHttpServer;
  PiiQObjectServer* _pObjectServer1;
  PiiQObjectServer* _pObjectServer2;
  PiiRemoteQObject<QObject>* _pClient1;
  PiiRemoteQObject<QObject>* _pClient2;
  ServerObject _serverObject1;
  ServerObject2 _serverObject2;
  bool _bServerStarted;
  int _iNumber;
  PiiVariant _variant;
};

#endif //_TESTPIIQOBJECTSERVER_H
