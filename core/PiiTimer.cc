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

#include "PiiTimer.h"
#include <ctime>

#if defined(Q_OS_WIN)
#include <windows.h>

#ifndef CLOCK_MONOTONIC
#  define CLOCK_MONOTONIC 0
#endif

static LARGE_INTEGER getFILETIMEoffset()
{
  SYSTEMTIME s;
  FILETIME f;
  LARGE_INTEGER t;

  s.wYear = 1970;
  s.wMonth = 1;
  s.wDay = 1;
  s.wHour = 0;
  s.wMinute = 0;
  s.wSecond = 0;
  s.wMilliseconds = 0;
  SystemTimeToFileTime(&s, &f);
  t.QuadPart = f.dwHighDateTime;
  t.QuadPart <<= 32;
  t.QuadPart |= f.dwLowDateTime;
  return t;
}

#if QT_VERSION < 0x050000

struct timespec
{
  time_t tv_sec;
  long tv_nsec;
};
#endif

struct PiiPerformanceCounter
{
  PiiPerformanceCounter()
  {
    bUsePerformanceCounter = false; //QueryPerformanceFrequency(&ticsPerSec);
    if (bUsePerformanceCounter)
      {
        QueryPerformanceCounter(&offset);
        dNsecsPerTic = 1000000000.0 / double(ticsPerSec.QuadPart);
      }
    else
      {
        offset = getFILETIMEoffset();
        dNsecsPerTic = 100;
        ticsPerSec.QuadPart = 10000000;
      }
  }

  void query(timespec* tp)
  {
    LARGE_INTEGER t;
    if (bUsePerformanceCounter)
      QueryPerformanceCounter(&t);
    else
      {
        FILETIME f;
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
      }
    t.QuadPart -= offset.QuadPart;
    tp->tv_sec = t.QuadPart / ticsPerSec.QuadPart;
    tp->tv_nsec = long(double(t.QuadPart % ticsPerSec.QuadPart) * dNsecsPerTic + 0.5);
  }

  bool bUsePerformanceCounter;
  double dNsecsPerTic;
  LARGE_INTEGER offset, ticsPerSec;
};

namespace Pii { static PiiPerformanceCounter performanceCounter; }

int clock_gettime(int, timespec* tp)
{
  Pii::performanceCounter.query(tp);
  return 0;
}
#elif defined(Q_OS_MAC)
#include <mach/mach_time.h>
#define CLOCK_MONOTONIC 0

struct PiiTimeBaseInfo
{
  PiiTimeBaseInfo() :
   uiOffset(mach_absolute_time())
  {
    mach_timebase_info(&data);
  }
  quint64 uiOffset;
  mach_timebase_info_data_t data;
};

namespace Pii { static PiiTimeBaseInfo timeBaseInfo; }

int clock_gettime(int, timespec* tp)
{
  quint64 uiTime = (mach_absolute_time() - Pii::timeBaseInfo.uiOffset) *
    Pii::timeBaseInfo.data.numer / Pii::timeBaseInfo.data.denom;
  tp->tv_sec = uiTime / 1000000000;
  tp->tv_nsec = uiTime % 1000000000;
  return 0;
}
#endif

class PiiTimer::Data
{
public:
  Data() :
    bStopped(false)
  {
    clock_gettime(CLOCK_MONOTONIC, &time);
  }
  Data(const Data& other) :
    time(other.time),
    bStopped(other.bStopped)
  {}

  inline qint64 usecsTo(timespec *t) const
  {
    /*qDebug("%d - %d = %d\n"
           "%d - %d = %d",
           t->tv_sec, time.tv_sec, t->tv_sec - time.tv_sec,
           t->tv_nsec, time.tv_nsec, t->tv_nsec - time.tv_nsec);
    */
    return qint64(t->tv_sec - time.tv_sec) * 1000000 + qint64(t->tv_nsec - time.tv_nsec) / 1000;
  }

  timespec time;
  bool bStopped;
};

PiiTimer::PiiTimer() :
  d(new Data)
{
}

PiiTimer::PiiTimer(const PiiTimer& other) :
  d(new Data(*other.d))
{
}

PiiTimer& PiiTimer::operator= (const PiiTimer& other)
{
  d->time = other.d->time;
  d->bStopped = other.d->bStopped;
  return *this;
}

PiiTimer::~PiiTimer()
{
  delete d;
}

bool PiiTimer::isRunning() const { return !d->bStopped; }

qint64 PiiTimer::restart()
{
  int iUsecs = 0;
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  if (!d->bStopped)
    iUsecs = d->usecsTo(&t);
  else
    d->bStopped = false;
  d->time = t;
  return iUsecs;
}

void PiiTimer::stop()
{
  d->bStopped = true;
}

qint64 PiiTimer::microseconds() const
{
  if (d->bStopped) return 0;
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return d->usecsTo(&t);
}

qint64 PiiTimer::milliseconds() const
{
  return microseconds()/1000;
}

double PiiTimer::seconds() const
{
  return (double)microseconds()/1000000.0;
}

double PiiTimer::minutes() const
{
  return (double)microseconds()/60000000.0;
}
