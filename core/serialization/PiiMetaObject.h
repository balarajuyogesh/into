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

#ifndef _PIIMETAOBJECT_H
#define _PIIMETAOBJECT_H

/**
 * @file
 *
 * Declarations, macros and functions for dealing with meta objects.
 *
 */

#include "PiiSerializationTraits.h"
#include "PiiSerializationGlobal.h"

/**
 * A convenience macro for fetching the meta object for the given
 * object. The parameter `obj` must be a reference type.
 *
 * @relates PiiMetaObject
 */
#define PII_GET_METAOBJECT(obj) PiiSerialization::piiMetaObject(obj)

/**
 * Meta objects store information specific to a certain data type. 
 * Class meta information contains a unique class name and a version
 * number. The tracking flag is an application-wide static constant
 * and not included in the meta object (see
 * PiiSerializationTraits::Tracking).
 *
 */
class PII_SERIALIZATION_EXPORT PiiMetaObject
{
public:
  /**
   * Create a new meta object with the given class name and version
   * number. The `storeClassInfo` flag determines whether the version
   * number is saved or not.
   */
  PiiMetaObject(const char* className,
                unsigned int version,
                bool storeClassInfo) :
    _pClassName(className),
    _uiVersion(version),
    _bStoreClassInfo(storeClassInfo)
  {}

  /**
   * Get the name of the class this meta object represents.
   */
  const char* className() const { return _pClassName; }

  /**
   * Get the current version number of the class.
   */
  unsigned int version() const { return _uiVersion; }

  /**
   * See if the class information (only version number, currently)
   * needs to be stored.
   */
  bool isClassInfoStored() const { return _bStoreClassInfo; }

private:
  const char* _pClassName;
  unsigned int _uiVersion;
  bool _bStoreClassInfo;
};


namespace PiiSerialization
{
  /**
   * Create a default PiiMetaObject instance for the type given as a
   * parameter.
   *
   * @return a meta object that takes class information from
   * PiiSerializationTraits.
   *
   * @relates PiiMetaObject
   */
  template <class T> inline PiiMetaObject defaultMetaObject(const T& /*obj*/)
  {
    return PiiMetaObject(PiiSerializationTraits::ClassName<T>::get(),
                         (unsigned int)PiiSerializationTraits::Version<T>::intValue,
                         PiiSerializationTraits::ClassInfo<T>::boolValue);
  }
}


#endif //_PIIMETAOBJECT_H
