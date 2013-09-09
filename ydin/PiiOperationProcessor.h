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

#ifndef _PIIOPERATIONPROCESSOR_H
#define _PIIOPERATIONPROCESSOR_H

#include "PiiYdinTypes.h"
#include "PiiDefaultOperation.h"
#include "PiiInputController.h"
#include <QThread>

class PiiInputSocket;
class PiiFlowController;

/**
 * A helper class used by PiiDefaultOperation to implement threaded
 * and non-threaded processing modes. It implements most of the
 * PiiOperation interface and adds a few functions of its own. The
 * input processor uses a @ref PiiFlowController "flow controller" to
 * synchronize objects read from the input sockets.
 *
 * %PiiOperationProcessor is intimately bound to PiiDefaultOperation
 * and not of much use elsewhere. Its interface is not guaranteed to
 * remain binary compatible. You have been warned.
 *
 * @internal
 * @ingroup Ydin
 */
class PiiOperationProcessor : public PiiInputController
{
public:
  virtual ~PiiOperationProcessor();

  virtual void check(bool reset) = 0;
  virtual void start() = 0;
  virtual void interrupt() = 0;
  virtual void pause() = 0;
  virtual void stop() = 0;
  virtual void reconfigure(const QString& propertySetName) = 0;

  virtual bool wait(unsigned long time = ULONG_MAX) = 0;

  /**
   * Set the processing priority of this processor to @p priority. 
   */
  virtual void setProcessingPriority(QThread::Priority priority) = 0;

  /**
   * Get the current processing priority of this processor.
   */
  virtual QThread::Priority processingPriority() const = 0;

  /**
   * Returns the id of the input group that should be processed by the
   * calling thread.
   */
  virtual int activeInputGroup() const = 0;
  
  /**
   * Set the flow controller that prepares the operation for
   * processing. This function is called by
   * PiiDefaultOperation::check().
   */
  void setFlowController(PiiFlowController* flowController) { _pFlowController = flowController; }

  /**
   * Get the current flow controller.
   */
  PiiFlowController* flowController() const { return _pFlowController; }

protected:
  /**
   * Creates a new %PiiOperationProcessor.
   *
   * @param parent the operation to be executed
   */
  PiiOperationProcessor(PiiDefaultOperation* parent) :
    _pParentOp(parent)
  {}

  /**
   * A pointer to the parent operation.
   */
  PiiDefaultOperation* _pParentOp;
  /**
   * A pointer to the currently installed flow controller.
   */
  PiiFlowController* _pFlowController;
};

#endif //_PIIOPERATIONPROCESSOR_H
