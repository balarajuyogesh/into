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

#ifndef _TESTPIIPISOOPERATION_H
#define _TESTPIIPISOOPERATION_H

#include <PiiOperationTest.h>

class TestPiiPisoOperation : public PiiOperationTest
{
  Q_OBJECT

private slots:
  void initTestCase();
  void groupedMode();
  void nonGroupedMode();

private:
  void ensureOutput(int group, int v0, int v1, int v2);
  void ensureOutput(int group, int v);
};


#endif //_TESTPIIPISOOPERATION_H
