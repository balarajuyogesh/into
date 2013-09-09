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

#include "PiiSocketDevice.h"

#include "PiiProgressController.h"

#include <QAbstractSocket>
#include <QLocalSocket>

PiiSocketDevice::Data::Data() :
  pDevice(0),
  type(IODevice)
{}

PiiSocketDevice::Data::Data(QIODevice* device) :
  pDevice(device),
  type(typeOf(device))
{}

PiiSocketDevice::Data::Data(QIODevice* device, Type t) :
  pDevice(device),
  type(t)
{}

PiiSocketDevice::Data::Data(const Data& other) :
  pDevice(other.pDevice),
  type(other.type)
{}

PiiSocketDevice::Type PiiSocketDevice::typeOf(QIODevice* device)
{
  if (qobject_cast<QAbstractSocket*>(device) != 0)
    return AbstractSocket;
  else if (qobject_cast<QLocalSocket*>(device) != 0)
    return LocalSocket;
  return IODevice;
}

PiiSocketDevice::PiiSocketDevice() :
  d(new Data)
{
}

PiiSocketDevice::PiiSocketDevice(QIODevice* device) :
  d(new Data(device))
{
}
PiiSocketDevice::PiiSocketDevice(QAbstractSocket* device) :
  d(new Data(device, AbstractSocket))
{
}

PiiSocketDevice::PiiSocketDevice(QLocalSocket* device) :
  d(new Data(device, LocalSocket))
{
}

PiiSocketDevice::PiiSocketDevice(const PiiSocketDevice& other) :
  d(other.d)
{
  d->reserve();
}

PiiSocketDevice::~PiiSocketDevice()
{
  d->release();
}

PiiSocketDevice& PiiSocketDevice::operator= (const PiiSocketDevice& other)
{
  other.d->assignTo(d);
  return *this;
}

PiiSocketDevice& PiiSocketDevice::operator= (QIODevice* device)
{
  d = d->detach();
  d->pDevice = device;
  d->type = typeOf(device);
  return *this;
}

bool PiiSocketDevice::isReadable() const
{
  if (d->pDevice == 0) return false;
  switch (d->type)
    {
    case AbstractSocket:
      return static_cast<QAbstractSocket*>(d->pDevice)->state() == QAbstractSocket::ConnectedState ||
        d->pDevice->bytesAvailable() > 0;
    case LocalSocket:
      return static_cast<QLocalSocket*>(d->pDevice)->state() == QLocalSocket::ConnectedState ||
        d->pDevice->bytesAvailable() > 0;
    case IODevice:
    default:
      return d->pDevice->openMode() & QIODevice::ReadOnly;
    }
}

bool PiiSocketDevice::isWritable() const
{
  if (d->pDevice == 0) return false;
  switch (d->type)
    {
    case AbstractSocket:
      return static_cast<QAbstractSocket*>(d->pDevice)->state() == QAbstractSocket::ConnectedState;
    case LocalSocket:
      return static_cast<QLocalSocket*>(d->pDevice)->state() == QLocalSocket::ConnectedState;
    case IODevice:
    default:
      return d->pDevice->openMode() & QIODevice::WriteOnly;
    }
}

bool PiiSocketDevice::waitForDataAvailable(int waitTime, PiiProgressController* controller)
{
  // The first isReadable() call checks for null device.
  int iTimeout = qMin(waitTime, 100);
  PiiTimer timer;
  do
    {
      if (!isReadable() || (controller != 0 && !controller->canContinue()))
        return false;
      else if (d->pDevice->waitForReadyRead(iTimeout))
        return true;
    }
  while (timer.milliseconds() < waitTime);
  return false;
}
  
qint64 PiiSocketDevice::readWaited(char* data, qint64 maxSize, int waitTime, PiiProgressController* controller)
{
  if (d->pDevice == 0) return -1;
  if (maxSize == 0) return 0;

  // Count the total number of bytes read during this call
  qint64 iBytesRead = 0;
  do
    {
      // Read what we can
      qint64 iBytesInPiece = d->pDevice->read(data, maxSize);
      if (iBytesInPiece > 0)
        {
          iBytesRead += iBytesInPiece;
          data += iBytesInPiece;
          maxSize -= iBytesInPiece;
        }
      else if (iBytesInPiece < 0) // error
        return iBytesRead > 0 ? iBytesRead : -1;
      else if (!waitForDataAvailable(waitTime, controller))
        return iBytesRead;
    }
  while (maxSize > 0);
  
  return iBytesRead;
}

bool PiiSocketDevice::waitForDataWritten(int waitTime, PiiProgressController* controller)
{
  int iTimeout = qMin(waitTime, 100);
  PiiTimer timer;
  do
    {
      if (!isWritable() || (controller != 0 && !controller->canContinue()))
        return false;
      else if (d->pDevice->waitForBytesWritten(iTimeout))
        {
          if (d->pDevice->bytesToWrite() <= 0)
            return true;
        }
      else
        return false;
    }
  while (timer.milliseconds() < waitTime);
  return true;
}
  
qint64 PiiSocketDevice::writeWaited(const char* data, qint64 maxSize, int waitTime, PiiProgressController* controller)
{
  if (d->pDevice == 0) return -1;
  
  qint64 iBytesWritten = 0;
  do
    {       
      qint64 iBytesInPiece = d->pDevice->write(data, maxSize);
      if (iBytesInPiece > 0)
        {
          data += iBytesInPiece;
          iBytesWritten += iBytesInPiece;
          maxSize -= iBytesInPiece;
        }
      else if (iBytesInPiece < 0)
        return iBytesWritten != 0 ? iBytesWritten : -1;
      else if (!waitForDataWritten(waitTime, controller))
        return iBytesWritten;
    }
  while (maxSize > 0);
  
  return iBytesWritten;
}

bool PiiSocketDevice::waitForConnected(int waitTime)
{
  if (d->pDevice == 0) return false;

  switch (d->type)
    {
    case AbstractSocket:
      {
        QAbstractSocket* pSocket = static_cast<QAbstractSocket*>(d->pDevice);
        return pSocket->state() == QAbstractSocket::ConnectedState ||
          pSocket->waitForConnected(waitTime);
      }
    case LocalSocket:
      {
        QLocalSocket* pSocket = static_cast<QLocalSocket*>(d->pDevice);
        return pSocket->state() == QLocalSocket::ConnectedState ||
          pSocket->waitForConnected(waitTime);
      }
    case IODevice:
      break;
    }
  return true;
}

void PiiSocketDevice::disconnect()
{
  switch (d->type)
    {
    case AbstractSocket:
      static_cast<QAbstractSocket*>(d->pDevice)->disconnectFromHost();
      break;
    case LocalSocket:
      static_cast<QLocalSocket*>(d->pDevice)->disconnectFromServer();
      break;
    case IODevice:
      break;
    }
}

bool PiiSocketDevice::waitForDisconnected(int waitTime)
{
  switch (d->type)
    {
    case AbstractSocket:
      {
        QAbstractSocket* pSocket = static_cast<QAbstractSocket*>(d->pDevice);
        return pSocket->state() == QAbstractSocket::UnconnectedState ||
          pSocket->waitForDisconnected(waitTime);
      }
    case LocalSocket:
      {
        QLocalSocket* pSocket = static_cast<QLocalSocket*>(d->pDevice);
        return pSocket->state() == QLocalSocket::UnconnectedState ||
          pSocket->waitForDisconnected(waitTime);
      }
    case IODevice:
      break;
    }
  return true;
}

QIODevice* PiiSocketDevice::device() const
{
  return d->pDevice;
}

PiiSocketDevice::operator QIODevice* () const
{
  return d->pDevice;
}
  
QIODevice* PiiSocketDevice::operator-> () const
{
  return d->pDevice;
}
