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

#include "PiiCacheOperation.h"

#include <PiiYdinTypes.h>

PiiCacheOperation::Data::Data() :
  iMaxBytes(2*1024*1024),
  iMaxObjects(0),
  bAllowOrderChanges(false),
  iConsumedMemory(0)
{
}

PiiCacheOperation::PiiCacheOperation() :
  PiiDefaultOperation(new Data)
{
  PII_D;
  addSocket(d->pKeyInput = new PiiInputSocket("key"));
  addSocket(d->pDataInput = new PiiInputSocket("data"));
  d->pDataInput->setOptional(true);

  addSocket(d->pFoundOutput = new PiiOutputSocket("found"));
  addSocket(d->pKeyOutput = new PiiOutputSocket("key"));
  addSocket(d->pDataOutput = new PiiOutputSocket("data"));
}

void PiiCacheOperation::process()
{
  PII_D;
  if (activeInputGroup() == d->pKeyInput->groupId())
    {
      QString strKey = PiiYdin::convertToQString(d->pKeyInput);
      QHash<QString,PiiVariant>::iterator i = d->hashObjects.find(strKey);
      if (i != d->hashObjects.end())
        d->pDataOutput->emitObject(i.value());
      else
        d->hashObjects.insert(strKey, d->pDataInput->firstObject());
    }
  /*
    int iSize = 64;
    if (PiiYdin::isMatrixType(obj.type()))
    iSize += PiiYdin::matrixRows(obj) * PiiYdin::matrixStride(obj);
    else
    PII_THROW_UNKNOWN_TYPE(inputAt(0));
  */
}


void PiiCacheOperation::setMaxBytes(int maxBytes) { _d()->iMaxBytes = maxBytes; }
int PiiCacheOperation::maxBytes() const { return _d()->iMaxBytes; }
void PiiCacheOperation::setMaxObjects(int maxObjects) { _d()->iMaxObjects = maxObjects; }
int PiiCacheOperation::maxObjects() const { return _d()->iMaxObjects; }
void PiiCacheOperation::setAllowOrderChanges(bool allowOrderChanges) { _d()->bAllowOrderChanges = allowOrderChanges; }
bool PiiCacheOperation::allowOrderChanges() const { return _d()->bAllowOrderChanges; }
