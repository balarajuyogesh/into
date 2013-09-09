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

#include "PiiModbusIoChannel.h"
#include <errno.h>

PiiModbusIoChannel::Data::Data() :
  iAddress(-1)
{
}

PiiModbusIoChannel::PiiModbusIoChannel(PiiModbusIoDriver *driver, int channel) :
  PiiDefaultIoChannel(new Data, driver, channel)
{
}

bool PiiModbusIoChannel::currentState() const
{
  const PII_D;
  
  ChannelMode mode = channelMode();
  if (mode == NoMode)
    channelError(tr("Cannot read current state (mode == NoMode)"));
  
  uint8_t bits;

  int res = 0;
  if (mode == Input)
    res = driver()->readInputBits(d->iAddress, 1, &bits);
  else 
    res = driver()->readBits(d->iAddress, 1, &bits);
  
  if (res == -1)
    channelError(tr("Cannot read current state (channel = %1): %2")
                 .arg(channelIndex()).arg(modbus_strerror(errno)));

  return bits == 1;
}

void PiiModbusIoChannel::setOutputState(bool active)
{
  PII_D;
  
  if (channelMode() member_of (Input, NoMode))
    channelError(tr("Cannot set output state (channelMode is wrong)"));

  if (driver()->writeBit(d->iAddress, active ? 1 : 0) != 1)
    channelError(tr("PiiModbusIoChannel : cannot set output state: %1").arg(modbus_strerror(errno)));
}

void PiiModbusIoChannel::setAddress(int address)
{
  _d()->iAddress = address;
}

int PiiModbusIoChannel::address() const
{
  return _d()->iAddress;
}
