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

#ifndef _PIISHAREDOBJECT_H
#define _PIISHAREDOBJECT_H

#include "PiiGlobal.h"
#include <PiiSerialization.h>
#include <PiiNameValuePair.h>
#include <PiiTrackedPointerHolder.h>
#include <QAtomicInt>

/**
 * A shared object. Shared objects are useful when multiple pointers
 * to a single memory location are concurrently in use. With a shared
 * object, it is possible that the users of the memory location are
 * unaware of each other. Each user just calls reserve() when it
 * starts using the pointer and release() after it no longer needs it. 
 * Each reserve() call increases and each release() call decreases a
 * reference counter. When the counter reaches zero, the object is
 * automatically deleted. This type of explicit sharing provides
 * minimum computational overhead but may not be the most convenient
 * way to the programmer. A typical usage scenario is as follows:
 *
 * ~~~(c++)
 * class MyObject : public PiiSharedObject { ... };
 *
 * // Sender creates the pointer and passes it.
 * MyObject* obj = new MyObject;
 * receiver.takeThis(obj);
 * otherReceiver.takeThis(obj);
 * // This may delete the object. You don't know, so don't fiddle with
 * // it any more.
 * obj->release();
 *
 * //in the receiver, we reserve the pointer for later use:
 * void Receiver::takeThis(MyObject* obj)
 * {
 *   internalVariable = obj->reserve();
 *   doWhatEverNeeded();
 * }
 * ~~~
 *
 * Once "Receiver" does not need the pointer any more, it calls
 * internalVariable->release() and forgets about the pointer.
 *
 * Usually, it is easier to use implicit sharing. For this, the
 * [PiiSharedPtr] class can be used.
 *
 * PiiSharedObject is thread safe. Its memory overhead is one integer
 * for the reference counter plus a vtable pointer, if one doesn't
 * already exists.
 *
 */
class PII_CORE_EXPORT PiiSharedObject
{
public:
  /**
   * Constructs a new shared object. The reference counter is
   * initialized to one.
   */
  PiiSharedObject() : _ref(1)
  {}

  virtual ~PiiSharedObject();
  
  /**
   * Reserves the object for use. The reference counter will be
   * incremented by one.
   */
  void reserve() const { _ref.ref(); }
  
  /**
   * Releases the object. The reference counter is decremented by one. 
   * Once it reaches zero, the object will be deleted.
   */
  void release() const
  {
    if (!_ref.deref())
      delete const_cast<PiiSharedObject*>(this);
  }

  /**
   * Returns the number of references to this object.
   */
  int references() const
  {
    return QATOMICINT_LOAD(_ref);
  }

  /**
   * Sets the number of references to `cnt`.
   */
  void setReferences(int cnt)
  {
    QATOMICINT_STORE(_ref, cnt);
  }

private:
  mutable QAtomicInt _ref;
};

/// @hide
class PiiSharedObjectHolder : public PiiTrackedPointerHolder
{
public:
  PiiSharedObjectHolder(const PiiSharedObject* ptr) : PiiTrackedPointerHolder(ptr)
  {
    ptr->reserve();
  }

  ~PiiSharedObjectHolder()
  {
    reinterpret_cast<const PiiSharedObject*>(_pointer)->release();
  }  
};

namespace PiiSerialization
{
  inline PiiTrackedPointerHolder* createTrackedPointerHolder(const PiiSharedObject* ptr)
  {
    return new PiiSharedObjectHolder(ptr);
  }
  
  /// A specialization that increases the refcount of `ptr` by one.
  inline void rereferencePointer(PiiSharedObject* ptr)
  {
    ptr->reserve();
  }
}
/// @endhide

#endif //_PIISHAREDOBJECT_H
