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

#include "PiiScript.h"

#include <PiiYdinResources.h>
#include <QScriptValueIterator>

namespace PiiScript
{
  const char
    *pInstanceOfXRequired = QT_TR_NOOP("%1() can be applied to instances of %2 only."),
    *pTakesNoArguments = QT_TR_NOOP("%1() takes no arguments."),
    *pTakesOneArgument = QT_TR_NOOP("%1() takes exactly one argument."),
    *pTakesAtLeastOneArgument = QT_TR_NOOP("%1() takes at least one argument."),
    *pTakesNArguments = QT_TR_NOOP("%1() takes %2 arguments."),
    *pArgumentNMustBeX = QT_TR_NOOP("%1(): argument %2 must be an instance of %3.");

  const QScriptEngine::QObjectWrapOptions defaultWrapOptions =
    QScriptEngine::ExcludeDeleteLater |
    QScriptEngine::AutoCreateDynamicProperties;

  PII_STATIC_TR_FUNC(PiiScript);

  void setSuperclass(QScriptValue prototype, const char* superclass)
  {
    prototype.setPrototype(prototype.engine()->globalObject().property(superclass).property("prototype"));
  }

  void configure(QScriptValue target, QScriptValue source)
  {
    QScriptValueIterator it(source);
    while (it.hasNext())
      {
        it.next();
        target.setProperty(it.name(), it.value());
      }
  }

  QScriptValue configure(QScriptValue object, const QVariantMap& map)
  {
    for (QVariantMap::const_iterator i = map.begin(); i != map.end(); ++i)
      object.setProperty(i.key(), object.engine()->newVariant(i.value()));
    return object;
  }

  QVariantMap objectToMap(QScriptValue object)
  {
    QVariantMap mapResult;
    QScriptValueIterator it(object);
    while (it.hasNext())
      {
        it.next();
        mapResult[it.name()] = it.value().toVariant();
      }
    return mapResult;
  }

  QScriptValue mapToObject(QScriptEngine* engine, const QVariantMap& map)
  {
    QScriptValue object(engine->newObject());
    return configure(object, map);
  }

  static QScriptValue createQObject(QScriptContext* context, QScriptEngine* engine)
  {
    QScriptValue constructor = context->callee();
    QString strName = constructor.property("className").toString();
    QObject* pObject = PiiYdin::createResource<QObject>(qPrintable(strName));
    if (pObject == 0)
      return context->throwError(tr("Could not create an instance of %1.").arg(strName));

    QString strParentClass = constructor.property("parentClass").toString();
    if (!strParentClass.isEmpty() && !Pii::isA(qPrintable(strParentClass), pObject))
      {
        delete pObject;
        return context->throwError(tr("Cannot convert %1 to %2.")
                                   .arg(pObject->metaObject()->className())
                                   .arg(strParentClass));
      }

    engine->newQObject(context->thisObject(), pObject, QScriptEngine::AutoOwnership, PiiScript::defaultWrapOptions);
    int iArgs = context->argumentCount();
    int i = 0;
    // If the first argument is a string, it is regarded as objectName.
    if (iArgs > 0 && context->argument(0).isString())
      {
        pObject->setObjectName(context->argument(0).toString());
        ++i;
      }
    for (; i<iArgs; ++i)
      PiiScript::configure(context->thisObject(), context->argument(i));

    // When the constructor is first invoked, use the operation
    // instance to get meta information and add it to the
    // constructor object.
    if (!constructor.property("__meta__").isValid())
      {
        PiiScript::configure(constructor, engine->newQMetaObject(pObject->metaObject()));
        constructor.setProperty("__meta__", true);
      }
    return engine->undefinedValue();
  }

  QScriptValue createQObjectConstructor(QScriptEngine* engine, const QString& name, const char* superClass)
  {
    QScriptValue constructor = engine->newFunction(createQObject);
    PiiScript::setSuperclass(constructor.property("prototype"), superClass);
    constructor.setProperty("className", name);
    return constructor;
  }

  void registerClasses(QScriptEngine* engine, const char* superClass, const QString& parentResource)
  {
    PiiResourceDatabase* pDb = PiiYdin::resourceDatabase();
    // Find all classes ...
    QList<QString> lstClasses = pDb->select(Pii::subject, Pii::attribute("pii:class") == superClass);

    // ... in the specified plug-in (if any)
    if (!parentResource.isEmpty())
      lstClasses = lstClasses && pDb->select(Pii::subject, Pii::attribute("pii:parent") == parentResource);

    QScriptValue globalObject = engine->globalObject();
    // Create a constructor function for each
    for (int i=0; i<lstClasses.size(); ++i)
      {
        int iTemplateIndex = lstClasses[i].indexOf('<');
        if (iTemplateIndex == -1)
          {
            // No template parameters
            if (!globalObject.property(lstClasses[i]).isObject())
              globalObject.setProperty(lstClasses[i],
                                       createQObjectConstructor(engine, lstClasses[i], superClass));
          }
        else
          {
            // Split Class<T> to Class and T
            QString strName = lstClasses[i].left(iTemplateIndex);
            QString strTemplateParam = lstClasses[i].mid(iTemplateIndex+1,
                                                         lstClasses[i].size() - iTemplateIndex - 2);
            QScriptValue tmpObj = globalObject.property(strName);
            if (!tmpObj.isObject())
              {
                tmpObj = engine->newObject();
                globalObject.setProperty(strName, tmpObj);
              }
            // Now, Class<T> can be created with "new Class.T()"
            tmpObj.setProperty(strTemplateParam,
                               createQObjectConstructor(engine, lstClasses[i], superClass));
          }
      }
  }
}
