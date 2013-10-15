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

#ifndef _PIIIOCHANNEL_H
#define _PIIIOCHANNEL_H
#include <QObject>
#include <PiiIoDriver.h>

/**
 * An interface for input/output channels.
 *
 */
class PII_IO_EXPORT PiiIoChannel : public QObject
{
  Q_OBJECT

public:
  /**
   * Immediately changes the state of an output channel to *state*.
   * May throw PiiIoDriverException.
   */
  virtual void setOutputState(bool state) = 0;

  /**
   * Returns the current state of the channel. This function works
   * with both input and output channels. May throw
   * PiiIoDriverException.
   */
  virtual bool currentState() const = 0;

  /**
   * Checks the channel to see if its state has been changed since
   * last invokation. If so, emits [inputStateChanged()]. May throw
   * PiiIoDriverException.
   */
  virtual void checkInputState() = 0;

  /**
   * Initialize channel. May throw PiiIoDriverException.
   */
  virtual void initializeChannel() = 0;
  

public slots:
  /**
   * Activates an output channel. The actual behavior depends on the
   * channel's configuration. For example, the channel may be
   * configured to create a fixed-length pulse when activated. May
   * throw PiiIoDriverException.
   */
  virtual void activate() = 0;

signals:
  /**
   * Emitted whenever the state of an input changes. The actual
   * behavior depends on the channel's configuration. For example, if
   * a channel is configured as PiiDefaultIoChannel::Input and
   * signalFallingEdge is false, only activations will be signaled. If
   * the signalFallingEdge is true, both activations and deactivations
   * will be signaled.
   */
  void inputStateChanged(bool state);
};

#endif //_PIIIOCHANNEL_H
