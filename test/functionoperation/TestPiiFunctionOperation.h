/* This file is part of Into.
 * Copyright (C) Intopii 2014.
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

#ifndef _TESTPIIFUNCTIONOPERATION_H
#define _TESTPIIFUNCTIONOPERATION_H

#include <PiiOperationTest.h>
#include <PiiFunctionOperation.h>

#ifdef PII_CXX11
double sum(double a, int b);
void sum2(double a, int b, int* c);

class SumTestOperation : public PII_FUNCTION_OP_FOR(sum)
{
  Q_OBJECT
public:
  typedef PII_FUNCTION_OP_FOR(sum) SuperType;
  SumTestOperation() :
    SuperType(sum, "sum", "a", "b") {}

  void checkSockets();

  int defaultB() const { return 10; }

  void setDefault() { setDefaultValue("b", &SumTestOperation::defaultB); }
  void unsetDefault() { setDefaultValue("b", nullptr); }
};

class Sum2TestOperation : public PII_FUNCTION_OP_FOR(sum2)
{
  Q_OBJECT
public:
  typedef PII_FUNCTION_OP_FOR(sum2) SuperType;
  Sum2TestOperation() :
    SuperType(sum2, "a", "b", "sum") {}

  void checkSockets();
};

#endif

class TestPiiFunctionOperation : public PiiOperationTest
{
  Q_OBJECT

private slots:
  void initTestCase();

#ifdef PII_CXX11
  void sumOperation();
  void sum2Operation();
  void setDefaultValue();
  void socketAt();

protected:
  void cleanup();
#endif
};


#endif //_TESTPIIFUNCTIONOPERATION_H
