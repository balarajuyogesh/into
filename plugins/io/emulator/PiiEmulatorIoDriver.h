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

#ifndef _PIIEMULATORIODRIVER_H
#define _PIIEMULATORIODRIVER_H

#include "PiiDefaultIoDriver.h"
#include "PiiEmulatorIoDriverGlobal.h"
#include <PiiEmulatorIoChannel.h>
#include <PiiIoChannel.h>

/**
 * An implementation of the PiiIoChannel-interface for emulator.
 *
 */
class PII_EMULATORIODRIVER_EXPORT PiiEmulatorIoDriver : public PiiDefaultIoDriver
{
  Q_OBJECT

public:
  PiiEmulatorIoDriver();
  ~PiiEmulatorIoDriver();

  /**
   * Select the I/O unit to use. The driver may be able to handle many
   * I/O boards. The active unit is selected by a generic unit id.
   * Possibilities include, for example, network addresses and numeric
   * board indices.
   *
   * @return `true` if the selection was successful, `false`
   * otherwise.
   */
  bool selectUnit(const QString& unit);

  /**
   * Close an initialized driver. After `close`(), [initialize()] must
   * be called again before the driver is functional.
   *
   * @return `true` on success, `false` otherwise
   */
  bool close();

  /**
   * Get the total number of I/O channels.
   */
  int channelCount() const;

  /**
   * Initialize the driver. This function is must be called before the
   * driver can be accessed
   *
   * @return `true` if the initialization was successful, `false`
   * otherwise.
   */
  bool initialize();

protected:
  /**
   * Create a new PiiIoChannel.
   *
   * @param channel the channel number, 0 to [channelCount()] - 1.
   */
  PiiIoChannel* createChannel(int channel);
};


#endif //_PIIEMULATORIODRIVER_H
