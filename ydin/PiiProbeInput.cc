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

PiiProbeInput::Data::Data() :
  PiiAbstractInputSocket::Data()
{}

PiiProbeInput::PiiProbeInput(const QString& name) :
  PiiAbstractInputSocket(name, new Data)
{}

PiiProbeInput::PiiProbeInput(PiiAbstractOutputSocket* output, const QObject* receiver,
                             const char* slot, Qt::ConnectionType type) :
  PiiAbstractInputSocket("probe", new Data)
{
  connectOutput(output);
  QObject::connect(this, SIGNAL(objectReceived(PiiVariant)), receiver, slot, type);
}

bool PiiProbeInput::tryToReceive(PiiAbstractInputSocket*, const PiiVariant& object) throw ()
{
  emit objectReceived(object);
  _d()->varSavedObject = object;
  return true;
}

PiiInputController* PiiProbeInput::controller() const
{
  return const_cast<PiiProbeInput*>(this);
}

PiiVariant PiiProbeInput::savedObject() const { return _d()->varSavedObject; }
void PiiProbeInput::setSavedObject(const PiiVariant& obj) { _d()->varSavedObject = obj; }
bool PiiProbeInput::hasSavedObject() const { return _d()->varSavedObject.isValid(); }
