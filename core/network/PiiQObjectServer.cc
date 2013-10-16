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

#include "PiiQObjectServer.h"

#include "PiiHttpDevice.h"
#include "PiiHttpException.h"
#include "PiiStreamBuffer.h"

#include <PiiUtil.h>
#include <PiiMetaTypeUtil.h>
#include <PiiConfigurable.h>

#include <QUrl>

#include "PiiNetworkEncoding.h"

static const int iQObjectFunctionCount = QObject::staticMetaObject.methodCount();

PiiQObjectServer::Data::Data(QObject* object, ExposedFeatures features) :
  pObject(object),
  pConfigurable(qobject_cast<PiiConfigurable*>(object)),
  features(features)
{}

PiiQObjectServer::Data::~Data()
{
  qDeleteAll(lstSlots);
}

PiiQObjectServer::MetaFunction::Data::Data(QObject* o, const QMetaMethod& m) :
  pObject(o), method(m)
{}

PiiQObjectServer::MetaFunction::MetaFunction(QObject* o, const QMetaMethod& m) :
  PiiGenericFunction(new Data(o,m))
{
  PII_D;
  d->iReturnType = QMetaType::type(m.typeName());

  // The signature string is initialized here because mapping a type
  // name to type id and back may be ambiguous. If the original type
  // name is a typedef, the corresponding type id will map back to a
  // different type name.
  QList<QByteArray> lstParams(m.parameterTypes());
  if (d->iReturnType != QMetaType::Void)
    {
      d->strSignature += m.typeName();
      d->strSignature += ' ';
    }
  d->strSignature += "%1(";
  for (int i=0; i<lstParams.size(); ++i)
    {
      if (i > 0)
        d->strSignature += ',';
      d->strSignature += lstParams[i];
      d->lstParamTypes << QMetaType::type(lstParams[i]);
    }
  d->strSignature += ')';
}

void PiiQObjectServer::MetaFunction::call(void** args)
{
  PII_D;
  QMetaObject::metacall(d->pObject, QMetaObject::InvokeMetaMethod, d->method.methodIndex(), args);
}

PiiQObjectServer::PiiQObjectServer(QObject* object, ExposedFeatures features) :
  PiiObjectServer(new Data(object, features))
{
  init();
}

PiiQObjectServer::PiiQObjectServer(Data* data) :
  PiiObjectServer(data)
{
  init();
}

void PiiQObjectServer::init()
{
  PII_D;
  listFunctions(d->pObject);
  if (d->pObject->isWidgetType()
#if QT_VERSION >= 0x050000
      || d->pObject->isWindowType()
#endif
      )
    setSafetyLevel(AccessFromMainThread);

  const QMetaObject* pMetaObj = d->pObject->metaObject();

  /* HACK. The properties listed in unsafeProperties class info field
   * can be set from the main thread only. This is required due to
   * deficiencies in Qt's threading architecture.
   */
  int index = pMetaObj->indexOfClassInfo("unsafeProperties");
  if (index != -1)
    {
      QMetaClassInfo info(pMetaObj->classInfo(index));
      QStringList lstProps(QString(info.value()).split(' '));
      foreach (QString strProp, lstProps)
        setPropertySafetyLevel(strProp, PiiObjectServer::AccessFromMainThread);
    }
  index = pMetaObj->indexOfClassInfo("unsafeFunctions");
  if (index != -1)
    {
      QMetaClassInfo info(pMetaObj->classInfo(index));
      QStringList lstSignatures(QString(info.value()).split(' '));
      foreach (QString strSignature, lstSignatures)
        setFunctionSafetyLevel(strSignature, PiiObjectServer::AccessFromMainThread);
    }
  index = pMetaObj->indexOfClassInfo("safetyLevel");
  if (index != -1)
    {
      QMetaClassInfo info(pMetaObj->classInfo(index));
      QMetaEnum safetyLevelEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("ThreadSafetyLevel"));
      int iValue = safetyLevelEnum.keyToValue(info.value());
      if (iValue != -1)
        setSafetyLevel(static_cast<ThreadSafetyLevel>(iValue));
    }
}

PiiQObjectServer::~PiiQObjectServer()
{
}

QObject* PiiQObjectServer::object() const
{
  return _d()->pObject;
}

void PiiQObjectServer::jsonProperties(PiiHttpDevice* dev, const QStringList& fields) const
{
  enum { Type = 1, Name = 2, Value = 4 };
  int iFields = fields.isEmpty() ? (Type | Name) : 0;
  for (QStringList::const_iterator it = fields.begin(); it != fields.end(); ++it)
    {
      if (*it == "type") iFields |= Type;
      else if (*it == "name") iFields |= Name;
      else if (*it == "value") iFields |= Value;
    }
  
  const PII_D;
  typedef QPair<int,QString> PropPair;
  QList<PropPair> lstProperties(properties<PropPair>());
  dev->print("[");
  // objectName is always a local property.
  for (int i=1; i<lstProperties.size(); ++i)
    {
      QVariantMap mapValues;
      if (iFields & Type)
        mapValues["type"] = QMetaType::typeName(lstProperties[i].first);
      if (iFields & Name)
        mapValues["name"] = lstProperties[i].second;
      if (iFields & Value)
        {
          QVariant varValue = d->pObject->property(qPrintable(lstProperties[i].second));
          if (int(varValue.type()) member_of<int> (QVariant::String,
                                                   QVariant::Int,
                                                   QVariant::Double,
                                                   QVariant::LongLong,
                                                   QVariant::UInt,
                                                   QVariant::ULongLong,
                                                   QVariant::Bool))
            mapValues["value"] = varValue;
          else
            mapValues["value"] = QString::fromUtf8(dev->encode(varValue));

        }

      if (i) dev->print(" ");
      dev->print(PiiNetwork::toJson(mapValues));
      if (i != lstProperties.size()-1)
        dev->print(",");
      dev->print("\n");
    }
  dev->print("]");
}

void PiiQObjectServer::listProperties(PiiHttpDevice* dev) const
{
  if (propertySafetyLevel() != AccessConcurrently)
    d->accessMutex.lock();
  
  if (dev->queryValue("format").toString() == "json")
    {
      dev->setHeader("Content-Type", "application/javascript");
      jsonProperties(dev, dev->queryValue("fields").toString().split(',', QString::SkipEmptyParts));
    }
  else
    // Default format is a plain text list
    dev->print(propertyDeclarations().join("\n"));
  
  if (propertySafetyLevel() != AccessConcurrently)
    d->accessMutex.unlock();
}

QVariant PiiQObjectServer::objectProperty(const QString& name) const
{
  const PII_D;
  ThreadSafetyLevel level = propertySafetyLevel(name);
  if (level != AccessConcurrently)
    d->accessMutex.lock();
  QVariant varResult = d->pConfigurable ?
    d->pConfigurable->property(qPrintable(name)) :
    d->pObject->property(qPrintable(name));
  if (level != AccessConcurrently)
    d->accessMutex.unlock();
  return varResult;
}

bool PiiQObjectServer::setPropertiesFromMainThread(const QVariantMap& props)
{
  return Pii::setProperties(_d()->pObject, props);
}

bool PiiQObjectServer::setObjectProperties(const QVariantMap& props)
{
  PII_D;
  switch (propertySafetyLevel())
    {
    case AccessFromMainThread:
      synchronized (d->accessMutex)
        {
          Q_ASSERT(thread() == qApp->thread());
          bool bResult = true;
          QMetaObject::invokeMethod(this, "setPropertiesFromMainThread",
                                    Qt::BlockingQueuedConnection,
#if (QT_VERSION >= 0x040800)
                                    Q_RETURN_ARG(bool, bResult),
#endif
                                    Q_ARG(QVariantMap, props));
          return bResult;
        }
    case AccessFromAnyThread:
      synchronized (d->accessMutex) return Pii::setProperties(d->pObject, props);
    case AccessConcurrently:
      return Pii::setProperties(d->pObject, props);
    }
  return false;
}

bool PiiQObjectServer::setObjectProperty(const QString& name, const QVariant& value)
{
  PII_D;
  switch (propertySafetyLevel(name))
    {
    case AccessFromMainThread:
      {
        Q_ASSERT(thread() == qApp->thread());
        bool bResult = true;
        QVariantMap mapProps;
        mapProps[name] = value;
        QMetaObject::invokeMethod(this, "setPropertiesFromMainThread",
                                  Qt::BlockingQueuedConnection,
#if (QT_VERSION >= 0x040800)
                                  Q_RETURN_ARG(bool, bResult),
#endif
                                  Q_ARG(QVariantMap, mapProps));
        return bResult;
      }
    case AccessFromAnyThread:
      synchronized (d->accessMutex)
        {
          return d->pConfigurable ?
            d->pConfigurable->setProperty(qPrintable(name), value) :
            d->pObject->setProperty(qPrintable(name), value);
        }
    case AccessConcurrently:
      return d->pConfigurable ?
        d->pConfigurable->setProperty(qPrintable(name), value) :
        d->pObject->setProperty(qPrintable(name), value);
    }
  return false;
}

void PiiQObjectServer::handleRequest(const QString& uri, PiiHttpDevice* dev,
                                     PiiHttpProtocol::TimeLimiter* controller)
{
  PII_D;
  bool bPostRequest = dev->requestMethod() == "POST";

  if (!bPostRequest && dev->requestMethod() != "GET")
    PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);
  
  QString strRequestPath = dev->requestPath(uri);

  // Property list/get/set
  if (strRequestPath.startsWith("properties/"))
    {
      // Only directory listing (empty) allowed
      // TODO: this isn't quite there yet.
      if (!(d->features & (ExposeProperties | ExposeDynamicProperties)))
        {
          if (strRequestPath.size() > 11)
            PII_THROW_HTTP_ERROR(NotFoundStatus);
          PII_REQUIRE_HTTP_METHOD("GET");
          return;
        }
      dev->startOutputFiltering(new PiiStreamBuffer);
      QString strPropName = strRequestPath.mid(11);
      if (strPropName.isEmpty())
        {
          // Set many properties at once
          if (bPostRequest)
            setObjectProperties(dev->requestValues());
          // List all properties
          else
            listProperties(dev);
        }
      // Get property
      else if (!dev->hasQuery() && !bPostRequest)
        {
          QByteArray aValue(dev->encode(objectProperty(strPropName)));
          dev->write(aValue);
        }
      // Set property
      else
        {
          bool bSuccess = false;
          if (bPostRequest)
            bSuccess = setObjectProperty(strPropName,
                                         dev->decodeVariant(dev->readBody()));
          else
            bSuccess = setObjectProperty(strPropName,
                                         dev->decodeVariant(QUrl::fromPercentEncoding(dev->queryString().toUtf8())));
          if (!bSuccess)
            PII_THROW_HTTP_ERROR(BadRequestStatus);
        }
    }
  else if (strRequestPath.startsWith("signals/"))
    {
      dev->startOutputFiltering(new PiiStreamBuffer);
      QString strFunction = strRequestPath.mid(8);
      // Empty function name -> list all signals
      if (strFunction.isEmpty())
        {
          PII_REQUIRE_HTTP_METHOD("GET");
          dev->print(signalSignatures().join("\n"));
        }
      else
        // Signals cannot be connected here
        PII_THROW_HTTP_ERROR(NotFoundStatus);
    }
  else if (strRequestPath.startsWith("enums/"))
    {
      dev->startOutputFiltering(new PiiStreamBuffer);
      QString strEnum = strRequestPath.mid(6);
      if (strEnum.isEmpty())
        {
          PII_REQUIRE_HTTP_METHOD("GET");
          QStringList lstEnums(d->lstEnums);
          lstEnums.replaceInStrings(QRegExp("$"), "/");
          dev->print(lstEnums.join("\n"));
        }
      else
        {
          if (!d->hashEnums.contains(strEnum))
            PII_THROW_HTTP_ERROR(NotFoundStatus);
          QStringList lstEnums = d->hashEnums[strEnum];
          for (int i=0; i<lstEnums.size(); ++i)
            {
              dev->print(lstEnums[i]);
              dev->putChar(' ');
              dev->print(QString::number(d->hashEnumValues[lstEnums[i]]));
              if (i < lstEnums.size()-1)
                dev->putChar('\n');
            }
        }
    }
  else
    PiiObjectServer::handleRequest(uri, dev, controller);
}

QStringList PiiQObjectServer::listRoot() const
{
  QStringList lstFolders = PiiObjectServer::listRoot();
  lstFolders << "signals/" << "properties/" << "enums/";
  return lstFolders;
}

void PiiQObjectServer::channelDeleted(Channel* channel)
{
  PII_D;
  for (SlotList::const_iterator it = d->lstSlots.constBegin(); it != d->lstSlots.constEnd(); ++it)
    {
      (*it)->lstChannels.removeOne(channel);
      if ((*it)->lstChannels.isEmpty())
        (*it)->dynamicDisconnect(d->pObject);
    }
  PiiObjectServer::channelDeleted(channel);
}

void PiiQObjectServer::disconnectFromChannel(Channel* channel, const QString& sourceId)
{
  if (sourceId.startsWith("signals/"))
    {
      PII_D;
      QString strSignal = sourceId.mid(8);
      ChannelSlot* pSlot = findSlot(strSignal);
      if (pSlot != 0)
        {
          pSlot->lstChannels.removeOne(channel);
          if (pSlot->lstChannels.isEmpty() &&
              !pSlot->dynamicDisconnect(d->pObject))
            PII_THROW_HTTP_ERROR_MSG(BadRequestStatus, tr("The signal called \"%1\" is not connected.").arg(strSignal));
        }
    }
  else
    PiiObjectServer::disconnectFromChannel(channel, sourceId);
}

PiiQObjectServer::ChannelSlot* PiiQObjectServer::findSlot(const QString& signal) const
{
  const PII_D;
  for (int i=0; i<d->lstSlots.size(); ++i)
    if (d->lstSlots[i]->signal() == signal)
      return d->lstSlots[i];
  return 0;
}

void PiiQObjectServer::connectToChannel(Channel* channel, const QString& sourceId)
{
  PII_D;
  if (sourceId.startsWith("signals/"))
    {
      QString strSignal = sourceId.mid(8);
      if (!d->lstSignals.contains(strSignal))
        PII_THROW_HTTP_ERROR_MSG(BadRequestStatus, tr("There is no signal called \"%1\".").arg(strSignal));

      ChannelSlot* pSlot = findSlot(strSignal);
      if (pSlot == 0)
        {
          pSlot = new ChannelSlot(&d->channelMutex);
          d->lstSlots << pSlot;
        }
      if (pSlot->lstChannels.isEmpty() &&
          pSlot->dynamicConnect(_d()->pObject,
                                qPrintable(strSignal), 0,
                                Qt::DirectConnection) == -1)
        PII_THROW_HTTP_ERROR_MSG(InternalServerErrorStatus, tr("Cannot connect to \"%1\".").arg(strSignal));
      pSlot->lstChannels << channel;
    }
  else
    PiiObjectServer::connectToChannel(channel, sourceId);
}

void PiiQObjectServer::listFunctions(QObject* object)
{
  PII_D;
  const QMetaObject* pMetaObject = (object == this ? &staticMetaObject : object->metaObject());
  for (int i = iQObjectFunctionCount; i < pMetaObject->methodCount(); ++i)
    {
      QMetaMethod method = pMetaObject->method(i);
      if (method.methodType() == QMetaMethod::Signal)
        {
          if (d->features & ExposeSignals)
            d->lstSignals << method.QMETAMETHOD_SIGNATURE();
        }
      else if ((method.access() == QMetaMethod::Public ||
    (method.access() == QMetaMethod::Protected && d->features & ExposeProtected) ||
    (method.access() == QMetaMethod::Private && d->features & ExposePrivate))
         &&
               ((method.methodType() == QMetaMethod::Method && (d->features & ExposeMethods)) ||
                (method.methodType() == QMetaMethod::Slot && (d->features & ExposeSlots))))
        {
          QString strSignature(method.QMETAMETHOD_SIGNATURE());
          addFunction(strSignature.left(strSignature.indexOf('(')), new MetaFunction(object, method));
        }
      else
  {
    piiDebug(QString("method: %0, type: %1, access: %2").arg(QString::fromLatin1(method.QMETAMETHOD_SIGNATURE())).arg(method.methodType()).arg(method.access()));
  }
    }
}

QStringList PiiQObjectServer::signalSignatures() const
{
  return _d()->lstSignals;
}

void PiiQObjectServer::addToEnums(const QString& name, const QMetaEnum& enumerator)
{
  PII_D;
  if (d->hashEnums.contains(name))
    return;
  d->lstEnums << name;
  QStringList lstKeys;
  for (int i=0; i<enumerator.keyCount(); ++i)
    {
      lstKeys << enumerator.key(i);
      d->hashEnumValues.insert(name, enumerator.value(i));
    }
}

template <>
QString PiiQObjectServer::makeProperty<QString>(int type, const QString& name)
{
  return QString("%1 %2").arg(QMetaType::typeName(type)).arg(name);
}

template <>
QPair<int,QString> PiiQObjectServer::makeProperty<QPair<int,QString> >(int type, const QString& name)
{
  return qMakePair(type, name);
}

QStringList PiiQObjectServer::propertyDeclarations() const
{
  return properties<QString>();
}

template <class T> QList<T> PiiQObjectServer::properties() const
{
  const PII_D;
  const QMetaObject* pMetaObject = d->pObject->metaObject();
  QList<T> lstResult;

  if (d->features & ExposeProperties)
    {
      // objectName is always a local property.
      for (int i=1; i<pMetaObject->propertyCount(); ++i)
        {
          QMetaProperty prop = pMetaObject->property(i);
          if (prop.isEnumType())
            lstResult << makeProperty<T>(QVariant::Int, prop.name());
          /*  {
              strPrefix = prop.isFlagType() ? "flag " : "enum ";
              addToEnums(prop.typeName(), prop.enumerator());
              }
          */
          else
            {
              int iType = prop.type();
              if (iType == QVariant::UserType)
                iType = prop.userType();
              lstResult << makeProperty<T>(iType, prop.name());
            }
        }
    }

  if (d->features & ExposeDynamicProperties)
    {
      QList<QByteArray> lstDynamicPropertyNames = d->pObject->dynamicPropertyNames();
      for (int i=0; i<lstDynamicPropertyNames.size(); ++i)
        {
          QVariant prop = d->pObject->property(lstDynamicPropertyNames[i]);
          int iType = prop.type();
          if (iType == QVariant::UserType)
            iType = prop.userType();
          lstResult << makeProperty<T>(iType, lstDynamicPropertyNames[i]);
        }
    }

  return lstResult;
}

bool PiiQObjectServer::ChannelSlot::invokeSlot(int id, void** args)
{
  QMutexLocker lock(_pMutex);
  if (lstChannels.isEmpty())
    return true;
  try
    {
      QByteArray aBody;
      if (argumentCount(id) > 0)
        aBody = PiiNetwork::toByteArray(argsToList(id, args), PiiNetwork::BinaryFormat);
      //piiDebug("Sending %d bytes to %s.", aBody.size(), signatureOf(id).constData());
      for (int i=0; i<lstChannels.size(); ++i)
        lstChannels[i]->enqueuePushData("signals/" + signatureOf(id), aBody);
    }
  catch (PiiSerializationException& ex)
    {
      piiWarning(tr("Cannot invoke slot %1: %2").arg(id).arg(ex.message()));
      return false;
    }
  return true;
}

void PiiQObjectServer::moveToMainThread()
{
  PiiObjectServer::moveToMainThread();
  _d()->pObject->moveToThread(qApp->thread());
}

void PiiQObjectServer::setPropertySafetyLevel(const QString& propertyName, ThreadSafetyLevel safetyLevel)
{
  PII_D;
  d->mapPropertySafetyLevels.insert(propertyName, safetyLevel);
  if (safetyLevel < d->propertySafetyLevel)
    d->propertySafetyLevel = safetyLevel;
}

PiiObjectServer::ThreadSafetyLevel PiiQObjectServer::propertySafetyLevel(const QString& propertyName) const
{
  const PII_D;
  SafetyLevelMap::const_iterator it = d->mapPropertySafetyLevels.find(propertyName);
  return it == d->mapPropertySafetyLevels.end() ?
    d->safetyLevel :
    *it;
}

void PiiQObjectServer::removePropertySafetyLevel(const QString& propertyName)
{
  PII_D;
  d->mapPropertySafetyLevels.remove(propertyName);
  // Find the minimum access level of all properties.
  d->propertySafetyLevel = AccessConcurrently;
  for (SafetyLevelMap::const_iterator it = d->mapPropertySafetyLevels.constBegin();
       it != d->mapPropertySafetyLevels.constEnd(); ++it)
    if (*it < d->propertySafetyLevel)
      d->propertySafetyLevel = *it;
}
