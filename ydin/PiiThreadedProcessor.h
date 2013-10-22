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

#ifndef _PIITHREADEDPROCESSOR_H
#define _PIITHREADEDPROCESSOR_H

#include "PiiOperationProcessor.h"
#include <QThread>

class QMutex;

/**
 * An implementation of a processor that runs in a separate thread. 
 * PiiThreadedProcessor creates a new thread when its start() function
 * is called. The thread sleeps when no data is available in inputs
 * and is awakened by each incoming object.
 *
 * @internal
 */
class PiiThreadedProcessor : public QThread, public PiiOperationProcessor
{
  Q_OBJECT
  
public:
  /**
   * Construct a new PiiThreadedProcessor.
   */
  PiiThreadedProcessor(PiiDefaultOperation* parent);
  
  void check(bool reset);
  
  /**
   * Execute the processor until interruption. This method starts a
   * new thread that sleeps until new objects appear on connected
   * inputs (if any). The process() method is called whenever all
   * connected inputs are ready to be read.
   *
   * If the processor is a producer that has no connected inputs, the
   * default implementation causes it to output objects as quickly as
   * the synchronous inputs at the receiving ends allow.
   */
  void start();

  /**
   * Sets the state to `Interrupted`. The state changes to `Stopped`
   * once the processor stops running.
   */
  void interrupt();

  /**
   * Sets the state to `Pausing`.
   */
  void pause();

  /**
   * Sets the state to `Stopping`.
   */
  void stop();

  void reconfigure(const QString& propertySetName);
  
  /**
   * Waits until the thread is finished.
   */
  bool wait(unsigned long time = ULONG_MAX);

  /**
   * Invoked when a new object appears on any input socket. This
   * function just signals the runner thread that new data is
   * available.
   */
  bool tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ();

  void setProcessingPriority(QThread::Priority priority);
  QThread::Priority processingPriority() const;

  int activeInputGroup() const;

protected:
  void run();

private slots:
  void setStopped();

private:
  inline void prepareAndProcess();

  PiiWaitCondition _inputCondition;
  Priority _priority;
  QMutex *_pStateMutex;
  bool _bMustReconfigure;
  QString _strPropertySetName;
};

#endif // _PIITHREADEDPROCESSOR_H
