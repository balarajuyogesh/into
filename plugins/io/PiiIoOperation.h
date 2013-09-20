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

#ifndef _PIIIOOPERATION_H
#define _PIIIOOPERATION_H

#include <PiiDefaultOperation.h>

#include "PiiIoChannel.h"
#include "PiiIoDriver.h"

/**
 * Common superclass for operations that handle digital inputs and
 * outputs.
 */
class PiiIoOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The name of the I/O driver to use.
   */
  Q_PROPERTY(QString ioDriver READ ioDriver WRITE setIoDriver);

  /**
   * An identifier for the I/O unit to use. The selected driver may be
   * able to access many hardware units. This property is used as an
   * identifier for the selected unit, and its format depends on the
   * driver.
   */
  Q_PROPERTY(QString ioUnit READ ioUnit WRITE setIoUnit);

  /**
   * A flag that can be used to stop receiving/sending I/O signals. 
   * Once this value is set to `false`, the operation will neither
   * react to changes in input signals nor activate any output
   * channels.
   */
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled);

  /**
   * The configuration maps for all input/output channels. Each entry
   * in this list is a QVariantMap that contains channels-specific
   * configuration. Each map must specify at least "channel", which
   * tells the I/O driver the index of the physical I/O channel in
   * question. Other propertis vary depending on channel type
   * (input/output) and the I/O driver.
   *
   * ~~~(c++)
   * // Create a new output signal operation
   * PiiOperation* pOutput = engine.createOperation("PiiIoOutputOperation");
   *
   * // To configure output channels(14,15,16) at once, do this:
   * QVariantMap props1;
   * props["channel"] = 14;
   * props["pulseWidth"] = 300;
   * props["pulseDelay"] = 0;
   * props["channelMode"] = "Output";
   * props["electricalConnection"] = "IttlConnection";
   *
   * QVariantMap props2;
   * props["channel"] = 15;
   * props["pulseWidth"] = 300;
   * props["pulseDelay"] = 0;
   * props["channelMode"] = "Output";
   * props["electricalConnection"] = "IttlConnection";
   *
   * QVariantMap props3;
   * props["channel"] = 16;
   * props["pulseWidth"] = 300;
   * props["pulseDelay"] = 0;
   * props["channelMode"] = "Output";
   * props["electricalConnection"] = "IttlConnection";
   *
   * pOutput->setProperty("channelConfigs", QVariantList() << props1 << props2 << props3);
   * ~~~   
   */
  Q_PROPERTY(QVariantList channelConfigs READ channelConfigs WRITE setChannelConfigs);

  /**
   * The number of I/O channels available.
   */
  Q_PROPERTY(int channelCount READ channelCount);

  /**
   * The current states of the channels.
   */
  Q_PROPERTY(QVariantList channelStates READ channelStates);
public:
  PiiIoOperation();
  ~PiiIoOperation();

  void check(bool reset);

protected:
  virtual void setChannelConfigs(const QVariantList& channelConfigs);
  QVariantList channelConfigs() const;

  int channelCount() const;
  QVariantList channelStates() const;
  
  void setIoDriver(const QString& ioDriver);
  QString ioDriver() const;

  void setIoUnit(const QString& ioUnit);
  QString ioUnit() const;

  void setEnabled(bool enabled);
  bool enabled() const;

private slots:
  void emitConnectionFailure();

protected:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    QVariantList lstChannelConfigs;
    QList<PiiIoChannel*> lstChannels;
    PiiIoDriver *pIoDriver;
    QString strIoDriver;
    QString strIoUnit;
    bool bEnabled;
    QMutex channelMutex;
  };
  PII_D_FUNC;

  /// @internal
  PiiIoOperation(Data*);
};

#endif //_PIIIOOPERATION_H
