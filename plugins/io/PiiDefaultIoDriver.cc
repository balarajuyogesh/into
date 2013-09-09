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

#include "PiiDefaultIoDriver.h"
#include <PiiDefaultIoChannel.h>

int PiiDefaultIoDriver::_iInstanceCounter = 0;
PiiIoThread* PiiDefaultIoDriver::_pSendingThread = 0;

QMutex* PiiDefaultIoDriver::instanceLock()
{
  static QMutex mutex;
  return &mutex;
}

PiiDefaultIoDriver::Data::Data()
{
}

PiiDefaultIoDriver::Data::~Data()
{
  qDeleteAll(lstChannels);
}

PiiDefaultIoDriver::PiiDefaultIoDriver(Data* data) : d(data)
{
  init();
}

PiiDefaultIoDriver::PiiDefaultIoDriver() : d(new Data)
{
  init();
}

void PiiDefaultIoDriver::init()
{
  synchronized (instanceLock())
    {
      if (_iInstanceCounter == 0)
        {
          _pSendingThread = new PiiIoThread;
          _pSendingThread->start();
        }
      _iInstanceCounter++;
    }
}

PiiDefaultIoDriver::~PiiDefaultIoDriver()
{
  synchronized (instanceLock())
    {
      // handle and remove output signals from the sending thread
      _pSendingThread->removeOutputList(d->lstChannels);

      //check if we must stop and delete thread
      _iInstanceCounter--;

      if (_iInstanceCounter == 0)
        {
          if (_pSendingThread->isRunning())
            {
              _pSendingThread->stop();
              while(!_pSendingThread->isFinished());
            }
          delete _pSendingThread;
          _pSendingThread = 0;
        }
    }

  delete d;
}

void PiiDefaultIoDriver::reconnect(PiiIoChannel* channel, const QString& message)
{
  if (!d->lstChannels.contains(channel))
    return;
  piiCritical(message);
  if (!reset())
    emit connectionLost();
}

void PiiDefaultIoDriver::sendSignal(PiiIoChannel *channel, bool value, qint64 time, int pulseWidth)
{
  if (_pSendingThread != 0)
    _pSendingThread->sendSignal(channel,value,time,pulseWidth);
}

PiiIoChannel* PiiDefaultIoDriver::channel(int channel)
{
  if (channel >= 0)
    {
      if (channel >= d->lstChannels.size())
        d->lstChannels.resize(channel+1);
      
      if (d->lstChannels[channel] == 0)
        {
          PiiIoChannel *pChannel = createChannel(channel);
          d->lstChannels[channel] = pChannel;
          if (pChannel)
            connect(pChannel, SIGNAL(channelFailed(PiiIoChannel*,QString)),
                    SLOT(reconnect(PiiIoChannel*,QString)), Qt::DirectConnection);
        }
      
      return d->lstChannels[channel];
    }
  return 0;
}

int PiiDefaultIoDriver::channelCount() const
{
  return d->lstChannels.size();
}

void PiiDefaultIoDriver::addPollingInput(PiiIoChannel *input)
{
  if (_pSendingThread)
    _pSendingThread->addPollingInput(input);
}
void PiiDefaultIoDriver::removePollingInput(PiiIoChannel *input)
{
  if (_pSendingThread)
    _pSendingThread->removePollingInput(input);
}
