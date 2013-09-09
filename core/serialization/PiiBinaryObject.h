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

#ifndef _PIIBINARYOBJECT_H
#define _PIIBINARYOBJECT_H

#include "PiiSerialization.h"

/**
 * @file
 *
 * Declaration of a binary data pair wrapper and a convenience macro.
 */

/**
 * A serialization wrapper for binary data. Note that this wrapper
 * only works with primitive data types.
 *
 * @ingroup Serialization
 */
template <class T> struct PiiBinaryObject
{
  /**
   * Creates a new binary data wrapper with the given pointer and
   * number of elements.
   */
  PiiBinaryObject(T*& p, unsigned int& s) : ptr(p), size(s) {}
  
  /**
   * Creates a new binary data wrapper with the given pointer and
   * number of elements.
   */
  PiiBinaryObject(const T*& p, unsigned int& s) : ptr(const_cast<T*&>(p)), size(s) {}

  PII_SEPARATE_SAVE_LOAD_MEMBERS

  /**
   * Calls @p archive.writeArray().
   */
  template <class Archive> void save(Archive& archive, const unsigned int)
  {
    archive.writeArray(ptr, size);
  }

  /**
   * Calls @p archive.readArray().
   */
  template <class Archive> void load(Archive& archive, const unsigned int)
  {
    archive.readArray(ptr, size);
  }
  
  T*& ptr;
  unsigned int& size;
};

namespace Pii
{
  /**
   * Constructs a PiiBinaryObject.
   *
   * @relates PiiBinaryObject
   */
  template <class T> PiiBinaryObject<T> binaryObject(T*& p, unsigned int& s)
  {
    return PiiBinaryObject<T>(p, s);
  }
  /**
   * Constructs a PiiBinaryObject.
   *
   * @relates PiiBinaryObject
   */
  template <class T> PiiBinaryObject<T> binaryObject(const T*& p, unsigned int& s)
  {
    return PiiBinaryObject<T>(p, s);
  }
}

PII_SERIALIZATION_TRACKING_TEMPLATE(PiiBinaryObject, false);
PII_SERIALIZATION_CLASSINFO_TEMPLATE(PiiBinaryObject, false);

#endif //_PIIBINARYOBJECT_H
