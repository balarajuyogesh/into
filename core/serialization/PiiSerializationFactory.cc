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

#include "PiiSerializationFactory.h"

PII_DEFINE_FACTORY_MAP(PiiSerialization::Void);

QList<const char*> PiiSerializationFactory::keys(MapType* map)
{
  QList<const char*> lstResult;
  for (MapType::const_iterator i = map->constBegin();
       i != map->constEnd();
       ++i)
    lstResult << i.key().ptr;
  return lstResult;
}


PiiSerializationFactory::~PiiSerializationFactory()
{
}

void PiiSerializationFactory::removeFrom(MapType* map)
{
  QMutableHashIterator<PiiConstCharWrapper, PiiSerializationFactory*> i(*map);
  while (i.hasNext())
    {
      i.next();
      if (i.value() == this)
        {
          i.remove();
          return;
        }
    }
}
