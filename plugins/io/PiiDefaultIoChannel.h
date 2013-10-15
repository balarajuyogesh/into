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

#ifndef _PIIDEFAULTIOCHANNEL_H
#define _PIIDEFAULTIOCHANNEL_H

#include "PiiIoChannel.h"

class PiiDefaultIoDriver;

/**
 * Default implementation of the PiiIoChannel interface.
 *
 */
class PII_IO_EXPORT PiiDefaultIoChannel : public PiiIoChannel
{
  Q_OBJECT
  
  /**
   * The index of the channel in an I/O device.
   */
  Q_PROPERTY(int channelIndex READ channelIndex);

  /**
   * The name of the channel.
   */
  Q_PROPERTY(QString channelName READ channelName WRITE setChannelName);

  /**
   * The mode of an channel can assume five different values:
   *
   * - `NoMode` - the default state.
   *
   * - `Input` - notice only pulses. The inputStateChanged()
   * signal will be sent every time the channel becomes active (its
   * state becomes [activeState]). If signalFallingEdge is enabled,
   * inputStateChanged() signal will be sent also in falling edge.
   *
   * - `Output` - emit the pulse depends on pulseWidth and
   * pulseDelay. If pulseWidth == 0, then this output requires
   * acknowledgment. It means that we only take the output to the High
   * and it is user responsibility to acknowledge the output to Low.
   */
  Q_PROPERTY(ChannelMode channelMode READ channelMode WRITE setChannelMode);
  Q_ENUMS(ChannelMode);
  
  /**
   * The width of an output pulse in milliseconds. 0 means
   * that output requires acknowledgement.
   */
  Q_PROPERTY(int pulseWidth READ pulseWidth WRITE setPulseWidth);
  /**
   * Initial delay of an output pulse, in milliseconds. This property
   * is effective only in  `Output`.
   */
  Q_PROPERTY(int pulseDelay READ pulseDelay WRITE setPulseDelay);

  
  /**
   * If signalFallingEdge is enabled, we send inputStateChanged()
   * signal also with falling edge. In the default state (false) the
   * inputStateChanged() signal will be emitted only with rising edge.
   * This property is effective only in `Input` mode.
   */
  Q_PROPERTY(bool signalFallingEdge READ signalFallingEdge WRITE setSignalFallingEdge);
  
  /**
   * The state in which the channel is active. By default, `true`
   * means an active signal. In some applications it may however be
   * more logical to turn this around so that activating an output
   * channel actually turns it to zero. In such case the "active
   * state" should be set to `false`.
   */
  Q_PROPERTY(bool activeState READ activeState WRITE setActiveState);

  /**
   * The current state of the channel.
   */
  Q_PROPERTY(bool currentState READ currentState);

public:
  ~PiiDefaultIoChannel();
  
  /**
   * Channel mode.
   */
  enum ChannelMode
    {
      NoMode,
      Input,
      Output
    };


  void checkInputState();
  void activate();

  /**
   * Returns a pointer to the I/O driver that owns this channel.
   */
  PiiDefaultIoDriver* driver() const;

  void setChannelName(const QString& channelName);
  QString channelName() const;

  void setChannelMode(const ChannelMode& channelMode);
  ChannelMode channelMode() const;

  void setPulseWidth(int pulseWidth);
  int pulseWidth() const;

  void setPulseDelay(int pulseDelay);
  int pulseDelay() const;

  void setSignalFallingEdge(bool signalFallingEdge);
  bool signalFallingEdge() const;

  void setActiveState(bool activeState);
  bool activeState() const;

  int channelIndex() const;

  /**
   * Initialize channel. Default Implementation does nothing.
   */
  void initializeChannel();

signals:
  void channelFailed(PiiIoChannel* channel, const QString& message) const;

protected:
  /// @internal
  class Data
  {
  public:
    Data();
    virtual ~Data();
    
    PiiDefaultIoDriver *pDriver;
    int iChannelIndex;
    QString strChannelName;
    ChannelMode channelMode;
    int iPulseWidth;
    int iPulseDelay;
    int iPreviousInputState;
    bool bSignalFallingEdge;
    bool bActiveState;
    bool bCurrentState;
  } *d;

  PiiDefaultIoChannel(Data* data, PiiDefaultIoDriver *driver, int channelIndex);
  PiiDefaultIoChannel(PiiDefaultIoDriver *driver, int channelIndex);
  void channelError(const QString& message) const;

private:
  void setSignalEnabled(bool enabled);
};

#endif //_PIIDEFAULTIOCHANNEL_H
