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

#include "PiiIoThread.h"
#include <PiiDelay.h>
#include <QDateTime>
#include <PiiIoDriverException.h>

PiiIoThread::PiiIoThread(QObject *parent) : QThread(parent), _bRunning(true)
{
}

void PiiIoThread::run()
{
  setPriority(TimeCriticalPriority);
  while (_bRunning)
    {
      _mutex.lock();
      
      for (int i=_lstPollingInputs.size(); i--; )
        {
          try
            {
              _lstPollingInputs[i]->checkInputState();
            }
          catch (PiiException &ex)
            {
            }
        }
      
      //HANDLE OUTPUTS ->
      qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
      int size = _lstWaitingOutputSignals.size();

      //handle all current output structs
      for (int i=0; i<size; i++)
        {
          OutputSignal stru = _lstWaitingOutputSignals[i];
          if (currentTime > stru.time)
            {
              try
                {
                  stru.channel->setOutputState(stru.active);
                }
              catch (PiiException &ex)
                {
                }
              _lstWaitingOutputSignals[i].handled = true;
            }
        }

      //remove all handled output structs
      for (int i=_lstWaitingOutputSignals.size(); i--; )
        if (_lstWaitingOutputSignals[i].handled)
          _lstWaitingOutputSignals.removeAt(i);
      
      _mutex.unlock();
      
      PiiDelay::msleep(10);
    }
}

void PiiIoThread::removeOutputList(const QVector<PiiIoChannel*>& lstChannels)
{
  _mutex.lock();

  // First handle all waiting output signals depends on lstChannels
  for (int i=0; i<_lstWaitingOutputSignals.size(); i++)
    {
      OutputSignal& stru = _lstWaitingOutputSignals[i];
      if (lstChannels.contains(stru.channel))
        {
          try
            {
              stru.channel->setOutputState(stru.active);
            }
          catch (PiiException &ex)
            {
            }
          stru.handled = true;
        }
    }

  // Second remove all handled output structs
  for (int i=_lstWaitingOutputSignals.size(); i--; )
    if (_lstWaitingOutputSignals[i].handled)
      _lstWaitingOutputSignals.removeAt(i);

  _mutex.unlock();
  
}

void PiiIoThread::addNewStruct(PiiIoChannel *channel, bool active, qint64 time, int width)
{
  OutputSignal stru;
  stru.handled = false;
  stru.channel = channel;
  stru.active = active;
  stru.time = time;
  stru.pulseWidth = width;
  _lstWaitingOutputSignals << stru;
}

void PiiIoThread::addPollingInput(PiiIoChannel *input)
{
  _mutex.lock();
  if (!_lstPollingInputs.contains(input))
    _lstPollingInputs << input;
  _mutex.unlock();
}

void PiiIoThread::removePollingInput(PiiIoChannel *input)
{
  _mutex.lock();
  _lstPollingInputs.removeAll(input);
  _mutex.unlock();
}


void PiiIoThread::sendSignal(PiiIoChannel *channel, bool active, qint64 time, int width)
{
  _mutex.lock();

  if (width == 0)
    addNewStruct(channel,active,time,width);
  else
    {
      /**
       * Now we have pulse output. That's why we must turn the state back
       * later.
       *
       * If there is the absolutely same signal in the queue, we can't
       * add new signal, we must reject it.
       *
       * If there is the transition between start and end time
       * of this signal, then we don't need add new struct at all. We
       * only move the current end time.
       *
       * If there is no any transitions between start and end time,
       * then we must add start and end transitions.
       */

      bool bAddNew = true;
      for (int i=0; i<_lstWaitingOutputSignals.size(); i++)
        {
          OutputSignal& stru = _lstWaitingOutputSignals[i];
          if (stru.channel == channel)
            {
              if (stru.time == time &&
                  stru.active == active)
                {
                  bAddNew = false;
                  break;
                }
              else if(stru.time >= time &&
                      stru.time <= (time + qint64(width)))
                {
                  stru.time = time + qint64(width);
                  bAddNew = false;
                  break;
                }
            }
        }

      if (bAddNew)
        {
          addNewStruct(channel, active, time, width);
          addNewStruct(channel, !active, time + qint64(width), 0);
        }
    }
  _mutex.unlock();
}
