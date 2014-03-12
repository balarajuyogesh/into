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

#ifndef _PIIIOTHREAD_H
#define _PIIIOTHREAD_H

#include <QMutex>
#include <QThread>
#include <QVector>
#include "PiiIoChannel.h"

class PiiIoThread : public QThread
{
  Q_OBJECT

public:
  struct OutputSignal
  {
    bool handled;
    PiiIoChannel *channel;
    bool active;
    qint64 time;
    int pulseWidth;
  };

  PiiIoThread(QObject *parent = 0);

  void run();
  void stop() { _bRunning = false; }

  void sendSignal(PiiIoChannel *channel, bool value, qint64 time, int pulseWidth);

  void addPollingInput(PiiIoChannel *input);
  void removePollingInput(PiiIoChannel *input);

  /**
   * Handle and remove all outputs from the _lstWaitingOutputSignals
   * depends on given parameter lstChannels.
   */
  void removeOutputList(const QVector<PiiIoChannel*>& lstChannels);

private:
  bool needAppend(PiiIoChannel *channel, bool active, qint64 checkTime);
  void addNewStruct(PiiIoChannel *channel, bool active, qint64 time, int width);

  bool _bRunning;
  QMutex _mutex;
  QList<OutputSignal> _lstWaitingOutputSignals;
  QList<PiiIoChannel*> _lstPollingInputs;
};

#endif //_PIIIOTHREAD_H
