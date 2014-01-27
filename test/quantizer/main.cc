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
#include "TestQuantizer.h"
#include <PiiQuantizer.h>

void TestQuantizer::divideEqually()
{
  // Six samples with 2 features in each
  PiiMatrix<int> data(1, 6, 5,3,6,2,1,4);
  PiiMatrix<int> limits(PiiQuantizer<int>::divideEqually(data, 3));

  QCOMPARE(limits(0),3);
  QCOMPARE(limits(1),5);

  PiiQuantizer<int> quantizer(limits);
  QCOMPARE(quantizer.quantize(0),0);
  QCOMPARE(quantizer.quantize(1),0);
  QCOMPARE(quantizer.quantize(2),0);
  QCOMPARE(quantizer.quantize(3),1);
  QCOMPARE(quantizer.quantize(4),1);
  QCOMPARE(quantizer.quantize(5),2);
  QCOMPARE(quantizer.quantize(6),2);
  QCOMPARE(quantizer.quantize(7),2);
}

QTEST_MAIN(TestQuantizer)

