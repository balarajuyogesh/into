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

#include "PiiYdin.h"
#include "PiiEngine.h"
#include "PiiPlugin.h"
#include "PiiProbeInput.h"

namespace PiiYdin
{
  const char* classPredicate = "pii:class";
  const char* parentPredicate = "pii:parent";
  const char* connectorPredicate = "pii:connector";
  const char* offsetPredicate = "pii:offset";

  PiiResourceDatabase* resourceDatabase()
  {
    static PiiResourceDatabase database;
    return &database;
  }

  bool isNameProperty(const char* propertyName)
  {
    return !strcmp(propertyName, "name");
  }
}

static const char* pluginName() { static const char* pName = "PiiYdin"; return pName; }

PII_REGISTER_CLASS(PiiProbeInput, QObject);

PII_BEGIN_STATEMENTS(PiiYdin)
  PII_REGISTER_SUPERCLASS(PiiOperation, QObject)
  PII_REGISTER_SUPERCLASS(PiiEngine, PiiOperationCompound)
  PII_REGISTER_SUPERCLASS(PiiOperationCompound, PiiOperation)
PII_END_STATEMENTS
