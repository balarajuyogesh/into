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

#ifndef _PIIMODBUSIODRIVER_H
#define _PIIMODBUSIODRIVER_H

#include "PiiDefaultIoDriver.h"
#include "PiiModbusIoDriverGlobal.h"
#include <PiiIoChannel.h>

#include <modbus.h>

class PiiModbusIoChannel;

/**
 * An implementation of the PiiIoChannel-interface for Modbus I/O
 * driver.
 *
 */
class PII_MODBUSIODRIVER_EXPORT PiiModbusIoDriver : public PiiDefaultIoDriver
{
  Q_OBJECT

public:
  PiiModbusIoDriver();
  ~PiiModbusIoDriver();
  
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
   * Initialize the driver. This function is must be called before the
   * driver can be accessed
   *
   * @return `true` if the initialization was successful, `false`
   * otherwise.
   */
  bool initialize();

  bool reset();

  /**
   * Returns the handle of the modbus driver.
   */
  modbus_t* handle() const;
  
protected:
  /**
   * Create a new PiiIoChannel.
   *
   * @param channel the channel number, 0 to [channelCount()] - 1.
   */
  PiiIoChannel* createChannel(int channel);

private:
  /// @internal
  class Data : public PiiDefaultIoDriver::Data
  {
  public:
    Data();

    modbus_t *pHandle;
    QString strUnit;
  };
  PII_D_FUNC;

  struct Instance
  {
    Instance() {}
    Instance(const QString& unit, PiiModbusIoDriver* driver) :
      strUnit(unit)
    {
      lstDriverInstances << driver;
    }
    
    bool operator== (const QString& unit) const { return strUnit == unit; }
    bool operator== (PiiModbusIoDriver* d) const { return lstDriverInstances.contains(d); }

    QList<PiiModbusIoDriver*> lstDriverInstances;
    QString strUnit;
  };

  bool initialize(const QString& unit);
  friend class PiiModbusIoChannel;
  int readInputBits(int addr, int nb, uint8_t *dest);
  int readBits(int addr, int nb, uint8_t *dest);
  int writeBit(int addr, int status);
  
  typedef QList<Instance> InstanceList;
  template <class T> static InstanceList::iterator findInstance(const T& value);
  static InstanceList _lstInstances;
  static QMutex _instanceMutex;
};


#endif //_PIIMODBUSIODRIVER_H
