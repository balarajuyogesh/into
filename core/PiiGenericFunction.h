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

#ifndef _PIIGENERICFUNCTION_H
#define _PIIGENERICFUNCTION_H

#include "PiiPreprocessor.h"
#include "PiiGlobal.h"
#include "PiiTypeTraits.h"
#include "PiiMetaTemplate.h"
#include <QList>
#include <QVariantList>
#include <QMetaType>

/**
 * A class that can wrap any member function in a generic manner.
 */
class PII_CORE_EXPORT PiiGenericFunction
{
public:
  PiiGenericFunction();
  virtual ~PiiGenericFunction();

  /**
   * Invokes the function with the given @a params. The fist entry in
   * @a params points to a space allocated for the function's return
   * value. The rest point to function parameters. Subclasses must
   * know the parameter types and cast the pointers to the correct
   * types.
   */
  virtual void call(void** params) = 0;

  /**
   * Calls the function with the given parameters. This function
   * converts @a params to the types listed in #paramTypes(), calls
   * the function and returns its result. If the function has no
   * return value, returns an invalid QVariant.
   *
   * @exception PiiInvalidArgumentException& if the number of
   * parameters is wrong or any of them cannot be converted to the
   * correct type.
   */
  QVariant call(QVariantList& params);

  /**
   * Returns the number of elements in @a params whose type exactly
   * matches the expected types, or -1 if @p params cannot be
   * converted to the correct types at all. See Pii::scoreOverload().
   */
  int scoreOverload(const QVariantList& params);

  /**
   * Returns the function's signature. The signature consists of a
   * return type (if the function has one), function @a name, and a
   * comma-separated list of parameter types in parentheses.
   */
  QString signature(const QString& name) const;

  /**
   * Returns the return type (QMetaType id) of the function.
   */
  int returnType() const;
  /**
   * Returns the types (QMetaType ids) of the function's parameters.
   */
  QList<int> paramTypes() const;

protected:
  /// @internal
  class Data
  {
  public:
    Data();
    virtual ~Data();
    int iReturnType;
    QList<int> lstParamTypes;
    mutable QString strSignature;
  } *d;
  /// @internal
  PiiGenericFunction(Data*);
};

/// @cond null
namespace Pii
{
  template <class T> struct ToPlainMetaType { typedef T Type; };
  template <class T> struct ToPlainMetaType<const T&> { typedef T Type; };
  template <class T> struct ToPlainMetaType<T&> { typedef T Type; };
  template <class T> struct ToPlainMetaType<const T*> { typedef T* Type; };
  template <class T> inline int metaTypeId() { return qMetaTypeId<typename ToPlainMetaType<T>::Type>(); }
  template <> inline int metaTypeId<void>() { return QMetaType::Void; }
}

#define PII_MAP_METATYPE(FROM,TO)                                       \
  namespace Pii {                                                       \
    template <> inline int metaTypeId<FROM >() { return qMetaTypeId<TO >(); } \
    template <> struct ToPlainMetaType<FROM> { typedef TO Type; };      \
  }

template <class Object, class Function, class R> class PiiGenericFunctionBase : public PiiGenericFunction
{
public:
  PiiGenericFunctionBase(Object* o, Function f) : _pObject(o), _pFunction(f)
  {
    this->d->iReturnType = Pii::metaTypeId<R>();
  }

protected:
  Object* _pObject;
  Function _pFunction;
};

#define PII_GENF_TPL_PARAM(N, PARAM) , class PARAM
#define PII_GENF_TPL_IMPL(N, PARAM) , PARAM
#define PII_GENF_FUNC_PARAM(N, PARAM) PARAM
#define PII_GENF_FUNC_PARAM_CAST(N, PARAM) *(typename Pii::ToPlainMetaType<PARAM>::Type*)(args[1+N])
#define PII_GENF_PARAM_TYPE(N, PARAM) << Pii::metaTypeId<PARAM >()

#define PII_CREATE_GENERIC_FUNCTION(N, PARAMS)                          \
  template <class Object, bool isConst, class R PII_FOR_N(PII_GENF_TPL_PARAM, N, PARAMS) > class PiiGenericFunction##N : \
  public Pii::If<isConst,                                               \
                 PiiGenericFunctionBase<Object,R(Object::*)(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM, PII_COMMA_SEP, N, PARAMS))const,R>, \
                 PiiGenericFunctionBase<Object,R(Object::*)(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM, PII_COMMA_SEP, N, PARAMS)),R> >::Type \
  {                                                                     \
  public:                                                               \
    typedef typename Pii::If<isConst,                                   \
      R(Object::*)(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM, PII_COMMA_SEP, N, PARAMS))const, \
      R(Object::*)(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM, PII_COMMA_SEP, N, PARAMS)) >::Type FuncType; \
    PiiGenericFunction##N(Object* o, FuncType f) :                      \
      PiiGenericFunctionBase<Object,FuncType,R>(o, f)                   \
    {                                                                   \
      this->d->lstParamTypes = QList<int>() PII_FOR_N(PII_GENF_PARAM_TYPE, N, PARAMS); \
    }                                                                   \
    struct Void { static void call(Object* o, FuncType f, void** args)   \
    {                                                                   \
      Q_UNUSED(args);                                                   \
      (o->*(f))(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM_CAST, PII_COMMA_SEP, N, PARAMS)); \
    }};                                                                 \
    struct NonVoid { static void call(Object* o, FuncType f, void** args) \
    {                                                                   \
      if (args[0] != 0)                                                 \
        *(R*)(args[0]) = (o->*(f))(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM_CAST, PII_COMMA_SEP, N, PARAMS)); \
      else                                                              \
        (o->*(f))(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM_CAST, PII_COMMA_SEP, N, PARAMS)); \
    }};                                                                 \
    void call(void** args)                                             \
    {                                                                   \
      Pii::IfClass<Pii::IsVoid<R>, Void, NonVoid>::Type::call(this->_pObject, this->_pFunction, args); \
    }                                                                   \
  };                                                                    \
  namespace Pii                                                         \
  {                                                                     \
    template <class Object, class R PII_FOR_N(PII_GENF_TPL_PARAM, N, PARAMS) > \
    PiiGenericFunction* genericFunction(Object* obj, R(Object::*func)(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM, PII_COMMA_SEP, N, PARAMS))) \
    {                                                                   \
      return new PiiGenericFunction##N<Object, false, R PII_FOR_N(PII_GENF_TPL_IMPL, N, PARAMS) >(obj, func); \
    }                                                                   \
    template <class Object, class R PII_FOR_N(PII_GENF_TPL_PARAM, N, PARAMS) > \
    PiiGenericFunction* genericFunction(Object* obj, R(Object::*func)(PII_FOR_N_SEP(PII_GENF_FUNC_PARAM, PII_COMMA_SEP, N, PARAMS)) const) \
    {                                                                   \
      return new PiiGenericFunction##N<Object, true, R PII_FOR_N(PII_GENF_TPL_IMPL, N, PARAMS) >(obj, func); \
    }                                                                   \
  }

PII_CREATE_GENERIC_FUNCTION(0, ());
PII_CREATE_GENERIC_FUNCTION(1, (P1));
PII_CREATE_GENERIC_FUNCTION(2, (P1,P2));
PII_CREATE_GENERIC_FUNCTION(3, (P1,P2,P3));
PII_CREATE_GENERIC_FUNCTION(4, (P1,P2,P3,P4));
PII_CREATE_GENERIC_FUNCTION(5, (P1,P2,P3,P4,P5));
PII_CREATE_GENERIC_FUNCTION(6, (P1,P2,P3,P4,P5,P6));
PII_CREATE_GENERIC_FUNCTION(7, (P1,P2,P3,P4,P5,P6,P7));
PII_CREATE_GENERIC_FUNCTION(8, (P1,P2,P3,P4,P5,P6,P7,P8));

/// @endcond

#endif //_PIIGENERICFUNCTION_H
