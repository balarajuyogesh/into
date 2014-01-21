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

#ifndef _PIITIMER_H
#define _PIITIMER_H

#include "PiiGlobal.h"

/**
 * PiiTimer uses QTime to achieve microsecond-resolution timing of
 * intervals. The functionality is almost identical to QTime, but this
 * class provides microsecond-resolution timing (on systems that
 * support such resolution) and adds a stop() function that can be
 * used to stop counting.
 *
 */
class PII_CORE_EXPORT PiiTimer
{
public:
  /**
   * Constructs a new timer that is initialized to the system's
   * current time. The timer starts counting automatically.
   */
  PiiTimer();

  PiiTimer(const PiiTimer& other);

  PiiTimer& operator= (const PiiTimer& other);

  ~PiiTimer();

  /**
   * Restarts counting. Returns the number of microseconds elapsed
   * since the last restart or construction of the object, whichever
   * is later.
   */
  qint64 restart();

  /**
   * Stops counting. When the timer is stopped, elapsed time is always
   * 0. One needs to call [restart()] to restart counting.
   */
  void stop();

  /**
   * Returns the time elapsed from the construction of this object or
   * the previous restart() call, whichever is later. Time is measured
   * in microseconds. If the timer is stopped, returns 0.
   */
  qint64 microseconds() const;

  /**
   * Returns the elapsed time in milliseconds.
   */
  qint64 milliseconds() const;

  /**
   * Returns the elapsed time in seconds.
   */
  double seconds() const;

  /**
   * Returns the elapsed time in minutes.
   */
  double minutes() const;

  bool isRunning() const;

private:
  class Data;
  Data* d;
};

#endif //_PIITIMER_H
