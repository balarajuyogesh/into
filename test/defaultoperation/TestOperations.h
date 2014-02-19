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

#ifndef _TESTOPERATION_H
#define _TESTOPERATION_H

#include <PiiDefaultOperation.h>

class CounterOperation : public PiiDefaultOperation
{
  Q_OBJECT

  Q_PROPERTY(int prop1 READ prop1 WRITE setProp1);
  Q_CLASSINFO("prop1.range", "0:");

  Q_PROPERTY(double prop2 READ prop2 WRITE setProp2);
  Q_CLASSINFO("prop2.range", "0.5:1.5:6.0");

  Q_PROPERTY(int prop3 READ prop3 WRITE setProp3);
  Q_CLASSINFO("prop3.range", ":255");

  Q_PROPERTY(int prop4 READ prop4 WRITE setProp4);
  Q_CLASSINFO("prop4.range", "-10:2:10");

public:
  CounterOperation();

  void setProp1(int prop1) { _iProp1 = prop1; }
  int prop1() const { return _iProp1; }
  void setProp2(double prop2) { _dProp2 = prop2; }
  double prop2() const { return _dProp2; }
  void setProp3(int prop3) { _iProp3 = prop3; }
  int prop3() const { return _iProp3; }
  void setProp4(int prop4) { _iProp4 = prop4; }
  int prop4() const { return _iProp4; }

protected:
  void process();
  int _iProp1;
  double _dProp2;
  int _iProp3;
  int _iProp4;
};

class BufferOperation : public PiiDefaultOperation
{
  Q_OBJECT
public:
  BufferOperation();

  QList<QPair<int,int> > lstData;

protected:
  void process();
};



#endif //_TESTOPERATION_H
