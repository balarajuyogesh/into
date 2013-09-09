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

#include "PiiOperationPtrWrapper.h"

#include "PiiScript.h"
#include "PiiScriptObjectWrapper.h"

#include <PiiOperation.h>

namespace PiiOperationPtrWrapper
{
  PII_STATIC_TR_FUNC(PiiOperationPtr)

  static QScriptValue createPiiOperationPtr(QScriptContext *context, QScriptEngine *engine)
  {
    PII_CHECK_ARGUMENT_COUNT_RANGE(PiiOperationPtr, 0, 1);
    PiiOperationPtr variant;
    PiiOperation* pOperation = 0;
    if (context->argumentCount() == 1)
      {
        pOperation = qscriptvalue_cast<PiiOperation*>(context->argument(0));
        if (pOperation == 0)
          return context->throwError(tr(PiiScript::pInstanceOfXRequired).arg("PiiOperationPtr").arg("PiiOperation"));
        piiDebug(QString("Creating %1.").arg(pOperation->metaObject()->className()));
      }
    engine->newQObject(context->thisObject(),
                       PiiScriptObjectWrapper::create(PiiOperationPtr(pOperation->clone())),
                       QScriptEngine::AutoOwnership,
                       PiiScript::defaultWrapOptions);
    return engine->undefinedValue();
  }
  
  static QScriptValue convertPiiOperationPtrToScriptValue(QScriptEngine* engine, const PiiOperationPtr& ptr)
  {
    return engine->newQObject(PiiScriptObjectWrapper::create(ptr),
                              QScriptEngine::AutoOwnership,
                              PiiScript::defaultWrapOptions);
  }
  
  static void convertScriptValueToPiiOperationPtr(const QScriptValue& scriptObj, PiiOperationPtr& ptr)
  {
    PiiScriptObjectWrapper* pWrapper = qobject_cast<PiiScriptObjectWrapper*>(scriptObj.toQObject());
    if (pWrapper != 0)
      {
        PiiOperationPtr pOperation = pWrapper->value<PiiOperationPtr>();
        if (pOperation)
          ptr = pOperation;
      }
  }
}

void initPiiOperationPtr(QScriptEngine* engine)
{
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiOperationPtrWrapper, PiiOperationPtr);
  engine->globalObject().setProperty("PiiOperationPtr", engine->newFunction(PiiOperationPtrWrapper::createPiiOperationPtr));
}
