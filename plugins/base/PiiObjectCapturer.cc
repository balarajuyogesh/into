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

#include "PiiObjectCapturer.h"

#include <PiiYdinTypes.h>

// PENDING maximum amount of buffered data

PiiObjectCapturer::Data::Data() :
  iDynamicInputCount(1),
  listMode(OneListPerInput)
{
}

PiiObjectCapturer::PiiObjectCapturer() :
  PiiDefaultOperation(new Data)
{
  PII_D;
  
  addSocket(d->pSyncInput = new PiiInputSocket("sync"));
  d->pSyncInput->setOptional(true);

  setDynamicInputCount(1);

  setProtectionLevel("dynamicInputCount", WriteWhenStopped);
  setProtectionLevel("listMode", WriteWhenStopped);
}

PiiObjectCapturer::~PiiObjectCapturer()
{}

void PiiObjectCapturer::check(bool reset)
{
  PiiDefaultOperation::check(reset);
  if (reset)
    clearObjects();
}

PiiInputSocket* PiiObjectCapturer::input(const QString &name) const
{
  if (name == "input")
    return PiiDefaultOperation::input("input0");
  return PiiDefaultOperation::input(name);
}

void PiiObjectCapturer::clearObjects()
{
  PII_D;
  
  d->syncObject = PiiVariant();
  d->lstObjects.clear();

  if (d->listMode == OneListPerInput)
    {
      for (int i=0; i<d->iDynamicInputCount; i++)
        d->lstObjects.insert(i,QVariantList());
    }
}

void PiiObjectCapturer::process()
{
  PII_D;

  if (activeInputGroup() == 0)
    d->syncObject = d->pSyncInput->firstObject();
  else
    {
      if (d->pSyncInput->isConnected())
        {
          if (d->listMode == OneListPerInput)
            {
              for (int i=0; i<d->iDynamicInputCount; i++)
                d->lstObjects[i].append(QVariant::fromValue(inputAt(i+1)->firstObject()));
            }
          else
            {
              QVariantList lstObjects;
              for (int i=1; i<=d->iDynamicInputCount; i++)
                lstObjects << QVariant::fromValue(inputAt(i)->firstObject());
              d->lstObjects << lstObjects;
            }
        }
      else
        {
          if (d->iDynamicInputCount == 1)
            emit objectCaptured(inputAt(1)->firstObject());
          else
            {
              QVariantList lstObjects;
              for (int i=1; i<=d->iDynamicInputCount; i++)
                lstObjects << QVariant::fromValue(inputAt(i)->firstObject());

              emit objectsCaptured(lstObjects);
            }
        }
    }
}

void PiiObjectCapturer::syncEvent(SyncEvent* event)
{
  PII_D;

  if (event->type() == SyncEvent::StartInput)
    {
      d->syncObject = PiiVariant();
      if (d->listMode == OneListPerInput)
        for (int i=0; i<d->iDynamicInputCount; i++)
          d->lstObjects[i].clear();
    }
  else
    {
      QVariantList lstObjects;
      for (int i=0; i<d->lstObjects.size(); i++)
        lstObjects << QVariant(d->lstObjects[i]);
      emit objectsCaptured(d->syncObject, lstObjects);
      clearObjects();
    }
}

void PiiObjectCapturer::setDynamicInputCount(int dynamicInputCount)
{
  PII_D;
  
  if (dynamicInputCount < 1)
    return;
  
  d->iDynamicInputCount = dynamicInputCount;
  setNumberedInputs(d->iDynamicInputCount, 1);

  for (int i=1; i<=d->iDynamicInputCount; ++i)
    PiiDefaultOperation::inputAt(i)->setGroupId(1);
  
  clearObjects();
}

int PiiObjectCapturer::dynamicInputCount() const { return _d()->iDynamicInputCount; }

void PiiObjectCapturer::setListMode(ListMode listMode) { _d()->listMode = listMode; }
PiiObjectCapturer::ListMode PiiObjectCapturer::listMode() const { return _d()->listMode; }
