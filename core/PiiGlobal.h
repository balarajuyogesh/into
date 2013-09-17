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

/**
 * @file
 *
 * Export/import macros and other global definitions such as d-pointer
 * stuff.
 */

// Disable documentation
/// @cond null

#define INTO_VERSION_STR "2.0.0-beta-3"
#define INTO_VERSION 0x02000003

#include <QtGlobal>
#include <QAtomicInt>

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

#define PII_SHARED_D_FUNC \
  inline Data* _d() { return static_cast<Data*>(this->d = static_cast<Data*>(this->d->detach())); } \
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

#define PII_SERIALIZATION_D Data* d = Archive::InputArchive ? \
  static_cast<Data*>(this->d = static_cast<Data*>(this->d->detach())) : \
  static_cast<Data*>(this->d)

#define PII_Q(CLASS) CLASS* const q = owner()

#define PII_DISABLE_COPY(CLASS) private: CLASS(const CLASS&); CLASS& operator= (const CLASS& other)

#ifdef PII_CXX0X
#  define PII_MOVE std::move
#else
#  define PII_MOVE
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
#  define QATOMICINT_LOAD(x) (x).load()
#  define QATOMICINT_STORE(a, b) (a).store(b)
inline bool operator!= (const QAtomicInt& i1, int i2) { return i1.load() != i2; }
#else
#  define QMETAMETHOD_SIGNATURE signature
#  define QATOMICINT_LOAD(x) static_cast<int>(x)
#  define QATOMICINT_STORE(a,b) (a) = b
#endif

template <class Derived> struct PiiSharedD
{
  typedef PiiSharedD<Derived>* RefCountedPtrType; // see PiiInputArchive.h

  PiiSharedD() : iRefCount(1) {}

  Derived* self() { return static_cast<Derived*>(this); }
  const Derived* self() const { return static_cast<const Derived*>(this); }

  void assignTo(Derived*& d) { reserve(); d->release(); d = self(); }
  void reserve() { iRefCount.ref(); }
  Derived* reserved() { reserve(); return self(); }
  void release() { if (!iRefCount.deref()) delete self(); }

  Derived* detach()
  {
    if (iRefCount != 1)
      {
        Derived* pData = self()->clone();
        release();
        return pData;
      }
    return self();
  }

  Derived* clone() const
  {
    return new Derived(*self());
  }
    
  QAtomicInt iRefCount;
};

#include "PiiValueSet.h"
#include "PiiLog.h"
#include "PiiSynchronized.h"
/// @endcond

namespace Pii
{
  PII_CORE_EXPORT int intoVersion();
  PII_CORE_EXPORT const char* intoVersionString();
}

#endif //_PIIGLOBAL_H
