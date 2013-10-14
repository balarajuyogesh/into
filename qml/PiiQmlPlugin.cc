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
#include "PiiSocketWrapper.h"
#include <PiiProbeInput.h>

class PiiValueTypeProvider : public QQmlValueTypeProvider
{
public:
  bool create(int type, QQmlValueType *&v)
  {
    if (type == qMetaTypeId<PiiVariant>())
      v = new PiiVariantValueType;
    else if (type == qMetaTypeId<PiiAbstractInputSocket*>())
      v = new PiiInputSocketPtrValueType;
    else if (type == qMetaTypeId<PiiAbstractOutputSocket*>())
      v = new PiiOutputSocketPtrValueType;
    else if (type == qMetaTypeId<PiiEngine::Plugin>())
      v = new PiiPluginValueType;
    else
      return false;
    return true;
  }

  static PiiValueTypeProvider* instance()
  {
    static PiiValueTypeProvider provider;
    return &provider;
  }
};

void PiiQmlPlugin::registerTypes(const char *uri)
{
  Q_ASSERT(uri == QLatin1String("Into"));
  qmlRegisterUncreatableType<PiiOperation>(uri, 2, 0, "PiiOperation", "PiiOperation is an abstract superclass that cannot be instantiated.");
  qmlRegisterType<PiiOperationCompound>(uri, 2, 0, "PiiEngine");
  qmlRegisterType<PiiEngine>(uri, 2, 0, "PiiEngine");
  qmlRegisterType<PiiProbeInput>(uri, 2, 0, "PiiProbeInput");

  QQml_addValueTypeProvider(PiiValueTypeProvider::instance());
}

void PiiQmlPlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
  Q_ASSERT(uri == QLatin1String("Into"));
  v8::HandleScope handleScope;
  v8::Local<v8::Context> context(v8::Local<v8::Context>::New(engine->handle()->context()));
  v8::Context::Scope contextScope(context);
  v8::Local<v8::Object> globalObject(PiiQml::globalObject());

  v8::Local<v8::Object> into(v8::Object::New());
  globalObject->Set(v8::String::New(uri), into);

  QV8Engine* pEngine = engine->handle();

  // Register all QObjects in "PiiYdin". This creates constructors for
  // base types such as PiiOperation, PiiOperationCompound, PiiEngine,
  // PiiSocket and PiiProbeInput.
  PiiQml::registerClasses(pEngine, into, "PiiYdin");

  //PiiOperationWrapper::init(engine, globalObject);
  PiiOperationCompoundWrapper::init(pEngine, into);
  PiiEngineWrapper::init(pEngine, into);
}
