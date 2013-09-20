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

#ifndef _PIISMARTPTR_H
#define _PIISMARTPTR_H

#include "PiiGlobal.h"

/**
 * A utility class that works as a simple alternative to
 * std::auto_ptr. PiiSmartPtr is used as a stack-allocated object that
 * deletes its pointer upon destruction. It doesn't provide copy
 * semantics. Contrary to std::auto_ptr, PiiSmartPtr also works for
 * arrays.
 *
 * The main use of PiiSmartPtr is in guarding heap-allocated pointers
 * in functions that may throw exceptions. Use like this:
 *
 * ~~~(c++)
 * PiiSmartPtr<int[]> ptr(new int[5]); // arrays
 * PiiSmartPtr<int> ptr(new int); // any other pointers
 * ~~~
 *
 */
template <class T> class PiiSmartPtr
{
public:
  PiiSmartPtr() : _ptr(0) {}
  /**
   * Creates a new PiiSmartPtr that takes the ownership of *ptr*.
   */
  PiiSmartPtr(T* ptr) : _ptr(ptr) {}
  
  /**
   * Deletes the wrapped pointer.
   */
  ~PiiSmartPtr() { delete _ptr; }

  operator T* () const { return _ptr; }

  T* operator-> () const { return _ptr; }

  PiiSmartPtr& operator= (T* ptr)
  {
    delete _ptr;
    _ptr = ptr;
    return *this;
  }
  
  /**
   * Releases the ownership of the pointer and returns it.
   */
  T* release()
  {
    T* result = _ptr;
    _ptr = 0;
    return result;
  }

  T*& ptrRef() { return _ptr; }
  
private:
  T* _ptr;
  PII_DISABLE_COPY(PiiSmartPtr);
};

template <class T> class PiiSmartPtr<T[]>
{
public:
  PiiSmartPtr(T* ptr = 0) : _ptr(ptr) {}
  ~PiiSmartPtr() { delete[] _ptr; }

  operator T* () const { return _ptr; }
  T* release()
  {
    T* result = _ptr;
    _ptr = 0;
    return result;
  }
  
private:
  T* _ptr;
};

#endif //_PIISMARTPTR_H
