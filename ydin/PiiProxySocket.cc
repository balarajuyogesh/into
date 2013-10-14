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

#include <PiiProxySocket.h>
#include <PiiAlgorithm.h>

PiiProxySocket::Data::Data(PiiProxySocket* owner) :
  PiiSocket::Data(Proxy)
  pbInputCompleted(0)
{
}

PiiProxySocket::PiiProxySocket() :
  PiiSocket(new Data(this))
{}

PiiProxySocket::~PiiProxySocket()
{
  delete[] _d()->pbInputCompleted;
}

PiiAbstractOutputSocket* PiiProxyOutputSocket::Data::rootOutput() const
{
  if (pInput->pConnectedOutput)
    return pInput->pConnectedOutput->rootOutput();
  return 0;
}

bool PiiProxySocket::Data::setInputConnected(bool connected)
{
  return setOutputConnected(connected);
}

PiiInputController* PiiProxyInputSocket::controller() const
{
  return const_cast<Data*>(_d());
}

bool PiiProxyInputSocket::Data::tryToReceive(PiiAbstractInputSocket* /*sender*/, const PiiVariant& object) throw ()
{
  PII_D;
  bool bAllCompleted = true;
  int iCnt = pOutput->lstInputs.size();
  // Try to send to inputs that haven't already accepted the object.
  for (int i=0; i<iCnt; ++i)
    if (!pbInputCompleted[i])
      bAllCompleted &= (pbInputCompleted[i] =
                        pOutput->lstInputs.controllerAt(i)->tryToReceive(pOutput->lstInputs.inputAt(i), object));

  // Everything is done -> reinitialize completion flags and return
  // true
  if (bAllCompleted)
    {
      Pii::fillN(pbInputCompleted, iCnt, false);
      return true;
    }
  // Something is still missing
  return false;
}

void PiiProxySocket::inputReady(PiiAbstractInputSocket*)
{
  // Pass this signal to the proxied output
  _d()->pListener->inputReady(this);
}

void PiiProxySocket::inputConnected(PiiAbstractInputSocket*)
{
  reset();
}

void PiiProxySocket::inputDisconnected(PiiAbstractInputSocket*)
{
  reset();
}

void PiiProxySocket::reset()
{
  PII_D;
  //qDebug("PiiOutputSocket: creating flag array for %d connections.", d->lstInputs.size());
  delete[] d->pbInputCompleted;
  if (d->lstInputs.size() > 0)
    {
      d->pbInputCompleted = new bool[d->lstInputs.size()];
      Pii::fillN(d->pbInputCompleted, d->lstInputs.size(), false);
    }
  else
    d->pbInputCompleted = 0;
}

PiiProxySocket* PiiProxySocket::socket() { return this; }
PiiAbstractInputSocket* PiiProxySocket::asInput() { return this; }
PiiAbstractOutputSocket* PiiProxySocket::asOutput() { return this; }
