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
  _pObjectServer(0),
  _pClient(0),
  _bServerStarted(false),
  _iNumber(0)
{}

void TestPiiRemoteObject::serverThread()
{
  _pHttpServer = PiiHttpServer::addServer("PiiObjectServer", "tcp://0.0.0.0:3142");
  _pObjectServer = new PiiQObjectServer(&_serverObject);
  _pHttpServer->protocol()->registerUriHandler("/", _pObjectServer);

  _bServerStarted = _pHttpServer->start();

  if (_bServerStarted)
    {
      QEventLoop eventLoop;
      eventLoop.exec();
    }

  _pHttpServer->stop(PiiNetwork::InterruptClients);
  delete _pObjectServer;
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
      //_pClient = new PiiRemoteQObject<QObject>("tcp://256.0.0.1/");
      //QFAIL("Client shouldn't be able to connect to 256.0.0.1");
    }
  catch (...) {}

  try
    {
      _pClient = new PiiRemoteQObject<QObject>("tcp://127.0.0.1:3142/");
    }
  catch (PiiException& ex)
    {
      piiWarning(ex.location("", ": ") + ex.message());
      QFAIL("Could not connect to tcp://127.0.0.1:3142/");
    }
}

void TestPiiRemoteObject::cleanupTestCase()
{
  try
    {
      delete _pClient;

      _pServerThread->quit();
      _pServerThread->wait();
    }
  catch (PiiException& ex)
    {
      piiDebug(ex.location() + ": " + ex.message());
    }
}

void TestPiiRemoteObject::properties()
{
  const QMetaObject* pMetaObject = _pClient->metaObject();
  QCOMPARE(pMetaObject->propertyCount(), 4);
  QCOMPARE(pMetaObject->property(1).name(), "number");

  //qDebug("setting number");
  QVERIFY(_pClient->setProperty("number", 314));
  QCOMPARE(_serverObject.iNumber, 314);
  QCOMPARE(_pClient->property("number").toInt(), 314);
  QCOMPARE(_pClient->property("floatingPoint").toDouble(), 3.14159);
  //qDebug("setting matrix");
  PiiMatrix<int> testMat(1,3,1,2,3);
  QVERIFY(_pClient->setProperty("variant", Pii::createQVariant(testMat)));
  QCOMPARE(_serverObject.varValue.type(), unsigned(PiiYdin::IntMatrixType));
  QVERIFY(Pii::equals(_serverObject.varValue.valueAs<PiiMatrix<int> >(),
                      testMat));
  //qDebug("getting variant");
  PiiVariant varProp = _pClient->property("variant").value<PiiVariant>();
  QCOMPARE(varProp.type(), unsigned(PiiYdin::IntMatrixType));
  QVERIFY(Pii::equals(varProp.valueAs<PiiMatrix<int> >(), testMat));
}

void TestPiiRemoteObject::functionSignatures()
{
  const QMetaObject* pMetaObject = _pClient->metaObject();
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
  QVERIFY(connect(this, SIGNAL(test1()), _pClient, SLOT(test1())));
  QVERIFY(connect(this, SIGNAL(test1(int)), _pClient, SLOT(test1(int))));

  emit test1();
  QVERIFY(_serverObject.bTest1Called);
  emit test1(-123);
  QCOMPARE(_serverObject.iTest1Value, -123);
}

void TestPiiRemoteObject::remoteSignals()
{
  //qApp->exec();
  //exit(1);
  QVERIFY(connect(_pClient, SIGNAL(numberChanged(int)), this, SLOT(storeNumber(int)), Qt::DirectConnection));
  QVERIFY(connect(_pClient, SIGNAL(variantChanged(PiiVariant)), this, SLOT(storeVariant(PiiVariant)), Qt::DirectConnection));
  QVERIFY(_pClient->setProperty("number", 314));
  QCOMPARE(_serverObject.iNumber, 314);
  QVERIFY(_pClient->setProperty("variant", Pii::createQVariant(PiiMatrix<double>())));
  PiiDelay::msleep(100);
  QCOMPARE(_iNumber, 314);
  QCOMPARE(_variant.type(), unsigned(PiiYdin::DoubleMatrixType));
  QVERIFY(Pii::equals(_variant.valueAs<PiiMatrix<double> >(), PiiMatrix<double>()));
}

void TestPiiRemoteObject::functionCalls()
{
  QString strReturn;
  QVERIFY(_pClient->metaObject()->method(_pClient->metaObject()->methodOffset() + 4)
          .invoke(_pClient, Qt::DirectConnection, QGenericReturnArgument()));
  QVERIFY(QMetaObject::invokeMethod(_pClient, "test2", Qt::DirectConnection,
                                    Q_RETURN_ARG(QString, strReturn)));
  QCOMPARE(strReturn, QString("test2"));
  QVERIFY(QMetaObject::invokeMethod(_pClient, "test2", Qt::DirectConnection,
                                    Q_RETURN_ARG(QString, strReturn),
                                    Q_ARG(QString, "foobar")));
  QCOMPARE(strReturn, QString("foobar"));
  QCOMPARE(_pClient->call<int>("functions/plus", 1, 2), 3);
}

void TestPiiRemoteObject::exceptions()
{
  try
    {
      _pClient->call<void>("functions/thrower", 0);
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
      _pClient->call<void>("functions/thrower", 1);
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
      _pClient->call<void>("functions/thrower", 2);
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
