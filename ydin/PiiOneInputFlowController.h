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

#ifndef _PIIONEINPUTFLOWCONTROLLER_H
#define _PIIONEINPUTFLOWCONTROLLER_H

#include "PiiFlowController.h"
#include <QVector>
#include <QList>

/**
 * A flow controller that does not handle synchronization between
 * sibling sockets. PiiOneInputFlowController can be used with
 * operations that have only one connected input.
 *
 */
class PII_YDIN_EXPORT PiiOneInputFlowController : public PiiFlowController
{
public:
  /**
   * Create a new flow controller.
   *
   * @param inputs the single synchronized input to be controlled.
   *
   * @param outputs the a list of outputs to be controlled. The class
   * will only consider outputs whose [PiiSocket::setGroupId()]
   * "group ID" matches that of the input.
   */
  PiiOneInputFlowController(PiiInputSocket* input,
                            const QList<PiiOutputSocket*>& outputs);
  
  PiiFlowController::FlowState prepareProcess();

protected:
  /// @internal
  class Data : public PiiFlowController::Data
  {
  public:
    Data(PiiInputSocket* input, const QList<PiiOutputSocket*>& outputs);

    PiiInputSocket* pInput;
    QVector<PiiOutputSocket*> vecOutputs;
    int iOutputCount; //optimization
  };
  PII_D_FUNC;
};

#endif //_PIIONEINPUTFLOWCONTROLLER_H
