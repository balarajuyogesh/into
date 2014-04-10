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

#ifndef _PIITHREADSAFETIMER_H
#define _PIITHREADSAFETIMER_H

#include "PiiGlobal.h"
#include <QObject>
#include <QThread>
#include <QMultiMap>
#include <QMutex>
#include <QWaitCondition>

class PiiTimerThread;

/**
 * A thread-safe alternative to [QTimer]. PiiThreadSafeTimer creates
 * a global timer thread that emits [timeout()] signals for all
 * registered timers. This class provides a thread-safe API for
 * creating timer events.
 */
class PII_CORE_EXPORT PiiThreadSafeTimer : public QObject
{
  Q_OBJECT
  /**
   * If this property is `true`, the timer will fire only once and
   * automatically turn inactive. The default behavior is to fire
   * repeatedly until explicitly stopped.
   */
  Q_PROPERTY(bool singleShot READ isSingleShot WRITE setSingleShot);
  /**
   * The timeout interval in milliseconds. If the interval changed
   * while the timer is active, any pending timeout will be discarded.
   * The default value is 0.
   */
  Q_PROPERTY(int interval READ interval WRITE setInterval);
  /**
   * This property is `true` if the timer is running and `false`
   * otherwise.
   */
  Q_PROPERTY(bool active READ isActive);
  /**
   * The number of milliseconds to the next timeout. If a timeout
   * signal is overdue, this property is zero. If the timer is not
   * active, this property is -1.
   */
  Q_PROPERTY(int remainingTime READ remainingTime);

public:
  PiiThreadSafeTimer(QObject* parent = 0);
  ~PiiThreadSafeTimer();

  void setSingleShot(bool singleShot);
  bool isSingleShot() const;
  void setInterval(int interval);
  int interval() const;
  bool isActive() const;
  int remainingTime() const;

public slots:
  /**
   * Sets the interval to *interval* and starts the timer.
   */
  void start(int interval);
  void start();
  void stop();

signals:
  /**
   * This signal is emitted when the timer times out. To avoid
   * blocking the global timer thread, it is advisable not to connect
   * to this signal using `Qt::DirectConnection`. If you do so and try
   * to modify the emitting timer in a slot, the program will
   * deadlock.
   */
  void timeout();

private:
  class Data;
  friend class PiiTimerThread;
  static void startThread();
  static void stopThread();
  static PiiTimerThread* _pTimerThread;
  static QMutex _threadMutex;
  static int _iInstanceCount;
  Data* d;
};

/// @internal
class PiiTimerThread : public QThread
{
  Q_OBJECT
public:
  void addEvent(PiiThreadSafeTimer* timer);
  void removeEvent(PiiThreadSafeTimer* timer);
  void run();
  static inline qint64 currentTime();
  QMutex _eventMapMutex;
  QWaitCondition _eventCondition;
  QMultiMap<qint64,PiiThreadSafeTimer*> _mapEvents;
};

#endif //_PIITHREADSAFETIMER_H
