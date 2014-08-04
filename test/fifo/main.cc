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

#include <QtTest>
#include <PiiFifoBuffer.h>
#include "TestPiiFifoBuffer.h"
#include <cstdlib>

void TestPiiFifoBuffer::oneThread()
{
  QFETCH(int, blockSize);
  PiiFifoBuffer bfr(16);
  bfr.setWaitTime(500);
  Reader r(&bfr);
  r.setBlockSize(blockSize);
  r.setBufferSize(60);
  r.start();

  Writer w(&bfr);
  w.setBufferSize(60);
  w.setBlockSize(blockSize);
  w.reset(false);
  unsigned char *data = w.getArray();

  bfr.write((char*)data, 10);
  bfr.write((char*)data, 20);
  bfr.write((char*)data, 30);

  r.wait();
  unsigned char* read = r.getArray();
  QVERIFY(std::memcmp(read, data, 10) == 0);
  QVERIFY(std::memcmp(read+10, data, 20) == 0);
  QVERIFY(std::memcmp(read+30, data, 30) == 0);

  char buffer[60];

  bfr.reset();
  w.start();
  bfr.read(buffer, 10);
  bfr.read(buffer+10, 20);
  bfr.read(buffer+30, 30);
  w.wait();

  /*if (std::memcmp(buffer, data, 60))
    {
      for (int i=0; i<60; i++)
        printf("%d ", (int)buffer[i]);
      printf("\n");
      for (int i=0; i<60; i++)
        printf("%d ", (int)data[i]);
      printf("\n");
    }
  */
  QVERIFY(std::memcmp(buffer, data, 60) == 0);

}

void TestPiiFifoBuffer::oneThread_data()
{
  QTest::addColumn<int>("blockSize");

  QTest::newRow("1") << 1;
  QTest::newRow("5") << 2;
  QTest::newRow("20") << 20;
  QTest::newRow("50") << 50;
}


void TestPiiFifoBuffer::twoThreads()
{
  QFETCH(int, bufferSize);
  QFETCH(int, writerBlock);
  QFETCH(int, readerBlock);

  PiiFifoBuffer bfr(bufferSize);
  bfr.setWaitTime(500);
  Writer w(&bfr);
  Reader r(&bfr);
  w.setBlockSize(writerBlock);
  r.setBlockSize(readerBlock);

  w.start();
  r.start();
  w.wait();
  r.wait();
  QVERIFY(std::memcmp(w.getArray(), r.getArray(), BUFFERSIZE) == 0);
}

void TestPiiFifoBuffer::twoThreads_data()
{
  QTest::addColumn<int>("bufferSize");
  QTest::addColumn<int>("writerBlock");
  QTest::addColumn<int>("readerBlock");

  QTest::newRow("16,1,1") << 16 << 1 << 1;
  QTest::newRow("59,2,1") << 59 << 2 << 1;
  QTest::newRow("64,1,3") << 64 << 1 << 3;
  QTest::newRow("7,9,15") << 7 << 9 << 15;
  QTest::newRow("103,103,103") << 103 << 103 << 103;
}

QTEST_MAIN(TestPiiFifoBuffer)
