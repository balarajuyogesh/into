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

#ifndef _PIICONSTCHARWRAPPER
#define _PIICONSTCHARWRAPPER

#include "PiiGlobal.h"
#include <cstring>

/**
 * A wrapper structure for `const char` pointers that can be
 * used with QHash and QMap as a light-weight replacement for QString.
 *
 */
struct PII_CORE_EXPORT PiiConstCharWrapper
{
  PiiConstCharWrapper(const char* p) : ptr(p) {}
  bool operator== (const PiiConstCharWrapper& other) const { return !std::strcmp(ptr, other.ptr); }
  bool operator< (const PiiConstCharWrapper& other) const { return std::strcmp(ptr, other.ptr) < 0; }
  operator const char* () const { return ptr; }
  const char* ptr;
};

/**
 * Calculates a hash value for the character string wrapped by *key*.
 *
 * @relates PiiConstCharWrapper
 */
PII_CORE_EXPORT uint qHash(const PiiConstCharWrapper& key);

namespace std
{
  template <class T> struct hash;
  template <> struct hash<PiiConstCharWrapper>
  {
    std::size_t operator() (const PiiConstCharWrapper& key) const
    {
      return qHash(key);
    }
  };
}

#endif //_PIICONSTCHARWRAPPER
