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

#include "TestPiiMultipartDecoder.h"

#include <QtTest>

#include <PiiMultipartDecoder.h>
#include <PiiException.h>

void TestPiiMultipartDecoder::nestedMultiparts()
{
  QByteArray aMessage(
"Content-Type: multipart/form-data; boundary=AaB03x\n\
\n\
Preamble.\n\
--AaB03x\n\
Content-Disposition: form-data; name=\"submit-name\"\n\
\n\
Larry\n\
--AaB03x\n\
Content-Disposition: form-data; name=\"files\"\n\
Content-Type: multipart/mixed; boundary=BbC04y\n\
\n\
--BbC04y\n\
Content-Disposition: file; filename=\"file1.txt\"\n\
Content-Type: text/plain\n\
\n\
Contents of file1.txt\n\
--BbC04y\n\
Content-Disposition: file; filename=\"file2.gif\"\n\
Content-Type: image/gif\n\
Content-Transfer-Encoding: binary\n\
\n\
Contents of file2.gif\n\
--BbC04y--\n\
--AaB03x--\n");
  QBuffer bfr(&aMessage);
  bfr.open(QIODevice::ReadOnly);
  PiiMultipartDecoder decoder(&bfr);

  try
    {
      QVERIFY(decoder.nextMessage());
      QCOMPARE(decoder.depth(), 2);
      QCOMPARE(decoder.header(1).preamble(), QByteArray("Preamble.\n"));
      QByteArray aBody = decoder.readAll();
      QCOMPARE(aBody, QByteArray("Larry\n"));

      QVERIFY(decoder.nextMessage());
      QCOMPARE(decoder.depth(), 3);
      aBody = decoder.readAll();
      QCOMPARE(aBody, QByteArray("Contents of file1.txt\n"));
      QCOMPARE(decoder.header().contentType(), QString("text/plain"));

      QVERIFY(decoder.nextMessage());
      QCOMPARE(decoder.depth(), 3);
      aBody = decoder.readAll();
      QCOMPARE(aBody, QByteArray("Contents of file2.gif\n"));
      QCOMPARE(decoder.header().contentType(), QString("image/gif"));

      QVERIFY(!decoder.nextMessage());
    }
  catch (PiiException& ex)
    {
      QFAIL(qPrintable(ex.location() + ": " + ex.message()));
    }
}

void TestPiiMultipartDecoder::prefetchedHeader()
{
  QByteArray aMessage(
"Content-Type: multipart/mixed-replace; boundary=\"243F6A8885A308D3\"\r\n\
\r\n\
Preamble.\n\
--243F6A8885A308D3\r\n\
X-URI: empty\r\n\
\r\n\
--243F6A8885A308D3\r\n");
  QBuffer bfr(&aMessage);
  bfr.open(QIODevice::ReadOnly);

  PiiMimeHeader header(bfr.readLine());
  bfr.readLine();

  PiiMultipartDecoder decoder(&bfr, header);
  // Read preamble
  QByteArray aPreamble = decoder.readAll();
  QCOMPARE(aPreamble, QByteArray("Preamble.\n"));

  try
    {
      QVERIFY(decoder.nextMessage());
      QCOMPARE(decoder.depth(), 2);
      QCOMPARE(decoder.header().value("X-URI"), QString("empty"));
      QByteArray aBody = decoder.readAll();
      QVERIFY(aBody.isEmpty());

      QVERIFY(!decoder.nextMessage());
    }
  catch (PiiException& ex)
    {
      QFAIL(qPrintable(ex.location() + ": " + ex.message()));
    }
}

QTEST_MAIN(TestPiiMultipartDecoder)
