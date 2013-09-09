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

#include "PiiEventProcessor.h"
#include <QCoreApplication>

/* NOTE
 * If your compiler complains it cannot find these headers, make sure
 * $QTDIR/include and $QTDIR/include/Qt are in INCLUDEPATH. On Unix,
 * the "private" directory is not installed to $QTDIR/include/Qt by
 * default and needs to be symlinked to the sources. Go to
 * $QTDIR/include/Qt and make the link:
 *
 * ln -s relative/path/to/qt/sources/include/Qt/private/ .
 *
 * These headers are needed by a hack that works around a Qt crash by
 * altering a private member of QObject.
 */
#if (QT_VERSION < 0x040200)
#include <Qt/private/qobject_p.h>
#include <Qt/private/qthread_p.h>
#endif

QMutex PiiEventProcessor::_lock;

void PiiEventProcessor::stop()
{
  _lock.lock();
  _bRunning = false;
  _lock.unlock();
}

void PiiEventProcessor::startThread()
{
  _lock.lock();
  _bRunning = true;
  start();
  _lock.unlock();
}

void PiiEventProcessor::eventLoop()
{
  // An event loop that exits even if stop() was called before the
  // thread enters it.
  do
    {
      QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 100);
      msleep(10);
    }
  while (_bRunning);
}

void PiiEventProcessor::run()
{
  _lock.lock();
  bool bMainThread = false;
  // If QCoreApplication isn't instantiated yet, we need to create an instance
  if (!QCoreApplication::instance())
    {
      // This is the main thread
      bMainThread = true;
      int argc = 0;
      new QCoreApplication(argc, 0);
    }
  _lock.unlock();

  // Enter per-thread event loop
  eventLoop();
  _lock.lock();

  // Main thread dies
  if (bMainThread)
    delete QCoreApplication::instance();
  _lock.unlock();
}


void PiiEventProcessor::takeObject(QObject* obj)
{
  moveToThread(obj, this);
}

void PiiEventProcessor::moveToThread(QObject* obj, QThread* thread)
{
  // On versions starting from 4.2 we can move a non-threaded object
  // to the current thread first. On earlier versions, we need to
  // hack.
  if (obj->thread() == 0)
#if (QT_VERSION < 0x040200)
    moveToThread(obj, QThreadData::get(thread)->id);
  else
#else
    obj->moveToThread(QThread::currentThread());
#endif
  obj->moveToThread(thread);
}

#if (QT_VERSION < 0x040200)
/* HACK
 *
 * QObject::moveToThread() crashes if the current thread is not a
 * QThread.
 */
void PiiEventProcessor::moveToThread(QObject* obj, int newThreadId)
{
  QObjectPrivate* d = reinterpret_cast<QObjectPrivate**>(obj)[1];
  d->thread = newThreadId;
  for (int i=d->children.size(); i--; )
    moveToThread(d->children.at(i), newThreadId);
}
#endif

PiiEventProcessorRunner::PiiEventProcessorRunner()
{
  startThread();
}

PiiEventProcessorRunner::~PiiEventProcessorRunner()
{
  // Send stopping signal
  stop();
  // Wait until the thread is done before allowing destruction
  wait();
}

