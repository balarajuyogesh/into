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

#include "PiiProbeInput.h"

#include <QMutex>
#include <PiiTimer.h>

#include "PiiYdinTypes.h"

static int iProbeInputMetaType = qRegisterMetaType<PiiProbeInput*>("PiiProbeInput*");

class PiiProbeInput::Data : public PiiAbstractInputSocket::Data
{
public:
  Data() :
    PiiAbstractInputSocket::Data(),
    bDiscardControlObjects(false),
    iSignalInterval(0),
    iTimerId(-1)
  {}

  PiiVariant varSavedObject;
  bool bDiscardControlObjects;
  int iSignalInterval;
  int iTimerId;
  PiiTimer emissionTimer;
  QMutex mutex;
};

PiiProbeInput::PiiProbeInput(const QString& name) :
  PiiAbstractInputSocket(name, new Data)
{
  Q_UNUSED(iProbeInputMetaType);
}

PiiProbeInput::PiiProbeInput(PiiAbstractOutputSocket* output, const QObject* receiver,
                             const char* slot, Qt::ConnectionType type) :
  PiiAbstractInputSocket("probe", new Data)
{
  connectOutput(output);
  QObject::connect(this, SIGNAL(objectReceived(PiiVariant,PiiProbeInput*)), receiver, slot, type);
}

PiiProbeInput::~PiiProbeInput()
{}

void PiiProbeInput::timerEvent(QTimerEvent*)
{
  PII_D;
  d->mutex.lock();
  PiiVariant varSavedObject = d->varSavedObject;
  d->emissionTimer.restart();
  d->mutex.unlock();

  emit objectReceived(varSavedObject, this);
}

bool PiiProbeInput::tryToReceive(PiiAbstractInputSocket*, const PiiVariant& object) throw ()
{
  PII_D;
  if (!(d->bDiscardControlObjects && PiiYdin::isControlType(object.type())))
    {
      d->mutex.lock();
      d->varSavedObject = object;
      // Negative interval means no signals whatsoever
      if (d->iSignalInterval >= 0)
        {
          // If enough time has passed since the last emission, emit
          // again.
          qint64 iElapsed = d->emissionTimer.milliseconds();
          if (iElapsed >= d->iSignalInterval)
            {
              // If we had a timer running for a pending emission, kill it
              // now.
              if (d->iTimerId != -1)
                killTimer(d->iTimerId);
              d->emissionTimer.restart();
              d->mutex.unlock();
              emit objectReceived(object, this);
              return true;
            }
          // Oops, too fast. Do we already have a timer running?
          else if (d->iTimerId != -1)
            {
              // No, start it
              d->iTimerId = startTimer(d->iSignalInterval - iElapsed, Qt::PreciseTimer);
            }
        }
      d->mutex.unlock();
    }
  return true;
}

PiiInputController* PiiProbeInput::controller() const
{
  return const_cast<PiiProbeInput*>(this);
}

PiiVariant PiiProbeInput::savedObject() const { return _d()->varSavedObject; }
void PiiProbeInput::setSavedObject(const PiiVariant& obj) { _d()->varSavedObject = obj; }
bool PiiProbeInput::hasSavedObject() const { return _d()->varSavedObject.isValid(); }

void PiiProbeInput::setDiscardControlObjects(bool discardControlObjects) { _d()->bDiscardControlObjects = discardControlObjects; }
bool PiiProbeInput::discardControlObjects() const { return _d()->bDiscardControlObjects; }

void PiiProbeInput::setSignalInterval(int signalInterval) { _d()->iSignalInterval = qMax(-1, signalInterval); }
int PiiProbeInput::signalInterval() const { return _d()->iSignalInterval; }
