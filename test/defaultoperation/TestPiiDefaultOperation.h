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

#ifndef _TESTPIIDEFAULTOPERATION_H
#define _TESTPIIDEFAULTOPERATION_H

#include <PiiProbeInput.h>
#include <PiiEngine.h>

#include "TestOperations.h"

class TestPiiDefaultOperation : public QObject
{
  Q_OBJECT

private slots:
  void initTestCase();
  void fullName();
  void connectedInputNames();
  void metaProperty();
  void process();
  void process_data();

private:
  enum { sequenceLength = 2048 };
  PiiEngine _engine;
  CounterOperation* _pCounter;
  BufferOperation* _pBuffer;
};


#endif //_TESTPIIDEFAULTOPERATION_H
