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

#include "PiiRemoteMetaObject.h"

#include "PiiNetworkException.h"
#include "PiiHttpProtocol.h"
#include "PiiStreamBuffer.h"
#include "PiiMultipartDecoder.h"

#include <PiiUtil.h>
#include <PiiAsyncCall.h>
#include <PiiDelay.h>
#include <PiiMetaTypeUtil.h>
#include <PiiInvalidArgumentException.h>

#include "PiiNetworkEncoding.h"

#include <QUrl>

namespace PiiNetworkPrivate
{
  enum
  {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
    StdCppSet = 0x00000100,
//     Override = 0x00000200,
    Constant = 0x00000400,
    Final = 0x00000800,
    Designable = 0x00001000,
    ResolveDesignable = 0x00002000,
    Scriptable = 0x00004000,
    ResolveScriptable = 0x00008000,
    Stored = 0x00010000,
    ResolveStored = 0x00020000,
    Editable = 0x00040000,
    ResolveEditable = 0x00080000,
    User = 0x00100000,
    ResolveUser = 0x00200000,
    Notify = 0x00400000
  };
}

static int iMethodIndex = 4;
static int iPropertyIndex = 6;
//static int iEnumIndex = 8;
static int iSignalIndex = 13;
static int iMetaHeaderSize = 14;

#if QT_VERSION < 0x050000
#  include "qt4/PiiRemoteMetaObjectQt4.cc"
#else
#  include "qt5/PiiRemoteMetaObjectQt5.cc"
#endif

PiiRemoteMetaObject::PiiRemoteMetaObject(QObject* object) :
  PiiRemoteObject(new Data(object))
{}

PiiRemoteMetaObject::PiiRemoteMetaObject(QObject* object, const QString& serverUri) :
  PiiRemoteObject(new Data(object), serverUri)
{
  createMetaObject(); // may throw
}

PiiRemoteMetaObject::~PiiRemoteMetaObject()
{}

const QMetaObject* PiiRemoteMetaObject::metaObject() const
{
  const PII_D;
  return d->bMetaCreated ? &d->metaObject : &QObject::staticMetaObject;
}

int PiiRemoteMetaObject::qt_metacall(QMetaObject::Call call, int id, void** args)
{
  PII_D;
  id = d->pObject->QObject::qt_metacall(call, id, args);
  if (id < 0)
    return id;

  // Try three times. First two with no delays, third time after a
  // while.
  for (int iTry=0; iTry<3; ++iTry)
    {
      try
        {
          return metaCall(call, id, args);
        }
      catch (PiiException& ex)
        {
          piiWarning(ex.location("", ": ") + ex.message());
          closeConnection();
          if (iTry == 0)
            piiWarning(tr("Remote method call failed. Trying to reconnect."));
          else
            PiiDelay::msleep(d->iRetryDelay);
        }
    }
  addFailure();
  return id;
}

int PiiRemoteMetaObject::metaCall(QMetaObject::Call callType, int id, void** args)
{
  PII_D;
  if (callType == QMetaObject::InvokeMetaMethod)
    {
      // The fist ids are signals, which we stored in another list.
      id -= d->lstSignals.size();

      const QList<int>& lstTypes = d->lstFunctions[id].lstParamTypes;
      QVariant varReturnValue = lstTypes.size() > 0 ?
        callList("functions/" + d->lstFunctions[id].strName,
                 Pii::argsToList(lstTypes, args+1)) :
        callList("functions/" + d->lstFunctions[id].strName,
                 QVariantList());

      if (args[0] != 0 && d->lstFunctions[id].returnType != 0 &&
          !Pii::copyMetaType(varReturnValue, d->lstFunctions[id].returnType, args))
        PII_THROW(PiiNetworkException, tr("Unexpected return value from %1.")
                  .arg(QString::fromLatin1(d->lstFunctions[id].aSignature)));

      id -= d->lstFunctions.size();
    }
  else if (callType == QMetaObject::ReadProperty)
    {
      HttpDevicePtr pDev = openConnection();

      //pDev->startOutputFiltering(new PiiStreamBuffer);
      pDev->setRequest("GET", d->strPath + "properties/" + d->lstProperties[id].strName);
      pDev->finish();

      PII_CHECK_SERVER_RESPONSE;

      QByteArray aBody(pDev->readBody());
      QVariant varReply = pDev->decodeVariant(aBody);
      //piiDebug(QString("%1 = %2 (%3)").arg(d->lstProperties[id].strName).arg(QString(aBody)).arg(varReply.toString()));
      if (!Pii::copyMetaType(varReply, d->lstProperties[id].type, args))
        PII_THROW(PiiNetworkException, tr("Server returned a QVariant with type id %1, but %2 was expected for %3.")
                  .arg(varReply.type()).arg(d->lstProperties[id].type).arg(d->lstProperties[id].strName));
      id -= d->lstProperties.size();
    }
  else if (callType == QMetaObject::WriteProperty)
    {
      HttpDevicePtr pDev = openConnection();

      pDev->setRequest("POST", d->strPath + "properties/" + d->lstProperties[id].strName);
      pDev->startOutputFiltering(new PiiStreamBuffer);
      pDev->write(pDev->encode(Pii::argsToVariant(args, d->lstProperties[id].type)));
      pDev->finish();

      PII_CHECK_SERVER_RESPONSE;

      pDev->discardBody();

      id -= d->lstProperties.size();
    }
  else
    id -= d->lstProperties.size();

  return id;
}

void PiiRemoteMetaObject::emitSignal(int id, const QByteArray& data)
{
  PII_D;
  QVariantList lstArgs;
  if (!data.isEmpty())
    lstArgs = PiiNetwork::fromByteArray<QVariantList>(data);
  if (Pii::scoreOverload(lstArgs, d->lstSignals[id].lstParamTypes) == -1)
    PII_THROW(PiiInvalidArgumentException, tr("Cannot emit %1: deserialized parameter types do not match.")
              .arg(QString::fromLatin1(d->lstSignals[id].aSignature)));
  void* args[11];
  args[0] = 0;
  for (int i=0; i<lstArgs.size(); ++i)
    {
      if (d->lstSignals[id].lstParamTypes[i] < int(QVariant::UserType))
        lstArgs[i].convert(QVariant::Type(d->lstSignals[id].lstParamTypes[i]));
      args[i+1] = const_cast<void*>(lstArgs[i].constData());
    }
  QMetaObject::activate(d->pObject, &d->metaObject, id, args);
}

void PiiRemoteMetaObject::decodePushedData(const QString& sourceId, const QByteArray& data)
{
  //piiDebug(QString("Received %1 bytes to %2.").arg(data.size()).arg(strId));
  if (sourceId.startsWith("signals/"))
    {
      PII_D;
      QByteArray aSignature = sourceId.mid(8).toLatin1();
      for (int i=0; i<d->lstSignals.size(); ++i)
        if (d->lstSignals[i].aSignature == aSignature)
          {
            emitSignal(i, data);
            return;
          }
    }
  else
    PiiRemoteObject::decodePushedData(sourceId, data);
}

void PiiRemoteMetaObject::connectSignal(const char* signal)
{
  PII_D;
  for (int i=0; i<d->lstSignals.size(); ++i)
    if (!std::strcmp(d->lstSignals[i].aSignature, signal))
      {
        try
          {
            if (d->lstSignals[i].iConnectionCount == 0)
              connectToChannel("signals/" + d->lstSignals[i].aSignature);
            ++d->lstSignals[i].iConnectionCount;
          }
        catch (PiiException& ex)
          {
            addFailure();
            piiWarning(ex.message());
          }
        break;
      }
}

void PiiRemoteMetaObject::disconnectSignal(const char* signal)
{
  PII_D;
  for (int i=0; i<d->lstSignals.size(); ++i)
    if (!std::strcmp(d->lstSignals[i].aSignature, signal))
      {
        try
          {
            if (--d->lstSignals[i].iConnectionCount == 0)
              disconnectFromChannel("signals/" + d->lstSignals[i].aSignature);
          }
        catch (PiiException& ex)
          {
            addFailure();
            piiWarning(ex.message());
          }
        break;
      }
}

void PiiRemoteMetaObject::serverUriChanged(const QString&)
{
  createMetaObject();
}

QList<PiiRemoteMetaObject::Signal>& PiiRemoteMetaObject::signalList()
{
  return _d()->lstSignals;
}
