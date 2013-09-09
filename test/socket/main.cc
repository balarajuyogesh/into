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

#include "TestPiiSocket.h"
#include <PiiInputSocket.h>
#include <PiiOutputSocket.h>
#include <PiiProxySocket.h>
#include <QtTest>

/*
  Capital letters represent proxies

   ,-- b     ,-- F
  a     ,-- D
   `-- C     `-- G -- h
        `-- e
 */
void TestPiiSocket::isConnected()
{
  PiiOutputSocket a("");
  PiiInputSocket b(""), e(""), h("");
  PiiProxySocket c, d, f, g;

  QVERIFY(!a.isConnected());
  QVERIFY(!b.isConnected());
  QVERIFY(!e.isConnected());
  QVERIFY(!h.isConnected());

  a.connectInput(&c);
  QVERIFY(!a.isConnected());
  a.connectInput(&b);
  QVERIFY(a.isConnected());
  QVERIFY(b.isConnected());
  a.disconnectInput(&b);
  QVERIFY(!a.isConnected());
  c.connectInput(&d);
  d.connectInput(&f);
  QVERIFY(!a.isConnected());
  d.connectInput(&g);
  QVERIFY(!a.isConnected());
  g.connectInput(&h);
  QVERIFY(a.isConnected());
  QVERIFY(h.isConnected());
  a.connectInput(&b);
  QVERIFY(a.isConnected());
  c.connectInput(&e);
  QVERIFY(e.isConnected());
  c.disconnectInput(&d);
  QVERIFY(a.isConnected());
  QVERIFY(!h.isConnected());
  c.disconnectInput(&e);
  QVERIFY(a.isConnected());
  QVERIFY(!e.isConnected());
  a.disconnectInput(&b);
  QVERIFY(!a.isConnected());
}

QTEST_MAIN(TestPiiSocket)
