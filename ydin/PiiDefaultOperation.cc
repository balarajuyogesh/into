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

#include "PiiDefaultOperation.h"
#include "PiiYdinTypes.h"
#include "PiiSimpleProcessor.h"
#include "PiiThreadedProcessor.h"
#include "PiiMultiThreadedProcessor.h"
#include "PiiDefaultFlowController.h"
#include "PiiOneInputFlowController.h"
#include "PiiOneGroupFlowController.h"

PiiDefaultOperation::Data::Data() :
  pFlowController(0), pProcessor(0),
  bChecked(false),
  processLock(PiiReadWriteLock::Recursive),
  iThreadCount(0),
  threadingCapabilities(NonThreaded | SingleThreaded)
{
}

PiiDefaultOperation::Data::~Data()
{
  delete pFlowController;
  delete pProcessor;
}

PiiDefaultOperation::PiiDefaultOperation() :
  PiiBasicOperation(new Data)
{
  init();
}

PiiDefaultOperation::PiiDefaultOperation(Data* data) :
  PiiBasicOperation(data)
{
  init();
}

void PiiDefaultOperation::init()
{
  setProtectionLevel("threadCount", WriteWhenStoppedOrPaused);
  createProcessor();
}

PiiDefaultOperation::~PiiDefaultOperation()
{
}

void PiiDefaultOperation::createProcessor()
{
  PII_D;
  delete d->pProcessor;
  switch (d->iThreadCount)
    {
    case 0:
      d->pProcessor = new PiiSimpleProcessor(this);
      break;
    case 1:
      d->pProcessor = new PiiThreadedProcessor(this);
      break;
    default:
      d->pProcessor = new PiiMultiThreadedProcessor(this);
      break;
    }
}

void PiiDefaultOperation::setThreadCount(int threadCount)
{
  PII_D;
  // No change after check()
  if (d->bChecked)
    return;

  if (isAcceptableThreadCount(threadCount))
    {
      d->iThreadCount = threadCount;
      createProcessor();
    }
}

bool PiiDefaultOperation::isAcceptableThreadCount(int threadCount) const
{
  const PII_D;
  return (threadCount == 0 && (d->threadingCapabilities & NonThreaded)) ||
    (threadCount == 1 && (d->threadingCapabilities & SingleThreaded)) ||
    (threadCount > 1 && (d->threadingCapabilities & MultiThreaded));
}

int PiiDefaultOperation::threadCount() const { return _d()->iThreadCount; }

void PiiDefaultOperation::setPriority(int priority)
{
  _d()->pProcessor->setProcessingPriority((QThread::Priority)priority);
}

int PiiDefaultOperation::priority() const { return _d()->pProcessor->processingPriority(); }

void PiiDefaultOperation::syncEvent(SyncEvent* /*event*/) {}

void PiiDefaultOperation::interrupt()
{
  interruptOutputs();

  // Interrupt processor
  _d()->pProcessor->interrupt();
}

void PiiDefaultOperation::check(bool reset)
{
  PII_D;
  PiiBasicOperation::check(reset);

  // Install input controller
  for (int i=0; i<d->lstInputs.size(); ++i)
    d->lstInputs[i]->setController(d->pProcessor);

  // Make all output sockets listeners to their connected inputs.
  for (int i=0; i<d->lstOutputs.size(); ++i)
    d->lstOutputs[i]->setInputListener();

  // Install flow controller
  delete d->pFlowController;
  d->pFlowController = createFlowController();

  if (reset)
    PiiFlowController::SyncListener::reset();

  // Store flow controller to the processor
  d->pProcessor->setFlowController(d->pFlowController);
  d->pProcessor->check(reset);
  d->bChecked = true;
}

PiiFlowController* PiiDefaultOperation::createFlowController()
{
  PII_D;
  int iConnectedInputCount = 0, iPositiveGroupIdCount = 0;
  QList<int> lstGroupIds;
  PiiInputSocket* pLastConnectedInput = 0;
  for (int i=0; i<d->lstInputs.size(); ++i)
    {
      PiiInputSocket* pInput = d->lstInputs[i];
      if (!pInput->isConnected()) continue;
      if (!lstGroupIds.contains(pInput->groupId()))
        {
          lstGroupIds << pInput->groupId();
          if (pInput->groupId() >= 0)
            ++iPositiveGroupIdCount;
        }
      ++iConnectedInputCount;
      pLastConnectedInput = pInput;
    }

  if (iConnectedInputCount == 0)
    return 0;

  // If there is only one connected input, PiiOneInputFlowController
  // is our choice.
  if (iConnectedInputCount == 1)
    return new PiiOneInputFlowController(pLastConnectedInput, d->lstOutputs);

  // Only one sync group -> PiiOneGroupFlowController.
  if (lstGroupIds.size() == 1)
    return new PiiOneGroupFlowController(d->lstInputs, d->lstOutputs);

  PiiDefaultFlowController::RelationList lstRelations;

  // If there are more than one non-negative group id, assign loose
  // relationships between them.
  if (iPositiveGroupIdCount > 1)
    {
      qSort(lstGroupIds.begin(), lstGroupIds.end());

      // Find first non-negative id
      int i=0;
      while (i < lstGroupIds.size()-1 && lstGroupIds[i] < 0) ++i;

      // Assign relationships
      while (i < lstGroupIds.size()-1)
        {
          lstRelations << PiiDefaultFlowController::Relation(lstGroupIds[i], lstGroupIds[i+1], false);
          ++i;
        }
    }

  // No better choice, damnit
  return new PiiDefaultFlowController(d->lstInputs,
                                      d->lstOutputs,
                                      lstRelations);
}

void PiiDefaultOperation::start()
{
  PII_D;
  if (!d->bChecked)
    {
      piiWarning(tr("Tried to call %1::start() without check().").arg(metaObject()->className()));
      return;
    }
  d->pProcessor->start();
  d->bChecked = false;
}

void PiiDefaultOperation::pause()
{
  _d()->pProcessor->pause();
}

void PiiDefaultOperation::reconfigure(const QString& propertySetName)
{
  PII_D;
  QMutexLocker lock(&d->stateMutex);
  if (d->state member_of (Paused, Stopped))
    // No tags this time.
      PiiOperation::applyPropertySet(propertySetName);
  else
    d->pProcessor->reconfigure(propertySetName);
}

void PiiDefaultOperation::stop()
{
  _d()->pProcessor->stop();
}

bool PiiDefaultOperation::wait(unsigned long time)
{
  return _d()->pProcessor->wait(time);
}

int PiiDefaultOperation::activeInputGroup() const
{
  return _d()->pProcessor->activeInputGroup();
}

bool PiiDefaultOperation::isChecked() const
{
  return _d()->bChecked;
}

PiiReadWriteLock* PiiDefaultOperation::processLock()
{
  return &_d()->processLock;
}

bool PiiDefaultOperation::setProperty(const char* name, const QVariant& value)
{
  PiiWriteLocker lock(&_d()->processLock);
  return PiiBasicOperation::setProperty(name, value);
}

QVariant PiiDefaultOperation::property(const char* name) const
{
  PiiReadLocker lock(&_d()->processLock);
  return PiiBasicOperation::property(name);
}

void PiiDefaultOperation::setThreadingCapabilities(ThreadingCapabilities threadingCapabilities)
{
  PII_D;

  if (d->bChecked)
    return;

  d->threadingCapabilities = threadingCapabilities;

  // Fix thread count if necessary
  if (!isAcceptableThreadCount(d->iThreadCount))
    {
      if (threadingCapabilities & NonThreaded)
        d->iThreadCount = 0;
      else if (threadingCapabilities & SingleThreaded)
        d->iThreadCount = 1;
      else
        d->iThreadCount = 2;
      createProcessor();
    }
}

PiiDefaultOperation::ThreadingCapabilities PiiDefaultOperation::threadingCapabilities() const
{
  return _d()->threadingCapabilities;
}
