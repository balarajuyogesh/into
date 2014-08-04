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

#include "PiiQml.h"

#include <PiiYdinResources.h>

namespace PiiQml
{
  const char
    *pInstanceOfXRequired = QT_TR_NOOP("%1() can be applied to instances of %2 only."),
    *pTakesNoArguments = QT_TR_NOOP("%1() takes no arguments."),
    *pTakesOneArgument = QT_TR_NOOP("%1() takes exactly one argument."),
    *pTakesAtLeastOneArgument = QT_TR_NOOP("%1() takes at least one argument."),
    *pTakesNArguments = QT_TR_NOOP("%1() takes %2 arguments."),
    *pArgumentNMustBeX = QT_TR_NOOP("%1(): argument %2 must be an instance of %3.");

  void configure(v8::Handle<v8::Object> obj, v8::Handle<v8::Value> configMap)
  {
    if (!configMap->IsObject()) return;
    v8::Local<v8::Object> v8ConfigObj(v8::Object::Cast(*configMap));
    v8::Local<v8::Array> v8PropNames(v8ConfigObj->GetOwnPropertyNames());
    for (unsigned i=0; i<v8PropNames->Length(); ++i)
      obj->Set(v8PropNames->Get(i),
               v8ConfigObj->Get(v8PropNames->Get(i)));
  }

  static PII_DECLARE_V8_FUNCTION(createQObject)
  {
    if (!args.IsConstructCall())
      PII_V8_THROW_ERROR("Constructor called as function");

    QV8Engine* pV8Engine = V8ENGINE();
    v8::Local<v8::Function> constructor(args.Callee());
    // The class name is usually the same as the constructor's name, but not always.
    // Template classes (e.g. PiiSom<double>) can be different (e.g. PiiSom.double).
    QString strName(pV8Engine->toString(constructor->GetHiddenValue(v8::String::New("pii::className"))));
    QObject* pObject = PiiYdin::createResource<QObject>(qPrintable(strName));
    if (pObject == 0)
      PII_V8_THROW_ERROR(QString("Cannot create an instance of %1.").arg(strName));

    v8::Handle<v8::Object> v8Obj(v8::Handle<v8::Object>::Cast(pV8Engine->newQObject(pObject)));
    int iArgs = args.Length();
    int i = 0;
    // If the first argument is a string, it is regarded as objectName.
    if (iArgs > 0 && args[0]->IsString())
      {
        pObject->setObjectName(pV8Engine->toString(args[i]));
        ++i;
      }
    for (; i<iArgs; ++i)
      configure(v8Obj, args[i]);

    // Function objects created without an InstanceTemplate don't seem
    // to automatically apply the prototype to created objects.
    v8Obj->SetPrototype(constructor->Get(v8::String::New("prototype")));

    return v8Obj;
  }

  v8::Local<v8::Value> createQObjectConstructor(QV8Engine* engine,
                                                v8::Handle<v8::Object> globalObject,
                                                const QString& className,
                                                const QMetaObject* meta,
                                                v8::Handle<v8::Value> superProto)
  {
    // If there is already a property with the same name, do nothing.
    v8::Local<v8::String> v8strClassName(engine->toString(className));
    if (!globalObject->Get(v8strClassName)->IsUndefined())
      return v8::Local<v8::Value>();

    v8::Local<v8::FunctionTemplate> tmpl(v8::FunctionTemplate::New(createQObject, v8::External::New(engine)));
    tmpl->SetClassName(v8strClassName);

    // Set enum values as properties to the function object
    for (int i=meta->enumeratorOffset(); i<meta->enumeratorCount(); ++i)
      {
        QMetaEnum enumerator = meta->enumerator(i);
        for (int j=0; j < enumerator.keyCount(); ++j)
          tmpl->Set(v8::String::New(enumerator.key(j)), v8::Integer::New(enumerator.value(j)));
      }

    v8::Local<v8::Function> constructor(tmpl->GetFunction());
    if (!superProto.IsEmpty())
      {
        v8::Local<v8::Object> prototype(v8::Object::New());
        prototype->Set(v8::String::New("__proto__"), superProto);
        constructor->Set(v8::String::New("prototype"), prototype);
      }
    constructor->SetHiddenValue(v8::String::New("pii::className"), v8strClassName);

    int iTemplateIndex = className.indexOf('<');
    if (iTemplateIndex != -1)
      {
        // Take "Class" from "Class<T>"
        v8::Local<v8::String> v8strClassBaseName(engine->toString(className.left(iTemplateIndex)));

        if (globalObject->Get(v8strClassBaseName)->IsUndefined())
          globalObject->Set(v8strClassName, v8::Object::New());

        // Make the "Class<T>" property of the global object also available as "Class.T".
        if (globalObject->Get(v8strClassBaseName)->IsObject())
          {
            v8::Local<v8::Object>::Cast(globalObject->Get(v8strClassBaseName))->
              Set(PiiQml::toString(className.mid(iTemplateIndex+1, className.size() - iTemplateIndex - 2)),
                  constructor);
          }
      }

    globalObject->Set(v8strClassName, constructor);

    return constructor;
  }

  static bool isOperation(const QMetaObject* meta)
  {
    if (!meta)
      return false;
    if (!std::strcmp(meta->className(), "PiiOperation"))
      return true;
    return isOperation(meta->superClass());
  }

  // TODO: this doesn't work with template classes derived from
  // template classes due to the lack of template support in moc.
  static v8::Local<v8::Value> findClassPrototype(v8::Handle<v8::Object> globalObject, const QMetaObject* meta)
  {
    if (!meta)
      return v8::Local<v8::Value>();

    v8::Local<v8::Value> constructor(globalObject->Get(v8::String::New(meta->className())));
    if (constructor->IsObject())
      return v8::Local<v8::Object>::Cast(constructor)->Get(v8::String::New("prototype"));

    return findClassPrototype(globalObject, meta->superClass());
  }

  void registerClasses(QV8Engine* engine, v8::Handle<v8::Object> globalObject, const QString& parent)
  {
    PiiResourceDatabase* pDb = PiiYdin::resourceDatabase();
    //pDb->dump();
    // Find all metaobjects in the specified plug-in (or other
    // group). This selects all statements of the form (class,
    // "pii:qmetaobject", ptr), where "class" is the name of a C++
    // class and ptr a pointer to its static QMetaObject.
    QList<PiiResourceStatement> lstMetaObjects =
      !parent.isEmpty() ?
      pDb->select(Pii::subject == pDb->select(Pii::subject, Pii::attribute(PiiYdin::parentPredicate) == parent) &&
                  Pii::predicate == PiiYdin::metaObjectPredicate) :
      pDb->select(Pii::predicate == PiiYdin::metaObjectPredicate);

    // Create a constructor function for each class
    for (int i=0; i<lstMetaObjects.size(); ++i)
      {
        const QMetaObject* pMeta = reinterpret_cast<QMetaObject*>(lstMetaObjects[i].objectPtr());
        createQObjectConstructor(engine,
                                 globalObject,
                                 lstMetaObjects[i].subject(),
                                 pMeta,
                                 findClassPrototype(globalObject, pMeta));
      }
  }

  v8::Local<v8::Object> globalObject()
  {
    // HACK: Global object is frozen, but we can modify its __proto__
    return v8::Local<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->Get(v8::String::New("__proto__")));
  }

  v8::Local<v8::Object> globalIntoObject()
  {
    return v8::Local<v8::Object>::Cast(globalObject()->Get(v8::String::New("Into")));
  }
}
