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

#include "PiiThreadSafeTimer.h"

#include <QDateTime>

PiiTimerThread* PiiThreadSafeTimer::_pTimerThread = 0;
QMutex PiiThreadSafeTimer::_threadMutex;
int PiiThreadSafeTimer::_iInstanceCount = 0;

class PiiThreadSafeTimer::Data
{
public:
  Data() :
    bSingleShot(false),
    iNextFiringTime(0),
    iInterval(0)
  {}

  bool bSingleShot;
  qint64 iNextFiringTime;
  int iInterval;
};

PiiThreadSafeTimer::PiiThreadSafeTimer(QObject* parent) :
  QObject(parent),
  d(new Data)
{
  synchronized (_threadMutex)
    if (++_iInstanceCount == 1)
      startThread();
}

PiiThreadSafeTimer::~PiiThreadSafeTimer()
{
  stop();
  synchronized (_threadMutex)
    if (--_iInstanceCount == 0)
      stopThread();
  delete d;
}

void PiiThreadSafeTimer::startThread()
{
  _pTimerThread = new PiiTimerThread;
  synchronized (_pTimerThread->_eventMapMutex)
    {
      _pTimerThread->start();
      _pTimerThread->_eventCondition.wait(&_pTimerThread->_eventMapMutex);
    }
}

void PiiThreadSafeTimer::stopThread()
{
  PiiTimerThread* pThread = _pTimerThread;
  synchronized (_pTimerThread->_eventMapMutex)
    {
      _pTimerThread = 0;
      pThread->_eventCondition.wakeOne();
    }
  pThread->wait();
  delete pThread;
}

void PiiThreadSafeTimer::start(int interval)
{
  synchronized (_pTimerThread->_eventMapMutex)
    {
      if (isActive())
        _pTimerThread->removeEvent(this);
      d->iInterval = interval;
      _pTimerThread->addEvent(this);
    }
}

void PiiThreadSafeTimer::start()
{
  synchronized (_pTimerThread->_eventMapMutex)
    _pTimerThread->addEvent(this);
}

void PiiThreadSafeTimer::stop()
{
  synchronized (_pTimerThread->_eventMapMutex)
    _pTimerThread->removeEvent(this);
}

void PiiThreadSafeTimer::setSingleShot(bool singleShot)
{
  synchronized (_pTimerThread->_eventMapMutex)
    d->bSingleShot = singleShot;
}
bool PiiThreadSafeTimer::isSingleShot() const { return d->bSingleShot; }
void PiiThreadSafeTimer::setInterval(int interval)
{
  synchronized (_pTimerThread->_eventMapMutex)
    {
      if (isActive())
        {
          _pTimerThread->removeEvent(this);
          d->iInterval = qMax(0, interval);
          _pTimerThread->addEvent(this);
        }
      else
        d->iInterval = qMax(0, interval);
    }
}
int PiiThreadSafeTimer::interval() const { return d->iInterval; }
bool PiiThreadSafeTimer::isActive() const { return d->iNextFiringTime != 0; }
int PiiThreadSafeTimer::remainingTime() const
{
  if (d->iNextFiringTime == 0) return -1;
  return qMax(0, int(d->iNextFiringTime - PiiTimerThread::currentTime()));
}

qint64 PiiTimerThread::currentTime()
{
  return QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void PiiTimerThread::run()
{
  _eventMapMutex.lock();
  _eventCondition.wakeOne();
  unsigned long ulTimeToNextEvent = ULONG_MAX;
  forever
    {
      _eventCondition.wait(&_eventMapMutex, ulTimeToNextEvent);
      if (!PiiThreadSafeTimer::_pTimerThread)
        break;
      qint64 iCurrentTime = currentTime();
      QMultiMap<qint64,PiiThreadSafeTimer*>::iterator it = _mapEvents.begin();
      // Send all events whose time has passed
      while (it != _mapEvents.end() && it.key() <= iCurrentTime)
        {
          qint64 iTime = it.key();
          PiiThreadSafeTimer* pTimer = it.value();
          _mapEvents.erase(it);

          emit pTimer->timeout();

          // If this timer is a repeating one, add a new event to
          // queue.
          if (!pTimer->isSingleShot())
            _mapEvents.insert(pTimer->d->iNextFiringTime = iTime + pTimer->interval(), pTimer);
          else
            pTimer->d->iNextFiringTime = 0;

          it = _mapEvents.begin();
        }
      // If there are no more events to handle, wait until a new one
      // is added to the queue.
      if (it == _mapEvents.end())
        ulTimeToNextEvent = ULONG_MAX;
      // Otherwise wait just enough.
      else
        ulTimeToNextEvent = qMax(0ll, it.key() - iCurrentTime);
    }
  _eventMapMutex.unlock();
}

// _eventMapMutex must be locked when calling this function
void PiiTimerThread::addEvent(PiiThreadSafeTimer* timer)
{
  qint64 iFiringTime = currentTime() + timer->d->iInterval;
  // If this event is the next one in the queue, wake up the sender thread.
  if (_mapEvents.isEmpty() || _mapEvents.constBegin().key() > iFiringTime)
    _eventCondition.wakeOne();
  _mapEvents.insert(timer->d->iNextFiringTime = iFiringTime, timer);
}

// _eventMapMutex must be locked when calling this function
void PiiTimerThread::removeEvent(PiiThreadSafeTimer* timer)
{
  QMultiMap<qint64,PiiThreadSafeTimer*>::iterator it = _mapEvents.find(timer->d->iNextFiringTime, timer);
  if (it != _mapEvents.end())
    {
      _mapEvents.erase(it);
      timer->d->iNextFiringTime = 0;
    }
}
