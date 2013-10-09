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
  const char* metaObjectPredicate = "pii:qmetaobject";

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

static const char* pluginName() { return "PiiYdin"; }

PII_REGISTER_CLASS(PiiProbeInput, QObject);

#define PII_REGISTER_YDIN_OPERATION(CLASS, SUPER) \
  PII_REGISTER_RESOURCE_STATEMENT(CLASS, PiiYdin::parentPredicate, PiiYdin) \
  PII_REGISTER_SUPERCLASS(CLASS, SUPER) \
  PII_REGISTER_RESOURCEPTR_STATEMENT(CLASS, PiiYdin::metaObjectPredicate, const_cast<QMetaObject*>(&CLASS::staticMetaObject))

PII_BEGIN_STATEMENTS(PiiYdin)
  PII_REGISTER_YDIN_OPERATION(PiiOperation, QObject)
  PII_REGISTER_YDIN_OPERATION(PiiOperationCompound, PiiOperation)
  PII_REGISTER_YDIN_OPERATION(PiiEngine, PiiOperationCompound)
PII_END_STATEMENTS
