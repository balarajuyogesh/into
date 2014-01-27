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

#include "PiiIoInputOperation.h"

#include <PiiYdinTypes.h>

PiiIoInputOperation::Data::Data()
{
}

PiiIoInputOperation::PiiIoInputOperation() : PiiIoOperation(new Data)
{
  PII_D;
  addSocket(d->pChannelOutput = new PiiOutputSocket("channel"));
  setThreadingCapabilities(NonThreaded);
}

void PiiIoInputOperation::check(bool reset)
{
  PII_D;

  // PiiIoOperation::check() may change channels -> disconnect the old ones
  for (int i=0; i<d->lstChannels.size(); ++i)
    d->lstChannels[i]->disconnect(this);

  PiiIoOperation::check(reset);

  // Reconnect (possibly) new ones
  for (int i=0; i<d->lstChannels.size(); ++i)
    connect(d->lstChannels[i], SIGNAL(inputStateChanged(bool)), this, SLOT(sendSignal(bool)));
}

void PiiIoInputOperation::setChannelConfigs(const QVariantList& channelConfigs)
{
  PiiIoOperation::setChannelConfigs(channelConfigs);
  setNumberedOutputs(channelConfigs.size(), 1);
}

void PiiIoInputOperation::process()
{
  // never called
}

void PiiIoInputOperation::sendSignal(bool state)
{
  PII_D;
  int iChannel = d->lstChannels.indexOf(static_cast<PiiIoChannel*>(sender()));
  d->pChannelOutput->emitObject(iChannel);
  emitObject(iChannel + 1, state);
}
