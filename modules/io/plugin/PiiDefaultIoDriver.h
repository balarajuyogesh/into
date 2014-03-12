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

#ifndef _PIIDEFAULTIODRIVER_H
#define _PIIDEFAULTIODRIVER_H

#include "PiiIoDriver.h"
#include "PiiIoThread.h"
#include "PiiIoDriverException.h"
#include <QVector>

class PiiDefaultIoChannel;

/**
 * The default implementation of the PiiIoDriver-interface for input/output drivers.
 *
 */
class PII_IO_EXPORT PiiDefaultIoDriver : public PiiIoDriver
{
  Q_OBJECT
public:
  ~PiiDefaultIoDriver();

  /**
   * Get a pointer to a PiiIoChannel object that stores the
   * configuration of the given channel.
   *
   * @param channel the channel number.
   */
  PiiIoChannel* channel(int channel);

  /**
   * Get the number of I/O channels.
   */
  int channelCount() const;

protected:
  /**
   * Create a PiiIoChannel depends on given channel-index.
   */
  virtual PiiIoChannel* createChannel(int channel) = 0;

  class Data
  {
  public:
    Data();
    virtual ~Data();

    QVector<PiiIoChannel*> lstChannels;
  } *d;

  PiiDefaultIoDriver();
  PiiDefaultIoDriver(Data* data);

private slots:
  void reconnect(PiiIoChannel* channel, const QString& message);

private:
  friend class PiiDefaultIoChannel;

  void init();

  /**
   * Sends a signal to an output channel.
   *
   * @param channel - the pointer to the output-channel
   * @param value - true = on, false = off
   * @param time - msecs from 1970-01-01T00:00:00.000
   * @param pulseWidth - pulse width in msecs.
   */
  void sendSignal(PiiIoChannel *channel, bool value, qint64 time, int pulseWidth);

  /**
   * Add the input channel in the polling input list.
   */
  void addPollingInput(PiiIoChannel *input);

  /**
   * Remove the input channel from the polling input list.
   */
  void removePollingInput(PiiIoChannel *input);

  static int _iInstanceCounter;
  static PiiIoThread *_pSendingThread;
  static QMutex* instanceLock();
};

#endif //_PIIDEFAULTIODRIVER_H
