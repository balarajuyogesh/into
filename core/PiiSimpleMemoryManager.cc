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

#include "PiiSimpleMemoryManager.h"
#include <PiiBits.h>
#include <QMutexLocker>
#include <cstdlib>

/* Memory arrangement
 *
 *   +- head
 *   |
 *   v
 * +-+-------+-+-------+-+-------+-+ ... +-------+-+
 * | | block |p| block |p| block |p|     | block |p|
 * +-+-------+-+-------+-+-------+-+ ... +-------+-+
 *            |^        |^        |      ^
 *            ||        ||        |      |
 *            ++        ++        +------+
 *
 * Each memory block is followed by a void* that points to the
 * beginning of the next free memory block. Each memory block is
 * aligned at a 16-byte boundary. In the beginning, empty space may be
 * left for proper alignment. The head pointer always points to the
 * first free memory block.
 */

#define PII_NEXT_POINTER(buffer) \
  reinterpret_cast<void**>(static_cast<char*>(buffer) + stBlockSize)

class PiiSimpleMemoryManager::Data
{
public:
  Data(void* memory, bool own, std::size_t memorySize, std::size_t blockSize) :
    // Align blocks to 16-byte boundaries
    stBlockSize(Pii::alignAddress(blockSize + sizeof(void*), 0xf)),
    bOwn(own),
    pMemory(memory),
    pHead(0) // Initialize the manager as fully allocated
  {
    // Align first block
    std::size_t stStartAddress = reinterpret_cast<std::size_t>(Pii::alignAddress(pMemory, 0xf));
    // Bytes available after alignment
    std::size_t stBytesAvailable = reinterpret_cast<std::size_t>(pMemory) + memorySize - stStartAddress;
    // Total number of blocks
    ulBlockCount = ulong(stBytesAvailable/stBlockSize);
    // Start of last block
    pLastAddress = reinterpret_cast<void*>(stStartAddress + (ulBlockCount-1) * stBlockSize);

    std::size_t stFullBlockSize = stBlockSize;
    // This is the number of bytes available to the user
    stBlockSize -= sizeof(void*);

    // Optimization...
    stLastAddress = std::size_t(pLastAddress) - std::size_t(pMemory);

    // Since head is now zero, the buffer is full. We need to release
    // all memory blocks.
    char* pBuffer = static_cast<char*>(pLastAddress);
    for (ulong i=0; i<ulBlockCount; ++i, pBuffer -= stFullBlockSize)
      deallocate(pBuffer);
  }

  ~Data()
  {
    if (bOwn) std::free(pMemory);
  }

  void* allocate(std::size_t bytes);
  bool deallocate(void* ptr);

  std::size_t stBlockSize;
  bool bOwn;
  void *pMemory, *pLastAddress, *pHead;
  ulong ulBlockCount;
  std::size_t stLastAddress;
  QMutex mutex;
};

PiiSimpleMemoryManager::PiiSimpleMemoryManager(std::size_t memorySize, std::size_t blockSize) :
  d(new Data(std::malloc(memorySize), true, memorySize, blockSize)) // Allocate memory block from heap
{}

PiiSimpleMemoryManager::PiiSimpleMemoryManager(void* memory, std::size_t memorySize, std::size_t blockSize) :
  d(new Data(memory, false, memorySize, blockSize))
{}

PiiSimpleMemoryManager::~PiiSimpleMemoryManager()
{
  delete d;
}

void* PiiSimpleMemoryManager::allocate(std::size_t bytes)
{
  // No need to allocate anything.
  if (bytes == 0)
    return 0;

  QMutexLocker lock(&d->mutex);
  return d->allocate(bytes);
}

void* PiiSimpleMemoryManager::Data::allocate(std::size_t bytes)
{
  // Do we have free blocks? Can we allocate this many bytes?
  if (pHead != 0 && bytes <= stBlockSize)
    {
      void* pBuffer = pHead;
      // Move the head pointer to the next free block
      pHead = *PII_NEXT_POINTER(pHead);
      return pBuffer;
    }
  return 0;
}

bool PiiSimpleMemoryManager::deallocate(void* buffer)
{
  if (buffer == 0)
    return true;
  QMutexLocker lock(&d->mutex);
  return d->deallocate(buffer);
}

bool PiiSimpleMemoryManager::Data::deallocate(void* buffer)
{
  //if (buffer >= pMemory && buffer <= pLastAddress)
  if (std::size_t(buffer) - std::size_t(pMemory) <= stLastAddress)
    {
      // Make the pointer at the end of the released buffer to point
      // to the old head.
      *PII_NEXT_POINTER(buffer) = pHead;
      // Mark the just released buffer as the new head.
      pHead = buffer;
      return true;
    }
  return false;
}

ulong PiiSimpleMemoryManager::blockCount() const
{
  return d->ulBlockCount;
}

std::size_t PiiSimpleMemoryManager::blockSize() const
{
  return d->stBlockSize;
}
