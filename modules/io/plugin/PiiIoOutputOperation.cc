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

#include "PiiIoOutputOperation.h"

#include <PiiYdinTypes.h>
#include "PiiIoDriverException.h"

PiiIoOutputOperation::Data::Data()
{
}

PiiIoOutputOperation::PiiIoOutputOperation() :
  PiiIoOperation(new Data)
{
  PII_D;

  addSocket(d->pChannelInput = new PiiInputSocket("channel"));
  addSocket(d->pValueInput = new PiiInputSocket("value"));
  d->pValueInput->setOptional(true);
}

PiiIoOutputOperation::~PiiIoOutputOperation()
{
}

QVariantList PiiIoOutputOperation::pulseWidths() const
{
  const PII_D;
  QVariantList lstWidths;
  for (int i=0; i<d->lstChannels.size(); ++i)
    lstWidths << d->lstChannels[i]->property("pulseWidth").toInt();
  return lstWidths;
}

void PiiIoOutputOperation::setChannelState(int channel, bool value)
{
  // PENDING stateLock() ?
  PiiWriteLocker lock(processLock());
  if (state() == Running)
    {
      PII_D;
      int iCount = 0;
      while (iCount < 2)
        {
          try
            {
              if (channel >=0 && channel < d->lstChannels.size())
                d->lstChannels[channel]->setOutputState(value);

              return;
            }
          catch (PiiIoDriverException& ex)
            {
              iCount++;
            }
        }
      piiWarning(tr("Couldn't set channel states."));
    }
}

void PiiIoOutputOperation::activateChannel(int channel)
{
  PII_D;
  if (d->bEnabled && channel >= 0 && channel < d->lstChannels.size())
    {
      int iCount = 0;
      while (iCount < 2)
        {
          try
            {
              d->lstChannels[channel]->activate();
              return;
            }
          catch (PiiIoDriverException &ex)
            {
              iCount++;
            }
        }
      piiWarning(tr("Couldn't activate channel %1").arg(channel));
    }
}

void PiiIoOutputOperation::process()
{
  PII_D;
  if (!d->pValueInput->isConnected() || PiiYdin::convertPrimitiveTo<bool>(d->pValueInput))
    {
      PiiVariant obj(d->pChannelInput->firstObject());
      if (obj.type() == PiiVariant::IntType)
        {
          activateChannel(obj.valueAs<int>());
        }
      if (obj.type() == PiiYdin::IntMatrixType)
        {
          PiiMatrix<int> matIndices(obj.valueAs<PiiMatrix<int> >());
          if (matIndices.isEmpty())
            return;
          if (matIndices.columns() == 1)
            {
              for (int r=0; r<matIndices.rows(); ++r)
                activateChannel(matIndices(r,0));
            }
          else if (matIndices.rows() == 1)
            {
              for (int c=0; c<matIndices.columns(); ++c)
                activateChannel(matIndices(0,c));
            }
          else
            PII_THROW(PiiExecutionException,
                      tr("The channels to be activated must be represented as either a column or a row vector."));
        }
    }
}
