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

#include "PiiLookupTable.h"

#include <PiiYdinTypes.h>
#include <PiiMath.h>
#include <PiiUtil.h>

PiiLookupTable::Data::Data() :
  iMaxTableIndex(0),
  iMaxLookupIndex(0)
{
}

PiiLookupTable::PiiLookupTable() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("index"));
  setDynamicOutputCount(1);
}

void PiiLookupTable::setTable(const QVariantList& table)
{
  PII_D;
  d->lstTable = table;
  d->lstOutputValues.clear();

  if (table.size() == 0)
    return;

  // Multiple look-up lists
  if (table[0].type() == QVariant::List)
    {
      // Check that each is of equal length
      int len = table[0].toList().size();

      for (int i=1; i<table.size(); i++)
        if (table[i].type() != QVariant::List || table[i].toList().size() != len)
          return;

      // Convert each to a list of PiiVariants.
      for (int i=0; i<table.size(); i++)
        d->lstOutputValues.append(Pii::variantsToList<PiiVariant>(table[i].toList()));
    }
  // Only one list -> use as such
  else
    d->lstOutputValues << Pii::variantsToList<PiiVariant>(table);
}

void PiiLookupTable::setDynamicOutputCount(int cnt)
{
  if (cnt < 1) cnt = 1;
  setNumberedOutputs(cnt);
}

void PiiLookupTable::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);
  d->iMaxTableIndex = Pii::min(inputCount(), d->lstOutputValues.size()) - 1;
  d->iMaxLookupIndex = d->lstOutputValues.isEmpty() ? -1 : d->lstOutputValues[0].size() - 1;

  // Check that we were not given nulls.
  for (int i=d->lstOutputValues.size(); i--; )
    for (int j=d->lstOutputValues[i].size(); j--; )
      if (!d->lstOutputValues[i][j].isValid())
        PII_THROW(PiiExecutionException, tr("The provided look-up table contains invalid values."));
}

void PiiLookupTable::process()
{
  PII_D;
  PiiVariant obj = readInput();

  int index = 0;
  switch (obj.type())
    {
      PII_PRIMITIVE_CASES(index = (int)PiiYdin::primitiveAs, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }

  // If there is no lookup table entry, we need to resort to the
  // default value.
  if (index < 0 || index > d->iMaxLookupIndex)
    {
      if (d->varDefaultValue.isValid())
        {
          for (int i=outputCount(); i--; )
            emitObject(d->varDefaultValue, i);
          return;
        }
      else
        PII_THROW(PiiExecutionException, tr("The value of the index input (%1) is out of range (0-%2).").arg(index).arg(d->iMaxLookupIndex));
    }

  int i = 0;
  // Emit look-up table values to all outputs that have the look-up
  // table defined.
  for (; i<=d->iMaxTableIndex; ++i)
    emitObject(d->lstOutputValues[i][index], i);
  // Use the last valid value for the rest of the outputs.
  for (; i<d->lstOutputs.size(); ++i)
    emitObject(d->lstOutputValues[d->iMaxTableIndex][index], i);
}

QVariantList PiiLookupTable::table() const { return _d()->lstTable; }
int PiiLookupTable::dynamicOutputCount() const { return outputCount(); }
void PiiLookupTable::setDefaultValue(const PiiVariant& defaultValue) { _d()->varDefaultValue = defaultValue; }
PiiVariant PiiLookupTable::defaultValue() const { return _d()->varDefaultValue; }
