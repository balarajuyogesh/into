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

#ifndef _PIIMODBUSIOCHANNEL_H
#define _PIIMODBUSIOCHANNEL_H

#include "PiiDefaultIoChannel.h"
#include "PiiModbusIoDriver.h"
#include <modbus.h>

/**
 * I/O channel accessed through the Modbus I/O-driver.
 *
 * @ingroup PiiIoPlugin
 */
class PII_MODBUSIODRIVER_EXPORT PiiModbusIoChannel : public PiiDefaultIoChannel
{
  Q_OBJECT

  /**
   * The address for the writing and reading registers from the
   * modbus.
   */
  Q_PROPERTY(int address READ address WRITE setAddress);
  
public:
  PiiModbusIoChannel(PiiModbusIoDriver *parent, int channelIndex);

  /**
   * Change output state.
   */
  void setOutputState(bool state);
  
  /**
   * Returns the current state of the channel.
   */
  bool currentState() const;

  /**
   * Set channel address for the writing and reading registers from
   * the modbus.
   */
  void setAddress(int address);

  /**
   * Get a channel address.
   */
  int address() const;

  PiiModbusIoDriver* driver() const { return static_cast<PiiModbusIoDriver*>(PiiDefaultIoChannel::driver()); }

private:
  /// @internal
  class Data : public PiiDefaultIoChannel::Data
  {
  public:
    Data();
    int iAddress;
  };
  PII_D_FUNC;

};


#endif //_PIIMODBUSIOCHANNEL_H
