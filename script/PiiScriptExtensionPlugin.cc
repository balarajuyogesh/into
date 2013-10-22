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

#include "PiiScriptExtensionPlugin.h"

#include <QScriptEngine>

#include "PiiScript.h"

#include "PiiSocketWrapper.h"
#include "PiiOperationWrapper.h"
#include "PiiOperationPtrWrapper.h"
#include "PiiOperationCompoundWrapper.h"
#include "PiiEngineWrapper.h"
#include "PiiVariantWrapper.h"
#include "PiiLogWrapper.h"
#include "PiiQtTypeWrapper.h"

#include <PiiYdinTypes.h>

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(piiscript, PiiScriptExtensionPlugin);
#endif

namespace PiiScript
{
  PII_STATIC_TR_FUNC(PiiScript)
    
  static QScriptValue configure(QScriptContext* context, QScriptEngine* /*engine*/)
  {
    PII_CHECK_AT_LEAST_ONE_ARGUMENT(configure);
    for (int i=0; i<context->argumentCount(); ++i)
      configure(context->thisObject(), context->argument(i));
    return context->thisObject();
  }
}

void PiiScriptExtensionPlugin::initialize(const QString& /*key*/, QScriptEngine* engine)
{
  engine->globalObject().setProperty("PiiYdin", engine->newQMetaObject(&PiiYdin::staticMetaObject));
  engine->globalObject().property("Object").property("prototype").setProperty("configure",
                                                                              engine->newFunction(PiiScript::configure));

  initPiiSocket(engine);
  initPiiOperation(engine);
  initPiiOperationPtr(engine);
  initPiiOperationCompound(engine);
  initPiiEngine(engine);
  initPiiVariant(engine);
  initPiiLog(engine);
  initQtTypes(engine);
}

QStringList PiiScriptExtensionPlugin::keys() const
{
  return QStringList() << "Into";
}
