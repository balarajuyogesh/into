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

#include "PiiClassInfoRegisterer.h"
#include <PiiResourceStatement.h>

PiiClassInfoRegisterer::PiiClassInfoRegisterer(const char* parent,
                                               const char* child,
                                               const char* superClass,
                                               ptrdiff_t offset) :
  d(new Data)
{
  QList<PiiResourceStatement> lstStatements;
  if (parent != 0)
    lstStatements << PiiResourceStatement(child,
                                          PiiYdin::parentPredicate,
                                          parent,
                                          PiiResourceStatement::ResourceType);
  if (superClass != 0)
    {
      lstStatements << PiiResourceStatement(child,
                                            PiiYdin::classPredicate,
                                            superClass,
                                            PiiResourceStatement::ResourceType);
      if (offset != 0)
        lstStatements << PiiResourceStatement("#",
                                              PiiYdin::offsetPredicate,
                                              QString::number(offset),
                                              PiiResourceStatement::LiteralType);
    }

  d->lstIds = PiiYdin::resourceDatabase()->addStatements(lstStatements);
}

PiiClassInfoRegisterer::~PiiClassInfoRegisterer()
{
  PiiYdin::resourceDatabase()->removeStatements(d->lstIds);
  delete d;
}


