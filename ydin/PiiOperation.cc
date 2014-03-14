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

#include "PiiOperation.h"
#include <PiiUtil.h>
#include <PiiSerializationFactory.h>
#include <PiiSerializableExport.h>
#include "PiiYdinResources.h"
#include <PiiMath.h>

PII_DEFINE_VIRTUAL_METAOBJECT_FUNCTION(PiiOperation);
PII_SERIALIZABLE_EXPORT(PiiOperation);
PII_SERIALIZABLE_EXPORT(PiiQVariantWrapper::Template<PiiOperation*>);
PII_SERIALIZABLE_EXPORT(PiiQVariantWrapper::Template<PiiOperationPtr>);

static int iOperationStateMetaType = qRegisterMetaType<PiiOperation::State>("PiiOperation::State");
static int iOperationMetaType = qRegisterMetaType<PiiOperation*>("PiiOperation*");
static int iOperationPtrMetaType = qRegisterMetaType<PiiOperationPtr>("PiiOperationPtr");

PiiOperation::Data::Data() :
  activityMode(Enabled),
  stateMutex(QMutex::Recursive),
  bCachingProperties(false),
  bApplyingPropertySet(false),
  pmapMetaPropertyCache(0)
{}

PiiOperation::Data::~Data()
{}

PiiOperation::PiiOperation() :
  d(new Data)
{
  Q_UNUSED(iOperationMetaType); // suppresses compiler warning
  Q_UNUSED(iOperationPtrMetaType);
  Q_UNUSED(iOperationStateMetaType);
}

PiiOperation::PiiOperation(Data* data) :
  d(data)
{}

PiiOperation::~PiiOperation()
{
  delete d;
}

int PiiOperation::inputCount() const
{
  return inputs().size();
}

int PiiOperation::outputCount() const
{
  return outputs().size();
}

PiiAbstractInputSocket* PiiOperation::input(const QString& name) const
{
  foreach (PiiAbstractInputSocket* pInput, inputs())
    if (pInput->objectName() == name)
      return pInput;
  return 0;
}

PiiAbstractInputSocket* PiiOperation::inputAt(int index) const
{
  if (index >= 0 && index < inputCount())
    return inputs()[index];
  return 0;
}

QStringList PiiOperation::inputNames() const
{
  const QList<PiiAbstractInputSocket*> lstInputs = inputs();
  QStringList lstResult;
  for (int i=0; i<lstInputs.size(); i++)
    lstResult << lstInputs[i]->objectName();
  return lstResult;
}

PiiAbstractOutputSocket* PiiOperation::output(const QString& name) const
{
  foreach (PiiAbstractOutputSocket* pOutput, outputs())
    if (pOutput->objectName() == name)
      return pOutput;
  return 0;
}

PiiAbstractOutputSocket* PiiOperation::outputAt(int index) const
{
  if (index >= 0 && index < outputCount())
    return outputs()[index];
  return 0;
}

QStringList PiiOperation::outputNames() const
{
  const QList<PiiAbstractOutputSocket*> lstOutputs = outputs();
  QStringList lstResult;
  for (int i=0; i<lstOutputs.size(); i++)
    lstResult << lstOutputs[i]->objectName();
  return lstResult;
}

QString PiiOperation::fullName() const
{
  QString strName;
  QList<PiiOperation*> lstParent = Pii::findAllParents<PiiOperation*>(this);
  for (int i=lstParent.size()-2; i>=0; --i)
    strName += lstParent[i]->objectName() + '.';
  return strName + objectName();
}

bool PiiOperation::connectOutput(const QString& outputName, PiiAbstractInputSocket* input)
{
  PiiAbstractOutputSocket *out = output(outputName);
  if (out == 0)
    {
      piiWarning(tr("There is no \"%1\" output in %2.").arg(outputName, metaObject()->className()));
      return false;
    }
  out->connectInput(input);
  return true;
}

bool PiiOperation::connectOutput(const QString& outputName, PiiOperation* other, const QString& inputName)
{
  if (other == 0)
    {
      piiWarning(tr("Cannot connect \"%1\" to the \"%2\" input of a null operation. "
                    "You probably forgot to load a required plug-in").arg(outputName, inputName));
      return false;
    }
  PiiAbstractInputSocket *in = other->input(inputName);
  if (in == 0)
    {
      piiWarning(tr("There is no \"%1\" input in %2.").arg(inputName, other->metaObject()->className()));
      return false;
    }
  return connectOutput(outputName, in);
}

bool PiiOperation::connectOutput(const QString& outputName, const QVariant& input)
{
  PiiAbstractInputSocket* pInput = qobject_cast<PiiAbstractInputSocket*>(input.value<QObject*>());
  if (!pInput)
    {
      pInput = this->input(input.toString());
      if (!pInput)
        {
          piiWarning(tr("There is no \"%1\" input in %2.").arg(input.toString(), metaObject()->className()));
          return false;
        }
    }
  return connectOutput(outputName, pInput);
}

const char* PiiOperation::stateName(State state)
{
  static const char* states[] = { "Stopped", "Starting", "Running", "Pausing", "Paused", "Stopping", "Interrupted" };
  return state >= 0 && state < sizeof(states)/sizeof(states[0]) ? states[state] : 0;
}

void PiiOperation::startPropertySet(const QString& name)
{
  d->bCachingProperties = true;
  d->strPropertySetName = name;
}

bool PiiOperation::isCachingProperties() const { return d->bCachingProperties; }
QString PiiOperation::propertySetName() const { return d->strPropertySetName; }

void PiiOperation::endPropertySet()
{
  d->bCachingProperties = false;
  d->strPropertySetName = QString();
}

void PiiOperation::removePropertySet(const QString& name)
{
  d->mapCachedProperties.remove(name);
}

void PiiOperation::applyPropertySet(const QString& name)
{
  // Don't apply to itself.
  if (d->bCachingProperties && name == d->strPropertySetName)
    return;

  d->bApplyingPropertySet = true;
  Pii::setProperties(this, d->mapCachedProperties[name]);
  d->bApplyingPropertySet = false;
}

void PiiOperation::addPropertyToList(PropertyList& properties,
                                     const QString& name,
                                     const QVariant& value)
{
  for (int i=0; i<properties.size(); ++i)
    {
      if (properties[i].first == name)
        {
          properties[i].second = value;
          return;
        }
    }
  properties << qMakePair(name, value);
}

QVariant PiiOperation::applyLimits(const char* property, const QVariant& value) const
{
  // Can apply limits to numbers only
  if (value.type() == QVariant::Int)
    return applyLimits<int>(property, value);
  else if (value.type() == QVariant::Double)
    return applyLimits<double>(property, value);
  return value;
}

template <class T>
QVariant PiiOperation::applyLimits(const char* property, const QVariant& value) const
{
  const QVariantMap mapRange(metaProperties(property));
  if (mapRange.isEmpty())
    return value;

  T numericValue = value.value<T>();
  T minValue = 0;
  QVariantMap::const_iterator it = mapRange.find("min");
  if (it != mapRange.end())
    {
      minValue = it->value<T>();
      if (numericValue < minValue)
        return *it;
    }
  it = mapRange.find("max");
  if (it != mapRange.end() && numericValue > it->value<T>())
    return *it;
  it = mapRange.find("step");
  if (it != mapRange.end())
    return numericValue - Pii::mod(numericValue - minValue, it->value<T>());

  return value;
}

bool PiiOperation::setProperty(const char* name, const QVariant& value)
{
  QMutexLocker lock(&d->stateMutex);
  if (d->bCachingProperties)
    {
      addPropertyToList(d->mapCachedProperties[d->strPropertySetName], name, value);
      return true;
    }
  ProtectionLevel level = protectionLevel(name);
  if (level != WriteAlways)
    {
      State currentState = state();
      // Setting properties via applyPropertySet() is equivalent
      // to pausing.
      if (currentState != Stopped && d->bApplyingPropertySet)
        currentState = Paused;
      if ((level == WriteWhenStoppedOrPaused && currentState not_member_of (Stopped, Paused)) ||
          (level == WriteWhenStopped && currentState != Stopped))
        return false;
    }
  return QObject::setProperty(name, applyLimits(name, value));
}

bool PiiOperation::setProperty(const char* name, const PiiVariant& value)
{
  return setProperty(name, QVariant::fromValue(value));
}

bool PiiOperation::setProperty(const QString& name, const QVariant& value)
{
  return setProperty(qPrintable(name), value);
}

QVariant PiiOperation::property(const char* name) const
{
  return QObject::property(name);
}

QVariant PiiOperation::property(const QString& name) const
{
  return property(qPrintable(name));
}

QVariant PiiOperation::parseNumber(const QString& number)
{
  bool bOk = false;
  int i = number.toInt(&bOk);
  if (bOk) return i;
  double d = number.toDouble(&bOk);
  if (bOk) return d;
  return number;
}

void PiiOperation::parseRange(QVariantMap& map, const QString& range)
{
  QStringList lstParts(range.split(':'));
  if (lstParts.size() member_of (2,3))
    {
      if (!lstParts[0].isEmpty())
        map["min"] = parseNumber(lstParts[0]);
      if (!lstParts.last().isEmpty())
        map["max"] = parseNumber(lstParts.last());
      if (lstParts.size() == 3 && !lstParts[1].isEmpty())
        map["step"] = parseNumber(lstParts[1]);
    }
}

void PiiOperation::parseProperty(QVariantMap& map, const QString& metaPropertyName, const QString& value)
{
  if (metaPropertyName == "range")
    parseRange(map, value);
  else
    map[metaPropertyName] = parseNumber(value);
}

QVariant PiiOperation::metaProperty(const QString& propertyName, const QString& metaPropertyName) const
{
  if (d->pmapMetaPropertyCache ||
      (d->pmapMetaPropertyCache = createMetaPropertyCache(metaObject())))
    {
      // Two-level lookup in class-specific cache
      QMap<QString,QVariantMap>::const_iterator it = d->pmapMetaPropertyCache->find(propertyName);
      if (it != d->pmapMetaPropertyCache->end())
        {
          QVariantMap::const_iterator it2 = it->find(metaPropertyName);
          if (it2 != it->end())
            return *it2;
        }
    }
  return QVariant();
}

QVariantMap PiiOperation::metaProperties(const QString& propertyName) const
{
  if (d->pmapMetaPropertyCache ||
      (d->pmapMetaPropertyCache == createMetaPropertyCache(metaObject())))
    {
      QMap<QString,QVariantMap>::const_iterator it = d->pmapMetaPropertyCache->find(propertyName);
      if (it != d->pmapMetaPropertyCache->end())
        return *it;
    }
  return QVariantMap();
}

const QMap<QString,QVariantMap>* PiiOperation::createMetaPropertyCache(const QMetaObject* metaObj)
{
  static QMutex cacheMutex;
  QMutexLocker lock(&cacheMutex);

  // If there is already a cache for the class, simply return a
  // pointer to it.
  MetaPropertyCache* pCache = metaPropertyCache();
  MetaPropertyCache::iterator it = pCache->find(metaObj->className());
  if (it != pCache->end())
    return &(*it);

  // Otherwise create a new cache.
  QMap<QString,QVariantMap> mapCache;
  // Parse all class info fields
  for (int i=0; i<metaObj->classInfoCount(); ++i)
    {
      QMetaClassInfo info = metaObj->classInfo(i);
      QStringList lstParts(QString(info.name()).split('.'));
      // The info is a metaproperty if it begins with a property name.
      if (lstParts.size() == 2 &&
          metaObj->indexOfProperty(qPrintable(lstParts[0])) != -1)
        {
          // Add this property to the map of metaproperties
          parseProperty(mapCache[lstParts[0]], lstParts[1], info.value());
        }
    }
  return &(*pCache->insert(metaObj->className(), mapCache));
}


PiiOperation::MetaPropertyCache* PiiOperation::metaPropertyCache()
{
  static MetaPropertyCache cache;
  return &cache;
}

PiiOperation* PiiOperation::clone() const
{
  // objectName() won't work for template classes, but we have the
  // serialization meta object...
  const char *className = PiiYdin::resourceName(this);

  // Use the serialization factory to create an instance of the class
  PiiOperation *op = PiiYdin::createResource<PiiOperation>(className);

  // Copy properties from the old to the new one
  if (op != 0)
    Pii::setProperties(op, Pii::propertyList(this, 0, Pii::WritableProperties | Pii::DynamicProperties));

  return op;
}

void PiiOperation::disconnectAllInputs()
{
  QList<PiiAbstractInputSocket*> lstInputs = inputs();

  for (int i=0; i<lstInputs.size(); ++i)
    lstInputs[i]->disconnectOutput();
}

void PiiOperation::disconnectAllOutputs()
{
  QList<PiiAbstractOutputSocket*> lstOutputs = outputs();

  for (int i=0; i<lstOutputs.size(); ++i)
    lstOutputs[i]->disconnectInputs();
}

int PiiOperation::indexOf(const char* property) const
{
  for (int i=0; i<d->lstProtectionLevels.size(); ++i)
    if (!strcmp(d->lstProtectionLevels[i].first, property))
      return i;
  return -1;
}

void PiiOperation::setProtectionLevel(const char* property, ProtectionLevel level)
{
  int iIndex = indexOf(property);
  if (level != WriteAlways)
    {
      // Add new protection level
      if (iIndex == -1)
        d->lstProtectionLevels.append(qMakePair(property, level));
      // Update existing one
      else
        d->lstProtectionLevels[iIndex].second = level;
    }
  else if (iIndex != -1) // Remove protection
    d->lstProtectionLevels.removeAt(iIndex);
}

PiiOperation::ProtectionLevel PiiOperation::protectionLevel(const char* property) const
{
  int iIndex = indexOf(property);
  if (iIndex == -1) return WriteAlways;
  return d->lstProtectionLevels[iIndex].second;
}

bool PiiOperation::isCompound() const { return d->isCompound(); }

QMutex* PiiOperation::stateLock() { return &d->stateMutex; }

PiiOperation::ProtectionLevel PiiOperation::protectionLevel(const QString& property) const
{
  return protectionLevel(qPrintable(property));
}

QVariant PiiOperation::socketData(PiiSocket*, int) const { return QVariant(); }

void PiiOperation::setActivityMode(ActivityMode activityMode)
{
  QMutexLocker lock(&d->stateMutex);
  if (state() member_of (Stopped, Paused))
    {
      if (activityMode != d->activityMode)
        {
          d->activityMode = activityMode;
          emit activityModeChanged(activityMode);
        }
      updateActivityMode(activityMode);
    }
}

PiiOperation::ActivityMode PiiOperation::activityMode() const { return d->activityMode; }
void PiiOperation::updateActivityMode(ActivityMode) {}

void PiiOperation::setErrorString(const QString& errorString) { d->strErrorString = errorString; }
QString PiiOperation::errorString() const { return d->strErrorString; }

bool PiiOperation::hasError() const { return !d->strErrorString.isEmpty(); }
