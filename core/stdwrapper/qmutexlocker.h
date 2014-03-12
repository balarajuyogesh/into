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

#ifndef _QMUTEXLOCKER_H
#define _QMUTEXLOCKER_H

#include <PiiGlobal.h>
#include "qmutex.h"

class PII_CORE_EXPORT QMutexLocker
{
public:
  QMutexLocker(QMutex* mutex) : _pMutex(mutex) { mutex->lock(); }
  ~QMutexLocker() { _pMutex->unlock(); }
  QMutex* mutex() { return _pMutex; }
private:
  PII_DISABLE_COPY(QMutexLocker);
  QMutex* _pMutex;
};

#endif //_QMUTEXLOCKER_H
