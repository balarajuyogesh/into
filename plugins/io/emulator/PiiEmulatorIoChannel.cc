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

#include "PiiEmulatorIoChannel.h"
#include <PiiEmulatorIoDriver.h>

PiiEmulatorIoChannel::Data::Data() :
  bCurrentState(false)
{
}

PiiEmulatorIoChannel::PiiEmulatorIoChannel(PiiEmulatorIoDriver *driver, int channel) :
  PiiDefaultIoChannel(new Data, driver, channel)
{
}

bool PiiEmulatorIoChannel::currentState() const
{
  if (channelMode() == NoMode)
    channelError(tr("Cannot read current state (mode == NoMode)"));

  return _d()->bCurrentState;
}

void PiiEmulatorIoChannel::setOutputState(bool state)
{
  PII_D;

  if (channelMode() == NoMode)
    channelError(tr("Cannot set output state (mode == NoMode)"));

  d->bCurrentState = state;
}
