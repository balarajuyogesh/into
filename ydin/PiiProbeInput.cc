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
#include <QTime>
#include <PiiThreadSafeTimer.h>

#include "PiiYdinTypes.h"

static int iProbeInputMetaType = qRegisterMetaType<PiiProbeInput*>("PiiProbeInput*");

class PiiProbeInput::Data :
  public PiiAbstractInputSocket::Data,
  public PiiInputController
{
public:
  Data(PiiProbeInput* owner) :
    q(owner),
    bDiscardControlObjects(false),
    bEnoughTimeElapsed(true),
    bObjectPending(false),
    iSignalInterval(0)
  {
    emissionTimer.setSingleShot(true);
  }

  /*
   * Emits [objectReceived()] and saves the received object.
   */
  bool tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ();

  PiiProbeInput* q;
  PiiVariant varSavedObject;
  bool bDiscardControlObjects;
  bool bEnoughTimeElapsed;
  bool bObjectPending;
  int iSignalInterval;
  PiiThreadSafeTimer emissionTimer;
  QMutex mutex;
};

PiiProbeInput::PiiProbeInput(const QString& name) :
  PiiAbstractInputSocket(name, new Data(this))
{
  Q_UNUSED(iProbeInputMetaType);
  connect(&_d()->emissionTimer, SIGNAL(timeout()), SLOT(emitPendingObject()));
}

PiiProbeInput::PiiProbeInput(PiiAbstractOutputSocket* output, const QObject* receiver,
                             const char* slot, Qt::ConnectionType type) :
  PiiAbstractInputSocket("probe", new Data(this))
{
  connectOutput(output);
  connect(this, SIGNAL(objectReceived(PiiVariant,PiiProbeInput*)), receiver, slot, type);
  connect(&_d()->emissionTimer, SIGNAL(timeout()), SLOT(emitPendingObject()));
}

PiiProbeInput::~PiiProbeInput()
{}

void PiiProbeInput::emitPendingObject()
{
  PII_D;
  d->mutex.lock();
  // Timer fired, but no new objects were received meanwhile.
  if (!d->bObjectPending)
    {
      d->bEnoughTimeElapsed = true;
      d->mutex.unlock();
      return;
    }
  PiiVariant varSavedObject = d->varSavedObject;
  d->bObjectPending = false;
  d->mutex.unlock();

  emit objectReceived(varSavedObject, this);

  d->emissionTimer.start();
}

bool PiiProbeInput::Data::tryToReceive(PiiAbstractInputSocket*, const PiiVariant& object) throw ()
{
  if (!(bDiscardControlObjects && PiiYdin::isControlType(object.type())))
    {
      mutex.lock();
      varSavedObject = object;

      if (iSignalInterval > 0)
        {
          if (bEnoughTimeElapsed)
            {
              bEnoughTimeElapsed = false;
              bObjectPending = false;
              mutex.unlock();
              emit q->objectReceived(object, q);
              emissionTimer.start();
              return true;
            }
          else
            bObjectPending = true;
        }
      else if (iSignalInterval == 0)
        {
          mutex.unlock();
          emit q->objectReceived(object, q);
          return true;
        }
      mutex.unlock();
    }
  return true;
}

PiiInputController* PiiProbeInput::controller() const { return const_cast<Data*>(_d()); }

PiiVariant PiiProbeInput::savedObject() const { return _d()->varSavedObject; }
void PiiProbeInput::setSavedObject(const PiiVariant& obj) { _d()->varSavedObject = obj; }
bool PiiProbeInput::hasSavedObject() const { return _d()->varSavedObject.isValid(); }

void PiiProbeInput::setDiscardControlObjects(bool discardControlObjects) { _d()->bDiscardControlObjects = discardControlObjects; }
bool PiiProbeInput::discardControlObjects() const { return _d()->bDiscardControlObjects; }

void PiiProbeInput::setSignalInterval(int signalInterval)
{
  PII_D;
  d->iSignalInterval = signalInterval;
  d->emissionTimer.setInterval(signalInterval);
}
int PiiProbeInput::signalInterval() const { return _d()->iSignalInterval; }
