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

#ifndef _PIIPTRHOLDER_H
#define _PIIPTRHOLDER_H

#include "Pii.h"
#include "PiiSharedObject.h"

/// @hide

/**
 * A reference counting wrapper for any pointer type. The PiiPtrHolder
 * class is used with primitive types, with arrays of them, and with
 * object types that don't support the reserve/release reference
 * counting mechanism. In these scenarios, a pointer to a PiiPtrHolder
 * object is passed instead of the object pointer itself.
 *
 */
template <class T> class PiiPtrHolder : public PiiSharedObject
{
public:
  PiiPtrHolder() : pointer(0) {}

  /**
   * Creates a PiiPtrHolder that wraps the given pointer *ptr*. The
   * pointer will be owned by PiiPtrHolder and must not be deleted by
   * the caller.
   */
  PiiPtrHolder(T* ptr) : pointer(ptr) {}

  /**
   * Deletes the wrapped pointer.
   */
  ~PiiPtrHolder() { delete pointer; }

  T* pointer;
private:
  PII_DISABLE_COPY(PiiPtrHolder);
};

template <class T> class PiiPtrHolder<T[]> : public PiiSharedObject
{
public:
  PiiPtrHolder() : pointer(0) {}
  PiiPtrHolder(T* ptr) : pointer(ptr) {}
  ~PiiPtrHolder() { delete[] pointer; }
  T* pointer;
private:
  PII_DISABLE_COPY(PiiPtrHolder);
};

/// @endhide

#endif //_PIIPTRHOLDER_H
