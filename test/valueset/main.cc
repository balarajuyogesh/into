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

#include "TestPiiValueSet.h"

#include <PiiValueSet.h>
#include <QtTest>

void TestPiiValueSet::memberOf()
{
  QVERIFY(1 member_of (1,2));
  QVERIFY(2 member_of (1,2));
  QVERIFY(!(-1 member_of (1,2)));
  QVERIFY(-1 not_member_of (1,2));

  QVERIFY(1 member_of (1,2,3));
  QVERIFY(2 member_of (1,2,3));
  QVERIFY(3 member_of (1,2,3));
  QVERIFY(!(-1 member_of (1,2,3)));
  QVERIFY(-1 not_member_of (1,2,3));

  QVERIFY(1 member_of (1,2,3,4));
  QVERIFY(2 member_of (1,2,3,4));
  QVERIFY(3 member_of (1,2,3,4));
  QVERIFY(4 member_of (1,2,3,4));
  QVERIFY(!(-1 member_of (1,2,3,4)));
  QVERIFY(-1 not_member_of (1,2,3,4));

  QVERIFY(1 member_of (1,2,3,4,5));
  QVERIFY(2 member_of (1,2,3,4,5));
  QVERIFY(3 member_of (1,2,3,4,5));
  QVERIFY(4 member_of (1,2,3,4,5));
  QVERIFY(5 member_of (1,2,3,4,5));
  QVERIFY(!(-1 member_of (1,2,3,4,5)));
  QVERIFY(-1 not_member_of (1,2,3,4,5));

  QVERIFY(1 member_of (1,2,3,4,5,6));
  QVERIFY(2 member_of (1,2,3,4,5,6));
  QVERIFY(3 member_of (1,2,3,4,5,6));
  QVERIFY(4 member_of (1,2,3,4,5,6));
  QVERIFY(5 member_of (1,2,3,4,5,6));
  QVERIFY(6 member_of (1,2,3,4,5,6));
  QVERIFY(!(-1 member_of (1,2,3,4,5,6)));
  QVERIFY(-1 not_member_of (1,2,3,4,5,6));

  QVERIFY(1 member_of (1,2,3,4,5,6,7));
  QVERIFY(2 member_of (1,2,3,4,5,6,7));
  QVERIFY(3 member_of (1,2,3,4,5,6,7));
  QVERIFY(4 member_of (1,2,3,4,5,6,7));
  QVERIFY(5 member_of (1,2,3,4,5,6,7));
  QVERIFY(6 member_of (1,2,3,4,5,6,7));
  QVERIFY(7 member_of (1,2,3,4,5,6,7));
  QVERIFY(!(-1 member_of (1,2,3,4,5,6,7)));
  QVERIFY(-1 not_member_of (1,2,3,4,5,6,7));

  QVERIFY(1 member_of (1,2,3,4,5,6,7,8));
  QVERIFY(2 member_of (1,2,3,4,5,6,7,8));
  QVERIFY(3 member_of (1,2,3,4,5,6,7,8));
  QVERIFY(4 member_of (1,2,3,4,5,6,7,8));
  QVERIFY(5 member_of (1,2,3,4,5,6,7,8));
  QVERIFY(6 member_of (1,2,3,4,5,6,7,8));
  QVERIFY(7 member_of (1,2,3,4,5,6,7,8));
  QVERIFY(8 member_of (1,2,3,4,5,6,7,8));
  QVERIFY(!(-1 member_of (1,2,3,4,5,6,7,8)));
  QVERIFY(-1 not_member_of (1,2,3,4,5,6,7,8));

  QVERIFY(1 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(2 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(3 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(4 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(5 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(6 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(7 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(8 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(9 member_of (1,2,3,4,5,6,7,8,9));
  QVERIFY(!(-1 member_of (1,2,3,4,5,6,7,8,9)));
  QVERIFY(-1 not_member_of (1,2,3,4,5,6,7,8,9));

  QVERIFY(1 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(2 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(3 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(4 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(5 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(6 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(7 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(8 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(9 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(10 member_of (1,2,3,4,5,6,7,8,9,10));
  QVERIFY(!(-1 member_of (1,2,3,4,5,6,7,8,9,10)));
  QVERIFY(-1 not_member_of (1,2,3,4,5,6,7,8,9,10));

  QVERIFY("abc" member_of ("abc", "def"));
  QVERIFY("def" member_of ("abc", "def"));
  QVERIFY(!("ghi" member_of ("abc", "def")));
  QVERIFY("ghi" not_member_of ("abc", "def"));
}

QTEST_MAIN(TestPiiValueSet)
