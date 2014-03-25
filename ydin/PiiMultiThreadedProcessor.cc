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

#include "PiiMultiThreadedProcessor.h"

#include <PiiTimer.h>

class PiiMultiProcessorThread : public QThread
{
public:
  PiiMultiProcessorThread(PiiMultiThreadedProcessor* processor) :
    _pProcessor(processor),
    _threadId(0),
    _iGroupId(0),
    _bFreeRun(false)
  {
  }

  void process(int groupId)
  {
    _iGroupId = groupId;
    _iProcessCounter.ref();
    _processCondition.wakeOne();
  }

  void stop()
  {
    _bFreeRun = false;
    _processCondition.wakeOne();
  }

  void start(Priority priority, bool freeRun = false)
  {
    QMutexLocker lock(&_startMutex);
    _bFreeRun = freeRun;
    _iProcessCounter = 1;
    QThread::start(priority);
    _threadStartedCondition.wait(&_startMutex);
  }

  Qt::HANDLE id() const { return _threadId; }

  int group() const { return _iGroupId; }

protected:
  void run()
  {
    synchronized (_startMutex)
      {
        _threadId = QThread::currentThreadId();
        _threadStartedCondition.wakeOne();
      }
    QMutex* pThreadMutex = &_pProcessor->_threadMutex;
    try
      {
        if (_bFreeRun)
          {
            while (_bFreeRun)
              {
                synchronized (pThreadMutex) _pProcessor->startEmit(_threadId);
                _pProcessor->process(); // may throw
                _pProcessor->endEmit(_threadId); // may throw
              }
          }
        else
          {
            PiiTimer tmr;
            long iWaitTime = 0, iProcessTime = 0, iEndEmitTime = 0, iUnassignTime = 0;
            forever
              {
                _processCondition.wait();
                iWaitTime += tmr.restart();

                // Each wake consumes one process round. Stop()
                // doesn't increase the counter, which therefore
                // reaches zero when the thread needs to exit.
                if (!_iProcessCounter.deref())
                  break;

                // No startEmit() here; emission turns are assigned in tryToReceive()
                _pProcessor->process(); // may throw
                iProcessTime += tmr.restart();
                _pProcessor->endEmit(_threadId); // may throw
                iEndEmitTime += tmr.restart();
                synchronized (pThreadMutex)
                  {
                    _pProcessor->unassignInputs(_iGroupId, _threadId);
                    _pProcessor->processFinished(this);
                  }
                iUnassignTime += tmr.restart();
              }
            piiDebug("Thread %p times: wait %ld, process %ld, endEmit %ld, unassign %ld, total %ld",
                     (void*)_threadId, iWaitTime, iProcessTime, iEndEmitTime, iUnassignTime,
                     iWaitTime + iProcessTime + iEndEmitTime + iUnassignTime);
          }
        synchronized (pThreadMutex) _pProcessor->threadFinished(this);
      }
    catch (PiiExecutionException& ex)
      {
        synchronized (pThreadMutex) _pProcessor->threadFinished(this, ex);
      }
  }

private:
  PiiMultiThreadedProcessor* _pProcessor;
  QMutex _startMutex;
  QWaitCondition _threadStartedCondition;
  PiiWaitCondition _processCondition;
  Qt::HANDLE _threadId;
  int _iGroupId;
  bool _bFreeRun;
  QAtomicInt _iProcessCounter;
};

PiiMultiThreadedProcessor::PiiMultiThreadedProcessor(PiiDefaultOperation* parent) :
  PiiOperationProcessor(parent),
  _bReset(false), _bBlocked(false),
  _pStateMutex(&(parent->_d()->stateMutex)),
  _priority(QThread::InheritPriority),
  _bSingleInputGroup(false),
  _iSingleInputGroupId(0)
{
}

PiiMultiThreadedProcessor::~PiiMultiThreadedProcessor()
{
  QMutexLocker lock(&_threadMutex);
  destroyAllThreads();
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::startEmit(Qt::HANDLE threadId)
{
  for (int i=0; i<_lstConnectedOutputs.size(); ++i)
    _lstConnectedOutputs.at(i)->startEmit(threadId);
}

// _threadMutex MUST NOT be held when calling this function because
// startEmit() needs to wait until objects from the previous round
// have been flushed.
void PiiMultiThreadedProcessor::endEmit(Qt::HANDLE threadId)
{
  const int iCnt = _lstConnectedOutputs.size();
  QVarLengthArray<bool> abOutputCompleted(iCnt);

  bool bAllCompleted = true;
  // First try -> initialize completion flags
  for (int i=0; i<iCnt; ++i)
    bAllCompleted &= abOutputCompleted[i] =
      _lstConnectedOutputs.at(i)->tryEndEmit(threadId); // may throw

  // Retry emission to the outputs that haven't finished yet.
  while (!bAllCompleted && _bReset)
    {
      _freeInputCondition.wait();
      bAllCompleted = true;

      for (int i=0; i<iCnt; ++i)
        if (!abOutputCompleted[i])
          bAllCompleted &= abOutputCompleted[i] =
            _lstConnectedOutputs.at(i)->tryEndEmit(threadId); // may throw
    }
}

void PiiMultiThreadedProcessor::inputReady(PiiAbstractInputSocket*)
{
  _freeInputCondition.wakeOne();
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::assignInputs(int groupId, Qt::HANDLE threadId)
{
  for (int i=0; i<_lstConnectedInputs.size(); ++i)
    {
      PiiInputSocket* pInput = _lstConnectedInputs.at(i);
      if (pInput->groupId() == groupId)
        pInput->assignFirstObject(threadId);
    }
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::unassignInputs(int groupId, Qt::HANDLE threadId)
{
  for (int i=0; i<_lstConnectedInputs.size(); ++i)
    {
      PiiInputSocket* pInput = _lstConnectedInputs.at(i);
      if (pInput->groupId() == groupId)
        pInput->unassignFirstObject(threadId);
    }
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::threadFinished(PiiMultiProcessorThread* thread)
{
  _lstFinishedThreads << thread;
  _lstAllThreads.removeOne(thread);
  _lstFreeThreads.removeOne(thread);
  _freeThreadCondition.wakeOne();
  // If all threads are done, we may need to change state
  if (_lstAllThreads.isEmpty())
    {
      startEmit(thread->id());
      if (_pParentOp->state() == PiiOperation::Pausing)
        {
          try { _pParentOp->operationPaused(); } catch (...) {}
          synchronized (_pStateMutex) _pParentOp->setState(PiiOperation::Paused);
        }
      else
        {
          try { _pParentOp->operationStopped(); } catch (...) {}
          synchronized (_pStateMutex) _pParentOp->setState(PiiOperation::Stopped);
        }
      endEmit(thread->id());
    }
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::processFinished(PiiMultiProcessorThread* thread)
{
  _lstFreeThreads << thread;
  _freeThreadCondition.wakeOne();
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::threadFinished(PiiMultiProcessorThread* thread, const PiiExecutionException& ex)
{
  if (ex.code() != PiiExecutionException::Finished)
    {
      synchronized (_pStateMutex) _pParentOp->setState(PiiOperation::Interrupted);
      piiDebug("Thread %p caused an error.", (void*)thread->id());
      emit _pParentOp->errorOccured(_pParentOp, ex.message());
    }
  threadFinished(thread);
}

// _threadMutex must be held when calling this function
PiiMultiProcessorThread* PiiMultiThreadedProcessor::reserveThread()
{
  if (!_bReset)
    return 0;

  // Add new threads until the pool is full
  if (_lstAllThreads.size() < _pParentOp->threadCount())
    {
      PiiMultiProcessorThread* pThread = new PiiMultiProcessorThread(this);
      _lstAllThreads << pThread;
      // If there is no flow controller, let the thread run freely
      pThread->start(_priority, _pFlowController == 0);
      return pThread;
    }
  else if (!_lstFreeThreads.isEmpty())
    return _lstFreeThreads.takeFirst();

  _freeThreadCondition.wait(&_threadMutex);
  // This may happen if a thread fails
  if (_lstFreeThreads.size() == 0)
    return 0;
  return _lstFreeThreads.takeFirst();
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::stopAllThreads()
{
  for (ThreadList::const_iterator i=_lstAllThreads.begin(); i!=_lstAllThreads.end(); ++i)
    (*i)->stop();
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::startAllThreads()
{
  // Start all threads
  for (int i=_pParentOp->threadCount(); i--; )
    {
      PiiMultiProcessorThread* pThread = reserveThread();
      if (pThread == 0)
        return;
    }
}


void PiiMultiThreadedProcessor::destroyAllThreads()
{
  stopAllThreads();
  // This waits until every thread has signaled exit.
  waitAllThreadsToExit();
}

// _threadMutex must be held when calling this function
bool PiiMultiThreadedProcessor::waitAllThreadsToExit(unsigned long time)
{
  PiiTimer timer;
  while (_lstAllThreads.size() > 0)
    {
      _freeThreadCondition.wait(&_threadMutex, qMin((unsigned long)100, time));
      if ((unsigned long)timer.milliseconds() > time)
        return false;
    }

  // All threads should now be in _lstFinishedThreads.
  Q_ASSERT(_lstFreeThreads.isEmpty());

  // This waits until the threads are really done.
  for (ThreadList::const_iterator i=_lstFinishedThreads.begin(); i!=_lstFinishedThreads.end(); ++i)
    {
      (*i)->wait();
      delete *i;
    }
  _lstFinishedThreads.clear();

  return true;
}

// Waits all threads to finish process().
// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::waitAllThreadsToStop()
{
  while (_lstFreeThreads.size() < _lstAllThreads.size())
    _freeThreadCondition.wait(&_threadMutex);
}

// _threadMutex must be held when calling this function
void PiiMultiThreadedProcessor::finish(Qt::HANDLE callingThreadId, PiiOperation::State finalState)
{
  if (_lstAllThreads.size() > 0)
    {
      synchronized (_pStateMutex)
        _pParentOp->setState(finalState == PiiOperation::Stopped ?
                             PiiOperation::Stopping :
                             PiiOperation::Pausing);
      destroyAllThreads();
    }
  else
    {
      startEmit(callingThreadId);
      try
        {
          if (finalState == PiiOperation::Paused)
            _pParentOp->operationPaused(); // throws
          else
            _pParentOp->operationStopped(); // throws
        }
      catch (...)
        {}
      synchronized (_pStateMutex) _pParentOp->setState(finalState);
      endEmit(callingThreadId);
    }
}

/* Flow control (prepareProcess) is ran in the calling thread. If it
 * creates sync events, all active threads are waited before sync
 * events are sent.
 *
 * Whenever a processing round needs to be started, input objects are
 * first assigned to a thread allocated from the thread pool. The
 * thread is also given an output turn in each output socket. Once the
 * process() call finishes, the thread-specific input objects are
 * released and the output turn ended.
 */

static long iIdleTime = 0, iSyncTime = 0, iReserveThreadTime = 0, iAssignInputsTime = 0, iStartEmitTime = 0, iStartProcessTime = 0;

bool PiiMultiThreadedProcessor::tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ()
{
  static PiiTimer tmr;
  iIdleTime += tmr.restart();
  synchronized (_pStateMutex)
    {
      // If the processor has not been initialized for execution, we
      // discard of the object right away.
      if (!_bReset)
        return true;

      else if (_pParentOp->_d()->state member_of (PiiOperation::Stopped, PiiOperation::Paused))
        _pParentOp->setState(PiiOperation::Running);
    }

  try
    {
      QMutexLocker lock(&_threadMutex);

      PiiInputSocket* pInput = static_cast<PiiInputSocket*>(sender);
      if (!pInput->canReceive())
        return false;

      pInput->receive(object);

      /*PiiInputSocket* pInput = static_cast<PiiInputSocket*>(sender);
      qDebug("%s: %d objects in queue",
             qPrintable(pInput->objectName()), pInput->queueLength());
      */
      if (_bBlocked)
        return true;

      Qt::HANDLE callingThreadId = QThread::currentThreadId();
      do
        {
          PiiFlowController::FlowState state;

          // See if we can process the objects.
          //qDebug("%s: calling flow controller", qPrintable(_pParentOp->objectName()));
          startEmit(callingThreadId);
          state = _pFlowController->prepareProcess(); // may throw
          //qDebug("%s: flow controller returned %d", qPrintable(_pParentOp->objectName()), int(state));
          if (state == PiiFlowController::IncompleteState)
            {
              endEmit(callingThreadId); // may throw
              break;
            }

          _bBlocked = true;

          // Must ensure sync events are not sent concurrently
          // with process().
          if (_pFlowController->hasSyncEvents())
            {
              waitAllThreadsToStop();
              lock.unlock();

              _pParentOp->sendSyncEvents(_pFlowController); // may throw
            }
          else
            lock.unlock();

          endEmit(callingThreadId); // may throw

          lock.relock();
          _bBlocked = false;

          iSyncTime += tmr.restart();
          switch (state)
            {
            case PiiFlowController::ProcessableState:
              {
                PiiMultiProcessorThread* pThread = reserveThread();
                iReserveThreadTime += tmr.restart();
                if (pThread == 0)
                  PII_THROW(PiiExecutionException, _pParentOp->tr("Could not reserve a thread."));
                int iGroup = _pFlowController->activeInputGroup();
                assignInputs(iGroup, pThread->id());
                iAssignInputsTime += tmr.restart();
                startEmit(pThread->id());
                iStartEmitTime += tmr.restart();
                pThread->process(iGroup); // starts processing in another thread
                iStartProcessTime += tmr.restart();
              }
            case PiiFlowController::SynchronizedState:
            case PiiFlowController::IncompleteState:
              break;
            case PiiFlowController::PausedState:
              finish(callingThreadId, PiiOperation::Paused);
              return true;
            case PiiFlowController::FinishedState:
              finish(callingThreadId, PiiOperation::Stopped);
              return true;
            case PiiFlowController::ResumedState:
              startEmit(callingThreadId);
              _pParentOp->operationResumed(); // may throw
              endEmit(callingThreadId); // may throw
              break;
            case PiiFlowController::ReconfigurableState:
              waitAllThreadsToStop();
              startEmit(callingThreadId);
              _pParentOp->applyPropertySet(_pFlowController->propertySetName()); // may throw
              endEmit(callingThreadId); // may throw
              break;
            }
        }
      while (_bReset);
    }
  catch (PiiExecutionException& ex)
    {
      //qDebug("%s caught %s.", _pParentOp->metaObject()->className(),
      //       PiiExecutionException::errorName(ex.code()));
      // Exceptions won't be thrown from the critical section
      // above. Therefore, the mutex is not held if we are here. Must relock.
      QMutexLocker lock(&_threadMutex);

      _bBlocked = false;

      // Only errors are handled here. Stopping/pausing is handled
      // in threadFinished().
      emit _pParentOp->errorOccured(_pParentOp, ex.message());

      synchronized (_pStateMutex)
        {
          _bReset = false;
          _pParentOp->setState(PiiOperation::Interrupted);
        }
      destroyAllThreads();
      synchronized (_pStateMutex) _pParentOp->setState(PiiOperation::Stopped);
    }

  return true;
}


void PiiMultiThreadedProcessor::check(bool reset)
{
  _bBlocked = false;
  if (reset)
    _bReset = true;

  // Optimization for activeInputGroup(). Check all connected inputs
  // to see if there are more than one connected input group.
  _bSingleInputGroup = true;
  _iSingleInputGroupId = 0;
  int iGroupCount = 0;
  _lstConnectedInputs.clear();
  for (int i=0; i<_pParentOp->inputCount(); ++i)
    {
      PiiInputSocket* pInput = _pParentOp->inputAt(i);
      if (!pInput->isConnected()) continue;

      if (iGroupCount == 0)
        {
          _iSingleInputGroupId = pInput->groupId();
          ++iGroupCount;
        }
      else if (_iSingleInputGroupId != pInput->groupId())
        {
          _bSingleInputGroup = false;
          break;
        }
      _lstConnectedInputs << pInput;
    }

  _lstConnectedOutputs.clear();
  for (int i=0; i<_pParentOp->outputCount(); ++i)
    {
      PiiOutputSocket* pOutput = _pParentOp->outputAt(i);
      if (pOutput->isConnected())
        {
          _lstConnectedOutputs << pOutput;
          // Make this the listener for all connected inputs.
          pOutput->setInputListener(this);
        }
    }

  qDeleteAll(_lstFinishedThreads);
  _lstFinishedThreads.clear();
}

void PiiMultiThreadedProcessor::start()
{
  QMutexLocker lock(_pStateMutex);

  if (_pParentOp->state() not_member_of (PiiOperation::Stopped, PiiOperation::Paused))
    return;

  // There are connected inputs
  if (_pFlowController != 0)
    {
      // Paused changes to running when resume tags are received.
      if (_pParentOp->state() != PiiOperation::Paused)
        _pParentOp->setState(PiiOperation::Running);
    }
  // No connected inputs
  else
    {
      // If an operation resumes from pause and has no connected
      // inputs, it must send a resume tag to all outputs now.
      if (_pParentOp->state() == PiiOperation::Paused)
        {
          try { _pParentOp->operationResumed(); } catch (...) {}
        }

      _pParentOp->setState(PiiOperation::Running);

      synchronized (_threadMutex) startAllThreads();
    }
}

void PiiMultiThreadedProcessor::interrupt()
{
  QMutexLocker lock(_pStateMutex);
  if (_pParentOp->state() == PiiOperation::Stopped)
    return;

  _bReset = false;
  _freeInputCondition.wakeOne();

  synchronized (_threadMutex)
    {
      // If there are threads running, kill them immediately.
      if (_lstAllThreads.size() > 0)
        {
          _pParentOp->setState(PiiOperation::Interrupted);
          // This may leave some dead threads in _lstFinishedThreads.
          stopAllThreads();
        }
      // No threads running -> stop directly
      else
        _pParentOp->setState(PiiOperation::Stopped);
    }
}

void PiiMultiThreadedProcessor::pause()
{
  QMutexLocker lock(_pStateMutex);
  if (_pParentOp->state() != PiiOperation::Running)
    return;

  _pParentOp->setState(PiiOperation::Pausing);
  // If there are no connected inputs, must stop the threads
  // immediately. Otherwise tryToReceive() will change the state when
  // it receives the stop tags.
  if (_pFlowController == 0)
    // This may leave some dead threads in _lstFinishedThreads.
    synchronized (_threadMutex) stopAllThreads();
}

void PiiMultiThreadedProcessor::reconfigure(const QString& propertySetName)
{
  try
    {
      // At least one connected input -> reconfigure only after
      // receiving tags.
      if (_pFlowController != 0)
        return;

      synchronized (_threadMutex) destroyAllThreads();
      // Set properties and send tags.
      _pParentOp->applyPropertySet(propertySetName); // may throw
      synchronized (_threadMutex) startAllThreads();
    }
  catch (PiiExecutionException& ex)
    {
      emit _pParentOp->errorOccured(_pParentOp,
                                    QCoreApplication::translate("PiiDefaultOperation",
                                                                "Reconfiguring %1 failed. %2")
                                    .arg(_pParentOp->metaObject()->className()).arg(ex.message()));
    }
}


void PiiMultiThreadedProcessor::stop()
{
  piiDebug("Spawner times: idle %ld, sync %ld, reserve %ld, assign %ld, startEmit %ld, startProcess %ld, total %ld",
           iIdleTime, iSyncTime, iReserveThreadTime, iAssignInputsTime, iStartEmitTime, iStartProcessTime,
           iIdleTime + iSyncTime + iReserveThreadTime + iAssignInputsTime + iStartEmitTime + iStartProcessTime);
  QMutexLocker lock(_pStateMutex);
  if (_pParentOp->state() != PiiOperation::Running)
    return;

  _pParentOp->setState(PiiOperation::Stopping);
  if (_pFlowController == 0)
    synchronized (_threadMutex) stopAllThreads();
}

bool PiiMultiThreadedProcessor::wait(unsigned long time)
{
  QMutexLocker lock(&_threadMutex);
  return waitAllThreadsToExit(time);
}

void PiiMultiThreadedProcessor::setProcessingPriority(QThread::Priority priority)
{
  _priority = priority;
}

QThread::Priority PiiMultiThreadedProcessor::processingPriority() const
{
  return _priority;
}

int PiiMultiThreadedProcessor::activeInputGroup() const
{
  // If there is only one input group, return its ID.
  if (_bSingleInputGroup)
    return _iSingleInputGroupId;

  // Otherwise scan all threads and see which one is calling us.
  Qt::HANDLE currentThreadId = QThread::currentThreadId();
  synchronized (_threadMutex)
    {
      for (ThreadList::const_iterator i=_lstAllThreads.begin(); i!=_lstAllThreads.end(); ++i)
        if ((*i)->id() == currentThreadId)
          return (*i)->group();
    }
  return 0;
}
