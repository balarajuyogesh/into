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

#ifndef _PIIQML_H
#define _PIIQML_H

#include <private/qqmlglobal_p.h>

// Declares FUNCTION as a v8-callable function.
#define PII_DECLARE_V8_FUNCTION(FUNCTION) v8::Handle<v8::Value> FUNCTION(const v8::Arguments& args)

#define PII_V8_FUNCTION(FUNCTION) v8::FunctionTemplate::New(FUNCTION, v8::External::New(engine))->GetFunction()

// Defines a static tr() function that uses CLASS as the translation context.
#define PII_STATIC_TR_FUNC(CLASS) \
  static inline QString tr(const char* text)                \
  {                                                         \
    return QCoreApplication::translate(#CLASS, text);       \
  }

// Checks that the "this" object in current context is an instance of
// CLASS.
#define PII_CHECK_THIS_TYPE(CLASS, FUNCTION)                            \
  CLASS* pThis = qv8_cast<CLASS*>(args.This());                         \
  if (pThis == 0)                                                       \
    PII_V8_THROW_TYPE(tr(PiiQml::pInstanceOfXRequired).arg(#FUNCTION).arg(#CLASS))

// Ensures that the function was called without arguments.
#define PII_CHECK_NO_ARGUMENTS(FUNCTION)                                \
  if (args.Length() != 0)                                               \
    PII_V8_THROW_ERROR(tr(PiiQml::pTakesNoArguments).arg(#FUNCTION))

// Ensures that there is exactly one argument in the calling context.
#define PII_CHECK_ONE_ARGUMENT(FUNCTION)                                \
  if (args.Length() != 1)                                               \
    PII_V8_THROW_ERROR(tr(PiiQml::pTakesOneArgument).arg(#FUNCTION))

/// Ensures that there is at least one argument in the calling context.
#define PII_CHECK_AT_LEAST_ONE_ARGUMENT(FUNCTION)                       \
  if (args.Length() == 0)                                               \
    PII_V8_THROW_ERROR(tr(PiiQml::pTakesAtLeastOneArgument).arg(#FUNCTION))

/// Ensures that there is exactly one argument in the calling context.
#define PII_CHECK_ARGUMENT_COUNT(FUNCTION, ARGS)                        \
  if (args.Length() != ARGS)                                            \
    PII_V8_THROW_ERROR(tr(PiiQml::pTakesNArguments).arg(#FUNCTION).arg(ARGS))

/// Ensures that the argument count is in [MIN,MAX].
#define PII_CHECK_ARGUMENT_COUNT_RANGE(FUNCTION, MIN, MAX)              \
  if (args.Length() < MIN || args.Length() > MAX)                       \
    PII_V8_THROW_ERROR(tr(PiiQml::pTakesNArguments).arg(#FUNCTION).arg(#MIN "-" #MAX))

/* Returns an error object that says that the argument INDEX+1 of
 * FUNCTION() must be CLASS_NAME.
 */
#define PII_INVALID_ARGUMENT_TYPE(FUNCTION, INDEX, CLASS_NAME)          \
  PII_V8_THROW_TYPE(tr(PiiQml::pArgumentNMustBeX).arg(#FUNCTION).arg(INDEX+1).arg(CLASS_NAME))

/* Tries to cast the INDEXth argument of current context to CLASS.
 * Stores the return value to VARIABLE, and returns an error object on
 * failure.
 */
#define PII_CHECK_ARGUMENT_TYPE(FUNCTION, INDEX, VARIABLE, CLASS)       \
  VARIABLE = qv8_cast<CLASS*>(args[INDEX]);                             \
  if (VARIABLE == 0)                                                    \
    PII_INVALID_ARGUMENT_TYPE(FUNCTION, INDEX, #CLASS)

#define PII_V8_THROW_ERROR(STR) \
  { v8::ThrowException(v8::Exception::Error(::PiiQml::toString(STR))); \
    return v8::Handle<v8::Value>(); }

#define PII_V8_THROW_TYPE(STR) \
  { v8::ThrowException(v8::Exception::TypeError(::PiiQml::toString(STR))); \
    return v8::Handle<v8::Value>(); }

#define PII_CONSTRUCTOR_OF(CONSTRUCTOR_NAME) \
  v8::Local<v8::Function>::Cast(globalObject->Get(v8::String::New(CONSTRUCTOR_NAME)))

#define PII_PROTO_OF(CONSTRUCTOR_NAME) \
  v8::Local<v8::Object>::Cast(PII_CONSTRUCTOR_OF(CONSTRUCTOR_NAME)->Get(v8::String::New("prototype")))

// Converts the obj to a pointer to a QObject-derived type
// (T*). Returns 0 if obj cannot be converted to T*.
template <class T> inline T qv8_cast(v8::Handle<v8::Value> obj)
{
  if (!obj->IsObject())
    return 0;
  QV8QObjectResource* r = v8_resource_cast<QV8QObjectResource>(v8::Handle<v8::Object>::Cast(obj));
  return r ? qobject_cast<T>(r->object) : 0;
}

namespace PiiQml
{
  extern const char
    *pInstanceOfXRequired,
    *pTakesNoArguments,
    *pTakesOneArgument,
    *pTakesAtLeastOneArgument,
    *pTakesNArguments,
    *pArgumentNMustBeX;

  inline v8::Local<v8::String> toString(const QString& str)
  {
    return str.isEmpty() ?
      v8::String::Empty() :
      v8::String::New(reinterpret_cast<const uint16_t*>(str.data()), str.size());
  }

  inline v8::Local<v8::String> toString(const char* str)
  {
    return v8::String::New(str);
  }

  inline QString toString(v8::Handle<v8::Value> str)
  {
    return QV8Engine::toStringStatic(str);
  }

  inline QString toString(v8::Handle<v8::String> str)
  {
    return QV8Engine::toStringStatic(str);
  }

  void configure(v8::Handle<v8::Object> obj, v8::Handle<v8::Value> configMap);
  void registerClasses(QV8Engine* engine, v8::Handle<v8::Object> globalObject, const QString& parent);
  v8::Local<v8::Object> globalObject();
  v8::Local<v8::Object> globalIntoObject();
}

#endif //_PIIQML_H
