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

#ifndef _PIIGLOBAL_H
#define _PIIGLOBAL_H

/// @hide

#define INTO_VERSION_STR "2.0.0-beta-3"
#define INTO_VERSION 0x02000003

#include <QtGlobal>

#if defined(_WIN32)
#  define PII_DECL_EXPORT __declspec(dllexport)
#  define PII_DECL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#  define PII_DECL_EXPORT __attribute__ ((visibility("default")))
#  define PII_DECL_IMPORT __attribute__ ((visibility("default")))
#else
#  define PII_DECL_EXPORT
#  define PII_DECL_IMPORT
#endif

#ifdef PII_BUILD_CORE
#  define PII_CORE_EXPORT PII_DECL_EXPORT
#  define PII_BUILDING_CORE 1
#else
#  define PII_CORE_EXPORT PII_DECL_IMPORT
#  define PII_BUILDING_CORE 0
#endif

#if defined(__GNUC__) && !defined(__INSURE__)
#  define PII_PRINTF_ATTR(first,rest)  __attribute__ ((format (printf, first, rest)))
#else
#  define PII_PRINTF_ATTR(first,rest)
#endif

#define PII_D_FUNC \
  inline Data* _d() { return static_cast<Data*>(this->d); } \
  inline const Data* _d() const { return static_cast<const Data*>(this->d); } \
  friend class Data

// reinterpret_cast must be used instead of (the safer) static_cast if
// the Data class is defined outside of the containing class. In this
// case the compiler does not know if static_cast can be performed.
#define PII_UNSAFE_D_FUNC \
  inline Data* _d() { return reinterpret_cast<Data*>(this->d); } \
  inline const Data* _d() const { return reinterpret_cast<const Data*>(this->d); } \
  friend class Data

#define PII_Q_FUNC(CLASS) \
  inline CLASS* owner() { return static_cast<CLASS*>(this->q); } \
  inline const CLASS* owner() const { return static_cast<const CLASS *>(this->q); } \
  friend class CLASS

#define PII_UNSAFE_Q_FUNC(CLASS) \
  inline CLASS* owner() { return reinterpret_cast<CLASS*>(this->q); } \
  inline const CLASS* owner() const { return reinterpret_cast<const CLASS *>(this->q); } \
  friend class CLASS

#define PII_D Data* const d = _d()

#define PII_Q(CLASS) CLASS* const q = owner()

#define PII_DISABLE_COPY(CLASS) private: CLASS(const CLASS&); CLASS& operator= (const CLASS& other)

#ifdef PII_CXX11
#  define PII_MOVE std::move
#  define PII_CONSTEXPR constexpr
#else
#  define PII_MOVE
#  define PII_CONSTEXPR
#  define static_assert(EXPR,MSG) typedef typename ::Pii::StaticAssert<(EXPR)>::Type _PiiStaticAssert_
#endif

#define PII_TYPEMAP(NAME) namespace NAME
#define PII_MAP_PUT_DEFAULT(TYPE) template <class _T_> struct Mapper { typedef TYPE Type; }
#define PII_MAP_PUT_NO_DEFAULT template <class _T_> struct Mapper
#define PII_MAP_PUT_SELF_DEFAULT PII_MAP_PUT_DEFAULT(_T_)
#define PII_MAP_PUT_TEMPLATE(FROM, TO) template <class _T_> struct Mapper<FROM<_T_> > { typedef TO Type; }
#define PII_MAP_PUT(FROM, TO) template <> struct Mapper<FROM> { typedef TO Type; }
#define PII_MAP_PUT_SELF(TYPE) PII_MAP_PUT(TYPE, TYPE)

#define PII_MAP_TYPE(MAP, TYPE) MAP::Mapper<TYPE>::Type

#if QT_VERSION >= 0x050000
#  define QMETAMETHOD_SIGNATURE methodSignature
#else
#  define QMETAMETHOD_SIGNATURE signature
#endif

#ifndef PII_NO_QT
/**
 * Converts *str* to local 8-bit representation.
 */
#  define piiPrintable(STR) (STR).toLocal8Bit().constData()
#  include "PiiLog.h"
#  include "PiiSynchronized.h"
#  define PII_ITERATOR_VALUE(IT) (IT).value()
#  define PII_ITERATOR_KEY(IT) (IT).key()
#else
#  include <cstdio>
#  define piiDebug printf
#  define piiWarning printf
#  define piiCritical printf
#  define piiFatal printf
#  define piiPrintable(STR) (STR).c_str()
// Map-like types in stl use std::pair as value_type
#  define PII_ITERATOR_VALUE(IT) (IT)->second
#  define PII_ITERATOR_KEY(IT) (IT)->first
#endif
#include "PiiValueSet.h"
/// @endhide


namespace Pii
{
  template <bool b> struct StaticAssert {};
  template <> struct StaticAssert<true> { typedef void Type; };

  PII_CORE_EXPORT int intoVersion();
  PII_CORE_EXPORT const char* intoVersionString();
}

#endif //_PIIGLOBAL_H
