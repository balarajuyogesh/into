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

#include "PiiProxySocket.h"
#include "PiiSocket.h"
#include "PiiAbstractInputSocket.h"
#include "PiiAbstractOutputSocket.h"
#include "PiiInputController.h"

#include <PiiAlgorithm.h>

class PiiProxyInputSocket : public PiiAbstractInputSocket
{
public:
  PiiProxyInputSocket(const QString& name, QObject* parent);
  PiiInputController* controller() const;

  class Data;
  PII_UNSAFE_D_FUNC;
};

class PiiProxyOutputSocket : public PiiAbstractOutputSocket
{
public:
  PiiProxyOutputSocket(const QString& name, QObject* parent, PiiProxyInputSocket* input);

  class Data;
  PII_UNSAFE_D_FUNC;
};

class PiiProxyInputSocket::Data :
  public PiiAbstractInputSocket::Data,
  public PiiInputController
{
public:
  Data() :
    PiiAbstractInputSocket::Data(),
    pOutputData(0)
  {
    type = ProxyInput;
  }

  bool setInputConnected(bool connected);
  bool tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ();

  PiiProxyOutputSocket::Data* pOutputData;
};

class PiiProxyOutputSocket::Data :
  public PiiAbstractOutputSocket::Data,
  PiiInputListener
{
public:
  Data(PiiProxyInputSocket* input) :
    PiiAbstractOutputSocket::Data(),
    pbInputCompleted(0),
    pInput(input),
    pInputData(input->_d())
  {
    pInputData->pOutputData = this;
    type = ProxyOutput;
  }

  ~Data()
  {
    delete[] pbInputCompleted;
  }

  void reset();
  void inputConnected(PiiAbstractInputSocket* input);
  void inputDisconnected(PiiAbstractInputSocket* input);
  void inputReady(PiiAbstractInputSocket*);

  bool *pbInputCompleted;
  PiiProxyInputSocket* pInput;
  PiiProxyInputSocket::Data* pInputData;
};

class PiiProxySocket::Data
{
public:
  Data(const QString& inputName, const QString& outputName, PiiProxySocket* parent) :
    pInput(new PiiProxyInputSocket(inputName, parent)),
    pOutput(new PiiProxyOutputSocket(outputName, parent, pInput))
  {}

  ~Data()
  {
    // Output may access input during destruction via the parent
    // proxy. Therefore, we need to set it to zero here to avoid a
    // dangling pointer.
    delete pInput;
    pInput = 0;
    delete pOutput;
    pOutput = 0;
  }

  PiiProxyInputSocket* pInput;
  PiiProxyOutputSocket* pOutput;
};

PiiProxyInputSocket::PiiProxyInputSocket(const QString& name, QObject* parent) :
  PiiAbstractInputSocket(name, new Data)
{
  setParent(parent);
}

PiiProxyOutputSocket::PiiProxyOutputSocket(const QString& name, QObject* parent, PiiProxyInputSocket* input) :
  PiiAbstractOutputSocket(name, new Data(input))
{
  setParent(parent);
}


PiiProxySocket::PiiProxySocket(QObject* parent) :
  QObject(parent),
  d(new Data("", "", this))
{}

PiiProxySocket::PiiProxySocket(const QString& inputName, const QString& outputName, QObject* parent) :
  QObject(parent),
  d(new Data(inputName, outputName, this))
{}

PiiProxySocket::~PiiProxySocket()
{
  delete d;
}

bool PiiProxyInputSocket::Data::setInputConnected(bool connected)
{
  return pOutputData ? pOutputData->setOutputConnected(connected) : false;
}

PiiInputController* PiiProxyInputSocket::controller() const
{
  return const_cast<Data*>(_d());
}

bool PiiProxyInputSocket::Data::tryToReceive(PiiAbstractInputSocket* /*sender*/, const PiiVariant& object) throw ()
{
  bool bAllCompleted = true;
  int iCnt = pOutputData->lstInputs.size();
  // Try to send to inputs that haven't already accepted the object.
  for (int i=0; i<iCnt; ++i)
    if (!pOutputData->pbInputCompleted[i])
      bAllCompleted &= (pOutputData->pbInputCompleted[i] =
                        pOutputData->lstInputs.controllerAt(i)->tryToReceive(pOutputData->lstInputs.inputAt(i), object));

  // Everything is done -> reinitialize completion flags and return
  // true
  if (bAllCompleted)
    {
      Pii::fillN(pOutputData->pbInputCompleted, iCnt, false);
      return true;
    }
  // Something is still missing
  return false;
}

void PiiProxyOutputSocket::Data::inputReady(PiiAbstractInputSocket*)
{
  // Pass this signal to the proxied output
  pInputData->pListener->inputReady(pInput);
}

void PiiProxyOutputSocket::Data::inputConnected(PiiAbstractInputSocket*)
{
  reset();
}

void PiiProxyOutputSocket::Data::inputDisconnected(PiiAbstractInputSocket*)
{
  reset();
}

void PiiProxySocket::reset()
{
  d->pOutput->_d()->reset();
}

void PiiProxyOutputSocket::Data::reset()
{
  delete[] pbInputCompleted;
  if (lstInputs.size() > 0)
    {
      pbInputCompleted = new bool[lstInputs.size()];
      Pii::fillN(pbInputCompleted, lstInputs.size(), false);
    }
  else
    pbInputCompleted = 0;
}

PiiAbstractInputSocket* PiiProxySocket::input() const { return d->pInput; }
PiiAbstractOutputSocket* PiiProxySocket::output() const { return d->pOutput; }

PiiAbstractInputSocket* PiiProxySocket::input(PiiSocket* socket)
{
  return socket->isProxy() ? static_cast<PiiProxySocket*>(socket->parent())->input() : 0;
}

PiiAbstractOutputSocket* PiiProxySocket::output(PiiSocket* socket)
{
  return socket->isProxy() ? static_cast<PiiProxySocket*>(socket->parent())->output() : 0;
}

PiiProxySocket* PiiProxySocket::parent(PiiSocket* socket)
{
  return socket->isProxy() ? static_cast<PiiProxySocket*>(socket->parent()) : 0;
}

PiiAbstractOutputSocket* PiiProxySocket::root(PiiAbstractOutputSocket* output)
{
  while (output && output->isProxy())
    {
      PiiAbstractInputSocket* pInput = input(output);
      if (pInput)
        output = pInput->connectedOutput();
      else
        break;
    }
  return output;
}

QList<PiiAbstractInputSocket*> PiiProxySocket::connectedInputs(PiiAbstractInputSocket* input)
{
  QList<PiiAbstractInputSocket*> lstResult;
  if (input == 0)
    return lstResult;

  PiiAbstractOutputSocket* pOutput = output(input);
  // If input is part of a proxy, recurse to all connected inputs.
  if (pOutput != 0)
    {
      QList<PiiAbstractInputSocket*> lstInputs(pOutput->connectedInputs());
      for (int i=0; i<lstInputs.size(); ++i)
        lstResult.append(connectedInputs(lstInputs[i]));
    }
  else
    lstResult.append(input);

  return lstResult;
}

bool PiiProxySocket::isConnected(PiiAbstractInputSocket* in, PiiAbstractOutputSocket* out)
{
  if (in == 0)
    return false;

  while (out != 0)
    {
      PiiAbstractInputSocket* pInput = input(out);
      if (pInput == in)
        return true;
      out = pInput ? pInput->connectedOutput() : 0;
    }

  return false;
}
