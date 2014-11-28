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
  features(features),
  minPropertySafetyLevel(AccessPropertyFromAnyThread)
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
  for (int i = 0; i < lstParams.size(); ++i)
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

template <class Enum>
Enum PiiQObjectServer::stringToEnum(const char* enumName, const QString& str) const
{
  bool bSuccess = false;
  int iValue = str.toInt(&bSuccess);
  if (bSuccess)
    return static_cast<Enum>(iValue);
  QMetaEnum safetyLevelEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator(enumName));
  return static_cast<Enum>(qMax(0, safetyLevelEnum.keyToValue(qPrintable(str))));
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

  int index = pMetaObj->indexOfClassInfo("propertySafetyLevel");
  if (index != -1)
    {
      QMetaClassInfo info(pMetaObj->classInfo(index));
      QStringList lstProps(QString(info.value()).split(' '));
      foreach (const QString& strProp, lstProps)
        {
          QStringList lstParts(strProp.split(':'));
          if (lstParts.size() == 2)
            setPropertySafetyLevel(lstParts[0],
                                   stringToEnum<PropertySafetyLevel>("PropertySafetyLevel",
                                                                     lstParts[1]));
        }
    }
  index = pMetaObj->indexOfClassInfo("functionSafetyLevel");
  if (index != -1)
    {
      QMetaClassInfo info(pMetaObj->classInfo(index));
      QStringList lstSignatures(QString(info.value()).split(' '));
      foreach (const QString& strSignature, lstSignatures)
        {
          QStringList lstParts(strSignature.split(':'));
          if (lstParts.size() == 2)
            setFunctionSafetyLevel(lstParts[0],
                                   stringToEnum<ThreadSafetyLevel>("ThreadSafetyLevel",
                                                                   lstParts[1]));
        }
    }
  index = pMetaObj->indexOfClassInfo("safetyLevel");
  if (index != -1)
    {
      QMetaClassInfo info(pMetaObj->classInfo(index));
      setSafetyLevel(stringToEnum<ThreadSafetyLevel>("ThreadSafetyLevel", info.value()));
    }
}

PiiQObjectServer::~PiiQObjectServer()
{
}

QObject* PiiQObjectServer::object() const
{
  return _d()->pObject;
}

/******* Property listing helper functions *******/
template <class T>
static T makeProperty(int type, const QString& name, int flags);

template <>
QString makeProperty<QString>(int type, const QString& name, int flags)
{
  QString strDecl(QString("%1 %2").arg(QMetaType::typeName(type)).arg(name));
  if (flags & PiiNetwork::VolatileProperty)
    strDecl.prepend("volatile ");
  if (flags & PiiNetwork::ConstProperty)
    strDecl.prepend("const ");
  return strDecl;
}

struct PropertyInfo
{
  PropertyInfo(int t, const QString& n, int f) :
    type(t), name(n), flags(f)
  {}

  int type;
  QString name;
  int flags;
};

template <>
PropertyInfo makeProperty<PropertyInfo>(int type, const QString& name, int flags)
{
  return PropertyInfo(type, name, flags);
}
/*************************************************/

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
  QList<PropertyInfo> lstProperties(properties<PropertyInfo>());
  dev->print("[");
  // objectName is always a local property -> start at 1
  for (int i = 1; i < lstProperties.size(); ++i)
    {
      QVariantMap mapValues;
      const PropertyInfo& prop = lstProperties[i];
      if (iFields & Type)
        {
          mapValues["type"] = QMetaType::typeName(prop.type);
          if (prop.flags != 0)
            {
              QVariantList lstFlags;
              if (prop.flags & PiiNetwork::ConstProperty)
                lstFlags << "const";
              if (prop.flags & PiiNetwork::VolatileProperty)
                lstFlags << "volatile";
              mapValues["flags"] = lstFlags;
            }
        }
      if (iFields & Name)
        mapValues["name"] = prop.name;
      if (iFields & Value)
        {
          QVariant varValue = d->pObject->property(qPrintable(prop.name));
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

      if (i)
        dev->print(" ");
      dev->print(PiiNetwork::toJson(mapValues));
      if (i != lstProperties.size()-1)
        dev->print(",");
      dev->print("\n");
    }
  dev->print("]");
}

void PiiQObjectServer::listProperties(PiiHttpDevice* dev) const
{
  PropertySafetyLevel level = strictestPropertySafetyLevel();
  if (level != AccessPropertyConcurrently)
    d->accessMutex.lock();

  if (dev->queryValue("format").toString() == "json")
    {
      dev->setHeader("Content-Type", "application/javascript");
      jsonProperties(dev, dev->queryValue("fields").toString().split(',', QString::SkipEmptyParts));
    }
  else
    // Default format is a plain text list
    dev->print(propertyDeclarations().join("\n"));

  if (level != AccessPropertyConcurrently)
    d->accessMutex.unlock();
}

QVariant PiiQObjectServer::readProperty(const QString& name, bool lock)
{
  PII_D;
  if (lock)
    d->accessMutex.lock();
  QVariant varResult = d->pConfigurable ?
    d->pConfigurable->property(qPrintable(name)) :
    d->pObject->property(qPrintable(name));
  if (lock)
    d->accessMutex.unlock();
  return varResult;
}

QVariant PiiQObjectServer::objectProperty(const QString& name)
{
  switch (propertySafetyLevel(name))
    {
    case AccessPropertyFromMainThread:
      {
        Q_ASSERT(_d()->pObject->thread() == qApp->thread());
        QVariant varResult;
        if (QThread::currentThread() != qApp->thread())
          QMetaObject::invokeMethod(this, "readProperty",
                                    Qt::BlockingQueuedConnection,
                                    Q_RETURN_ARG(QVariant, varResult),
                                    Q_ARG(QString, name),
                                    Q_ARG(bool, true));
        else
          varResult = readProperty(name, true);
        return varResult;
      }
    case WritePropertyFromMainThread:
    case AccessPropertyFromAnyThread:
      return readProperty(name, true);
    case AccessPropertyConcurrently:
      return readProperty(name, false);
    }
  return QVariant();
}

bool PiiQObjectServer::setProperties(const QVariantMap& props, bool lock)
{
  PII_D;
  if (lock)
    d->accessMutex.lock();
  bool bResult = d->pConfigurable ?
    Pii::setProperties(d->pConfigurable, props) :
    Pii::setProperties(d->pObject, props);
  if (lock)
    d->accessMutex.unlock();
  return bResult;
}

QVariant PiiQObjectServer::setSingleProperty(const QString& name, const QVariant& value, bool lock)
{
  PII_D;
  if (lock)
    d->accessMutex.lock();
  QVariant varFinalValue;
  if (d->pConfigurable)
    {
      if (d->pConfigurable->setProperty(qPrintable(name), value))
        varFinalValue = d->pConfigurable->property(qPrintable(name));
    }
  else
    {
      if (d->pObject->setProperty(qPrintable(name), value))
        varFinalValue = d->pObject->property(qPrintable(name));
    }
  if (lock)
    d->accessMutex.unlock();

  return varFinalValue;
}

bool PiiQObjectServer::setObjectProperties(const QVariantMap& props)
{
  switch (strictestPropertySafetyLevel())
    {
    case AccessPropertyFromMainThread:
    case WritePropertyFromMainThread:
      {
        Q_ASSERT(_d()->pObject->thread() == qApp->thread());
        bool bResult = true;
        if (QThread::currentThread() != qApp->thread())
          QMetaObject::invokeMethod(this, "setProperties",
                                    Qt::BlockingQueuedConnection,
                                    Q_RETURN_ARG(bool, bResult),
                                    Q_ARG(QVariantMap, props),
                                    Q_ARG(bool, true));
        else
          bResult = setProperties(props, true);
        return bResult;
      }
    case AccessPropertyFromAnyThread:
      return setProperties(props, true);
    case AccessPropertyConcurrently:
      return setProperties(props, false);
    }
  return false;
}

QVariant PiiQObjectServer::setObjectProperty(const QString& name, const QVariant& value)
{
  switch (propertySafetyLevel(name))
    {
    case AccessPropertyFromMainThread:
    case WritePropertyFromMainThread:
      {
        Q_ASSERT(_d()->pObject->thread() == qApp->thread());
        QVariant varResult;
        if (QThread::currentThread() != qApp->thread())
          QMetaObject::invokeMethod(this, "setSingleProperty",
                                    Qt::BlockingQueuedConnection,
                                    Q_RETURN_ARG(QVariant, varResult),
                                    Q_ARG(QString, name),
                                    Q_ARG(QVariant, value),
                                    Q_ARG(bool, true));
        else
          varResult = setSingleProperty(name, value, true);
        return varResult;
      }
    case AccessPropertyFromAnyThread:
      return setSingleProperty(name, value, true);
    case AccessPropertyConcurrently:
      return setSingleProperty(name, value, false);
    }
  return QVariant();
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
            {
              if (!setObjectProperties(dev->requestValues()))
                PII_THROW_HTTP_ERROR(BadRequestStatus);
            }
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
          QVariant varInputValue = bPostRequest ?
            dev->decodeVariant(dev->readBody()) :
            dev->decodeVariant(QUrl::fromPercentEncoding(dev->queryString().toUtf8()));

          QVariant varFinalValue = setObjectProperty(strPropName, varInputValue);
          // TODO volatile property values don't need to be sent back
          if (!varFinalValue.isValid())
            PII_THROW_HTTP_ERROR(BadRequestStatus);

          // If the value wasn't set to the received value, tell the
          // client the real value.
          if (!Pii::equals(varInputValue, varFinalValue))
            dev->write(dev->encode(varFinalValue)); // TODO: check if we could use binary format

          // Inform all other clients about the change, if the
          // property is implicity notified (has no native
          // notification signal).
          QMap<QString, QString>::const_iterator it = d->mapNotifiedProps.constFind(strPropName);
          if (it != d->mapNotifiedProps.constEnd())
            sendToOtherClients(dev->requestHeader().value("X-Client-ID"),
                               "signals/" + *it,
                               PiiNetwork::toByteArray(QVariantList() << varFinalValue,
                                                       PiiNetwork::BinaryFormat));
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
          for (int i = 0; i < lstEnums.size(); ++i)
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
  // Destroy connections to all dynamic slots
  for (SlotList::const_iterator it = d->lstSlots.constBegin(); it != d->lstSlots.constEnd(); ++it)
    {
      (*it)->lstChannels.removeOne(channel);
      if ((*it)->lstChannels.isEmpty())
        (*it)->dynamicDisconnect(d->pObject);
    }
  // Destroy connections to implicit change notifiers
  for (ImplicitSignalMap::iterator it = d->mapImplicitNotifiers.begin();
       it != d->mapImplicitNotifiers.end(); ++it)
    it->removeOne(channel);

  PiiObjectServer::channelDeleted(channel);
}

void PiiQObjectServer::disconnectFromChannel(Channel* channel, const QString& sourceId)
{
  if (sourceId.startsWith("signals/"))
    {
      PII_D;
      QString strSignal = sourceId.mid(8);

      // Implicit property change signal
      ImplicitSignalMap::iterator it = d->mapImplicitNotifiers.find(strSignal);
      if (it != d->mapImplicitNotifiers.end())
          it->removeOne(channel);
      else
        {
          ChannelSlot* pSlot = findSlot(strSignal);
          if (pSlot != 0)
            {
              pSlot->lstChannels.removeOne(channel);
              // If this was the last channel interested in the
              // signal, disconnect the signal-slot connection.
              if (pSlot->lstChannels.isEmpty() &&
                  !pSlot->dynamicDisconnect(d->pObject))
                PII_THROW_HTTP_ERROR_MSG(BadRequestStatus,
                                         tr("The signal called \"%1\" is not connected.").arg(strSignal));
            }
        }
    }
  else
    PiiObjectServer::disconnectFromChannel(channel, sourceId);
}

PiiQObjectServer::ChannelSlot* PiiQObjectServer::findSlot(const QString& signal) const
{
  const PII_D;
  for (int i = 0; i < d->lstSlots.size(); ++i)
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

      // Implicit property change signal
      ImplicitSignalMap::iterator it = d->mapImplicitNotifiers.find(strSignal);
      if (it != d->mapImplicitNotifiers.end())
        {
          if (!it->contains(channel))
            it->append(channel);
        }
      else
        {
          if (!d->lstSignals.contains(strSignal))
            PII_THROW_HTTP_ERROR_MSG(BadRequestStatus, tr("There is no signal called \"%1\".").arg(strSignal));

          ChannelSlot* pSlot = findSlot(strSignal);
          if (pSlot == 0)
            {
              pSlot = new ChannelSlot(&d->channelMutex);
              d->lstSlots << pSlot;
            }
          // If the slot isn't currently connected to any channel, there
          // is no signal-slot connection either. It must be established.
          if (pSlot->lstChannels.isEmpty() &&
              pSlot->dynamicConnect(_d()->pObject,
                                    qPrintable(strSignal), 0,
                                    Qt::DirectConnection) == -1)
            PII_THROW_HTTP_ERROR_MSG(InternalServerErrorStatus, tr("Cannot connect to \"%1\".").arg(strSignal));
          pSlot->lstChannels << channel;
        }
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
          /*piiDebug(QString("method: %0, type: %1, access: %2")
                   .arg(QString::fromLatin1(method.QMETAMETHOD_SIGNATURE()))
                   .arg(method.methodType()).arg(method.access()));*/
        }
    }

  // Implicit property change signals.
  if (d->features & ExposeProperties)
    {
      foreach (const PropertyInfo& info, properties<PropertyInfo>())
        {
          // Check if there is a notification signal that follows the
          // naming pattern.
          QString strSignalSignature(QString("%1Changed(%2)")
                                     .arg(info.name).arg(QMetaType::typeName(info.type)));
          int iSignalIndex = pMetaObject->indexOfMethod(qPrintable(strSignalSignature));
          // There is no notification signal and the property can be
          // set -> need to create an implicit change signal.
          if (iSignalIndex == -1 && !(info.flags & PiiNetwork::ConstProperty))
            {
              d->mapImplicitNotifiers[strSignalSignature] = QList<Channel*>();
              d->mapNotifiedProps[info.name] = strSignalSignature;
            }
        }
    }
}

QStringList PiiQObjectServer::signalSignatures() const
{
  const PII_D;
  return d->lstSignals + d->mapImplicitNotifiers.keys();
}

void PiiQObjectServer::addToEnums(const QString& name, const QMetaEnum& enumerator)
{
  PII_D;
  if (d->hashEnums.contains(name))
    return;
  d->lstEnums << name;
  QStringList lstKeys;
  for (int i = 0; i < enumerator.keyCount(); ++i)
    {
      lstKeys << enumerator.key(i);
      d->hashEnumValues.insert(name, enumerator.value(i));
    }
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
      for (int i = 1; i < pMetaObject->propertyCount(); ++i)
        {
          QMetaProperty prop = pMetaObject->property(i);
          if (!prop.isValid())
            continue;

          int iFlags = 0;
          if (!prop.isWritable())
            iFlags |= PiiNetwork::ConstProperty;

          // Q_PROPERTY provides no way to mark volatile properties.
          // We need to use Q_CLASSINFO.
          int iVolatileIndex = pMetaObject->indexOfClassInfo(qPrintable(QString(prop.name()) + ".volatile"));
          if (iVolatileIndex != -1 &&
              QString(pMetaObject->classInfo(iVolatileIndex).value()) == "true")
            iFlags |= PiiNetwork::VolatileProperty;

          if (prop.isEnumType())
            lstResult << makeProperty<T>(QVariant::Int, prop.name(), iFlags);
          /*  TODO
              {
              strPrefix = prop.isFlagType() ? "flag " : "enum ";
              addToEnums(prop.typeName(), prop.enumerator());
              }
          */
          else
            {
              int iType = prop.type();
              if (iType == QVariant::UserType)
                iType = prop.userType();
              lstResult << makeProperty<T>(iType, prop.name(), iFlags);
            }
        }
    }

  if (d->features & ExposeDynamicProperties)
    {
      QList<QByteArray> lstDynamicPropertyNames = d->pObject->dynamicPropertyNames();
      for (int i = 0; i < lstDynamicPropertyNames.size(); ++i)
        {
          QVariant prop = d->pObject->property(lstDynamicPropertyNames[i]);
          int iType = prop.type();
          if (iType == QVariant::UserType)
            iType = prop.userType();
          lstResult << makeProperty<T>(iType, lstDynamicPropertyNames[i], 0);
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
      for (int i = 0; i < lstChannels.size(); ++i)
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

void PiiQObjectServer::setPropertySafetyLevel(const QString& propertyName, PropertySafetyLevel safetyLevel)
{
  PII_D;
  d->mapPropertySafetyLevels.insert(propertyName, safetyLevel);
  if (safetyLevel < d->minPropertySafetyLevel)
    d->minPropertySafetyLevel = safetyLevel;
}

PiiQObjectServer::PropertySafetyLevel PiiQObjectServer::propertySafetyLevel(const QString& propertyName) const
{
  const PII_D;
  QMap<QString, PropertySafetyLevel>::const_iterator it = d->mapPropertySafetyLevels.find(propertyName);
  return it == d->mapPropertySafetyLevels.end() ?
    PropertySafetyLevel(d->safetyLevel + 1) :
    *it;
}

void PiiQObjectServer::removePropertySafetyLevel(const QString& propertyName)
{
  PII_D;
  d->mapPropertySafetyLevels.remove(propertyName);
  // Find the minimum access level of all properties.
  d->minPropertySafetyLevel = AccessPropertyConcurrently;
  for (QMap<QString, PropertySafetyLevel>::const_iterator it = d->mapPropertySafetyLevels.constBegin();
       it != d->mapPropertySafetyLevels.constEnd(); ++it)
    if (*it < d->minPropertySafetyLevel)
      d->minPropertySafetyLevel = *it;
}
