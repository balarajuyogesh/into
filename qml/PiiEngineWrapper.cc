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

#include "PiiEngineWrapper.h"
#include "PiiQml.h"

PiiPluginTypeProvider* PiiPluginTypeProvider::instance()
{
  static PiiPluginTypeProvider provider;
  return &provider;
}

namespace PiiEngineWrapper
{
  PII_STATIC_TR_FUNC(PiiEngine)

  static v8::Handle<v8::Value> loadPlugin(QV8Engine* engine,
                                          v8::Handle<v8::String> pluginName)
  {
    v8::Local<v8::Context> context(v8::Local<v8::Context>::New(engine->context()));
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObject(PiiQml::globalObject(context));
    try
      {
        PiiEngine::Plugin plugin = PiiEngine::loadPlugin(PiiQml::toString(pluginName));
        PiiQml::registerClasses(engine, globalObject, plugin.resourceName());
        return engine->fromVariant(QVariant::fromValue(plugin));
      }
    catch (PiiLoadException& ex)
      {
        PII_V8_THROW_ERROR(ex.message());
      }
  }
  
  static PII_DECLARE_V8_FUNCTION(loadPlugins)
  {
    PII_CHECK_AT_LEAST_ONE_ARGUMENT(loadPlugins);
    QV8Engine *pV8Engine = V8ENGINE();
    for (int i=0; i<args.Length(); ++i)
      if (!args[i]->IsString())
        PII_INVALID_ARGUMENT_TYPE(loadPlugins, i, "string");

    v8::Local<v8::Array> v8aPlugins(v8::Array::New(args.Length()));
    for (int i=0; i<args.Length(); ++i)
      {
        v8::Handle<v8::Value> v8Plugin(loadPlugin(pV8Engine, args[i]->ToString()));
        if (v8Plugin.IsEmpty())
          return v8Plugin;
        v8aPlugins->Set(i, v8Plugin);
      }
    return v8aPlugins;
  }

  static PII_DECLARE_V8_FUNCTION(loadPlugin)
  {
    PII_CHECK_ONE_ARGUMENT(loadPlugin);
    if (!args[0]->IsString())
      PII_INVALID_ARGUMENT_TYPE(loadPlugin, 0, "string");
    return loadPlugin(V8ENGINE(), args[0]->ToString());
  }

  static PII_DECLARE_V8_FUNCTION(execute)
  {
    PII_CHECK_THIS_TYPE(PiiEngine, execute);
    try
      {
        pThis->execute();
      }
    catch (PiiExecutionException& ex)
      {
        PII_V8_THROW_ERROR(ex.message());
      }
    return v8::Undefined();
  }

  static PII_DECLARE_V8_FUNCTION(save)
  {
    PII_CHECK_THIS_TYPE(PiiEngine, save);
    PII_CHECK_ARGUMENT_COUNT_RANGE(save, 1, 3);

    QV8Engine* pV8Engine = V8ENGINE();
    QVariantMap mapConfig;
    PiiEngine::FileFormat format = PiiEngine::TextFormat;
    if (args.Length() == 3)
      {
        if (args[2]->IsNumber())
          format = (PiiEngine::FileFormat)args[2]->Uint32Value();
        else
          PII_INVALID_ARGUMENT_TYPE(save, 2, "number");
      }
    if (args.Length() > 1)
      {
        if (args[1]->IsObject())
          mapConfig = pV8Engine->variantMapFromJS(v8::Local<v8::Object>::Cast(args[1]));
        else
          PII_INVALID_ARGUMENT_TYPE(save, 1, "object");
      }
    try
      {
        pThis->save(pV8Engine->toString(args[0]),
                    mapConfig,
                    format);
      }
    catch (PiiSerializationException& ex)
      {
        // FIXME: RTTI problem: at least GCC seems to determine the
        // type of ex incorrectly. This block is never reached.
        QString strMessage(ex.message());
        if (!ex.info().isEmpty())
          strMessage += " (" + ex.info() + ")";
        PII_V8_THROW_ERROR(strMessage);
      }
    catch (PiiException& ex)
      {
        PII_V8_THROW_ERROR(ex.message());
      }
    return v8::Undefined();
  }

  static PII_DECLARE_V8_FUNCTION(load)
  {
    PII_CHECK_ARGUMENT_COUNT_RANGE(load, 1, 2);
    QV8Engine* pV8Engine = V8ENGINE();
    QVariantMap mapConfig;
    try
      {
        PiiEngine* pEngine = PiiEngine::load(PiiQml::toString(args[0]),
                                             &mapConfig);
        if (args.Length() == 2)
          {
            if (args[1]->IsObject())
              PiiQml::configure(v8::Local<v8::Object>::Cast(args[1]), pV8Engine->variantMapToJS(mapConfig));
            else
              PII_INVALID_ARGUMENT_TYPE(load, 1, "object");
          }
        return pV8Engine->newQObject(pEngine);
      }
    catch (PiiSerializationException& ex)
      {
        QString strMessage(ex.message());
        if (!ex.info().isEmpty())
          strMessage += " (" + ex.info() + ")";
        PII_V8_THROW_ERROR(strMessage);
      }
    catch (PiiException& ex)
      {
        PII_V8_THROW_ERROR(ex.message());
      }
  }
  
  static PII_DECLARE_V8_FUNCTION(plugins)
  {
    PII_CHECK_NO_ARGUMENTS(plugins);
    QList<PiiEngine::Plugin> lstPlugins = PiiEngine::plugins();
    v8::Local<v8::Array> v8aResult(v8::Array::New(lstPlugins.size()));
    for (int i=0; i<lstPlugins.size(); ++i)
      v8aResult->Set(i, V8ENGINE()->fromVariant(QVariant::fromValue(lstPlugins[i])));
    return v8aResult;
  }
  
  static PII_DECLARE_V8_FUNCTION(pluginLibraryNames)
  {
    PII_CHECK_NO_ARGUMENTS(pluginLibraryNames);
    return V8ENGINE()->fromVariant(PiiEngine::pluginLibraryNames());
  }

  static PII_DECLARE_V8_FUNCTION(pluginResourceNames)
  {
    PII_CHECK_NO_ARGUMENTS(pluginResourceNames);
    return V8ENGINE()->fromVariant(PiiEngine::pluginResourceNames());
  }

  void init(QV8Engine* engine, v8::Handle<v8::Object> globalObject)
  {
    v8::Local<v8::Object> prototype(PII_PROTO_OF("PiiEngine"));
    prototype->Set(v8::String::New("execute"), PII_V8_FUNCTION(execute));
    prototype->Set(v8::String::New("save"), PII_V8_FUNCTION(save));
  
    v8::Local<v8::Function> constructor(PII_CONSTRUCTOR_OF("PiiEngine"));
    constructor->Set(v8::String::New("loadPlugins"), PII_V8_FUNCTION(loadPlugins));
    constructor->Set(v8::String::New("loadPlugin"), PII_V8_FUNCTION(loadPlugin));
    constructor->Set(v8::String::New("load"), PII_V8_FUNCTION(load));
    constructor->Set(v8::String::New("plugins"), PII_V8_FUNCTION(plugins));
    constructor->Set(v8::String::New("pluginLibraryNames"), PII_V8_FUNCTION(pluginLibraryNames));
    constructor->Set(v8::String::New("pluginResourceNames"), PII_V8_FUNCTION(pluginResourceNames));
  }
}
