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

#ifndef _PIIIOINPUTOPERATION_H
#define _PIIIOINPUTOPERATION_H

#include "PiiIoOperation.h"

/**
 * An operation that emits an object each time a digital input channel
 * is activated.
 *
 * Outputs
 * -------
 *
 * @out channel - the index of an activated input channel. This output
 * will emit an `int` each time an input channel becomes active.
 *
 * @out outputX - `true`, when digital input X is activated.
 *
 */
class PiiIoInputOperation : public PiiIoOperation
{
  Q_OBJECT


  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiIoInputOperation();

  void check(bool reset);

protected:
  void process();

  void setChannelConfigs(const QVariantList& channelConfigs);

private slots:
  void sendSignal(bool state);

private:
  /// @internal
  class Data : public PiiIoOperation::Data
  {
  public:
    Data();
    PiiOutputSocket* pChannelOutput;
  };
  PII_D_FUNC;
};


#endif //_PIIIOINPUTOPERATION_H
