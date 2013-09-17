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

#ifndef _PIIBASECLASS_H
#define _PIIBASECLASS_H

#include "PiiSerializationTraits.h"

/**
 * @file
 *
 * Base class serialization stuff.
 *
 */

/**
 * Invoke the serialization of the base class of your class. This
 * macro is used within a serialization function to store the state of
 * a parent class. It works both in member functions and in separate
 * serialization functions.
 *
 * @param archive an instance of an input or output archive
 *
 * @param base the name of the direct base class of the serializable
 * class
 *
 * Assume `MyDerivedClass` is derived from `MyClass`. Then:
 *
 * ~~~
 * template <class Archive>
 * void MyDerivedClass::serialize(Archive& archive, const unsigned int version)
 * {
 *   PII_SERIALIZE_BASE(archive, MyClass);
 *   archive & member;
 * }
 * ~~~
 */
#define PII_SERIALIZE_BASE(archive, Base) archive & PiiBaseClass<Base >(*this)


/**
 * A wrapper for serializing a base class. See [PII_SERIALIZE_BASE].
 *
 */
template <class T> struct PiiBaseClass
{
  /**
   * Create an instance of base class serializer with a reference to
   * the derived class.
   *
   * ~~~
   * template <class Archive>
   * void Derived::serialize(Archive& archive, const unsigned int)
   * {
   *   archive & PiiBaseClass<Base>(*this);
   * }
   * ~~~
   */
  PiiBaseClass(T& child) : derived(&child) {}

  PII_SEPARATE_SAVE_LOAD_MEMBERS
  
  template <class Archive> void load(Archive& archive, const unsigned int /*version*/) const
  {
    unsigned int iBaseClassVersion(0);
    // Needed to increase archive version number to store the version
    // number of a base class.
    if (PiiSerializationTraits::ClassInfo<T>::boolValue &&
        archive.majorVersion() > 0)
      archive >> iBaseClassVersion;
    PiiSerialization::serialize(archive, *derived, iBaseClassVersion);
  }

  template <class Archive> void save(Archive& archive, const unsigned int /*version*/) const
  {
    unsigned int iBaseClassVersion(PiiSerializationTraits::Version<T>::intValue);
    if (PiiSerializationTraits::ClassInfo<T>::boolValue)
      archive << iBaseClassVersion;
    PiiSerialization::serialize(archive, *derived, iBaseClassVersion);
  }

  mutable T* derived;
};

PII_SERIALIZATION_TRACKING_TEMPLATE(PiiBaseClass, false);
PII_SERIALIZATION_CLASSINFO_TEMPLATE(PiiBaseClass, false);

#endif //_PIIBASECLASS_H
