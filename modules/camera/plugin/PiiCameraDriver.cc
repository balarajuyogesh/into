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

#include "PiiCameraDriver.h"

#include <PiiSerializableExport.h>

PII_SERIALIZABLE_EXPORT(PiiCameraDriver);

PiiCameraDriver::Data::Data() :
  pListener(0)
{
}

PiiCameraDriver::PiiCameraDriver() : d(new Data)
{
}

PiiCameraDriver::~PiiCameraDriver()
{
  delete d;
}

bool PiiCameraDriver::requiresInitialization(const char* /*name*/) const
{
  return false;
}

int PiiCameraDriver::cameraType() const
{
  return (int)PiiCamera::AreaScan;
}

QVariantMap& PiiCameraDriver::propertyMap() { return d->mapProperties; }

QVariant PiiCameraDriver::property(const char* name) const
{
  if (isOpen())
    return QObject::property(name);
  else if (d->mapProperties.contains(name))
    return d->mapProperties[name];

  return QVariant();
}

bool PiiCameraDriver::setProperty(const char* name, const QVariant& value)
{
  if (!isOpen() || requiresInitialization(name))
    {
      const QMetaObject *pMetaObject = metaObject();
      QVariant::Type type = pMetaObject->property(pMetaObject->indexOfProperty(name)).type();

      if (value.type() == type)
        d->mapProperties[name] = value;
      else
        {
          QVariant copy(value);
          if (copy.convert(type))
            d->mapProperties[name] = copy;
          else
            return false;
        }

      return true;
    }
  else
    return QObject::setProperty(name, value);
}



void PiiCameraDriver::setListener(Listener* listener) { d->pListener = listener; }
PiiCameraDriver::Listener* PiiCameraDriver::listener() const { return d->pListener; }

PiiCameraDriver::Listener::~Listener() {}
void PiiCameraDriver::Listener::frameCaptured(int /*frameIndex*/, void* /*frameBuffer*/, qint64 /*elapsedTime*/) {}
void PiiCameraDriver::Listener::framesMissed(int /*startIndex*/, int /*endIndex*/) {}
void PiiCameraDriver::Listener::captureFinished(bool /*state*/) {}
void PiiCameraDriver::Listener::captureError(const QString& /*message*/) {}

