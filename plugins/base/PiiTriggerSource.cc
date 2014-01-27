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

#include "PiiTriggerSource.h"
#include <PiiYdinTypes.h>
#include <QMutexLocker>

PiiTriggerSource::Data::Data()
{
}

PiiTriggerSource::PiiTriggerSource() : PiiBasicOperation(new Data)
{
  addSocket(new PiiOutputSocket("trigger"));
}

template <class T> void PiiTriggerSource::emitValue(T value)
{
  QMutexLocker lock(&_d()->stateMutex);

  if (state() == Running)
    emitObject(value);
}

void PiiTriggerSource::trigger(const PiiVariant& value)
{
  emitValue(value);
}

void PiiTriggerSource::trigger()
{
  emitValue(1);
}

void PiiTriggerSource::trigger(int value)
{
  emitValue(value);
}

void PiiTriggerSource::trigger(double value)
{
  emitValue(value);
}

void PiiTriggerSource::trigger(const QString& value)
{
  emitValue(value);
}

void PiiTriggerSource::trigger(const QPoint& value)
{
  emitValue(PiiMatrix<int>(1, 2, value.x(), value.y()));
}

void PiiTriggerSource::trigger(const QPointF& value)
{
  emitValue(PiiMatrix<double>(1, 2, value.x(), value.y()));
}

void PiiTriggerSource::trigger(const QRect& value)
{
  emitValue(PiiMatrix<int>(1, 4, value.x(), value.y(), value.width(), value.height()));
}

void PiiTriggerSource::trigger(const QRectF& value)
{
  emitValue(PiiMatrix<double>(1, 4, value.x(), value.y(), value.width(), value.height()));
}

void PiiTriggerSource::stop()
{
  PII_D;

  d->stateMutex.lock();
  if (state() == Running)
    {
      setState(Stopped);
      d->stateMutex.unlock();

      emitObject(PiiYdin::createStopTag());
    }
  else
    d->stateMutex.unlock();
}

void PiiTriggerSource::pause()
{
  PII_D;

  d->stateMutex.lock();
  if (state() == Running)
    {
      setState(Paused);
      d->stateMutex.unlock();

      emitObject(PiiYdin::createPauseTag());
    }
  else
    d->stateMutex.unlock();

}

void PiiTriggerSource::start()
{
  QMutexLocker lock(stateLock());

  if (state() == Paused)
    outputAt(0)->resume(PiiSocketState());

  setState(Running);
}

void PiiTriggerSource::interrupt()
{
  QMutexLocker lock(stateLock());

  if (state() != Stopped)
    {
      interruptOutputs();
      setState(Stopped);
    }
}

void PiiTriggerSource::reconfigure(const QString& propertySetName)
{
  try
    {
      PiiBasicOperation::applyPropertySet(propertySetName);
    }
  catch (PiiExecutionException& ex)
    {
      emit errorOccured(this, tr("Reconfiguring %1 failed. %2")
                        .arg(metaObject()->className()).arg(ex.message()));
    }
}
