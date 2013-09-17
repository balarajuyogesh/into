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

#ifndef _PIIEVENTPROCESSOR_H
#define _PIIEVENTPROCESSOR_H

#include "PiiGlobal.h"
#include <QThread>
#include <QMutex>

/**
 * A home-made event loop. PiiEventProcessor is useful if the main
 * thread of an application is not controlled by Qt. Since event
 * delivery depends on the existence of a QCoreApplication instance,
 * one needs to create a separate thread that plays the role of a main
 * thread and instantiates QCoreApplication.
 *
 * When started, PiiEventProcessor creates a new instance of
 * QCoreApplication if one does not already exist, and enters its
 * event loop. All subsequent instances of PiiEventProcessor enter a
 * thread-specific event loop. The main thread must not be stopped
 * until all other threads have finished their execution.
 *
 * ~~~
 * // In a non-Qt thread
 * QObject* obj = new QObject;
 * PiiEventProcessor p;
 * p.start();
 * p.takeObject(obj);
 *
 * // do whatever needed
 * p.stop();
 * delete obj;
 * ~~~
 *
 * Note that PiiEventProcessor also works with ordinary Qt
 * applications. In such a case, QCoreApplication will not be
 * instantiated again, and run() always enters a thread-specific event
 * loop.
 *
 */
class PII_CORE_EXPORT PiiEventProcessor : public QThread
{
  Q_OBJECT

public:
  /**
   * Create a new PiiEventProcessor instance.
   */
  PiiEventProcessor() : _bRunning(false) {}

  /**
   * Stops the thread. Calls QCoreApplication::exit() if this is the
   * main thread. Otherwise calls QThread::exit().
   */
  void stop();

  /**
   * Start the event loop in a new thread. Please do not call
   * QThread::start() directly. If you do, the thread exits
   * immediately.
   */
  void startThread();

  /**
   * Move an object to this thread's event loop. This is a work-around
   * for a Qt bug that causes a crash when moving an object from a
   * non-Qt thread. Use this function to enable event processing for
   * objects created in a non-Qt thread.
   *
   * @see moveToThread(QObject*, QThread*)
   */
  void takeObject(QObject* obj);

  /**
   * Move `obj` to `thread`. This is a work-around for a Qt bug that
   * causes a crash when trying to move an object away from a non-Qt
   * thread (fixed in 4.2). Use this function to enable event
   * processing for objects created in a non-Qt thread. The function
   * calls obj->moveToThread() if the object is already owned by a
   * QThread.  In this case, the restrictions of
   * QObject::moveToThread() apply. Otherwise, this function is only
   * ensured to work if
   *
   * - `obj` has no parent (it may have children, though).
   *
   * - there are no pending events for `obj`
   *
   * - the target thread's event loop is not processing an event
   * (the event loop is empty).
   *
   * This function is not thread-safe.
   */
  static void moveToThread(QObject* obj, QThread* thread);

protected:
  /**
   * Instantiates QCoreApplication if one does not already exist, and
   * enters the an event loop. Deletes the QCoreApplication instance
   * once the thread finishes, if one was created.
   */
  void run();
  
private:
  void eventLoop();
  
#if (QT_VERSION < 0x040200)
  static void moveToThread(QObject* obj, int newThreadId);
#endif

  volatile bool _bRunning;
  static QMutex _lock;
};

/**
 * A utility class that automatically starts the event processing
 * thread upon construction and stops it when destructed. An easy way
 * to create the main event loop in a non-Qt application is to create
 * a static instance of PiiEventProcessorRunner.
 *
 * ~~~
 * class MyClass
 * {
 * public:
 *   static PiiEventProcessorRunner mainThread;
 * };
 * ~~~
 *
 * Now, event processing can be enabled by invoking
 * MyClass::mainThread.takeObject(obj).
 *
 */
class PII_CORE_EXPORT PiiEventProcessorRunner : public PiiEventProcessor
{
  Q_OBJECT
  
public:
  /**
   * Creates a new event processing thread and starts it.
   */
  PiiEventProcessorRunner();
  /**
   * Stops the thread and waits until it is finished.
   */
  ~PiiEventProcessorRunner();
};


#endif //_PIIEVENTPROCESSOR_H
