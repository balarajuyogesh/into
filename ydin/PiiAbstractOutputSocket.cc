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

#include "PiiAbstractOutputSocket.h"

#include "PiiAbstractInputSocket.h"
#include "PiiInputController.h"
#include "PiiProxySocket.h"

PiiAbstractOutputSocket::Data::Data() :
  PiiSocket::Data(Output)
{}

PiiAbstractOutputSocket::Data::~Data()
{}

bool PiiAbstractOutputSocket::Data::setOutputConnected(bool connected)
{
  bool bBranchConnected = false;
  // An output is forward-connected only if at least one of its child
  // branches is connected.
  for (int i=lstInputs.size(); i--; )
    bBranchConnected |= lstInputs.inputAt(i)->_d()->setInputConnected(connected);

  // The output is fully connected if it is both backward and forward
  // connected.
  return bBranchConnected && connected;
}

PiiAbstractOutputSocket::PiiAbstractOutputSocket(const QString& name, Data* data) :
  PiiSocket(data)
{
  setObjectName(name);
}

PiiAbstractOutputSocket::~PiiAbstractOutputSocket()
{
  // Break all connections
  disconnectInputs();
}

int PiiAbstractOutputSocket::connectedInputCount() const
{
  return _d()->lstInputs.size();
}

QStringList PiiAbstractOutputSocket::connectedInputNames() const
{
  QList<PiiAbstractInputSocket*> lstInputs(connectedInputs());
  QStringList lstFqns;
  for (int i=0; i<lstInputs.size(); ++i)
    lstFqns.append(lstInputs[i]->objectName());
  return lstFqns;
}

QList<PiiAbstractInputSocket*> PiiAbstractOutputSocket::connectedInputs() const
{
  const PII_D;
  QList<PiiAbstractInputSocket*> lstInputs;
  for (int i=0; i<d->lstInputs.size(); ++i)
    lstInputs << d->lstInputs.inputAt(i);
  return lstInputs;
}

void PiiAbstractOutputSocket::updateInput(PiiAbstractInputSocket* socket)
{
  PII_D;
  if (d->lstInputs.indexOf(socket) != -1)
    {
      d->lstInputs.updateController(socket);
      d->inputUpdated(socket);
    }
}

void PiiAbstractOutputSocket::connectInput(PiiAbstractInputSocket* input)
{
  PII_D;
  if (input != 0 &&
      d->lstInputs.indexOf(input) == -1 &&
      // disallow looping back to proxy input
      !PiiProxySocket::isConnected(input, this))
    {
      // Disconnect old output
      if (input->connectedOutput() != 0)
        input->disconnectOutput();

      // Store the connected input and its controller
      d->lstInputs.append(input);

      input->_d()->pConnectedOutput = this;

      PiiAbstractOutputSocket* pRootOutput = PiiProxySocket::root(this);
      if (pRootOutput != 0)
        pRootOutput->_d()->setOutputConnected(true);

      d->inputConnected(input);
    }
}

void PiiAbstractOutputSocket::disconnectInput(PiiAbstractInputSocket* input)
{
  PII_D;
  if (input != 0)
    disconnectInputAt(d->lstInputs.indexOf(input));
  else //disconnect all if input == 0
    {
      for (int i=d->lstInputs.size(); i--;)
        disconnectInputAt(i);
    }
}

void PiiAbstractOutputSocket::disconnectInputAt(int index)
{
  PII_D;
  if (index >= 0 && index < d->lstInputs.size())
    {
      PiiAbstractInputSocket* input = d->lstInputs.takeInputAt(index);

      input->_d()->pConnectedOutput = 0;
      input->_d()->pListener = 0;
      input->_d()->setInputConnected(false);

      PiiAbstractOutputSocket* pRootOutput = PiiProxySocket::root(this);
      if (pRootOutput != 0)
        pRootOutput->_d()->setOutputConnected(true);

      d->inputDisconnected(input);
    }
}

void PiiAbstractOutputSocket::reconnect(PiiAbstractOutputSocket* output, PiiAbstractInputSocket* input)
{
  PII_D;
  // Store connected inputs
  InputList lstInputs = d->lstInputs;
  // Disconnect them all
  disconnectInputs();
  // Reconnect inputs to the new output
  for (int i=0; i<lstInputs.size(); ++i)
    output->connectInput(lstInputs.inputAt(i));
  // Reconnect this output to new input
  if (input != 0)
    connectInput(input);
}

void PiiAbstractOutputSocket::Data::inputConnected(PiiAbstractInputSocket*)
{}

void PiiAbstractOutputSocket::Data::inputUpdated(PiiAbstractInputSocket*)
{}

void PiiAbstractOutputSocket::Data::inputDisconnected(PiiAbstractInputSocket*)
{}


int PiiAbstractOutputSocket::InputList::indexOf(PiiAbstractInputSocket* input) const
{
  int i = size();
  while (i--)
    if (at(i).first == input)
      break;
  return i;
}

PiiAbstractInputSocket* PiiAbstractOutputSocket::InputList::takeInputAt(int index)
{
  PiiAbstractInputSocket* pInput = inputAt(index);
  remove(index);
  return pInput;
}

PiiAbstractInputSocket* PiiAbstractOutputSocket::InputList::inputAt(int index) const
{
  return at(index).first;
}

PiiInputController* PiiAbstractOutputSocket::InputList::controllerAt(int index) const
{
  return at(index).second;
}

void PiiAbstractOutputSocket::InputList::append(PiiAbstractInputSocket* input)
{
  InputListParent::append(qMakePair(input, input->controller()));
}

void PiiAbstractOutputSocket::InputList::updateController(PiiAbstractInputSocket* input)
{
  int i = size();
  while (i--)
    if (at(i).first == input)
      {
        operator[](i).second = input->controller();
        return;
      }
}
