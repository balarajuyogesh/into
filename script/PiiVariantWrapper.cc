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

#include "PiiVariantWrapper.h"

#include "PiiScript.h"

#include <PiiVariant.h>
#include <PiiYdinTypes.h>

QString PiiVariantScriptObject::toString() const
{
  return PiiYdin::convertToQString(variant);
}

int PiiVariantScriptObject::toInt() const
{
  return PiiYdin::convertPrimitiveTo<int>(variant);
}

double PiiVariantScriptObject::toDouble() const
{
  return PiiYdin::convertPrimitiveTo<double>(variant);
}

bool PiiVariantScriptObject::toBool() const
{
  return PiiYdin::convertPrimitiveTo<bool>(variant);
}

namespace PiiVariantWrapper
{
  PII_STATIC_TR_FUNC(PiiVariant)

  static QScriptValue createPiiVariant(QScriptContext *context, QScriptEngine *engine)
  {
    PII_CHECK_ARGUMENT_COUNT_RANGE(PiiVariant, 0, 1);
    PiiVariant variant;
    if (context->argumentCount() == 1)
      {
        QScriptValue arg = context->argument(0);
        if (arg.isNumber())
          {
            // int
            if (arg.toNumber() - arg.toInt32() == 0.0)
              variant = PiiVariant(arg.toInt32());
            // double
            else
              variant = PiiVariant(arg.toNumber());
          }
        // QString
        else if (arg.isString())
          variant = PiiVariant(arg.toString());
      }
    engine->newQObject(context->thisObject(),
                       new PiiVariantScriptObject(variant),
                       QScriptEngine::AutoOwnership,
                       PiiScript::defaultWrapOptions);
    return engine->undefinedValue();
  }
  static QScriptValue convertPiiVariantToScriptValue(QScriptEngine* engine, const PiiVariant& var)
  {
    return engine->newQObject(new PiiVariantScriptObject(var),
                              QScriptEngine::AutoOwnership,
                              PiiScript::defaultWrapOptions);
  }
  static void convertScriptValueToPiiVariant(const QScriptValue& scriptObj, PiiVariant& var)
  {
    PiiVariantScriptObject* pWrapper = qobject_cast<PiiVariantScriptObject*>(scriptObj.toQObject());
    if (pWrapper != 0)
      var = pWrapper->variant;
  }
}

void initPiiVariant(QScriptEngine* engine)
{
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiVariantWrapper, PiiVariant);
  engine->globalObject().setProperty("PiiVariant", engine->newFunction(PiiVariantWrapper::createPiiVariant));
}
