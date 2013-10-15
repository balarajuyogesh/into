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

#ifndef _PIIONEGROUPFLOWCONTROLLER_H
#define _PIIONEGROUPFLOWCONTROLLER_H

#include "PiiFlowController.h"
#include <QVector>
#include <QList>

/**
 * A flow controller that synchronizes a group of sockets with the
 * same [group ID](PiiSocket::setGroupId()).
 *
 */
class PII_YDIN_EXPORT PiiOneGroupFlowController : public PiiFlowController
{
public:
  /**
   * Create a new flow controller.
   *
   * @param inputs a list of inputs to be controlled. All should be
   * synchronous and in the same synchronization group.
   *
   * @param outputs the a list of outputs to be controlled. This class
   * only considers outputs whose [PiiSocket::setGroupId()] "group
   * ID" matches that of the input.
   */
  PiiOneGroupFlowController(const QList<PiiInputSocket*>& inputs,
                            const QList<PiiOutputSocket*>& outputs);

  PiiFlowController::FlowState prepareProcess();

private:
  void shiftInputs();

  class Data : public PiiFlowController::Data
  {
  public:
    Data(const QList<PiiInputSocket*>& inputs,
         const QList<PiiOutputSocket*>& outputs);
    QVector<PiiInputSocket*> vecInputs;
    QVector<PiiOutputSocket*> vecOutputs;
  };
  
  PII_D_FUNC;
};

#endif //_PIIONEGROUPFLOWCONTROLLER_H
