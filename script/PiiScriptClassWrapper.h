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

#ifndef _PIISCRIPTCLASSWRAPPER_H
#define _PIISCRIPTCLASSWRAPPER_H

#include <QScriptValue>
#include <QScriptEngine>
#include <PiiPreprocessor.h>
#include "PiiScript.h"

// Avoid recursive call to PII_EXPAND
#define PII_SCRIPT_EXPAND(X) X

#define PII_CONVERT_TO_SCRIPT_FUNC(CLASS) \
  static QScriptValue PII_JOIN3(convert, CLASS, ToScriptValue)(QScriptEngine* engine, const CLASS& obj)
#define PII_CONVERT_FROM_SCRIPT_FUNC(CLASS) \
  static void PII_JOIN(convertScriptValueTo, CLASS)(const QScriptValue& scriptObj, CLASS& obj)
#define PII_SCRIPT_CONSTRUCTOR(CLASS) \
  static QScriptValue PII_JOIN(create, CLASS)(QScriptContext *context, QScriptEngine *engine)

#define PII_DO_SET_SCRIPT_PROPERTY(TYPE, INITIAL, REST)      \
  result.setProperty(PII_STRINGIZE(PII_JOIN(INITIAL, REST)), \
                     obj.PII_JOIN(INITIAL, REST)());
#define PII_SET_SCRIPT_PROPERTY(N, PROP) PII_SCRIPT_EXPAND(PII_DO_SET_SCRIPT_PROPERTY PROP)

#define PII_DO_SET_OBJ_PROPERTY(TYPE, INITIAL, REST)                    \
  obj.PII_JOIN3(set, PII_UPPERCASE(INITIAL), REST)(scriptObj.property(PII_STRINGIZE(PII_JOIN(INITIAL, REST))).PII_JOIN(to, TYPE)());
#define PII_SET_OBJ_PROPERTY(N, PROP) PII_SCRIPT_EXPAND(PII_DO_SET_OBJ_PROPERTY PROP)

#define PII_DO_CONSTRUCT_OBJ(TYPE, INITIAL, REST)                       \
  context->thisObject().setProperty(PII_STRINGIZE(PII_JOIN(INITIAL, REST)), \
                                    context->argument(iCurrentArgument++).PII_JOIN(to, TYPE)());
#define PII_CONSTRUCT_OBJ(N, PROP) PII_SCRIPT_EXPAND(PII_DO_CONSTRUCT_OBJ PROP)

#endif //_PIISCRIPTCLASSWRAPPER_H

PII_CONVERT_TO_SCRIPT_FUNC(PII_SCRIPT_CLASS)
{
  QScriptValue result = engine->newObject();
  PII_FOR_N(PII_SET_SCRIPT_PROPERTY, PII_SCRIPT_PROPERTY_CNT, (PII_SCRIPT_PROPERTIES));
  return result;
}

PII_CONVERT_FROM_SCRIPT_FUNC(PII_SCRIPT_CLASS)
{
  PII_FOR_N(PII_SET_OBJ_PROPERTY, PII_SCRIPT_PROPERTY_CNT, (PII_SCRIPT_PROPERTIES));
}

#ifndef PII_SCRIPT_CUSTOM_CONSTRUCTOR
PII_SCRIPT_CONSTRUCTOR(PII_SCRIPT_CLASS)
{
  int iCurrentArgument = 0;
  PII_FOR_N(PII_CONSTRUCT_OBJ, PII_SCRIPT_PROPERTY_CNT, (PII_SCRIPT_PROPERTIES));
  PII_CHECK_ARGUMENT_COUNT(PII_SCRIPT_CLASS, iCurrentArgument);
  return engine->undefinedValue();
}
#else
#  undef PII_SCRIPT_CUSTOM_CONSTRUCTOR
#endif

#undef PII_SCRIPT_CLASS
#undef PII_SCRIPT_PROPERTIES
#undef PII_SCRIPT_PROPERTY_CNT
