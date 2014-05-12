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

#include "TestPiiSimpleMemoryManager.h"

#include <PiiSimpleMemoryManager.h>
#include <PiiBits.h>
#include <QtTest>
#include <cstdlib>

void TestPiiSimpleMemoryManager::allocation()
{
  PiiSimpleMemoryManager manager(1024, 128-sizeof(void*));
  void* buffers[8];
  QVERIFY(manager.blockCount() <= 8);
  // Seven blocks is possible if the memory buffer is oddly aligned
  QVERIFY(manager.blockCount() >= 7);
  QVERIFY(!manager.allocate(0));
  // Reserve all blocks.
  for (unsigned i=0; i<manager.blockCount(); ++i)
    {
      buffers[i] = manager.allocate(100);
      QVERIFY(buffers[i] != 0);
      // Aligned?
      QCOMPARE(long(buffers[i]) & 0xf, 0l);
    }
  // No space left, should return 0
  QCOMPARE(manager.allocate(1), static_cast<void*>(0));
  // Free one block
  QVERIFY(manager.deallocate(buffers[1]));
  // Try to free blocks outside of the buffer
  QVERIFY(!manager.deallocate(reinterpret_cast<void*>(1)));
  // Try to free null pointer
  QVERIFY(manager.deallocate(0));
  QVERIFY(!manager.deallocate(static_cast<char*>(buffers[0]) + 10000));
  // Try to allocate too large block, should return 0
  QCOMPARE(manager.allocate(128), static_cast<void*>(0));
  // Allocate a block that should just fit.
  QCOMPARE(manager.allocate(128-sizeof(void*)), buffers[1]);
  // Full again...
  QCOMPARE(manager.allocate(128-sizeof(void*)), static_cast<void*>(0));

  // Deallocate all
  for (unsigned i=0; i<manager.blockCount(); ++i)
    QVERIFY(manager.deallocate(buffers[i]));

  // All should be free...
  for (unsigned i=0; i<manager.blockCount(); ++i)
    {
      buffers[i] = manager.allocate(10);
      QVERIFY(buffers[i] != 0);
      QCOMPARE(long(buffers[i]) & 0xf, 0l);
    }
}

void TestPiiSimpleMemoryManager::externalBuffer()
{
  void* bfr = malloc(200); // Will not be freed if a test fails, but who cares?
  PiiSimpleMemoryManager manager(bfr, 200, 50);
  QCOMPARE(manager.blockSize(), std::size_t(64 - sizeof(void*)));
  for (unsigned i=0; i<manager.blockCount(); ++i)
    QVERIFY(manager.allocate(50));
  QVERIFY(!manager.allocate(50));
  free(bfr);
}

class A
{
public:
  virtual ~A() {}
};

class B : public A
{
public:
  B(int value) : _iMember(value) {}

  void* operator new (std::size_t size)
  {
    void* ptr = _manager.allocate(size);
    if (ptr != 0) return ptr;
    return ::operator new(size);
  }

  void operator delete (void* ptr)
  {
    if (!_manager.deallocate(ptr))
      ::operator delete(ptr);
  }

  int member() const { return _iMember; }

private:
  int _iMember;
  static PiiSimpleMemoryManager _manager;
};

PiiSimpleMemoryManager B::_manager(Pii::alignAddress((sizeof(B)+sizeof(void*)),0xf)*16, sizeof(B));


void TestPiiSimpleMemoryManager::overriddenNewDelete()
{
  A* ptrs[17];
  for (int i=0; i<17; ++i)
    ptrs[i] = new B(i);
  for (int i=0; i<17; ++i)
    {
      QCOMPARE(static_cast<B*>(ptrs[i])->member(), i);
      delete ptrs[i];
    }
}

QTEST_MAIN(TestPiiSimpleMemoryManager)
