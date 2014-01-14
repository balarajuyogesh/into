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

#ifndef _PIISYNCHRONIZED_H
#define _PIISYNCHRONIZED_H

#include "PiiPreprocessor.h"
#include <QMutex>

/// @hide

struct PII_CORE_EXPORT PiiMutexLocker
{
  explicit inline PiiMutexLocker(const QMutex& mutex) :
    pMutex(const_cast<QMutex*>(&mutex)), bLocked(true)
  {
    pMutex->lock();
  }
    
  explicit inline PiiMutexLocker(const QMutex* mutex) :
    pMutex(const_cast<QMutex*>(mutex)), bLocked(true)
  {
    pMutex->lock();
  }

  ~PiiMutexLocker() { pMutex->unlock(); }

  operator bool () const { return bLocked; }

  void setUnlocked() { bLocked = false; }

  QMutex* pMutex;
  bool bLocked;
};

#define PII_SYNCHRONIZED(LOCK,MUTEX) for (PiiMutexLocker LOCK(MUTEX); LOCK; LOCK.setUnlocked())

/// @endhide

/**
 * Declares a critical section in which mutual exclusion is handled by
 * the given *MUTEX*. Critical sections protected by the same mutex
 * won't be executed simultaneously.
 *
 * ~~~(c++)
 * void MyOperation::pause()
 * {
 *   synchronized (stateLock())
 *     {
 *       if (state() == Running)
 *         {
 *           piiDebug("I'm pausing!");
 *           setState(Pausing);
 *         }
 *     }
 * }
 * ~~~
 */
#define synchronized(MUTEX) PII_SYNCHRONIZED(PII_JOIN(piiSyncronizedLock,__LINE__), MUTEX)

#endif //_PIISYNCHRONIZED_H
