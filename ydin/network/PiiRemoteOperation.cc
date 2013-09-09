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

#if 0
#include "PiiRemoteOperation.h"

PiiRemoteOperation::Data::Data() :
  bConfigured(false)
{}

PiiRemoteOperation::PiiRemoteOperation()
{
}

PiiRemoteOperation::PiiRemoteOperation(const QString& serverUri) :
  PiiRemoteObject(serverUri)
{
  configure(); // may throw
}

PiiRemoteOperation::~PiiRemoteOperation()
{
}

void PiiRemoteOperation::serverUriChanged(const QString& uri)
{
  PiiRemoteMetaObject::serverUriChanged(uri);
  configure(); // may throw
}

void PiiRemoteOperation::configure()
{
  PII_D;
  if (d->bConfigured)
    return;
  d->bConfigured = true;
  
  QList<QByteArray> lstInputNames = readDirectoryList("inputs/"); // may throw
  QList<QByteArray> lstOutputNames = readDirectoryList("outputs/"); // may throw
  
  for (int i=0; i<lstInputNames.size(); ++i)
    d->lstInputs << new PiiRemoteInputSocket(lstInputNames[i]);

  for (int i=0; i<lstOutputNames.size(); ++i)
    d->lstOutputs << new PiiRemoteOutputSocket(lstOutputNames[i]);
}

void PiiRemoteOperation::check(bool reset)
{
  call("check", reset);
}

void PiiRemoteOperation::start() { call("start"); }

void PiiRemoteOperation::pause() { call("pause"); }

void PiiRemoteOperation::stop() { call("stop"); }

void PiiRemoteOperation::interrupt() { call("interrupt"); }

bool PiiRemoteOperation::wait(unsigned long time)
{
  return call<bool>("wait", time);
}

PiiOperation::State PiiRemoteOperation::state() const
{
  return static_cast<PiiOperation::State>(call("state").toInt());
}

PiiAbstractInputSocket* PiiRemoteOperation::input(const QString& name) const
{
  const PII_D;
  for (int i=0; i<d->lstInputs.size(); ++i)
    if (d->lstInputs[i].objectName() == name)
      return d->lstInputs[i];
  return 0;
}

PiiAbstractOutputSocket* PiiRemoteOperation::output(const QString& name) const
{
  const PII_D;
  for (int i=0; i<d->lstOutputs.size(); ++i)
    if (d->lstOutputs[i].objectName() == name)
      return d->lstOutputs[i];
  return 0;
}

QList<PiiAbstractInputSocket*> PiiRemoteOperation::inputs() const
{
  const PII_D;
  QList<PiiAbstractInputSocket*> lstResult;
  for (int i=0; i<d->lstInputs.size(); ++i)
    lstResult << lstInputs[i];
  return lstResult;
}

QList<PiiAbstractOutputSocket*> PiiRemoteOperation::outputs() const
{
  const PII_D;
  QList<PiiAbstractOutputSocket*> lstResult;
  for (int i=0; i<d->lstOutputs.size(); ++i)
    lstResult << lstOutputs[i];
  return lstResult;
}

int PiiRemoteOperation::inputCount() const { return _d()->lstInputs.size(); }
int PiiRemoteOperation::outputCount() const { return _d()->lstOutputs.size(); }

void PiiRemoteOperation::startPropertySet(const QString& name)
{
  call("startPropertySet", name);
}

void PiiRemoteOperation::endPropertySet() { call("endPropertySet"); }

void PiiRemoteOperation::removePropertySet(const QString& name)
{
  call("removePropertySet", name);
}

void PiiRemoteOperation::reconfigure(const QString& propertySetName)
{
  call("reconfigure", propertySetName);
}

PiiRemoteOperation* PiiRemoteOperation::clone() const
{
  return new PiiRemoteOperation(serverUri());
}

void PiiRemoteOperation::decodePushedData(const QString& sourceId, const QByteArray& data)
{
  PII_D;
  //piiDebug(QString("Received %1 bytes to %2.").arg(data.size()).arg(strId));
  if (sourceId.startsWith("outputs/"))
    {
      QString strName = sourceId.mid(8);
      for (int i=0; i<d->lstOutputs.size(); ++i)
        if (d->lstOutputs[i]->socket()->objectName() == strName)
          {
            //...
            return;
          }
    }
}

bool PiiRemoteInputSocket::tryToReceive(PiiAbstractInputSocket*, const PiiVariant& object) throw ()
{
  call(d->strUri, object);
}
#endif
