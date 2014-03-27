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

#include "TestPiiBits.h"


#include <PiiBits.h>
#include <QtTest>

void TestPiiBits::rol()
{
  // Generic versions
  QCOMPARE(Pii::rol<8>(0x88u,1),0x11u);
  QCOMPARE(Pii::rol<16>(0x8008u,1),0x11u);
  QCOMPARE(Pii::rol<32>(0x80000008u,1),0x11u);

  // Inline assembly versions
  QCOMPARE(Pii::rol((unsigned char)0x88,1),(unsigned char)0x11);
  QCOMPARE(Pii::rol((unsigned short)0x8008,1),(unsigned short)0x11);
  QCOMPARE(Pii::rol(0x80000008u,1),0x11u);
}

void TestPiiBits::ror()
{
  // Generic versions
  QCOMPARE(Pii::ror<8>(0x11u,1),0x88u);
  QCOMPARE(Pii::ror<16>(0x11u,1),0x8008u);
  QCOMPARE(Pii::ror<32>(0x11u,1),0x80000008u);

  // Inline assembly versions
  QCOMPARE(Pii::ror((unsigned char)0x11,1),(unsigned char)0x88);
  QCOMPARE(Pii::ror((unsigned short)0x11,1),(unsigned short)0x8008);
  QCOMPARE(Pii::ror(0x11u,1),0x80000008u);
}

void TestPiiBits::lastOneBit()
{
  int i = 0;
  QCOMPARE(Pii::lastOneBit(i), -1);
  i = 0x10;
  QCOMPARE(Pii::lastOneBit(i), 4);
  i = 0x11;
  QCOMPARE(Pii::lastOneBit(i), 4);
  i = 0x11;
  QCOMPARE(Pii::lastOneBit(i), 4);
  i = 0x3ffff;
  QCOMPARE(Pii::lastOneBit(i), 17);

  ushort u = 0;
  QCOMPARE(Pii::lastOneBit(u), -1);
  u = 0xffff;
  QCOMPARE(Pii::lastOneBit(u), 15);
}

QTEST_MAIN(TestPiiBits)
