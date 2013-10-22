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

#ifndef _PIIMULTITHREADEDPROCESSOR_H
#define _PIIMULTITHREADEDPROCESSOR_H

#include "PiiOperationProcessor.h"
#include "PiiInputListener.h"

class PiiMultiProcessorThread;

/**
 * A processor that creates a user-defined number of threads into a
 * pool and calls process() concurrently from them.
 *
 * @internal
 */
class PiiMultiThreadedProcessor :
  public PiiOperationProcessor,
  public PiiInputListener
{
public:
  PiiMultiThreadedProcessor(PiiDefaultOperation* parent);
  ~PiiMultiThreadedProcessor();

  bool tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ();
  void check(bool reset);
  void start();
  void interrupt();
  void pause();
  void stop();
  void reconfigure(const QString& propertySetName);
  bool wait(unsigned long time);
  void setProcessingPriority(QThread::Priority priority);
  QThread::Priority processingPriority() const;
  int activeInputGroup() const;

  void inputReady(PiiAbstractInputSocket* input);

private:
  friend class PiiMultiProcessorThread;

  void finish(Qt::HANDLE callingThreadId, PiiOperation::State finalState);
  
  void startEmit(Qt::HANDLE threadId);
  void endEmit(Qt::HANDLE threadId);
  void assignInputs(int groupId, Qt::HANDLE threadId);
  void unassignInputs(int groupId, Qt::HANDLE threadId);
  void threadFinished(PiiMultiProcessorThread* thread);
  void processFinished(PiiMultiProcessorThread* thread);
  void threadFinished(PiiMultiProcessorThread* thread, const PiiExecutionException& ex);
  PiiMultiProcessorThread* reserveThread();
  void startAllThreads();
  void stopAllThreads();
  void destroyAllThreads();
  bool waitAllThreadsToExit(unsigned long time = ULONG_MAX);
  void waitAllThreadsToStop();

  inline void process() { _pParentOp->processLocked(); }
  
  volatile bool _bReset;
  bool _bBlocked;
  QMutex* _pStateMutex;
  QThread::Priority _priority;
  typedef QLinkedList<PiiMultiProcessorThread*> ThreadList;
  ThreadList _lstAllThreads, _lstFinishedThreads, _lstFreeThreads;
  QWaitCondition _freeThreadCondition;
  PiiWaitCondition _freeInputCondition;
  QMutex _threadMutex;
  bool _bSingleInputGroup;
  int _iSingleInputGroupId;
  QList<PiiInputSocket*> _lstConnectedInputs;
  QList<PiiOutputSocket*> _lstConnectedOutputs;
};

#endif //_PIIMULTITHREADEDPROCESSOR_H
