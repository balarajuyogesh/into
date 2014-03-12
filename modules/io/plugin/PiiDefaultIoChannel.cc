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

#include "PiiDefaultIoChannel.h"
#include "PiiDefaultIoDriver.h"
#include <QDateTime>
#include <QTime>

PiiDefaultIoChannel::Data::Data() :
  pDriver(0),
  iChannelIndex(0),
  channelMode(NoMode),
  iPulseWidth(50),
  iPulseDelay(0),
  iPreviousInputState(-1),
  bSignalFallingEdge(false),
  bActiveState(true),
  bCurrentState(true)
{
}

PiiDefaultIoChannel::Data::~Data()
{
}

PiiDefaultIoChannel::PiiDefaultIoChannel(Data *data, PiiDefaultIoDriver *driver, int channelIndex) : d(data)
{
  d->iChannelIndex = channelIndex;
  d->pDriver = driver;

  setParent(driver);
}

PiiDefaultIoChannel::PiiDefaultIoChannel(PiiDefaultIoDriver *driver, int channelIndex) : d(new Data)
{
  d->iChannelIndex = channelIndex;
  d->pDriver = driver;

  setParent(driver);
}

PiiDefaultIoChannel::~PiiDefaultIoChannel()
{
  d->pDriver->removePollingInput(this);
  delete d;
}

void PiiDefaultIoChannel::setSignalFallingEdge(bool signalFallingEdge)
{
  d->bSignalFallingEdge = signalFallingEdge;
}

void PiiDefaultIoChannel::setActiveState(bool activeState)
{
  d->bActiveState = activeState;
}

void PiiDefaultIoChannel::setChannelMode(const ChannelMode& channelMode)
{
  d->channelMode = channelMode;
}

void PiiDefaultIoChannel::setSignalEnabled(bool enabled)
{
  d->pDriver->removePollingInput(this);
  if (enabled)
    d->pDriver->addPollingInput(this);
}

void PiiDefaultIoChannel::checkInputState()
{
  bool bState = currentState();

  if (d->iPreviousInputState == -1)
    {
      d->iPreviousInputState = int(bState);
      return;
    }
  if (d->iPreviousInputState == int(bState))
    return;

  d->iPreviousInputState = int(bState);
  bool bActive = d->bActiveState ? bState : !bState;

  if (d->bSignalFallingEdge || bActive)
    emit inputStateChanged(bActive);
}

void PiiDefaultIoChannel::activate()
{
  if (d->pDriver != 0 && d->channelMode == Output)
    {
      qint64 time = QDateTime::currentDateTime().addMSecs(d->iPulseDelay).toMSecsSinceEpoch();
      d->pDriver->sendSignal(this, d->bActiveState, time, d->iPulseWidth);
    }
}

void PiiDefaultIoChannel::initializeChannel()
{
  // Poll this channel if it is an input.
  setSignalEnabled(d->channelMode == Input);

  // Init output state
  if (d->channelMode == Output)
    setOutputState(!activeState());
}

void PiiDefaultIoChannel::channelError(const QString& message) const
{
  emit channelFailed(const_cast<PiiDefaultIoChannel*>(this), message);
  PII_THROW(PiiIoDriverException, message);
}

PiiDefaultIoDriver* PiiDefaultIoChannel::driver() const { return d->pDriver; }
void PiiDefaultIoChannel::setChannelName(const QString& channelName) { d->strChannelName = channelName; }
QString PiiDefaultIoChannel::channelName() const { return d->strChannelName; }
PiiDefaultIoChannel::ChannelMode PiiDefaultIoChannel::channelMode() const { return d->channelMode; }
void PiiDefaultIoChannel::setPulseWidth(int pulseWidth) { d->iPulseWidth = pulseWidth; }
int PiiDefaultIoChannel::pulseWidth() const { return d->iPulseWidth; }
void PiiDefaultIoChannel::setPulseDelay(int pulseDelay) { d->iPulseDelay = pulseDelay; }
int PiiDefaultIoChannel::pulseDelay() const { return d->iPulseDelay; }
bool PiiDefaultIoChannel::signalFallingEdge() const { return d->bSignalFallingEdge; }
bool PiiDefaultIoChannel::activeState() const { return d->bActiveState; }
int PiiDefaultIoChannel::channelIndex() const { return d->iChannelIndex; }
