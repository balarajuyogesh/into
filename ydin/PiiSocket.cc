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

#include "PiiSocket.h"

PiiSocket::Data::Data(Type t) :
  type(t)
{}

PiiSocket::Data::~Data() {}

PiiSocket::PiiSocket(Data* d) :
  d(d)
{}

PiiSocket::~PiiSocket()
{
  delete d;
}

bool PiiSocket::isProxy() const
{
  return d->type >= ProxyInput;
}

bool PiiSocket::isInput() const
{
  return (d->type & 1) == 0;
}

bool PiiSocket::isOutput() const
{
  return (d->type & 1) == 1;
}
