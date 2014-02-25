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

#ifndef _TESTOPERATION_H
#define _TESTOPERATION_H

#include <PiiDefaultOperation.h>

class TestOperation : public PiiDefaultOperation
{
  Q_OBJECT

public:
  TestOperation() :
    bFail(false),
    _iCount(0)
  {
    addSocket(new PiiInputSocket("input"));
    addSocket(new PiiOutputSocket("output"));
    inputAt(0)->setOptional(true);
  }
  QVariant socketData(PiiSocket*, int role) const { return role; }

  void check(bool reset)
  {
    if (bFail)
      PII_THROW(PiiExecutionException, "");
    PiiDefaultOperation::check(reset);
    _iCount = 0;
  }

  bool bFail;

protected:
  void process()
  {
    if (inputAt(0)->isConnected())
      emitObject(readInput());
    else
      emitObject(_iCount++);
  }
private:
  int _iCount;
};

#endif //_TESTOPERATION_H
