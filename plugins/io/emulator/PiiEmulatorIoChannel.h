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

#ifndef _PIIEMULATORIOCHANNEL_H
#define _PIIEMULATORIOCHANNEL_H

#include "PiiDefaultIoChannel.h"
#include "PiiEmulatorIoDriverGlobal.h"

class PiiEmulatorIoDriver;

/**
 * I/O channel accessed through the emulator I/O-driver.
 *
 */
class PII_EMULATORIODRIVER_EXPORT PiiEmulatorIoChannel : public PiiDefaultIoChannel
{
  Q_OBJECT

public:
  PiiEmulatorIoChannel(PiiEmulatorIoDriver *parent, int channelIndex);

  void setOutputState(bool state);
  bool currentState() const;

private:
  /// @internal
  class Data : public PiiDefaultIoChannel::Data
  {
  public:
    Data();
    bool bCurrentState;
  };
  PII_D_FUNC;
};


#endif //_PIIEMULATORIOCHANNEL_H
