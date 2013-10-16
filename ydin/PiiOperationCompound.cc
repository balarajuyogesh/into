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

#include "PiiOperationCompound.h"

#include <PiiSerializationFactory.h>
#include <PiiUtil.h>
#include <PiiDelay.h>
#include <QTime>
#include <QtDebug>
#include <PiiYdinUtil.h>
#include <PiiYdinResources.h>
#include <PiiSerializableExport.h>


PII_DEFINE_VIRTUAL_METAOBJECT_FUNCTION(PiiOperationCompound);
PII_SERIALIZABLE_EXPORT(PiiOperationCompound);

static int iOperationCompoundMetaType = qRegisterMetaType<PiiOperationCompound*>("PiiOperationCompound*");

PiiOperationCompound::Data::Data() :
  state(PiiOperation::Stopped),
  bChecked(false),
  bWaiting(false)
{}

PiiOperationCompound::Data::~Data()
{}
  
PiiOperationCompound::PiiOperationCompound() :
  PiiOperation(new Data)
{
  Q_UNUSED(iOperationCompoundMetaType); // suppresses compiler warning
}

PiiOperationCompound::PiiOperationCompound(Data* data) :
  PiiOperation(data)
{}

PiiOperationCompound::~PiiOperationCompound()
{
  removeAllSockets();
}

template <class Socket> void PiiOperationCompound::clearSocketList(QList<Socket*>& list)
{
  for (int i=0; i<list.size(); ++i)
    delete PiiProxySocket::parent(list[i]);
  list.clear();
}

void PiiOperationCompound::removeAllSockets()
{
  PII_D;
  clearSocketList(d->lstInputs);
  clearSocketList(d->lstOutputs);
}

void PiiOperationCompound::clear()
{
  PII_D;
  removeAllSockets();
  qDeleteAll(d->lstOperations);
  d->lstOperations.clear();
}

void PiiOperationCompound::check(bool reset)
{
  PII_D;

  // Check for a parent if we haven't been derived.
  if (parent() == 0 &&
      PiiOperationCompound::metaObject() == metaObject())
    piiWarning(tr("%1 (objectName %2) has no parent.")
               .arg(metaObject()->className())
               .arg(objectName()));

  PiiCompoundExecutionException PII_MAKE_EXCEPTION(compoundEx, "");
  
  d->vecChildStates.resize(d->lstOperations.size());
  bool bError = false;
  for (int i=0; i<d->lstOperations.size(); ++i)
    {
      try
        {
          d->lstOperations[i]->check(d->lstOperations[i]->state() == Stopped);
        }
      catch (PiiExecutionException& ex)
        {
          bError = true;
          compoundEx.addException(d->lstOperations[i]->objectName(), ex);
        }
      d->vecChildStates[i] = qMakePair(d->lstOperations[i]->state(), false);
    }
  if (bError)
    throw compoundEx;
  if (reset)
    {
      resetProxies(d->lstInputs);
      resetProxies(d->lstOutputs);
    }
  d->bChecked = true;
}

void PiiOperationCompound::start()
{
  PII_D;
  if (!d->bChecked)
    return;
  QMutexLocker lock(&d->stateMutex);
  if (d->state == Stopped || d->state == Paused)
    {
      if (!d->lstOperations.isEmpty())
        {
          setState(Starting);
          commandChildren(Start());
        }
      else
        setState(Running);
      d->bChecked = false;
    }
}

void PiiOperationCompound::pause()
{
  PII_D;
  QMutexLocker lock(&d->stateMutex);
  if (d->state == Running)
    {
      if (!d->lstOperations.isEmpty())
        {
          setState(Pausing);
          commandChildren(Pause());
        }
      else
        setState(Paused);
    }
}

void PiiOperationCompound::stop()
{
  PII_D;
  QMutexLocker lock(&d->stateMutex);
  if (d->state member_of (Starting, Running, Stopping))
    {
      if (!d->lstOperations.isEmpty())
        {
          setState(Stopping);
          commandChildren(Stop());
        }
      else
        setState(Stopped);
    }
}

void PiiOperationCompound::interrupt()
{
  PII_D;
  // Since interrupt() is meant to really stop everything, we send the
  // signals even when the compound indicates it is already stopped.
  QMutexLocker lock(&d->stateMutex);
  if (!d->lstOperations.isEmpty())
    {
      if (d->state != Stopped)
        setState(Interrupted);
      commandChildren(Interrupt());
    }
  else
    setState(Stopped);
}

template <class Socket>
void PiiOperationCompound::resetProxies(const QList<Socket*>& list)
{
  for (int i=0; i<list.size(); ++i)
    {
      PiiProxySocket* pProxy = PiiProxySocket::parent(list[i]);
      if (pProxy)
        pProxy->reset();
    }
}

/* Check if a child's state change to newState causes a state change
 * on the parent. The state of the parent changes through an
 * intermediate state. (e.g. Running -> Stopping -> Stopped)
 */
bool PiiOperationCompound::checkSteadyStateChange(State newState, State intermediateState, State steadyState)
{
  // If the operation's new state is {Starting,Pausing,Stopping},
  // change state respectively
  if (newState == intermediateState ||
      (steadyState == Stopped && newState == Interrupted))
    setState(newState);
  // If an operation directly changes to the target steady state
  else if (newState == steadyState)
    {
      // Go through the intermediate state
      setState(intermediateState);
      // Everybody is in the steady state, so am I
      if (checkChildStates(steadyState))
        setState(steadyState);
    }
  else
    return false;

  // State changed
  return true;
}

QString PiiOperationCompound::fullName(QObject* operation)
{
  QStringList lstNames;
  while (operation)
    {
      QString strName = operation->metaObject()->className();
      if (!operation->objectName().isEmpty() &&
          operation->objectName() != operation->metaObject()->className())
        strName += "[" + operation->objectName() + "]";
      lstNames.prepend(strName);
      operation = operation->parent();
    }

  return lstNames.join("/");
}

void PiiOperationCompound::childStateChanged(int state)
{
  PII_D;
  QMutexLocker lock(&d->stateMutex);
  //piiDebug("\n%-11s %s", stateName((State)state), qPrintable(fullName(sender())));
  //piiDebug("%s %s, compound %s", sender()->metaObject()->className(), stateName((State)state), stateName(this->state()));

  int iIndex = d->lstOperations.indexOf(static_cast<PiiOperation*>(sender()));
  d->vecChildStates[iIndex].first = (State)state;
  if (state == Running)
    d->vecChildStates[iIndex].second = true;
  
  switch (d->state)
    {
    case Stopped:
      // Stopped can change to Running
      checkSteadyStateChange((State)state, Starting, Running);
      break;
    case Starting:
      // Starting can change to running ...
      if (checkChildStates(Running))
        {
          setState(Running);
          // Now, if any of the operations already stopped, we must
          // change state.
          for (int i=d->vecChildStates.size(); i--; )
            if (d->vecChildStates[i].first member_of (Stopping, Stopped))
              {
                checkSteadyStateChange(d->vecChildStates[i].first, Stopping, Stopped);
                break;
              }
        }
      // ... or stopped
      // If an operation that was already running stops, we won't
      // change state to Stopping yet.
      else if ((State)state not_member_of (Stopping, Stopped) ||
               !d->vecChildStates[iIndex].second)
        checkSteadyStateChange((State)state, Stopping, Stopped);
      break;
    case Running:
      // Running can change to Paused or Stopped
      if (!checkSteadyStateChange((State)state, Stopping, Stopped))
        checkSteadyStateChange((State)state, Pausing, Paused);
      break;
    case Pausing:
      // Pausing can change to Paused ...
      if (checkChildStates(Paused))
        setState(Paused);
      // ... or stopped
      else
        checkSteadyStateChange((State)state, Stopping, Stopped);
      break;
    case Paused:
      // Paused can change to Stopped and Running
      if (!checkSteadyStateChange((State)state, Stopping, Stopped))
        checkSteadyStateChange((State)state, Starting, Running);
      break;
    case Stopping:
    case Interrupted:
      // Stopping/Interrupted can only change to Stopped
      if (checkChildStates(Stopped))
        setState(Stopped);
      break;
    }
}

// Returns true if all children are in the given state.
bool PiiOperationCompound::checkChildStates(State state)
{
  PII_D;
  for (int i=d->vecChildStates.size(); i--; )
    // If the current state of a child is different from what we
    // expect, fail. But if we are checking for "Running", we need to
    // check if the child has been running.
    if (d->vecChildStates[i].first != state && !(state == Running && d->vecChildStates[i].second))
      return false;
  return true;
}

bool PiiOperationCompound::wait(unsigned long time)
{
  PII_D;
  QTime t;
  t.start();
  // Wait for all children
  bool allDone;
  do
    {
      allDone = true;
      for (int i=0; i<d->lstOperations.size(); ++i)
        {
          allDone = allDone && d->lstOperations[i]->wait(100);
          QCoreApplication::processEvents();
        }
    }
  while (!allDone &&
         (time == ULONG_MAX || static_cast<unsigned long>(t.elapsed()) < time));
  return allDone;
}

bool PiiOperationCompound::wait(State state, unsigned long time)
{
  PII_D;
  if (d->bWaiting)
    {
      if (state == d->state) return true;
      piiWarning(tr("Recursive call to PiiOperationCompound::wait() blocked."));
      return false;
    }
  d->bWaiting = true;
  QTime t;
  t.start();
  while (d->state != state && static_cast<unsigned long>(t.elapsed()) <= time)
    {
      QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents |
#if QT_VERSION < 0x050000
                                      QEventLoop::DeferredDeletion |
#endif
                                      QEventLoop::ExcludeUserInputEvents,
                                      10);
      PiiDelay::msleep(10);
    }
  d->bWaiting = false;
  return d->state == state;
}

QList<PiiAbstractInputSocket*> PiiOperationCompound::inputs() const
{
  return _d()->lstInputs;
}

QList<PiiAbstractOutputSocket*> PiiOperationCompound::outputs() const
{
  return _d()->lstOutputs;
}

int PiiOperationCompound::inputCount() const
{
  return _d()->lstInputs.size();
}

int PiiOperationCompound::outputCount() const
{
  return _d()->lstOutputs.size();
}

PiiProxySocket* PiiOperationCompound::createInputProxy(const QString& name)
{
  PII_D;
  if (findSocket(name, d->lstInputs))
    return 0;

  PiiProxySocket* pProxy = new PiiProxySocket(name, '.' + name,  this);
  d->lstInputs.append(pProxy->input());
  return pProxy;
}

PiiProxySocket* PiiOperationCompound::createOutputProxy(const QString& name)
{
  PII_D;
  if (findSocket(name, d->lstOutputs))
    return 0;

  PiiProxySocket* pProxy = new PiiProxySocket('.' + name, name, this);
  d->lstOutputs.append(pProxy->output());
  return pProxy;
}

PiiProxySocket* PiiOperationCompound::outputProxy(const QString& name) const
{
  PiiAbstractOutputSocket* pOutput = findSocket(name, _d()->lstOutputs);
  return pOutput ? PiiProxySocket::parent(pOutput) : 0;
}

bool PiiOperationCompound::exposeInput(PiiAbstractInputSocket* socket)
{
  if (socket == 0 || !Pii::isParent(this, socket)) return false;
  PII_D;
  // No duplicate entries with the same name
  if (findSocket(socket->objectName(), d->lstInputs))
    return false;

  d->lstInputs.append(socket);

  // Break connection when the socket is destroyed.
  connect(socket,
          SIGNAL(destroyed(QObject*)),
          SLOT(removeExposedInput(QObject*)),
          Qt::DirectConnection);

  return true;
}

PiiProxySocket* PiiOperationCompound::inputProxy(const QString& name) const
{
  PiiAbstractInputSocket* pInput = findSocket(name, _d()->lstInputs);
  return pInput ? PiiProxySocket::parent(pInput) : 0;
}

bool PiiOperationCompound::exposeOutput(PiiAbstractOutputSocket* socket)
{
  if (socket == 0 || !Pii::isParent(this, socket)) return false;
  PII_D;
  if (findSocket(socket->objectName(), d->lstOutputs))
    return false;

  d->lstOutputs.append(socket);
  
  // Break connection when the socket is destroyed
  connect(socket,
          SIGNAL(destroyed(QObject*)),
          SLOT(removeExposedOutput(QObject*)),
          Qt::DirectConnection);
  
  return true;
}

bool PiiOperationCompound::exposeInput(const QString& name)
{
  PiiAbstractInputSocket* pInput = input(name);
  if (pInput)
    return exposeInput(pInput);

  piiWarning(tr("There is no \"%1\" input in %2.").arg(name).arg(metaObject()->className()));
  return false;
}

bool PiiOperationCompound::exposeOutput(const QString& name)
{
  PiiAbstractOutputSocket* pOutput = output(name);
  if (pOutput)
    return exposeOutput(pOutput);

  piiWarning(tr("There is no \"%1\" output in %2.").arg(name).arg(metaObject()->className()));
  return false;
}

PiiProxySocket* PiiOperationCompound::createInputProxy(const QString& name, const QStringList& intputNames)
{
  PiiProxySocket* pProxy = createInputProxy(name);
  if (!pProxy)
    return 0;
  for (int i=0; i<intputNames.size(); ++i)
    pProxy->output()->connectInput(input(intputNames[i]));
  return pProxy;
}

PiiProxySocket* PiiOperationCompound::createOutputProxy(const QString& name, const QString& outputName)
{
  PiiProxySocket* pProxy = createInputProxy(name);
  if (!pProxy)
    return 0;
  pProxy->output()->connectInput(input(outputName));
  return pProxy;
}
  
bool PiiOperationCompound::removeInput(PiiAbstractInputSocket* input)
{
  return removeSocket(input, _d()->lstInputs);
}

bool PiiOperationCompound::removeOutput(PiiAbstractOutputSocket* output)
{
  return removeSocket(output, _d()->lstOutputs);
}

template <class Socket>
bool PiiOperationCompound::removeSocket(QObject* socket, QList<Socket*>& list)
{
  for (int i=list.size(); i--; )
    {
      if (list[i] == socket)
        {
          delete PiiProxySocket::parent(list.takeAt(i));
          return true;
        }
    }
  return false;
}

bool PiiOperationCompound::removeInput(const QString& name)
{
  return removeSocket(name, _d()->lstInputs);
}

bool PiiOperationCompound::removeOutput(const QString& name)
{
  return removeSocket(name, _d()->lstOutputs);
}

template <class Socket>
bool PiiOperationCompound::removeSocket(const QString& name, QList<Socket*>& list)
{
  for (int i=list.size(); i--; )
    {
      if (list[i]->objectName() == name)
        {
          delete PiiProxySocket::parent(list.takeAt(i));
          return true;
        }
    }
  return false;
}

void PiiOperationCompound::removeExposedInput(QObject* socket)
{
  _d()->lstInputs.removeOne(static_cast<PiiAbstractInputSocket*>(socket));
}

void PiiOperationCompound::removeExposedOutput(QObject* socket)
{
  _d()->lstOutputs.removeOne(static_cast<PiiAbstractOutputSocket*>(socket));
}

QList<PiiOperation*> PiiOperationCompound::childOperations() const
{
  return _d()->lstOperations;
}

QStringList PiiOperationCompound::childNames() const
{
  QStringList lstResult;
  foreach (PiiOperation* op, _d()->lstOperations)
    lstResult << op->objectName();
  return lstResult;
}

int PiiOperationCompound::childCount() const
{
  return _d()->lstOperations.size();
}

PiiOperation* PiiOperationCompound::childAt(int index) const
{
  const PII_D;
  if (index > 0 && index < d->lstOperations.size())
    return d->lstOperations[index];
  return 0;
}

void PiiOperationCompound::addOperation(PiiOperation* op)
{
  PII_D;
  QMutexLocker lock(&d->stateMutex);
  // Operations can only be added when the operation is stopped or
  // paused
  if (op == 0 || d->state not_member_of (Stopped, Paused))
    return;

  if (!d->lstOperations.contains(op))
    {
      // Don't allow empty names
      if (op->objectName().isEmpty())
        {
          QString className(op->metaObject()->className());
          int i = 1;
          QString name = className;
          // Find a unique objectName
          while (findChildOperation(name))
            name = className + QString::number(i++);
          op->setObjectName(name);
        }

      d->lstOperations.append(op);
      op->setParent(this);
      connect(op, SIGNAL(errorOccured(PiiOperation*,const QString&)),
              SLOT(handleError(PiiOperation*,const QString&)), Qt::QueuedConnection);
      connect(op, SIGNAL(stateChanged(int)), SLOT(childStateChanged(int)), Qt::QueuedConnection);
      connect(op, SIGNAL(destroyed(QObject*)), SLOT(childDestroyed(QObject*)), Qt::DirectConnection);
    }
}


void PiiOperationCompound::childDestroyed(QObject* op)
{
  PII_D;
  PiiOperation* child = static_cast<PiiOperation*>(op);
  if (child)
    {
      d->lstOperations.removeAll(child);
      if (d->state not_member_of (Stopped, Paused))
        emit errorOccured(child ? child : this, tr("An operation was destroyed while its parent was being executed."));
    }
}

void PiiOperationCompound::handleError(PiiOperation* sender, const QString& msg)
{
  piiCritical("An error occured with %s (%s): %s",
              sender->metaObject()->className(),
              piiPrintable(sender->objectName()),
              piiPrintable(msg));
  emit errorOccured(sender, msg);
  interrupt();
}

bool PiiOperationCompound::replaceOperation(PiiOperation *oldOp, PiiOperation* newOp)
{
  PII_D;
  QMutexLocker lock(&d->stateMutex);
  // Operations can only be replaced when the operation is stopped or
  // paused
  if (d->state not_member_of (Stopped, Paused) ||
      !d->lstOperations.contains(oldOp) ||
      d->lstOperations.contains(newOp))
    return false;

  if (newOp != 0)
    {
      addOperation(newOp);
  
      //get all inputs and outputs from the old operation
      QList<PiiAbstractInputSocket*> lstOldInputs = oldOp->inputs();
      QList<PiiAbstractOutputSocket*> lstOldOutputs = oldOp->outputs();
  
      //get all inputs and outputs from the new operation
      QList<PiiAbstractInputSocket*> lstNewInputs = newOp->inputs();
      QList<PiiAbstractOutputSocket*> lstNewOutputs = newOp->outputs();

      //disconnect all inputs and outputs from the new operation
      for (int i=0; i<lstNewInputs.size(); ++i)
        lstNewInputs[i]->disconnectOutput();
      for (int i=0; i<lstNewOutputs.size(); ++i)
        lstNewOutputs[i]->disconnectInput();
  
      //connect all inputs
      for (int i=0; i<lstOldInputs.size(); ++i)
        {
          PiiAbstractOutputSocket *pOutput = lstOldInputs[i]->connectedOutput();
          // This input is not connected
          if (pOutput == 0)
            continue;
          
          // Try to find a socket with a matching name.
          PiiAbstractInputSocket* pInput = newOp->input(lstOldInputs[i]->objectName());
          if (pInput != 0)
            pOutput->connectInput(pInput);
          else if (i < lstNewInputs.size())
            pOutput->connectInput(lstNewInputs[i]);

          lstOldInputs[i]->disconnectOutput();
        }

      //connect all outputs
      for (int i=0; i<lstOldOutputs.size(); ++i)
        {
          QList<PiiAbstractInputSocket*> lstInputs = lstOldOutputs[i]->connectedInputs();
          if (lstInputs.isEmpty())
            continue;
          lstOldOutputs[i]->disconnectInput();
          
          PiiAbstractOutputSocket* pOutput = newOp->output(lstOldOutputs[i]->objectName());
          if (pOutput != 0 && i < lstNewOutputs.size())
            pOutput = lstNewOutputs[i];
          if (pOutput != 0)
            for (int j=0; j<lstInputs.size(); ++j)
              pOutput->connectInput(lstInputs[j]);
        }
    }

  //remove the old operation
  d->lstOperations.removeAll(oldOp);
  oldOp->disconnect(this);
  oldOp->setParent(0);
  
  return true;
}

void PiiOperationCompound::removeOperation(PiiOperation* op)
{
  PII_D;
  QMutexLocker lock(&d->stateMutex);
  // Operations can only be removed when the operation is stopped or paused
  if (op == 0 || !d->lstOperations.contains(op) || d->state not_member_of (Stopped, Paused))
    return;

  op->disconnectAllInputs();
  op->disconnectAllOutputs();
  d->lstOperations.removeAll(op);
  op->disconnect(this);
  op->setParent(0);
}

PiiOperation* PiiOperationCompound::removeOperation(const QString& name)
{
  PiiOperation* pOp = findChildOperation(name);
  removeOperation(pOp);
  return pOp;
}

struct PiiOperationCompound::InputFinder
{
  typedef PiiAbstractInputSocket* Type;
  InputFinder(const QList<Type>& inputs) : lstInputs(inputs) {}
  Type find(PiiOperation* op, const QString& path) const { return op->input(path); }
  Type get(const QString& name) const { return findSocket(name, lstInputs); }
  const QList<Type>& lstInputs;
};

struct PiiOperationCompound::OutputFinder
{
  typedef PiiAbstractOutputSocket* Type;
  OutputFinder(const QList<Type>& outputs) : lstOutputs(outputs) {}
  Type find(PiiOperation* op, const QString& path) const { return op->output(path); }
  Type get(const QString& name) const { return findSocket(name, lstOutputs); }
  const QList<Type>& lstOutputs;
};

struct PiiOperationCompound::SetPropertyFinder
{
  typedef bool Type;
  SetPropertyFinder(PiiOperation* self, const QVariant& value) : _pSelf(self), _value(value) {}
  Type find(PiiOperation* op, const QString& path) const { return op->setProperty(qPrintable(path), _value); }
  // NOTE this bypasses PiiOperation::setProperty()
  Type get(const QString& name) const { return _pSelf->QObject::setProperty(qPrintable(name), _value); }
private:
  PiiOperation* _pSelf;
  const QVariant& _value;
};

struct PiiOperationCompound::GetPropertyFinder
{
  typedef QVariant Type;
  GetPropertyFinder(const PiiOperation* self) : _pSelf(self) {}
  Type find(PiiOperation* op, const QString& path) const { return op->property(qPrintable(path)); }
  // NOTE this bypasses PiiOperation::property()
  Type get(const QString& name) const { return _pSelf->QObject::property(qPrintable(name)); }
private:
  const PiiOperation* _pSelf;
};

struct PiiOperationCompound::OperationFinder
{
  typedef PiiOperation* Type;
  OperationFinder(const PiiOperationCompound* self) : _pSelf(self) {}
  Type find(PiiOperation* op, const QString& path) const
  {
    if (op->isCompound())
      return static_cast<PiiOperationCompound*>(op)->findChildOperation(path);
    return 0;
  }
  Type get(const QString& name) const { return _pSelf->findChildOperation(name); }
private:
  const PiiOperationCompound* _pSelf;
};

PiiAbstractInputSocket* PiiOperationCompound::input(const QString& path) const
{
  // Special treatment for the inner side sockets in proxy outputs.
  if (path.size() > 0 && path[0] == '.')
    {
      PiiAbstractOutputSocket* pProxy = findSocket(path.mid(1), _d()->lstOutputs);
      return pProxy ? PiiProxySocket::input(pProxy) : 0;
    }
  return find(InputFinder(_d()->lstInputs), path);
}

PiiAbstractOutputSocket* PiiOperationCompound::output(const QString& path) const
{
  // Special treatment for the inner side sockets in proxy inputs.
  if (path.size() > 0 && path[0] == '.')
    {
      PiiAbstractInputSocket* pProxy = findSocket(path.mid(1), _d()->lstInputs);
      return pProxy ? PiiProxySocket::output(pProxy) : 0;
    }
  return find(OutputFinder(_d()->lstOutputs), path);
}

void PiiOperationCompound::startPropertySet(const QString& name)
{
  PiiOperation::startPropertySet(name);
  commandChildren(StartPropertySet(), name);
}

void PiiOperationCompound::endPropertySet()
{
  commandChildren(EndPropertySet());
  PiiOperation::endPropertySet();
}

void PiiOperationCompound::removePropertySet(const QString& name)
{
  commandChildren(RemovePropertySet(), name);
}

void PiiOperationCompound::reconfigure(const QString& name)
{
  commandChildren(Reconfigure(), name);
}

bool PiiOperationCompound::setProperty(const char* name, const QVariant& value)
{
  return find(SetPropertyFinder(this, value), name);
}

QVariant PiiOperationCompound::property(const char* name) const
{
  return find(GetPropertyFinder(this), name);
}

PiiOperation* PiiOperationCompound::childOperation(const QString& name) const
{
  return find(OperationFinder(this), name);
}

template <class Finder> typename Finder::Type PiiOperationCompound::find(Finder f, const QString& path) const
{
  // First dot-separated part is the child name
  int dotIndex = path.indexOf('.');
  // There are no dots -> must be leaf node
  if (dotIndex == -1)
    return f.get(path);

  QString childName = path.mid(0, dotIndex);

  // Store the rest of the name
  QString rest;
  if (dotIndex < path.size()-1)
    rest = path.mid(dotIndex+1);

  // Special name for the parent operation -> we are not responsible
  // for the rest of the name.
  if (childName == "parent")
    {
      PiiOperation* parentOp = qobject_cast<PiiOperation*>(parent());
      if (parentOp)
        return f.find(parentOp, rest);
      return 0;
    }
  
  // Try to find the named (or indexed) child in our children
  PiiOperation* op = findChildOperation(childName);
  
  if (op != 0) // we found the child -> let it parse the rest
    return f.find(op, rest);

  return 0;
}

PiiOperation* PiiOperationCompound::findChildOperation(const QString& childName) const
{
  const PII_D;
  // Loop through all child operations to find the named operation
  for (int i=d->lstOperations.size(); i--; )
    if (d->lstOperations[i]->objectName() == childName)
      return d->lstOperations[i];
  return 0;
}

void PiiOperationCompound::setState(State state)
{
  PII_D;
  if (d->state != state)
    {
      aboutToChangeState(state);
      d->state = state;
      emit stateChanged(state);
    }
}

void PiiOperationCompound::aboutToChangeState(State /*newState*/)
{}

PiiOperation::State PiiOperationCompound::state() const
{
  return _d()->state;
}

PiiOperation* PiiOperationCompound::createOperation(const QString& className, const QString& objectName)
{
  PiiOperation* op = PiiYdin::createResource<PiiOperation>(qPrintable(className));

  // We got the pointer -> set objectName and add to operation list
  if (op != 0)
    {
      if (!objectName.isEmpty())
        op->setObjectName(objectName);
      addOperation(op);
    }
  return op;
}


PiiOperationCompound::EndPointType PiiOperationCompound::locateSocket(PiiSocket* socket,
                                                                      const PiiOperationCompound* context) const
{
  if (context == 0)
    {
      PiiOperation* parent = Pii::findFirstParent<PiiOperation*>(socket);
      return EndPointType(parent, socket->objectName());
    }
  else
    {
      QList<PiiOperation*> lstParents = Pii::findAllParents<PiiOperation*>(socket);

      int iContextIndex = lstParents.indexOf(const_cast<PiiOperationCompound*>(context));
      if (iContextIndex != -1)
        {
          // Join the names of all parent operations with dots.
          QString strFullName;
          for (int i=iContextIndex+1; i<lstParents.size(); ++i)
            strFullName += lstParents[i]->objectName() + '.';
          strFullName += socket->objectName();
          return EndPointType(const_cast<PiiOperationCompound*>(context), strFullName);
        }
      else
        return EndPointType(0, socket->objectName());
    }
}

PiiOperationCompound::EndPointListType
PiiOperationCompound::buildEndPointList(PiiAbstractOutputSocket* output,
                                        const PiiOperationCompound* context) const
{
  EndPointListType result;

  /*
    Store all connection end points for the given output socket
   
    We only look for internal connections between operations and
    proxies here. If the output socket is aliased, its external
    connections will be cloned/serialized by the parent operation.
   */ 
  QList<PiiAbstractInputSocket*> lstInputs = output->connectedInputs();
  for (int i=0; i<lstInputs.size(); ++i)
    {
      PiiAbstractInputSocket* pInput = lstInputs[i];
      // The input is not owned by me nor any of my child operations.
      if (!Pii::isParent(this, pInput))
        continue; // skip this input
      // I'm the father of my own proxies.
      else if (pInput->isProxy() && PiiProxySocket::parent(pInput)->parent() == this)
        result << EndPointType(const_cast<PiiOperationCompound*>(this), proxyInputName(lstInputs[i]));
      else
        result << locateSocket(lstInputs[i], context);
    }
  return result;
}

/* Returns ".output" for the input side of an output proxy called
   "output".

   Returns "input" for an input proxy called "input".
*/
QString PiiOperationCompound::proxyInputName(PiiAbstractInputSocket* input) const
{
  const PII_D;
  // Search output proxies first
  for (int i=0; i<d->lstOutputs.size(); ++i)
    if (PiiProxySocket::input(d->lstOutputs[i]) == input)
      return QString(".%0").arg(d->lstOutputs[i]->objectName());

  // No luck. Try input proxies
  for (int i=0; i<d->lstInputs.size(); ++i)
    if (d->lstInputs[i]->isProxy() && d->lstInputs[i] == input)
      return d->lstInputs[i]->objectName();
  
  return QString();
}

PiiOperationCompound* PiiOperationCompound::clone() const
{
  const PII_D;
  // objectName() won't work for template classes, but we have the
  // serialization meta object...
  const char *className = PiiYdin::resourceName(this);
  
  // Use the serialization factory to create an instance of the most
  // derived class.
  PiiOperationCompound* pResult = PiiYdin::createResource<PiiOperationCompound>(className);
  
  if (pResult == 0) return 0;

  // Get rid of everything but the compound (sub-operations, sockets)
  pResult->clear();

  // Set properties if we are the most derived class.
  if (PiiOperationCompound::metaObject() == metaObject())
    Pii::setProperties(pResult, Pii::propertyList(this, 0, Pii::WritableProperties | Pii::DynamicProperties));

  // Clone sub-operations
  for (int i=0; i<d->lstOperations.size(); ++i)
    {
      PiiOperation* pChildClone = d->lstOperations[i]->clone();
      if (pChildClone == 0)
        {
          delete pResult;
          return 0;
        }
      pResult->addOperation(pChildClone);
    }
  
  // Initialize inputs
  for (int i=0; i<d->lstInputs.size(); ++i)
    {
      PiiAbstractInputSocket* pSocket = d->lstInputs[i];
      if (pSocket->isProxy())
        pResult->createInputProxy(pSocket->objectName());
      else
        pResult->_d()->lstInputs << 0;
    }

  // Initialize outputs
  for (int i=0; i<d->lstOutputs.size(); ++i)
    {
      PiiAbstractOutputSocket* pSocket = d->lstOutputs[i];
      if (pSocket->isProxy())
        pResult->createOutputProxy(pSocket->objectName());
      else
        pResult->_d()->lstOutputs << 0;
    }

  // This must be done in two phases because an input may be directly
  // connected to an output proxy and vice versa.
  
  // Remap input aliases
  for (int i=0; i<d->lstInputs.size(); ++i)
    {
      PiiAbstractInputSocket* pSocket = d->lstInputs[i];
      if (!pSocket->isProxy())
        pResult->_d()->lstInputs[i] = pResult->input(locateSocket(pSocket, this).second);
    }
  
  // Remap output aliases
  for (int i=0; i<d->lstOutputs.size(); ++i)
    {
      PiiAbstractOutputSocket* pSocket = d->lstOutputs[i];
      if (!pSocket->isProxy())
        pResult->_d()->lstOutputs[i] = pResult->output(locateSocket(pSocket, this).second);
    }

  // At this point, all sockets (external and internal) exist and can be retrieved with their names.
  // We can proceed to connecting them.

  // Clone internal connections between operations and from operations to proxies.
  for (int i=0; i<d->lstOperations.size(); ++i)
    {
      // Find all output sockets
      QList<PiiAbstractOutputSocket*> lstOutputs = d->lstOperations[i]->outputs();
 
      // Clone all connections starting from this operation
      for (int j=0; j<lstOutputs.size(); ++j)
        {
          // Find all internal end points for this output socket.
          EndPointListType lstInputs = buildEndPointList(lstOutputs[j], this);
          
          for (int k=0; k<lstInputs.size(); ++k)
            {
              PiiAbstractInputSocket* target = pResult->input(lstInputs[k].second);

              // The index of the source operation is the same in the
              // clone's child list. The name of the output in the
              // context of our child operation is used to find the
              // output in the cloned operation.
              PiiAbstractOutputSocket *source = pResult->_d()->lstOperations[i]->outputAt(j);

              if (source != 0 && target != 0)
                source->connectInput(target);
            }
        }
    }

  // Clone connections from proxy inputs to operations and other proxies.
  for (int i=0; i<d->lstInputs.size(); ++i)
    {
      PiiAbstractInputSocket* pSocket = d->lstInputs[i];
      if (pSocket->isProxy())
        {
          EndPointListType lstExposedInputs = buildEndPointList(PiiProxySocket::output(pSocket), this);
          PiiAbstractOutputSocket* pOutput = PiiProxySocket::output(pResult->_d()->lstInputs[i]);
          for (int j=0; j<lstExposedInputs.size(); ++j)
            pOutput->connectInput(pResult->input(lstExposedInputs[j].second));
        }
    }

  // Corner case: someone routed a proxied output back to an internal operation or one of the exposed inputs.
  for (int i=0; i<d->lstOutputs.size(); ++i)
    {
      PiiAbstractOutputSocket* pSocket = d->lstOutputs[i];
      if (pSocket->isProxy())
        {
          EndPointListType lstExposedInputs = buildEndPointList(pSocket, this);
          for (int j=0; j<lstExposedInputs.size(); ++j)
            pResult->connectOutput(pSocket->objectName(), lstExposedInputs[j].second);
        }
    }
  
  return pResult;
}

template <class Socket>
Socket* PiiOperationCompound::findSocket(const QString& name, const QList<Socket*>& list)
{
  for (typename QList<Socket*>::const_iterator it=list.begin(); it != list.end(); ++it)
    if ((*it)->objectName() == name)
      return *it;
  return 0;
}

void PiiOperationCompound::connectAll(PiiAbstractOutputSocket* source, const EndPointListType& targets)
{
  if (source == 0) return;
  for (int i=0; i<targets.size(); ++i)
    {
      PiiAbstractInputSocket* pTarget = targets[i].first->input(targets[i].second);
      if (pTarget != 0)
        source->connectInput(pTarget);
    }
}
