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

#ifndef _PIIIOOUTPUTOPERATION_H
#define _PIIIOOUTPUTOPERATION_H

#include "PiiIoOperation.h"

/**
 * An operation that controls digital output channels based on input.
 *
 * Inputs
 * ------
 *
 * @in channel - the index of the output channel to be activated
 * (int). The index is not the physical index of the output channel in
 * the I/O device, but its index in the list of configured channels. 
 * Alternatively, a PiiMatrix<int> may be used to activate many
 * channels at once. The indices of the channels to be activated are
 * represented either as a row or a column vector.
 *
 * @in value - an optional boolean value (or an int) that determines
 * whether the specified channel should be activated or not. 
 * Connecting this input makes it possible to selectively
 * enable/disable I/O signals at run time.
 * 
 */
class PiiIoOutputOperation : public PiiIoOperation
{
  Q_OBJECT

  /**
   * A list of integers that stores the `pulseWidth` configuration
   * value for each output channel.
   */
  Q_PROPERTY(QVariantList pulseWidths READ pulseWidths);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiIoOutputOperation();
  ~PiiIoOutputOperation();

public slots:
  /**
   * Sets the state of *channel* to *value*.
   */
  void setChannelState(int channel, bool value);

protected:
  void process();

  QVariantList pulseWidths() const;

private:
  void activateChannel(int channel);
  
  /// @internal
  class Data : public PiiIoOperation::Data
  {
  public:
    Data();

    PiiInputSocket *pChannelInput, *pValueInput;
  };
  PII_D_FUNC;
};

#endif //_PIIIOOUTPUTOPERATION_H
