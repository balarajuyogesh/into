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

#include "PiiClassIndexMapper.h"
#include <PiiYdinTypes.h>
#include <PiiUtil.h>
#include <QtDebug>

PiiClassIndexMapper::PiiClassIndexMapper() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("class index"));
  addSocket(new PiiOutputSocket("class index"));
  addSocket(new PiiOutputSocket("class name"));
  addSocket(new PiiOutputSocket("list index"));
}

void PiiClassIndexMapper::process()
{
  PII_D;
  PiiVariant obj = readInput();

  int index;
  switch (obj.type())
    {
      PII_NUMERIC_CASES(index = (int)PiiYdin::primitiveAs, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }

  int classIndex = -1, listIndex = -1;
  QString className("Unknown");
  
  if (index >= 0)
    {
      if (index < d->lstClassIndices.size())
        classIndex = d->lstClassIndices[index];
      if (index < d->lstClassNames.size())
        {
          className = d->lstClassNames[index];
          listIndex = d->lstListIndices[index];
        }
    }

  emitObject(classIndex);
  outputAt(1)->emitObject(className);
  outputAt(2)->emitObject(listIndex);
}

void PiiClassIndexMapper::setClassIndexMap(const QVariantList& classIndexMap)
{
  PII_D;
  d->lstClassIndices = Pii::variantsToList<int>(classIndexMap);
  setClassInfoMap(d->classInfoMap);
}

QVariantList PiiClassIndexMapper::classIndexMap() const
{
  const PII_D;
  return Pii::listToVariants(d->lstClassIndices);
}

void PiiClassIndexMapper::setClassInfoMap(const QVariantMap& classInfoMap)
{
  PII_D;
  d->classInfoMap = classInfoMap;
  d->lstClassNames.clear();
  d->lstListIndices.clear();
  
  QList<int> indices;
  bool bHaveIndices = false;
  if (d->classInfoMap.contains("classIndices"))
    {
      indices = Pii::variantsToList<int>(d->classInfoMap["classIndices"].toList());
      bHaveIndices = true;
    }
  QStringList names;
  if (d->classInfoMap.contains("classNames"))
    names = d->classInfoMap["classNames"].toStringList();
  
  for ( int i=0; i<d->lstClassIndices.size(); i++ )
    {
      int value = d->lstClassIndices[i];
      int index = i;
      if (bHaveIndices)
        index = indices.indexOf(value);
      
      if ( index > -1 && names.size() > index)
        d->lstClassNames.append(names[index]);
      else
        d->lstClassNames.append("Unknown");

      d->lstListIndices.append(index);
    }
}

QVariantMap PiiClassIndexMapper::classInfoMap() const { return _d()->classInfoMap; }
