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

#include "PiiModbusIoDriver.h"
#include <PiiModbusIoChannel.h>
#include <errno.h>

PiiModbusIoDriver::InstanceList PiiModbusIoDriver::_lstInstances;
QMutex PiiModbusIoDriver::_instanceMutex;

PiiModbusIoDriver::Data::Data() :
  pHandle(0),
  strUnit("")
{
}

PiiModbusIoDriver::PiiModbusIoDriver() :
  PiiDefaultIoDriver(new Data)
{
}

PiiModbusIoDriver::~PiiModbusIoDriver()
{
  close();
}

modbus_t* PiiModbusIoDriver::handle() const { return _d()->pHandle; }

template <class T> PiiModbusIoDriver::InstanceList::iterator PiiModbusIoDriver::findInstance(const T& value)
{
  InstanceList::iterator it = _lstInstances.begin();
  for (; it != _lstInstances.end(); ++it)
    if (*it == value)
      break;
  return it;
}

bool PiiModbusIoDriver::initialize()
{
  synchronized (_instanceMutex) return initialize(_d()->strUnit);
  return false; // suppresses bogus compiler warning
}

bool PiiModbusIoDriver::initialize(const QString& unit)
{
  PII_D;
  QString strAddress = "127.0.0.1";
  int iPort = MODBUS_TCP_DEFAULT_PORT;

  if (!unit.isEmpty())
    {
      QStringList lstUnit = unit.split(":");
      if (lstUnit.size() > 0)
        strAddress = lstUnit[0];
      if (lstUnit.size() > 1)
        iPort = lstUnit[1].toInt();
    }

  InstanceList::iterator it = findInstance(unit);
  if (it != _lstInstances.end())
    {
      d->pHandle = it->lstDriverInstances.first()->handle();
      it->lstDriverInstances << this;
    }
  else
    {
      // Open driver
      d->pHandle = modbus_new_tcp(strAddress.toLatin1().constData(), iPort);

      if (d->pHandle == 0)
        {
          piiWarning(tr("Unable to allocate modbus context (%1:%2).").arg(strAddress).arg(iPort));
          return false;
        }

      timeval responseTimeout;

      responseTimeout.tv_sec = 5;
      responseTimeout.tv_usec = 0;
      modbus_set_response_timeout(d->pHandle, &responseTimeout);
      modbus_set_error_recovery(d->pHandle, MODBUS_ERROR_RECOVERY_PROTOCOL);

      if (modbus_connect(d->pHandle) == -1)
        {
          piiWarning(tr("Cannot connect to modbus device (%1:%2). %3")
                     .arg(strAddress).arg(iPort).arg(modbus_strerror(errno)));
          modbus_free(d->pHandle);
          d->pHandle = 0;
          return false;
        }
      if (modbus_set_slave(d->pHandle, 0) == -1)
        {
          piiWarning(tr("Cannot set modbus slave index."));
          modbus_free(d->pHandle);
          d->pHandle = 0;
          return false;
        }

      _lstInstances.append(Instance(unit, this));
    }

  piiDebug(tr("Modbus connected to %1:%2.").arg(strAddress).arg(iPort));

  //modbus_set_debug(d->pHandle, TRUE);

  return true;
}

bool PiiModbusIoDriver::selectUnit(const QString& unit)
{
  _d()->strUnit = unit;
  return true;
}

bool PiiModbusIoDriver::reset()
{
  PII_D;
  QMutexLocker lock(&_instanceMutex);
  InstanceList::iterator it = findInstance(this);
  if (it != _lstInstances.end())
    {
      QList<PiiModbusIoDriver*> lstOtherDrivers = it->lstDriverInstances;
      lstOtherDrivers.removeOne(this);

      // If my handle is zero, another driver instance using the same
      // handle has already reset itself.
      if (d->pHandle == 0)
        {
          for (int i=0; i<lstOtherDrivers.size(); ++i)
            if (lstOtherDrivers[i]->_d()->pHandle != 0)
              {
                d->pHandle = lstOtherDrivers[i]->_d()->pHandle;
                piiWarning(tr("Modbus reusing restored connection to %1.").arg(it->strUnit));
                return true;
              }
          return false;
        }

      QString strUnit = it->strUnit;
      modbus_flush(d->pHandle);
      modbus_close(d->pHandle);
      modbus_free(d->pHandle);

      for (int i=0; i<it->lstDriverInstances.size(); ++i)
        it->lstDriverInstances[i]->_d()->pHandle = 0;
      _lstInstances.erase(it);
      if (initialize(strUnit))
        {
          it = findInstance(this);
          it->lstDriverInstances << lstOtherDrivers;
          return true;
        }
    }
  return false;
}

bool PiiModbusIoDriver::close()
{
  PII_D;
  QMutexLocker lock(&_instanceMutex);
  if (d->pHandle)
    {
      // Cannot trust d->strUnit here as it may have been changed.
      InstanceList::iterator it = findInstance(this);
      if (it != _lstInstances.end())
        {
          it->lstDriverInstances.removeOne(this);
          if (it->lstDriverInstances.isEmpty())
            {
              modbus_flush(d->pHandle);
              modbus_close(d->pHandle);
              modbus_free(d->pHandle);
              _lstInstances.erase(it);
            }
        }
      d->pHandle = 0;
    }

  return true;
}

PiiIoChannel* PiiModbusIoDriver::createChannel(int channel)
{
  return new PiiModbusIoChannel(this, channel);
}

int PiiModbusIoDriver::readInputBits(int addr, int nb, uint8_t *dest)
{
  QMutexLocker lock(&_instanceMutex);
  PII_D;
  return d->pHandle ? modbus_read_input_bits(d->pHandle, addr, nb, dest) : -1;
}

int PiiModbusIoDriver::readBits(int addr, int nb, uint8_t *dest)
{
  QMutexLocker lock(&_instanceMutex);
  PII_D;
  return d->pHandle ? modbus_read_bits(d->pHandle, addr, nb, dest) : -1;
}

int PiiModbusIoDriver::writeBit(int addr, int status)
{
  QMutexLocker lock(&_instanceMutex);
  PII_D;
  return d->pHandle ? modbus_write_bit(d->pHandle, addr, status) : -1;
}
