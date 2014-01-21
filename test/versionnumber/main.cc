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

#include "TestPiiVersionNumber.h"

#include <PiiVersionNumber.h>
#include <QtTest>

void TestPiiVersionNumber::testAll()
{
 PiiVersionNumber v1("1.0.0");
 PiiVersionNumber v2("1.1-custom");
 PiiVersionNumber v3(0, 9);
 PiiVersionNumber v4(0, 9, 0, "custom");
 PiiVersionNumber v5(0, 9, 0);
 PiiVersionNumber v6("1.1-custom-2");
 PiiVersionNumber v7("2.0.0-gamma");
 PiiVersionNumber v8("2.0.0-delta");
 QVERIFY(v1 < v2);
 QVERIFY(v1 >= v3);
 QVERIFY(v4 != v3);
 QVERIFY(v4 < v3);
 QVERIFY(v5 > v4);
 QVERIFY(v6 > v2);
 QVERIFY(v7 < v8);
 QCOMPARE(v2.revision(), QString("custom"));
 QCOMPARE(v6.revision(), QString("custom-2"));

 QVERIFY(PiiVersionNumber("2.0.0-beta") < PiiVersionNumber("2.0.0-beta-2"));
 QVERIFY(PiiVersionNumber("2.0.0") < PiiVersionNumber("2.0.0-2"));
 QVERIFY(PiiVersionNumber("2.0.0-a") < PiiVersionNumber("2.0.0-b"));
}

QTEST_MAIN(TestPiiVersionNumber)
