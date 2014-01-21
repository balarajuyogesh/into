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

#ifndef _PIIDYNAMICTYPEFUNCTIONS_H
#define _PIIDYNAMICTYPEFUNCTIONS_H

#include "PiiMetaObject.h"
#include "PiiSerializationFactory.h"
#include "PiiSerializer.h"

/* This file contains functions for getting meta information,
 * serializers, and class factories for serializable types. The
 * functions are separated from context to ensure correct declaration
 * order of overloaded functions. If you include this file directly,
 * please ensure that all metaObjectPointer() and isDynamicType()
 * overloads have been declared beforehand.
 */

namespace PiiSerialization
{
  /**
   * This function is used to check if the real type of the class is
   * known at compile time or not. If a class can be serialized
   * through a base class pointer, its type cannot be known at compile
   * time. Thus, its type is *dynamic*. For such, types an overloaded
   * version of this function that returns `true` must be provided.
   * Typically, such a type will also have an overload for
   * [PiiSerialization::metaObjectPointer()]. The default
   * implementation returns `false`.
   *
   * @see serialization_serializers
   */
  inline bool isDynamicType(const void*) { return false; }

  /**
   * Default implementation of the meta object getter function. This
   * function returns a null pointer. If a serializable type has a
   * non-default meta object getter (such as a virtual function), one
   * needs to provide an overloaded version of this function.
   *
   * @relates PiiMetaObject
   */
  inline const PiiMetaObject* metaObjectPointer(const void*) { return 0; }

  /**
   * Get a meta object for the given object. This function calls the
   * metaObjectPointer() function, which needs to be overloaded if
   * a serializable type needs a non-default meta object.
   *
   * @see PII_SERIALIZATION_VIRTUAL_METAOBJECT
   *
   * @relates PiiMetaObject
   */
  template <class T> inline PiiMetaObject piiMetaObject(const T& obj)
  {
    /* The following trickery is needed to tweak C++'s resolution
     * order. There seems to be no way to do this statically with
     * templates. Hope the compiler is wise enough to optimize out the
     * unnecessary comparison in the beginning.
     *
     * If a superclass of type T has a virtual metaobject function, we
     * want it to be used by default. The problem is that the
     * overload resolution on C++ prefers template over base class.
     * Suppose we have a metaobject function template f(T) and a
     * specialization f(A). We want f(A) to be called for types
     * derived from A, but C++ would select the template as it matches
     * the derived type exactly.
     *
     * The metaObjectPointer() function is not a template. Therefore,
     * base classes are considered in overload resolution. The default
     * version has void* as the argument type, and will always come
     * last in resolution order. The problem is that void* has no
     * type. Thus, metaObjectPointer() cannot create a default
     * metaobject based on the type. This could be solved by adding
     * the metaObjectPointer() function a template parameter as its
     * second argument and use the first one for resolution only. But
     * this would force all metaObjectPointer() functions to be
     * templates, which is less than elegant.
     *
     * Therefore, we must first find out if there is an overloaded
     * metaObjectPointer() function. If such a function is
     * resolved, a non-null pointer will be returned. If a null
     * pointer is returned, a default metaobject will be constructed
     * based on the type T.
     */
    const PiiMetaObject* pMetaObject = metaObjectPointer(&obj);
    if (pMetaObject == 0)
      return defaultMetaObject(obj);
    return *pMetaObject;
  }

  /// @internal
  template <class Archive, class T>
  void FunctionSaver::serialize(Archive& archive, T& value, const unsigned int version)
  {
    save(archive, value, version);
  }

  /// @internal
  template <class Archive, class T>
  void FunctionLoader::serialize(Archive& archive, T& value, const unsigned int version)
  {
    load(archive, value, version);
  }
}

template <class T, class Archive> T* PiiSerializationFactory::create(const char* className, Archive& archive)
{
  if (PiiSerialization::isDynamicType((T*)0))
    {
      // Try archive-specific factory first
      PiiSerializationFactory* pFactory = factory<Archive>(className);
      if (pFactory == 0)
        {
          // If this is already the default factory, fail
          if (Pii::IsSame<Archive, PiiSerialization::Void>::boolValue ||
              // Otherwise try the default factory
              (pFactory = factory<PiiSerialization::Void>(className)) == 0)
            return 0;
        }
      return reinterpret_cast<T*>(pFactory->create(&archive));
    }
  return create<T>(archive);
}

template <class Archive>
template <class T> bool PiiSerializer<Archive>::serialize(const char* className,
                                                          Archive& archive,
                                                          T& value,
                                                          const unsigned int version)
{
  if (PiiSerialization::isDynamicType((T*)0))
    {
      const PiiSerializer* pSerializer = serializer(className);
      if (pSerializer == 0)
        return false;
      pSerializer->serialize(archive, (void*)&value, version);
    }
  else
    PiiSerialization::serialize(archive, const_cast<typename Pii::ToNonConst<T>::Type &>(value), version);

  return true;
}

#endif //_PIIDYNAMICTYPEFUNCTIONS_H
