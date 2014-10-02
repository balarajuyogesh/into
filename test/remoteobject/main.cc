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

#include "TestPiiRemoteObject.h"
#include <PiiException.h>
#include <PiiYdinTypes.h>
#include <PiiNetworkException.h>
#include <PiiInvalidArgumentException.h>
#include <PiiMimeException.h>
#include <QtTest>

#include <PiiAsyncCall.h>
#include <PiiDelay.h>

TestPiiRemoteObject::TestPiiRemoteObject() :
  _pServerThread(0),
  _pHttpServer(0),
  _pObjectServer1(0),
  _pObjectServer2(0),
  _pClient1(0),
  _pClient2(0),
  _bServerStarted(false),
  _iNumber(0)
{}

void TestPiiRemoteObject::serverThread()
{
  _pHttpServer = PiiHttpServer::addServer("PiiObjectServer", "tcp://0.0.0.0:3142");
  _pObjectServer1 = new PiiQObjectServer(&_serverObject1);
  _pObjectServer2 = new PiiQObjectServer(&_serverObject2);
  _pHttpServer->protocol()->registerUriHandler("/1/", _pObjectServer1);
  _pHttpServer->protocol()->registerUriHandler("/2/", _pObjectServer2);

  _bServerStarted = _pHttpServer->start();

  if (_bServerStarted)
    {
      QEventLoop eventLoop;
      eventLoop.exec();
    }

  _pHttpServer->stop(PiiNetwork::InterruptClients);
  delete _pObjectServer1;
  delete _pObjectServer2;
}

void TestPiiRemoteObject::initTestCase()
{
  _pServerThread = Pii::asyncCall(this, &TestPiiRemoteObject::serverThread);

  for (int i=0; i<100; ++i)
    {
      PiiDelay::msleep(10);
      if (_bServerStarted)
        break;
    }
  QVERIFY(_bServerStarted);

  try
    {
      _pClient1 = new PiiRemoteQObject<QObject>("tcp://127.0.0.1:3142/1/");
      _pClient2 = new PiiRemoteQObject<QObject>("tcp://127.0.0.1:3142/2/");
    }
  catch (PiiException& ex)
    {
      piiWarning(ex.location("", ": ") + ex.message());
      QFAIL("Could not connect to tcp://127.0.0.1:3142/");
    }

  QCOMPARE(_pClient1->id(), _pObjectServer1->id());
  QCOMPARE(_pClient2->id(), _pObjectServer2->id());
}

void TestPiiRemoteObject::cleanupTestCase()
{
  try
    {
      delete _pClient1;

      _pServerThread->quit();
      _pServerThread->wait();
    }
  catch (PiiException& ex)
    {
      piiDebug(ex.location() + ": " + ex.message());
    }
}

void TestPiiRemoteObject::safetyLevels()
{
  QCOMPARE(_pObjectServer1->strictestSafetyLevel(), PiiObjectServer::AccessFromAnyThread);
  QCOMPARE(_pObjectServer1->strictestPropertySafetyLevel(), PiiQObjectServer::AccessPropertyFromAnyThread);
  QCOMPARE(_pObjectServer2->strictestSafetyLevel(), PiiObjectServer::AccessFromMainThread);
  QCOMPARE(_pObjectServer2->strictestPropertySafetyLevel(), PiiQObjectServer::AccessPropertyFromMainThread);
  QCOMPARE(_pObjectServer2->functionSafetyLevel("test1()"), PiiObjectServer::AccessFromMainThread);
  QCOMPARE(_pObjectServer2->functionSafetyLevel("test2()"), PiiObjectServer::AccessConcurrently);
  QCOMPARE(_pObjectServer2->propertySafetyLevel("number"), PiiQObjectServer::AccessPropertyFromMainThread);
  QCOMPARE(_pObjectServer2->propertySafetyLevel("floatingPoint"), PiiQObjectServer::AccessPropertyConcurrently);
}

void TestPiiRemoteObject::properties()
{
  const QMetaObject* pMetaObject = _pClient1->metaObject();
  QCOMPARE(pMetaObject->propertyCount(), 4);
  QCOMPARE(pMetaObject->property(1).name(), "number");

  //qDebug("setting number");
  QVERIFY(_pClient1->setProperty("number", 314));
  QVERIFY(_serverObject1.pCallingThread != 0);
  QVERIFY(_serverObject1.pCallingThread != QThread::currentThread());
  QCOMPARE(_serverObject1.iNumber, 314);
  QVERIFY(_serverObject1.pCallingThread != QThread::currentThread());
  QCOMPARE(_pClient1->property("number").toInt(), 314);
  QVERIFY(_serverObject1.pCallingThread != QThread::currentThread());
  QCOMPARE(_pClient1->property("floatingPoint").toDouble(), 3.14159);
  QVERIFY(_serverObject1.pCallingThread != QThread::currentThread());
  //qDebug("setting matrix");
  PiiMatrix<int> testMat(1,3,1,2,3);
  QVERIFY(_pClient1->setProperty("variant", Pii::createQVariant(testMat)));
  QCOMPARE(_serverObject1.varValue.type(), unsigned(PiiYdin::IntMatrixType));
  QVERIFY(Pii::equals(_serverObject1.varValue.valueAs<PiiMatrix<int> >(),
                      testMat));
  //qDebug("getting variant");
  PiiVariant varProp = _pClient1->property("variant").value<PiiVariant>();
  QCOMPARE(varProp.type(), unsigned(PiiYdin::IntMatrixType));
  QVERIFY(Pii::equals(varProp.valueAs<PiiMatrix<int> >(), testMat));
  QVERIFY(_serverObject1.pCallingThread != QThread::currentThread());
}

void TestPiiRemoteObject::functionSignatures()
{
  const QMetaObject* pMetaObject = _pClient1->metaObject();
  QStringList lstMethods;
  // Discard destroyed() (two overloads), deleteLater(), and _q_reregisterTimers()
  for (int i = pMetaObject->methodOffset(); i < pMetaObject->methodCount(); ++i)
    {
      QMetaMethod method = pMetaObject->method(i);
      if (
#if QT_VERSION < 0x050000
          *method.typeName() != 0
#else
          method.returnType() not_member_of<int> (QMetaType::UnknownType, QMetaType::Void)
#endif
          )
        lstMethods << QString("%1 %2").arg(method.typeName()).arg(QString(method.QMETAMETHOD_SIGNATURE()));
      else
        lstMethods << method.QMETAMETHOD_SIGNATURE();
    }

  QCOMPARE(lstMethods,
           QStringList()
           << "numberChanged(int)"
           << "variantChanged(PiiVariant)"
           << "test1()"
           << "test1(int)"
           << "QString test2()"
           << "QString test2(QString)"
           << "int plus(int,int)"
           << "thrower(int)");

  int iFirstSlotIndex = pMetaObject->methodOffset() + 2;
  QCOMPARE(pMetaObject->indexOfMethod("test1()"), iFirstSlotIndex);
  QCOMPARE(pMetaObject->indexOfMethod("test1(int)"), iFirstSlotIndex + 1);
  QCOMPARE(pMetaObject->indexOfMethod("test2()"), iFirstSlotIndex + 2);
  QCOMPARE(pMetaObject->indexOfMethod("test2(QString)"), iFirstSlotIndex + 3);
  QCOMPARE(pMetaObject->indexOfMethod("plus(int,int)"), iFirstSlotIndex + 4);
  QCOMPARE(pMetaObject->indexOfMethod("thrower(int)"), iFirstSlotIndex + 5);
}

void TestPiiRemoteObject::remoteSlots()
{
  QVERIFY(connect(this, SIGNAL(test1()), _pClient1, SLOT(test1())));
  QVERIFY(connect(this, SIGNAL(test1(int)), _pClient1, SLOT(test1(int))));

  emit test1();
  QVERIFY(_serverObject1.bTest1Called);
  QVERIFY(_serverObject1.pCallingThread != QThread::currentThread());
  emit test1(-123);
  QCOMPARE(_serverObject1.iTest1Value, -123);
  QVERIFY(_serverObject1.pCallingThread != QThread::currentThread());
}

void TestPiiRemoteObject::remoteSignals()
{
  //qApp->exec();
  //exit(1);
  QVERIFY(connect(_pClient1, SIGNAL(numberChanged(int)), this, SLOT(storeNumber(int)), Qt::DirectConnection));
  QVERIFY(connect(_pClient1, SIGNAL(variantChanged(PiiVariant)), this, SLOT(storeVariant(PiiVariant)), Qt::DirectConnection));
  QVERIFY(_pClient1->setProperty("number", 314));
  QCOMPARE(_serverObject1.iNumber, 314);
  QVERIFY(_pClient1->setProperty("variant", Pii::createQVariant(PiiMatrix<double>())));
  PiiDelay::msleep(100);
  QCOMPARE(_iNumber, 314);
  QCOMPARE(_variant.type(), unsigned(PiiYdin::DoubleMatrixType));
  QVERIFY(Pii::equals(_variant.valueAs<PiiMatrix<double> >(), PiiMatrix<double>()));
}

void TestPiiRemoteObject::functionCalls()
{
  QString strReturn;
  QVERIFY(_pClient1->metaObject()->method(_pClient1->metaObject()->methodOffset() + 4)
          .invoke(_pClient1, Qt::DirectConnection, QGenericReturnArgument()));
  QVERIFY(QMetaObject::invokeMethod(_pClient1, "test2", Qt::DirectConnection,
                                    Q_RETURN_ARG(QString, strReturn)));
  QCOMPARE(strReturn, QString("test2"));
  QVERIFY(QMetaObject::invokeMethod(_pClient1, "test2", Qt::DirectConnection,
                                    Q_RETURN_ARG(QString, strReturn),
                                    Q_ARG(QString, "foobar")));
  QCOMPARE(strReturn, QString("foobar"));
  QCOMPARE(_pClient1->call<int>("functions/plus", 1, 2), 3);
}

void TestPiiRemoteObject::exceptions()
{
  try
    {
      _pClient1->call<void>("functions/thrower", 0);
      QFAIL("Call should have caused an exception.");
    }
  catch (PiiInvalidArgumentException& ex)
    {
      QCOMPARE(ex.message(), QString("InvalidArgument"));
    }
  catch (...)
    {
      QFAIL("Should have caught a PiiInvalidArgumentException.");
    }

  try
    {
      _pClient1->call<void>("functions/thrower", 1);
      QFAIL("Call should have caused an exception.");
    }
  catch (PiiMimeException& ex)
    {
      QCOMPARE(ex.code(), PiiMimeException::InvalidFormat);
    }
  catch (...)
    {
      QFAIL("Should have caught a PiiMimeException.");
    }

  try
    {
      _pClient1->call<void>("functions/thrower", 2);
      QFAIL("Call should have caused an exception.");
    }
  catch (PiiNetworkException& ex)
    {
      QCOMPARE(ex.message(), QString("Network"));
    }
  catch (...)
    {
      QFAIL("Should have caught a PiiNetworkException.");
    }
}

void TestPiiRemoteObject::singleThreaded()
{
  QCOMPARE(_pClient2->property("number").toInt(), 0);
  QVERIFY(_serverObject2.pCallingThread == QThread::currentThread());
  QVERIFY(_pClient2->setProperty("number", -123));
  QVERIFY(_serverObject2.pCallingThread == QThread::currentThread());
  _pClient2->call<void>("functions/test1");
  QVERIFY(_serverObject2.bTest1Called);
  QVERIFY(_serverObject2.pCallingThread == QThread::currentThread());
}

void ServerObject::thrower(int type)
{
  switch (type)
    {
    case 0:
      throw PiiInvalidArgumentException("InvalidArgument");
    case 1:
      throw PiiMimeException(PiiMimeException::InvalidFormat);
    case 2:
      throw PiiNetworkException("Network");
    }
}


QTEST_MAIN(TestPiiRemoteObject)
