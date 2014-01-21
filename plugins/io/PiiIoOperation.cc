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

#include "PiiIoOperation.h"

#include <PiiYdinTypes.h>
#include <PiiIoDriverException.h>

PiiIoOperation::Data::Data() :
  pIoDriver(0),
  strIoDriver(""),
  strIoUnit("0"),
  bEnabled(true)
{
}

PiiIoOperation::PiiIoOperation() : PiiDefaultOperation(new Data)
{
  setProtectionLevel("channelConfigs", WriteWhenStoppedOrPaused);
}

PiiIoOperation::PiiIoOperation(Data* data) : PiiDefaultOperation(data)
{
  setProtectionLevel("channelConfigs", WriteWhenStoppedOrPaused);
}

PiiIoOperation::~PiiIoOperation()
{
  delete _d()->pIoDriver;
}

void PiiIoOperation::emitConnectionFailure()
{
  emit errorOccured(this, tr("Connection to I/O device (%1) was lost.").arg(_d()->strIoUnit));
}

void PiiIoOperation::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);

  QMutexLocker lock(&d->channelMutex);

  d->lstChannels.clear();

  if (reset)
    {
      if (d->strIoDriver.isEmpty())
        piiWarning(tr("I/O driver has not been set."));
      else
        {
          PiiIoDriver* driver = PiiYdin::createResource<PiiIoDriver>(d->strIoDriver);

          if (driver != 0)
            {
              delete d->pIoDriver;
              d->pIoDriver = driver;
            }
          else
            PII_THROW(PiiExecutionException, tr("I/O driver (%1) has not been set or is not available.").arg(d->strIoDriver));

          if (d->pIoDriver)
            {
              connect(d->pIoDriver, SIGNAL(connectionLost()), SLOT(emitConnectionFailure()));
              d->pIoDriver->selectUnit(d->strIoUnit);

              if (!d->pIoDriver->initialize())
                PII_THROW(PiiExecutionException, tr("Cannot initialize I/O driver."));
            }
        }
    }

  if (d->pIoDriver != 0)
    {
      // Create channels
      for (int i=0; i<d->lstChannelConfigs.size(); ++i)
        {
          QVariantMap mapChannelConfig = d->lstChannelConfigs[i].toMap();
          int iChannelIndex = mapChannelConfig.value("channel", i).toInt();
          PiiIoChannel* pChannel = d->pIoDriver->channel(iChannelIndex);
          if (pChannel == 0)
            PII_THROW(PiiExecutionException, tr("There is no I/O channel %1.").arg(iChannelIndex));
          mapChannelConfig.remove("channel");
          Pii::setProperties(pChannel, mapChannelConfig);
          d->lstChannels << pChannel;
        }

      // Initialize channels
      for (int i=0; i<d->lstChannels.size(); i++)
        {
          try
            {
              d->lstChannels[i]->initializeChannel();
            }
          catch (PiiIoDriverException &ex)
            {
              piiWarning(tr("%1").arg(ex.message()));
            }
        }
    }
}

void PiiIoOperation::setChannelConfigs(const QVariantList& channelConfigs)
{
  _d()->lstChannelConfigs = channelConfigs;
}

QVariantList PiiIoOperation::channelConfigs() const { return _d()->lstChannelConfigs; }
int PiiIoOperation::channelCount() const { return _d()->lstChannelConfigs.size(); }

QVariantList PiiIoOperation::channelStates() const
{
  const PII_D;
  QMutexLocker lock(const_cast<QMutex*>(&d->channelMutex));

  int iCount = 0;
  while (iCount < 2)
    {
      try
        {
          QVariantList lstStates;
          for (int i=0; i<d->lstChannels.size(); ++i)
            {
              bool bState = d->lstChannels[i]->currentState();
              lstStates.append(d->lstChannels[i]->property("activeState").toBool() ? bState : !bState);
            }
          return lstStates;
        }
      catch (PiiIoDriverException& ex)
        {
          ++iCount;
        }
    }
  piiWarning(tr("PiiIoOperation: Couldn't read channel states"));
  return QVariantList();
}

void PiiIoOperation::setIoDriver(const QString& ioDriver) { _d()->strIoDriver = ioDriver; }
QString PiiIoOperation::ioDriver() const { return _d()->strIoDriver; }

void PiiIoOperation::setIoUnit(const QString& ioUnit) { _d()->strIoUnit = ioUnit; }
QString PiiIoOperation::ioUnit() const { return _d()->strIoUnit; }

void PiiIoOperation::setEnabled(bool enabled) { _d()->bEnabled = enabled; }
bool PiiIoOperation::enabled() const { return _d()->bEnabled; }
