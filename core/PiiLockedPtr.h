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

#ifndef _PIILOCKEDPTR_H
#define _PIILOCKEDPTR_H

#include "PiiGlobal.h"

/**
 * PiiLockedPtr is a wrapper class that can be used as an exclusively
 * owned pointer. When constructed, the class locks a mutex which will
 * remain locked until the last copy of the wrapper class goes out of
 * scope or is deleted. The class masquerades as the wrapped pointer
 * (T*) itself and can be mostly used as if it was a pointer.
 */
template <class T> class PiiLockedPtr
{
public:
  /**
   * Wraps *ptr* and locks *mutex*.
   */
  PiiLockedPtr(QMutex* mutex, T* ptr = 0);
  ~PiiLockedPtr();
  PiiLockedPtr(const PiiLockedPtr& other);
  
  PiiLockedPtr& operator= (const PiiLockedPtr& other);
  PiiLockedPtr& operator= (T* ptr);

  operator T* () const;
  T* operator-> () const;
    
private:
  class Data : public PiiSharedD<Data>
  {
  public:
    Data(QMutex* mutex, T* ptr);
    ~Data();

    QMutex* pMutex;
    T* pPtr;
  } *d;
};

template <class T> PiiLockedPtr<T>::Data::Data(QMutex* mutex, T* ptr) :
  pMutex(mutex),
  pPtr(ptr)
{
  pMutex->lock();
}

template <class T> PiiLockedPtr<T>::Data::~Data()
{
  pMutex->unlock();
}

template <class T> PiiLockedPtr<T>::PiiLockedPtr(QMutex* mutex, T* ptr) :
  d(new Data(mutex, ptr))
{}

template <class T> PiiLockedPtr<T>::PiiLockedPtr(const PiiLockedPtr& other) :
  d(other.d)
{
  d->reserve();
}

template <class T> PiiLockedPtr<T>::~PiiLockedPtr()
{
  d->release();
}

template <class T> PiiLockedPtr<T>& PiiLockedPtr<T>::operator= (const PiiLockedPtr& other)
{
  other.d->assignTo(d);
  return *this;
}

template <class T> PiiLockedPtr<T>& PiiLockedPtr<T>::operator= (T* ptr)
{
  d->pPtr = ptr;
  return *this;
}

template <class T> PiiLockedPtr<T>::operator T* () const { return d->pPtr; }
template <class T> T* PiiLockedPtr<T>::operator-> () const { return d->pPtr; }

#endif //_PIILOCKEDPTR_H
