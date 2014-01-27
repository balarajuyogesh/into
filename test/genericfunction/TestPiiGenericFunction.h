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

#ifndef _TESTPIIGENERICFUNCTION_H
#define _TESTPIIGENERICFUNCTION_H

#include <QObject>

class TestPiiGenericFunction : public QObject
{
  Q_OBJECT

private slots:
  void noArguments();
  void oneArgument();
  void threeArguments();
  void voidFunc();
  void constFunc();

private:
  int test0() { return 0; }
  int test1(int a) { _iArg = a; return 1; }
  char test3(double a, unsigned int b, const QString& c) { _dArg = a; _uiArg = b; _strArg = c; return 'a';}
  void testVoid() { _bVoidCalled = true; }
  int plus(int a, int b) const { return a+b; }

  int _iArg;
  double _dArg;
  unsigned int _uiArg;
  QString _strArg;
  bool _bVoidCalled;
};


#endif //_TESTPIIGENERICFUNCTION_H
