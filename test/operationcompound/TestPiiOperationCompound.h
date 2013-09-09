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

#ifndef _TESTPIIOPERATIONCOMPOUND_H
#define _TESTPIIOPERATIONCOMPOUND_H

#include <PiiOperationTest.h>
#include <PiiOperationCompound.h>

class TestPiiOperationCompound : public PiiOperationTest
{
  Q_OBJECT

private slots:
  void initTestCase();
  void serialize();
  void clone();
  void cleanupTestCase();

private:
  PiiOperationCompound _compound;
};


#endif //_TESTPIIOPERATIONCOMPOUND_H
