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

#include "PiiWaitCondition.h"

PiiWaitCondition::PiiWaitCondition(QueueMode mode) :
  _bQueue(mode == Queue), _iWaiters(0), _iWakeSignals(0)
{ }

bool PiiWaitCondition::wait(unsigned long time)
{
  //serialize access to _iWakeSignals and _iWaiters
  QMutexLocker locker(&_mutex);

  //see if we have already been signalled
  if (_iWakeSignals > 0)
    {
      //we are queuing signals -> just decrement by one
      if (_bQueue)
        _iWakeSignals--;
      //we just need one signal -> clear the queue
      else
        _iWakeSignals = 0;
      return true;
    }
  _iWaiters++;

  //Unlock the mutex and wait for a signal. The mutex is atomically
  //unlocked at the time this thread enters suspended mode and
  //atomically locked again when the suspension ends.
  return _condition.wait(&_mutex, time);
}

void PiiWaitCondition::wakeOne()
{
  _mutex.lock();
  //if no thread is waiting, build up a queue
  if (_iWaiters == 0)
    _iWakeSignals++;
  else
    {
      // Somebody is waiting: wake one up
      _condition.wakeOne();
      // Decrement the waiter count. In some rare cases, wait()
      // doesn't return immediately after wakeOne(), but multiple
      // wakeOne() calls may be invoked before wait() finishes.
      // Therefore, the waiter count must be decreased before the
      // wait() call actually returns.
      _iWaiters--;
    }
  _mutex.unlock();
}

void PiiWaitCondition::wakeAll()
{
  _mutex.lock();
  //release all waiting threads
  _condition.wakeAll();
  //make sure no signals are left in the queue
  _iWakeSignals = 0;
  _iWaiters = 0;
  _mutex.unlock();
}
