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

#ifndef _PIISIMPLEMEMORYMANAGER_H
#define _PIISIMPLEMEMORYMANAGER_H

#include <PiiGlobal.h>
#include <QMutex>

/**
 * A simple segregated storage. This class reserves a fixed amount of
 * memory in the heap when constructed. It divides this area into
 * blocks of user-specified size. Each of these memory blocks forms
 * one allocation unit, the size of which cannot be exceeded. If a
 * larger amount of memory is requested or the fixed buffer is full,
 * no memory will be allocated.
 *
 * The advantage of this inflexibility is that %PiiSimpleMemoryManager
 * is very fast. Memory allocation and deallocation are both O(1)
 * operations. That is, the size of the managed memory has no effect
 * on allocation/deallocation time. If you need to allocate lots of
 * small memory chunks, all of which are about the same size, chances
 * are that %PiiSimpleMemoryManager is faster than @p malloc() or @p
 * new. This may be attributed not only to faster
 * allocation/deallocation but also slightly better cache hit ratios.
 *
 * Memory allocation and deallocation are thread-safe operations.
 *
 * The following example shows how to override @p new and @p delete to
 * use a custom memory manager.
 *
 * @code
 * class MyClass
 * {
 * public:
 *   MyClass(int value) : _iMember(value) {}
 *
 *   void* operator new (size_t size)
 *   {
 *     void* ptr = _manager.allocate(size);
 *     // Fall back to the default implementation if the manager is already full.
 *     if (ptr != 0) return ptr;
 *     return ::operator new(size);
 *   }
 *
 *   void operator delete (void* ptr)
 *   {
 *     if (!_manager.deallocate(ptr))
 *       ::operator delete(ptr);
 *   }
 *
 * private:
 *   int _iMember;
 *   static PiiSimpleMemoryManager _manager;
 * };
 *
 * // Reserve one kilobyte of memory.
 * // Each memory block should exactly fit a MyClass
 * PiiSimpleMemoryManager MyClass::_manager(1024, sizeof(MyClass));
 * // To reserve memory for exactly N MyClass instances, use
 * // the formula Pii::alignAddress((sizeof(MyClass)+sizeof(void*)),0xf)*N
 *
 * // Now, the manager will be automatically used when you allocate
 * // MyClass instances from heap and delete them.
 * MyClass* instance = new MyClass(5);
 * delete instance;
 *
 * // Note that the overridden new and delete would work even if you
 * // derived MyClass from another class and deleted the base class
 * // pointer. You must just make the destructor of the base class
 * // virtual as usual.
 * @endcode
 *
 * @ingroup Core
 */
class PII_CORE_EXPORT PiiSimpleMemoryManager
{
public:
  /**
   * Creates a new memory manager that allocates at least @a blockSize
   * bytes for each memory block. The memory will be allocated from
   * the heap.
   *
   * @param memorySize the maximum amount of bytes to reserve for the
   * whole memory manager. The memory manager will not ensure that the
   * memory is large enough to hold a single block. If you set this
   * value to low, chances are that #allocate() returns 0.
   *
   * @param blockSize the desired size of memory blocks. The actual
   * size may be somewhat larger because %PiiSimpleMemoryManager
   * always aligns the memory blocks at 16-byte boundaries. In
   * addition, @p sizeof(void*) bytes will be reserved for book
   * keeping information for each block. Thus, the actual block size
   * will be at least @p blockSize + @p sizeof(void*). Using @f$16N -
   * \mathrm{sizeof(void*)}@f$ bytes as a block size wastes no memory
   * for alignment, except possibly at the beginning, if your @p
   * malloc() implementation returns an unaligned pointer (unlikely).
   *
   * Note that the initial values cannot be changed once the memory
   * manager has been constructed.
   */
  PiiSimpleMemoryManager(size_t memorySize, size_t blockSize);

  /**
   * Creates a new memory manager that uses a preallocated block of
   * memory.
   */
  PiiSimpleMemoryManager(void* buffer, size_t memorySize, size_t blockSize);
  
  /**
   * Deallocate the memory buffer.
   */
  ~PiiSimpleMemoryManager();
  
  /**
   * Allocate memory. The function returns a pointer to the allocated
   * memory block. If the number of bytes requested exceeds @a
   * blockSize or no more blocks are available, 0 will be returned.
   *
   * @param bytes the number of bytes to allocate. If @a bytes is
   * zero, 0 will be returned.
   *
   * @return a pointer to the allocated memory or 0 if the requsted
   * number of bytes cannot be reserved.
   */
  void* allocate(size_t bytes);

  /**
   * Deallocate a previously allocated buffer. If @p buffer is not
   * within the allocated memory area, @p false will be returned,
   * expect for a null pointer, for which @p true will be returned. 
   * Otherwise, the buffer will be deallocated and @p true will be
   * returned. If @a buffer points to an invalid memory location, the
   * behavior is undefined.
   */
  bool deallocate(void* buffer);

  /**
   * Get the number of allocated memory blocks. This value varies
   * depending on how the whole memory buffer was aligned when
   * allocated. The size of address type (@p void*) also affects the
   * number of blocks that fit into the memory buffer.
   */
  ulong blockCount() const;

  /**
   * Returns the size of one memory block. Note that this value may be
   * larger than what was initially requested due to memory alignment.
   */
  size_t blockSize() const;

private:
  class Data;
  Data* d;

  PII_DISABLE_COPY(PiiSimpleMemoryManager);
};

#endif //_PIISIMPLEMEMORYMANAGER_H
