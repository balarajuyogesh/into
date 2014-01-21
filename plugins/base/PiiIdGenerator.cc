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

#include "PiiIdGenerator.h"
#include <PiiYdinTypes.h>

PiiIdGenerator::Data::Data() :
  iNextIndex(0), iIndexWidth(6)
{
}

PiiIdGenerator::PiiIdGenerator() :
  PiiDefaultOperation(new Data)
{
  PII_D;
  d->pTriggerInput = new PiiInputSocket("trigger");
  addSocket(d->pTriggerInput);


  d->pIdOutput = new PiiOutputSocket("id");
  addSocket(d->pIdOutput);
}

void PiiIdGenerator::process()
{
  PII_D;
  QString id = QString("%1%2%3").arg(d->strPrefix).arg(d->iNextIndex++, d->iIndexWidth, 10, QChar('0')).arg(d->strSuffix);
  d->pIdOutput->emitObject(id);
}

void PiiIdGenerator::check(bool reset)
{
  PiiDefaultOperation::check(reset);
  //if (reset)
  //  d->iNextIndex = 0;
}

void PiiIdGenerator::setPrefix(const QString& prefix) { _d()->strPrefix = prefix; }
QString PiiIdGenerator::prefix() const { return _d()->strPrefix; }
void PiiIdGenerator::setSuffix(const QString& suffix) { _d()->strSuffix = suffix; }
QString PiiIdGenerator::suffix() const { return _d()->strSuffix; }
void PiiIdGenerator::setNextIndex(int nextIndex) { _d()->iNextIndex = nextIndex; }
int PiiIdGenerator::nextIndex() const { return _d()->iNextIndex; }
void PiiIdGenerator::setIndexWidth(int indexWidth) { if (indexWidth >= 0) _d()->iIndexWidth = indexWidth; }
int PiiIdGenerator::indexWidth() const { return _d()->iIndexWidth; }
