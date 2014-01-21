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

#include "PiiAbstractInputSocket.h"
#include "PiiAbstractOutputSocket.h"

PiiAbstractInputSocket::Data::Data() :
  PiiSocket::Data(Input),
  pConnectedOutput(0),
  pListener(0)
{}

PiiAbstractInputSocket::Data::~Data()
{}

bool PiiAbstractInputSocket::Data::setInputConnected(bool connected)
{
  return connected;
}

PiiAbstractInputSocket::PiiAbstractInputSocket(const QString& name, Data* data) :
  PiiSocket(data)
{
  setObjectName(name);
}

PiiAbstractInputSocket::~PiiAbstractInputSocket()
{
  disconnectOutput();
}

void PiiAbstractInputSocket::disconnectOutput()
{
  if (connectedOutput() != 0)
    connectedOutput()->disconnectInput(this);
}

void PiiAbstractInputSocket::connectOutput(PiiAbstractOutputSocket* output)
{
  disconnectOutput();
  if (output != 0)
    output->connectInput(this);
}

PiiAbstractOutputSocket* PiiAbstractInputSocket::connectedOutput() const { return _d()->pConnectedOutput; }
void PiiAbstractInputSocket::setListener(PiiInputListener* listener) { _d()->pListener = listener; }
PiiInputListener* PiiAbstractInputSocket::listener() const { return _d()->pListener; }
