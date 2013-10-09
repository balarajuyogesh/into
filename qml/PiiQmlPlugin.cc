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

#include "PiiQmlPlugin.h"
#include "PiiQml.h"

#include "PiiOperationCompoundWrapper.h"
#include "PiiEngineWrapper.h"
#include "PiiVariantWrapper.h"
#include <PiiProbeInput.h>

void PiiQmlPlugin::registerTypes(const char *uri)
{
  Q_ASSERT(uri == QLatin1String("Into"));
  qmlRegisterUncreatableType<PiiOperation>(uri, 1, 0, "PiiOperation", "PiiOperation is an abstract superclass that cannot be instantiated.");
  qmlRegisterType<PiiOperationCompound>(uri, 1, 0, "PiiEngine");
  qmlRegisterType<PiiEngine>(uri, 1, 0, "PiiEngine");
  qmlRegisterType<PiiProbeInput>(uri, 1, 0, "PiiProbeInput");

  QQml_addValueTypeProvider(PiiPluginTypeProvider::instance());
  QQml_addValueTypeProvider(PiiVariantTypeProvider::instance());
}

void PiiQmlPlugin::initializeEngine(QQmlEngine* engine, const char*)
{
  v8::HandleScope handleScope;
  v8::Local<v8::Context> context(v8::Local<v8::Context>::New(engine->handle()->context()));
  v8::Context::Scope contextScope(context);
  v8::Local<v8::Object> globalObject(PiiQml::globalObject(context));

  QV8Engine* pEngine = engine->handle();
  // Register all QObjects in "PiiYdin". This creates at least
  // PiiOperation, PiiOperationCompound, PiiEngine and PiiProbeInput
  // constructors.
  PiiQml::registerClasses(pEngine, globalObject, "PiiYdin");

  //PiiOperationWrapper::init(engine, globalObject);
  PiiOperationCompoundWrapper::init(pEngine, globalObject);
  PiiEngineWrapper::init(pEngine, globalObject);
}
