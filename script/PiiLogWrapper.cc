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

#include <PiiLog.h>

#include "PiiScript.h"

namespace PiiLogWrapper
{
  PII_STATIC_TR_FUNC(PiiLog)

  static QScriptValue piiLog(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_ARGUMENT_COUNT(piiLog, 3);
    ::piiLog(qPrintable(context->argument(0).toString()),
             (QtMsgType)qBound(0, context->argument(1).toInt32(), 3),
             "%s", context->argument(2).toString().toLocal8Bit().constData());
    return engine->undefinedValue();
  }

  static QScriptValue findLogModule(QScriptContext* context)
  {
    if (context == 0)
      return QScriptValue(QScriptValue::UndefinedValue);
    QScriptValue value = context->thisObject().property("PII_LOG_MODULE");
    if (!value.isUndefined())
      return value;
    return findLogModule(context->parentContext());
  }

  static QScriptValue piiLog(QScriptContext* context, QScriptEngine* engine, QtMsgType level)
  {
    QScriptValue module = findLogModule(context);
    QString strModule = module.isUndefined() ? "Into" : module.toString();
    piiLog(qPrintable(strModule),
           level,
           "%s", context->argument(0).toString().toLocal8Bit().constData());
    return engine->undefinedValue();
  }
  
#define PII_SCRIPT_DEBUG_FUNCTION(TYPE) \
  static QScriptValue pii ## TYPE(QScriptContext* context, QScriptEngine* engine) \
  {                                                                     \
    PII_CHECK_ARGUMENT_COUNT(pii ## TYPE, 1);                           \
    return piiLog(context, engine, Qt ## TYPE ## Msg);                  \
  }

  PII_SCRIPT_DEBUG_FUNCTION(Debug);
  PII_SCRIPT_DEBUG_FUNCTION(Warning);
  PII_SCRIPT_DEBUG_FUNCTION(Critical);
  PII_SCRIPT_DEBUG_FUNCTION(Fatal);
}


void initPiiLog(QScriptEngine* engine)
{
#define PII_LOG_WRAPPER_FUNC(NAME) \
  engine->globalObject().setProperty(#NAME, engine->newFunction(PiiLogWrapper::NAME));
  
  PII_LOG_WRAPPER_FUNC(piiLog);
  PII_LOG_WRAPPER_FUNC(piiDebug);
  PII_LOG_WRAPPER_FUNC(piiWarning);
  PII_LOG_WRAPPER_FUNC(piiCritical);
  PII_LOG_WRAPPER_FUNC(piiFatal);  
}
