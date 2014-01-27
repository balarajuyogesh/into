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

#include "TestPiiColor.h"

#include <PiiColor.h>
#include <PiiMath.h>
#include <QtTest>

void TestPiiColor::size()
{
  QCOMPARE(sizeof(PiiColor<>), size_t(3));
  QCOMPARE(sizeof(PiiColor<int>), 3 * sizeof(int));
  QCOMPARE(sizeof(PiiColor4<>), size_t(4));
  QCOMPARE(sizeof(PiiColor4<int>), 4 * sizeof(int));
}

void TestPiiColor::channels()
{
  {
    PiiColor<> clr(255, 127, 0);
    QCOMPARE(clr.c0, static_cast<unsigned char>(255));
    QCOMPARE(clr.c1, static_cast<unsigned char>(127));
    QCOMPARE(clr.c2, static_cast<unsigned char>(0));
    QCOMPARE(clr.channel(0), clr.rgbR);
    QCOMPARE(clr.channel(1), clr.rgbG);
    QCOMPARE(clr.channel(2), clr.rgbB);
    QCOMPARE(clr.rgbG, static_cast<unsigned char>(127));
    QCOMPARE(clr.channel(2), clr.hsvV);
    QCOMPARE(clr.c2, static_cast<unsigned char>(0));
    QCOMPARE(clr.c2, clr.rgbaB);
  }
  {
    PiiColor4<> clr(255, 127, 0, 5);
    QCOMPARE(clr.c0, static_cast<unsigned char>(255));
    QCOMPARE(clr.c1, static_cast<unsigned char>(127));
    QCOMPARE(clr.c2, static_cast<unsigned char>(0));
    QCOMPARE(clr.c3, static_cast<unsigned char>(5));
    QCOMPARE(clr.channel(0), clr.rgbR);
    QCOMPARE(clr.channel(1), clr.rgbG);
    QCOMPARE(clr.channel(2), clr.rgbB);
    QCOMPARE(clr.channel(3), clr.rgbaA);
    QCOMPARE(clr.c3, clr.cmykK);
  }
}

void TestPiiColor::constructors()
{
  PiiColor<int> a(1);
  PiiColor<int> b(a);
  PiiColor<char> c(1,2,3);
  PiiColor<int> d(c);
  PiiColor4<int> e(5,6,7,8);
  PiiColor<int> f(e);
  PiiColor4<int> g(2);
  PiiColor4<int> h(g);
  PiiColor4<int> i(a);
  PiiColor4<int> j(c);
  PiiColor4<char> k(3);
  PiiColor4<int> l(k);

  QCOMPARE(a.c0, 1); QCOMPARE(a.c1, 1); QCOMPARE(a.c2, 1);
  QCOMPARE(b.c0, 1); QCOMPARE(b.c1, 1); QCOMPARE(b.c2, 1);
  QCOMPARE(c.c0, char(1)); QCOMPARE(c.c1, char(2)); QCOMPARE(c.c2, char(3));
  QCOMPARE(d.c0, 1); QCOMPARE(d.c1, 2); QCOMPARE(d.c2, 3);
  QCOMPARE(e.c0, 5); QCOMPARE(e.c1, 6); QCOMPARE(e.c2, 7); QCOMPARE(e.c3, 8);
  QCOMPARE(f.c0, 5); QCOMPARE(f.c1, 6); QCOMPARE(f.c2, 7);                  ;
  QCOMPARE(g.c0, 2); QCOMPARE(g.c1, 2); QCOMPARE(g.c2, 2); QCOMPARE(g.c3, 2);
  QCOMPARE(h.c0, 2); QCOMPARE(h.c1, 2); QCOMPARE(h.c2, 2); QCOMPARE(h.c3, 2);
  QCOMPARE(i.c0, 1); QCOMPARE(i.c1, 1); QCOMPARE(i.c2, 1); QCOMPARE(i.c3, 0);
  QCOMPARE(j.c0, 1); QCOMPARE(j.c1, 2); QCOMPARE(j.c2, 3); QCOMPARE(j.c3, 0);
  QCOMPARE(k.c0, char(3)); QCOMPARE(k.c1, char(3)); QCOMPARE(k.c2, char(3)); QCOMPARE(k.c3, char(3));
  QCOMPARE(l.c0, 3); QCOMPARE(l.c1, 3); QCOMPARE(l.c2, 3); QCOMPARE(l.c3, 3);
}

void TestPiiColor::arithmetic()
{
  {
    typedef PiiColor4<int> Clr;
    Clr a(1,2,3,4), b(1,2,3,4);
    QVERIFY((a-b) == Clr());
    QVERIFY((a+b) == Clr(2,4,6,8));
    QVERIFY((a*b) == Clr(1,4,9,16));
    QVERIFY((a/b) == Clr(1,1,1,1));
    QVERIFY(a*2 == Clr(2,4,6,8));
    QVERIFY(a/2 == Clr(0,1,1,2));
  }
  {
    typedef PiiColor<int> Clr;
    Clr a(1,2,3), b(1,2,3);
    QVERIFY((a-b) == Clr());
    QVERIFY((a+b) == Clr(2,4,6));
    QVERIFY((a*b) == Clr(1,4,9));
    QVERIFY((a/b) == Clr(1,1,1));
    QVERIFY(a*2 == Clr(2,4,6));
    QVERIFY(a/2 == Clr(0,1,1));
    QVERIFY(a+2 == Clr(3,4,5));
    QVERIFY(a-2 == Clr(-1,0,1));
  }
  {
    PiiMatrix<int> mat(PiiMatrix<int>::identity(3)*2);
    PiiColor<int> source(1,2,3);
    PiiColor<int> target;

    Pii::multiply(source.constBegin(), mat, target.begin());
    QCOMPARE(target.c0, 2);
    QCOMPARE(target.c1, 4);
    QCOMPARE(target.c2, 6);
  }
  {
    PiiColor<int> a(1,2,3), a2 = a*2;
    PiiColor<float> b(1,2,3), b2 = b*5.0f;

    QCOMPARE(a2.c0, 2);
    QCOMPARE(a2.c1, 4);
    QCOMPARE(a2.c2, 6);

    QCOMPARE(b2.c0, 5.0f);
    QCOMPARE(b2.c1, 10.0f);
    QCOMPARE(b2.c2, 15.0f);
  }
}


QTEST_MAIN(TestPiiColor)
