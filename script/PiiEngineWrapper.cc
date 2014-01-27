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

#include "PiiOperationCompoundWrapper.h"
#include "PiiEngineWrapper.h"

#include <QScriptContextInfo>

#include <PiiEngine.h>
#include "PiiScript.h"

#include <QDebug>
#include <PiiYdinResources.h>

namespace PiiEngineWrapper
{
  PII_STATIC_TR_FUNC(PiiEngine)

  PII_QOBJECT_CONVERSION_FUNCTIONS(PiiEngine);
  PII_QOBJECT_CONSTRUCTOR(PiiEngine);

  static QScriptValue convertPiiPluginToScriptValue(QScriptEngine* engine, const PiiEngine::Plugin& plugin)
  {
    QScriptValue result = engine->newObject();
    result.setProperty("resourceName", plugin.resourceName());
    result.setProperty("libraryName", plugin.libraryName());
    result.setProperty("version", plugin.version().toString());
    return result;
  }

  static void registerClasses(QScriptEngine* engine, const QString& parentResource = "")
  {
    // HACK QML has a read-only global object
    QScriptValue globalObject = engine->globalObject();
    QScriptClass* pOldClass = globalObject.scriptClass();
    if (pOldClass != 0)
      globalObject.setScriptClass(0);

    PiiScript::registerClasses(engine, "PiiOperation", parentResource);
    PiiScript::registerClasses(engine, "PiiOperationCompound", parentResource);
    PiiScript::registerClasses(engine, "PiiEngine", parentResource);
    PiiScript::registerClasses(engine, "QWidget", parentResource);
    // PENDING other QObject derived classes/deeper inheritance hierarchies?

    if (pOldClass != 0)
      globalObject.setScriptClass(pOldClass);
  }

  static QScriptValue loadPlugins(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_AT_LEAST_ONE_ARGUMENT(loadPlugins);
    for (int i=0; i<context->argumentCount(); ++i)
      if (!context->argument(i).isString())
        return context->throwError(QScriptContext::TypeError,
                                   tr("loadPlugins(): argument %1 is not a string.").arg(i+1));
    try
      {
        QScriptValue lstPlugins = engine->newArray();
        for (int i=0; i<context->argumentCount(); ++i)
          {
            PiiEngine::Plugin plugin = PiiEngine::loadPlugin(context->argument(i).toString());
            registerClasses(engine, plugin.resourceName());
            lstPlugins.setProperty(i, convertPiiPluginToScriptValue(engine, plugin));
          }
        return lstPlugins;
      }
    catch (PiiLoadException& ex)
      {
        return context->throwError(QScriptContext::UnknownError, ex.message());
      }
  }

  static QScriptValue loadPlugin(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_ONE_ARGUMENT(loadPlugin);
    QScriptValue plugins = loadPlugins(context, engine);
    if (plugins.isError())
      return plugins;
    return plugins.property(0);
  }

  static QScriptValue execute(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_THIS_TYPE(PiiEngine, execute);
    try
      {
        pThis->execute();
      }
    catch (PiiExecutionException& ex)
      {
        return context->throwError(ex.message());
      }
    return engine->undefinedValue();
  }

  static QScriptValue save(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_THIS_TYPE(PiiEngine, save);
    PII_CHECK_ARGUMENT_COUNT_RANGE(save, 1, 3);

    QVariantMap mapConfig;
    PiiEngine::FileFormat format = PiiEngine::TextFormat;
    if (context->argumentCount() == 3)
      format = (PiiEngine::FileFormat)context->argument(2).toInt32();
    if (context->argumentCount() > 1)
      mapConfig = PiiScript::objectToMap(context->argument(1));
    try
      {
        pThis->save(context->argument(0).toString(),
                    mapConfig,
                    format);
      }
    catch (PiiSerializationException& ex)
      {
        // PENDING At least GCC seems to determine the type of ex
        // incorrectly. This block is never reached.
        QString strMessage(ex.message());
        if (!ex.info().isEmpty())
          strMessage += "(" + ex.info() + ")";
        return context->throwError(strMessage);
      }
    catch (PiiException& ex)
      {
        return context->throwError(ex.message());
      }
    return engine->undefinedValue();
  }

  static QScriptValue load(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_ARGUMENT_COUNT_RANGE(load, 1, 2);

    QVariantMap mapConfig;
    try
      {
        PiiEngine* pEngine = PiiEngine::load(context->argument(0).toString(),
                                             &mapConfig);
        if (context->argumentCount() == 2)
          PiiScript::configure(context->argument(1), mapConfig);
        return engine->newQObject(pEngine,
                                  QScriptEngine::AutoOwnership,
                                  PiiScript::defaultWrapOptions);
      }
    catch (PiiSerializationException& ex)
      {
        QString strMessage(ex.message());
        if (!ex.info().isEmpty())
          strMessage += "(" + ex.info() + ")";
        return context->throwError(strMessage);
      }
    catch (PiiException& ex)
      {
        return context->throwError(ex.message());
      }
  }

  static QScriptValue plugins(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_NO_ARGUMENTS(plugins);
    QList<PiiEngine::Plugin> lstPlugins = PiiEngine::plugins();
    QScriptValue lstResult = engine->newArray();
    for (int i=0; i<lstPlugins.size(); ++i)
      lstResult.setProperty(i, convertPiiPluginToScriptValue(engine, lstPlugins[i]));
    return lstResult;
  }

  static QScriptValue pluginLibraryNames(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_NO_ARGUMENTS(pluginLibraryNames);
    return qScriptValueFromSequence(engine, PiiEngine::pluginLibraryNames());
  }

  static QScriptValue pluginResourceNames(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_NO_ARGUMENTS(pluginResourceNames);
    return qScriptValueFromSequence(engine, PiiEngine::pluginResourceNames());
  }
};

void initPiiEngine(QScriptEngine* engine)
{
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiEngineWrapper, PiiEngine);

  QScriptValue prototype = engine->newObject();
  PiiScript::setSuperclass(prototype, "PiiOperationCompound");
  prototype.setProperty("execute", engine->newFunction(PiiEngineWrapper::execute));
  prototype.setProperty("save", engine->newFunction(PiiEngineWrapper::save));

  QScriptValue constructor = engine->newFunction(PiiEngineWrapper::createPiiEngine, prototype);
  PiiScript::configure(constructor, engine->newQMetaObject(&PiiEngine::staticMetaObject));
  constructor.setProperty("loadPlugins", engine->newFunction(PiiEngineWrapper::loadPlugins));
  constructor.setProperty("loadPlugin", engine->newFunction(PiiEngineWrapper::loadPlugin));
  constructor.setProperty("load", engine->newFunction(PiiEngineWrapper::load));
  constructor.setProperty("plugins", engine->newFunction(PiiEngineWrapper::plugins));
  constructor.setProperty("pluginLibraryNames", engine->newFunction(PiiEngineWrapper::pluginLibraryNames));
  constructor.setProperty("pluginResourceNames", engine->newFunction(PiiEngineWrapper::pluginResourceNames));

  engine->setDefaultPrototype(qMetaTypeId<PiiEngine*>(), prototype);
  engine->globalObject().setProperty("PiiEngine", constructor);

  PiiEngineWrapper::registerClasses(engine);
}
