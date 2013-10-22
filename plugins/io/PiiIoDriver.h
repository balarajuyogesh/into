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

#ifndef _PIIIODRIVER_H
#define _PIIIODRIVER_H

#include "PiiIoGlobal.h"
#include <QObject>
class PiiIoChannel;

/**
 * An interface for digital input/output drivers.
 *
 */
class PII_IO_EXPORT PiiIoDriver : public QObject
{
  Q_OBJECT

public:
  virtual ~PiiIoDriver();
  /**
   * Selects the I/O unit to use. The driver may be able to handle
   * many I/O boards. The active unit is selected by a generic unit
   * id. Possibilities include, for example, network addresses and
   * numeric board indices.
   *
   * @return `true` if the selection was successful, `false`
   * otherwise.
   */
  virtual bool selectUnit(const QString& unit) = 0;

  /**
   * Initializes the driver. This function is must be called before
   * the driver can be accessed
   *
   * @return `true` if the initialization was successful, `false`
   * otherwise.
   */
  virtual bool initialize() = 0;

  /**
   * Closes an initialized driver. After `close`(), [initialize()]
   * must be called again before the driver is functional. The default
   * implementation returns `true`.
   *
   * @return `true` on success, `false` otherwise
   */
  virtual bool close() = 0;

  /**
   * Resets the driver. The default implementation calls close() and
   * initialize().
   */
  virtual bool reset();

  /**
   * Returns the total number of I/O channels.
   */
  virtual int channelCount() const = 0;

  /**
   * Returns a pointer to a PiiIoChannel object that stores the
   * configuration of the given channel.
   *
   * @param channel the channel number.
   */
  virtual PiiIoChannel* channel(int channel) = 0;

signals:
  void connectionLost();
};

PII_SERIALIZATION_NAME(PiiIoDriver);

#endif //_PIIIODRIVER_H
