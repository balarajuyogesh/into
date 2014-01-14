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

#ifndef _PIISHAREDD_H
#define _PIISHAREDD_H

/// @hide

#include "PiiAtomicInt.h"

#define PII_SHARED_D_FUNC                                               \
  inline Data* _d() { return static_cast<Data*>(this->d = static_cast<Data*>(this->d->detach())); } \
  inline const Data* _d() const { return static_cast<const Data*>(this->d); } \
  friend class Data

#define PII_SHARED_D_FUNC_DECL \
  inline Data* _d(); \
  inline const Data* _d() const; \
  friend class Data

#define PII_SHARED_D_FUNC_DEF(CLASS) \
  CLASS::Data* CLASS::_d() { return static_cast<Data*>(this->d = static_cast<Data*>(this->d->detach())); } \
  const CLASS::Data* CLASS::_d() const { return static_cast<const Data*>(this->d); }

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
    
  PiiAtomicInt iRefCount;
};

/// @endhide

#endif //_PIISHAREDD_H
