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

#include "PiiOperationWrapper.h"

#include "PiiScript.h"
#include "PiiSocketWrapper.h"

#include <PiiOperation.h>
#include <QScriptValueIterator>

namespace PiiOperationWrapper
{
  PII_STATIC_TR_FUNC(PiiOperation)

  PII_QOBJECT_CONVERSION_FUNCTIONS(PiiOperation);

  static void connect(PiiAbstractOutputSocket* output, PiiOperation* operation, QScriptValue input)
  {
    PiiAbstractInputSocket* pInput = 0;
    if (input.isString())
      pInput = operation->input(input.toString());
    else if (input.isQObject())
      {
        PiiSocket* pSocket = qobject_cast<PiiSocket*>(input.toQObject());
        if (pSocket != 0)
          pInput = pSocket->asInput();
      }

    if (pInput == 0)
      piiWarning("connect(): input \"%s\" does not exist.", qPrintable(input.toString()));
    
    if (output != 0 && pInput != 0)
      output->connectInput(pInput);
  }
  
  static QScriptValue connect(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_ONE_ARGUMENT(connect);
    PII_CHECK_THIS_TYPE(PiiOperation, connect);

    QScriptValue connections = context->argument(0);
    QScriptValueIterator it(connections);
    while (it.hasNext())
      {
        it.next();
        //qDebug("%s : %s", qPrintable(it.name()), qPrintable(it.value().toString()));
        PiiAbstractOutputSocket* pOutput = pThis->output(it.name());
        if (pOutput == 0)
          piiWarning("connect(): output \"%s\" does not exist.", qPrintable(it.name())); 

        QScriptValue currentValue = it.value();
        if (currentValue.isArray())
          {
            int iInputCnt = currentValue.property("length").toInt32();
            for (int i=0; i<iInputCnt; ++i)
              connect(pOutput, pThis, currentValue.property(i));
          }
        else
          connect(pOutput, pThis, currentValue);
      }
    return engine->undefinedValue();
  }
}

void initPiiOperation(QScriptEngine* engine)
{
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiOperationWrapper, PiiOperation);

  // No constructor function for PiiOperation
  QScriptValue operationObject = engine->newQMetaObject(&PiiOperation::staticMetaObject);
  QScriptValue prototype = operationObject.property("prototype");
  prototype.setProperty("connect", engine->newFunction(PiiOperationWrapper::connect));

  engine->setDefaultPrototype(qMetaTypeId<PiiOperation*>(), prototype);

  engine->globalObject().setProperty("PiiOperation", operationObject);
}

