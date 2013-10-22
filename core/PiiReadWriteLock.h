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

#ifndef _PIIREADWRITELOCK_H
#define _PIIREADWRITELOCK_H

#include "PiiGlobal.h"

#include <QMutex>
#include <QWaitCondition>

/**
 * A read-write lock. This class provides functionality similar to
 * QReadWriteLock and has a similar interface. The difference is that
 * PiiReadWriteLock allows recursive locks to be re-acquired in
 * different mode. It is possible to lock for writing in a thread that
 * has already locked for reading and vice-versa.
 *
 * Note that there is no unlock() function. Instead, a read lock must
 * be released with unlockRead() and a write lock with unlockWrite().
 *
 */
class PII_CORE_EXPORT PiiReadWriteLock
{
public:
  enum RecursionMode { Recursive, NonRecursive };

  PiiReadWriteLock();
  PiiReadWriteLock(RecursionMode mode);
  ~PiiReadWriteLock();

  void lockForRead();
  void lockForWrite();
  void unlockRead();
  void unlockWrite();

private:
  typedef QHash<Qt::HANDLE, int> ThreadHash;
  class Data
  {
  public:
    Data (bool recursive);
    
    QMutex mutex;
    QWaitCondition readerWait, writerWait;
    ThreadHash hashCurrentReaders;
    Qt::HANDLE currentWriter;
    bool bRecursive;
    int iActiveReaders, iActiveWriters, iWaitingReaders, iWaitingWriters;
  } *d;

  inline void wakeUp();

  PII_DISABLE_COPY(PiiReadWriteLock);
};

/// @internal
class PII_CORE_EXPORT PiiReadLocker
{
public:
  PiiReadLocker(PiiReadWriteLock* lock) : _pLock(lock)
  {
    _pLock->lockForRead();
  }

  ~PiiReadLocker()
  {
    _pLock->unlockRead();
  }
  
private:
  PiiReadWriteLock* _pLock;
  PII_DISABLE_COPY(PiiReadLocker);
};

/// @internal
class PII_CORE_EXPORT PiiWriteLocker
{
public:
  PiiWriteLocker(PiiReadWriteLock* lock) : _pLock(lock)
  {
    _pLock->lockForWrite();
  }

  ~PiiWriteLocker()
  {
    _pLock->unlockWrite();
  }
  
private:
  PiiReadWriteLock* _pLock;
  PII_DISABLE_COPY(PiiWriteLocker);
};

#endif //_PIIREADWRITELOCK_H
