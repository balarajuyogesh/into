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

#ifndef _PIISHAREDPTR_H
#define _PIISHAREDPTR_H

#include "PiiPtrHolder.h"

/**
 * An implicitly shared pointer. The PiiSharedPtr class holds a
 * reference-counted pointer ([PiiSharedObject]) and masquerades as
 * one itself. The class automatically increases and decreases the
 * reference count when copies of it are requested. Thus, you never
 * need to care about deleting the memory. Typical usage (compare this
 * to the explicit sharing example in [PiiSharedObject]):
 *
 * ~~~(c++)
 * class MyObject : public PiiSharedObject;
 * typedef PiiSharedPtr<MyObject> MyPtr;
 *
 * // Sender creates an explicitly shared pointer and passes it.
 * MyPtr ptr(new MyObject);
 * receiver.takeThis(ptr);
 * otherReceiver.takeThis(ptr);
 *
 * //in the receiver, we just store the pointer. It is automatically
 * //released when the Receiver class is deleted.
 * void Receiver::takeThis(MyPtr ptr)
 * {
 *   //the MyPtr object works just as a pointer
 *   ptr->callMethod();
 *   internalVariable = ptr;
 *   doWhatEverNeeded();
 * }
 * ~~~
 *
 * The class supports all types types, including arrays. If the
 * reference-counted type is not directly derived from
 * PiiSharedObject, a reference-counted wrapper will be automatically
 * created.
 *
 * ~~~(c++)
 * PiiSharedPtr<string> ptr(new string("this is"));
 * *ptr += " a test";
 * cout << ptr->c_str() << endl; //outputs "this is a test"
 * ~~~
 *
 * ~~~(c++)
 * PiiSharedPtr<int> ptr(new int);
 * ptr = new int; //releases the old one
 * *ptr = 3; //use the class just like an int pointer
 * ~~~
 *
 * ~~~(c++)
 * PiiSharedPtr<int[]> ptr(new int[5]);
 * ptr = new int[6]; //releases the old array
 * ptr[0] = 3; //use the class just like an int array
 * ~~~
 *
 * The computational overhead of using a shared pointer instead of a
 * direct one is minimal. Copying a pointer costs one (non-virtual and
 * inlineable) function call, and accessing it is as fast as direct
 * access, provided that the pointer type is derived from
 * PiiSharedObject. If it is not, there will be one more memory
 * indirection.
 *
 * To ensure no memory leaks, one should never store the internal
 * pointer as an ordinary pointer only, although it is possible. Once
 * you initialize a PiiSharedPtr with a newly allocated pointer, just
 * forget about the pointer and make sure you only make assignments
 * between PiiSharedPtrs. You may, however, use the internal pointer
 * directly if you ensure that the PiiSharedPtr instance stays in
 * memory.
 *
 * ~~~(c++)
 * MyPtr ptr(new MyObject);
 * MyObject* ptr2 = ptr;
 * // You may now safely use ptr2 as long as ptr is in memory
 * ~~~
 *
 * One can assign PiiSharedPtrs just like ordinary pointers. That is,
 * a PiiSharedPtr<Base> can hold a pointer to a class derived from
 * Base.
 *
 * ! If the types are not derived from PiiSharedObject, multiple
 * inheritance is not always handled correctly. You can assign derived
 * pointers to base class pointers only if the base class is the first
 * one in inheritance order.
 *
 * @see PiiSharedObject
 */
template <class T> class PiiSharedPtr
{
  // Make the class serializable
  friend struct PiiSerialization::Accessor;
  PII_SEPARATE_SAVE_LOAD_MEMBERS
  template <class Archive> void save(Archive& archive, const unsigned int /*version*/)
  {
    archive << PII_NVP("ptr", unwrap(_ptr));
  }
  template <class Archive> void load(Archive& archive, const unsigned int /*version*/)
  {
    T* ptr;
    archive >> PII_NVP("ptr", ptr);
    if (_ptr != 0)
      _ptr->release();
    _ptr = wrap(ptr);
  }

public:
  /// @internal
  template <class U> struct OnlyDerived : Pii::IfClass<Pii::IsBaseOf<T,U>, Pii::Id<int>, Pii::Empty>::Type {};
  template <class U> struct OnlyArray : Pii::IfClass<Pii::IsArray<U>, Pii::Id<int>, Pii::Empty>::Type {};

#define PII_IS_FIRST_BASE(BASE, DERIVED)                                \
  (reinterpret_cast<void*>(static_cast<BASE*>(reinterpret_cast<DERIVED*>((void*)1))) == (void*)1)

  PiiSharedPtr() : _ptr(wrap(0)) {}

  /**
   * Creates an implicitly shared pointer that holds the given pointer.
   *
   * @param obj a pointer to the object to be shared. PiiSharedPtr
   * takes the ownership of the pointer.
   */
  PiiSharedPtr(T* obj) : _ptr(wrap(obj)) {}

  /**
   * Create a copy of a pointer.
   */
  PiiSharedPtr(const PiiSharedPtr& other) : _ptr(other._ptr)
  {
    if (_ptr != 0)
      _ptr->reserve();
  }

  /**
   * Create a copy of a pointer. This constructor only accepts
   * pointers to objects that are derived from the template type `T`
   * of this class.
   */
  template <class U> PiiSharedPtr(const PiiSharedPtr<U>& other, typename OnlyDerived<U>::Type = 0) :
    _ptr(other._ptr)
  {
    Q_ASSERT(PII_IS_FIRST_BASE(T,U)); // traps multiple inheritance problems
    if (_ptr != 0)
      _ptr->reserve();
  }

  ~PiiSharedPtr()
  {
    if (_ptr != 0)
      _ptr->release();
  }

#ifdef PII_CXX11
  PiiSharedPtr(PiiSharedPtr&& other) :
    _ptr(other._ptr)
  {
    other._ptr = nullptr;
  }

  PiiSharedPtr& operator= (PiiSharedPtr&& other)
  {
    _ptr = other._ptr;
    other._ptr = nullptr;
    return *this;
  }
#endif

  void swap(PiiSharedPtr& other) { qSwap(_ptr, other._ptr); }

  /**
   * Assigns a new value to this pointer and releases the old pointer.
   *
   * ~~~(c++)
   * PiiSharedPtr<MyObject> ptr1(new MyObject), ptr2(new MyObject);
   * ptr1 = ptr2; //releases the old pointer in ptr1, increases refcount in ptr2
   * ~~~
   */
  PiiSharedPtr& operator= (const PiiSharedPtr& other)
  {
    reassign(other._ptr);
    return *this;
  }

  /**
   * Assigns a new value to this pointer and release the old pointer.
   * This operator only accepts pointers to objects that are derived
   * from the template type `T` of this class.
   */
  template <class U> inline PiiSharedPtr& operator= (const PiiSharedPtr<U>& other)
  {
    copy(other);
    return *this;
  }

  /**
   * Returns a reference to the element at *index*, if the wrapped
   * pointer is an array.
   */
  inline T& operator[] (int index)
  {
    return elementAt(this, index);
  }

  /**
   * Returns the element at *index*, if the wrapped pointer is an
   * array.
   */
  inline const T& operator[] (int index) const
  {
    return elementAt(this, index);
  }

  /**
   * Compare two pointers.
   * @return true iff the internal pointers are equal
   */
  bool operator== (const PiiSharedPtr& other) const { return other._ptr == _ptr; }

  /**
   * Compare two pointers.
   * @return true iff the internal pointers are different
   */
  bool operator!= (const PiiSharedPtr& other) const { return other._ptr != _ptr; }

  /**
   * Compare the internal pointer to the given `ptr`.
   */
  bool operator== (const void* ptr) const { return ptr == _ptr; }

  /**
   * Compare the internal pointer to the given `ptr`.
   */
  bool operator!= (const void* ptr) const { return ptr != _ptr; }

  /**
   * Returns the internal pointer. This operator makes it possible to
   * use this class directly as a pointer.
   */
  T* operator-> () const { return unwrap(_ptr); }
  /**
   * Converts the shared pointer to a reference.
   */
  T& operator* () const { return *unwrap(_ptr); }
  /**
   * Casts the shared pointer to the internal pointer type.
   */
  operator T* () { return unwrap(_ptr); }
  /**
   * Casts the shared pointer to the internal pointer type.
   */
  operator const T* () const { return unwrap(_ptr); }

private:
  template <class U> friend class PiiSharedPtr;

  struct NonWrapped
  {
    static PiiSharedObject* wrap(T* ptr) { return ptr; }
    static T* unwrap(PiiSharedObject* ptr) { return static_cast<T*>(ptr); }
  };

  struct Wrapped
  {
    static PiiSharedObject* wrap(T* ptr) { return new PiiPtrHolder<T>(ptr); }
    static T* unwrap(PiiSharedObject* ptr) { return static_cast<PiiPtrHolder<T>*>(ptr)->pointer; }
  };

  // If T is already derived from PiiSharedObject, we don't need a
  // wrapper. Otherwise we do.
  typedef typename Pii::IfClass<Pii::IsBaseOf<PiiSharedObject, T>,
                                NonWrapped,
                                Wrapped>::Type Traits;

  inline static PiiSharedObject* wrap(T* ptr) { return Traits::wrap(ptr); }
  inline static T* unwrap(const PiiSharedObject* ptr) { return Traits::unwrap(const_cast<PiiSharedObject*>(ptr)); }

  void reassign(PiiSharedObject* ptr)
  {
    if (ptr != 0)
      ptr->reserve();
    if (_ptr != 0)
      _ptr->release();
    _ptr = ptr;
  }

  template <class U> void copy(const PiiSharedPtr<U>& other, typename OnlyDerived<U>::Type = 0)
  {
    Q_ASSERT(PII_IS_FIRST_BASE(T,U)); // traps multiple inheritance problems
    reassign(other._ptr);
  }

  template <class U> static inline T& elementAt(U* thisPtr, typename OnlyArray<U>::Type index)
  {
    return unwrap(thisPtr->_ptr)[index];
  }

  PiiSharedObject* _ptr;
};

PII_SERIALIZATION_TRACKING_TEMPLATE(PiiSharedPtr, false);

#endif //_PIISHAREDPTR_H
