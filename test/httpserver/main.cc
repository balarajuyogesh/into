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

#include "TestPiiHttpServer.h"

#include <QtTest>

#include <PiiHttpServer.h>
#include <PiiNetwork.h>
#include <PiiException.h>
#include <PiiFileUtil.h>
#include <PiiFileSystemUriHandler.h>
#include <PiiAsyncCall.h>

TestPiiHttpServer::TestPiiHttpServer() :
  _strBase(Pii::applicationBasePath() + "/data"),
  _pServerThread(0),
  _bServerRunning(false),
  _bSuccess(false)
{}

void TestPiiHttpServer::serverThread(const QString& address)
{
  PiiFileSystemUriHandler handler(_strBase);
  handler.setAllowedMethods(QStringList() << "GET" << "HEAD" << "PUT" << "MKCOL" << "DELETE");
  handler.setIndexFile("test.txt");
  PiiHttpServer* pServer = PiiHttpServer::addServer("TestServer", address);
  pServer->protocol()->registerUriHandler("/", &handler);
  if (pServer->start())
    {
      _bSuccess = _bServerRunning = true;
      _serverCondition.wakeOne();
      while (_bServerRunning)
        QCoreApplication::processEvents();
    }
  else
    _serverCondition.wakeOne();
  PiiHttpServer::removeServer("TestServer");
}

void TestPiiHttpServer::cleanup()
{
  if (_bServerRunning)
    {
      _bServerRunning = false;
      _pServerThread->wait();
    }
  delete _pServerThread;
  _pServerThread = 0;
}

void TestPiiHttpServer::httpRequest()
{
  // Clean up first
  QVERIFY(!QFileInfo(_strBase).exists() || Pii::deleteDirectory(_strBase));
  QDir baseDir(Pii::applicationBasePath());
  QVERIFY(baseDir.mkdir("data"));

  // Start server in another thread
  QFETCH(QString, address);
  _bSuccess = false;
  _pServerThread = Pii::asyncCall(this, &TestPiiHttpServer::serverThread, address);
  _serverCondition.wait();
  if (!_bSuccess)
    QFAIL("HTTP server could not start.");

  QByteArray aFileContents("Arbitrary test data.\n");
  try
    {
      PiiNetwork::putFile(address + "/test.txt", aFileContents);
      QCOMPARE(PiiNetwork::readFile(address + "/"), aFileContents);
      PiiNetwork::makeDirectory(address + "/test");
      PiiNetwork::putFile(address + "/test/test2.txt", aFileContents);

      QCOMPARE(PiiNetwork::readFile("file://" + _strBase + "/test.txt"), aFileContents);
      QCOMPARE(PiiNetwork::readFile("file://" + _strBase + "/test/test2.txt"), aFileContents);

      PiiNetwork::deleteFile(address + "/test.txt");
      
      QVERIFY(!QFileInfo(_strBase + "/test.txt").exists());
      
      PiiNetwork::deleteFile(address + "/test");

      QVERIFY(!QFileInfo(_strBase + "/test/test2.txt").exists());
      QVERIFY(!QFileInfo(_strBase + "/test").exists());
    }
  catch (PiiException& ex)
    {
      QFAIL(qPrintable(ex.message()));
    }
}

void TestPiiHttpServer::httpRequest_data()
{
  QTest::addColumn<QString>("address");

  QTest::newRow("tcp") << "tcp://0.0.0.0:31415";
  //QTest::newRow("ssl") << "ssl://127.0.0.1:31415";
  //QTest::newRow("local") << "local://" + _strBase + "/server.sock";
}

QTEST_MAIN(TestPiiHttpServer)
