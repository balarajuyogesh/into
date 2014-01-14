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

#ifndef _PIIMATRIXDATA_H
#define _PIIMATRIXDATA_H

#include <PiiGlobal.h>
#include <PiiAtomicInt.h>

/// @internal
struct PII_CORE_EXPORT PiiMatrixData
{
  enum BufferType { InternalBuffer, ExternalBuffer, ExternalOwnBuffer };

  // Constructs a null data
  PiiMatrixData() :
    iRefCount(1),
    iLastRef(1),
    iRows(0),
    iColumns(0),
    iStride(0),
    iCapacity(0),
    pSourceData(0),
    pBuffer(0),
    bufferType(InternalBuffer)
  {}
  
  PiiMatrixData(int rows, int columns, size_t stride) :
    iRefCount(1),
    iLastRef(1),
    iRows(rows),
    iColumns(columns),
    iStride(stride),
    iCapacity(rows),
    pSourceData(0)
  {}
  
  PiiAtomicInt iRefCount;
  // Destroy data when iRefCount goes below this value. Default is
  // one. Setting this value to two and increasing iRefCount by one
  // makes referenced data immutable.
  int iLastRef;
  int iRows, iColumns;
  // Number of bytes between beginnings of successive rows.
  size_t iStride;
  // Size of allocated buffer in rows
  int iCapacity;
  // Points to the source data if this matrix is a subwindow of
  // another matrix.
  PiiMatrixData* pSourceData;
  // Points to the first element of the matrix.
  void* pBuffer;
  BufferType bufferType;

  void* row(int index) { return static_cast<char*>(pBuffer) + iStride * index; }
  const void* row(int index) const { return static_cast<const char*>(pBuffer) + iStride * index; }

  // Aligns row width to a four-byte boundary
  static size_t alignedWidth(size_t bytes) { return (bytes + 3) & ~3; }
  // Returns a pointer to the beginning of an internally allocated buffer.
  char* bufferAddress() { return reinterpret_cast<char*>(this) + sizeof(*this); }

  void reserve() { iRefCount.ref(); }
  void release() { if (iRefCount-- == iLastRef) destroy(); }

  PiiMatrixData* clone(int capacity, size_t bytesPerRow);

  PiiMatrixData* makeImmutable()
  {
    // Increasing refcount forces all non-const functions to clone the
    // data.
    iRefCount.ref();
    iLastRef = 2;
    return this;
  }
  
  static PiiMatrixData* sharedNull();
  static PiiMatrixData* allocate(int rows, int columns, size_t stride);
  static PiiMatrixData* reallocate(PiiMatrixData* d, int rows);
  static PiiMatrixData* createUninitializedData(int rows, int columns, size_t bytesPerRow, size_t stride = 0);
  static PiiMatrixData* createInitializedData(int rows, int columns, size_t bytesPerRow, size_t stride = 0);
  static PiiMatrixData* createReferenceData(int rows, int columns, size_t stride, void* buffer);

  void destroy();
};

#endif //_PIIMATRIXDATA_H
