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

#include "PiiMatrixData.h"
#include <cstdlib>
#include <cstring>
#include <new>

PiiMatrixData* PiiMatrixData::sharedNull()
{
  static PiiMatrixData nullData;
  return &nullData;
}

PiiMatrixData* PiiMatrixData::allocate(int rows, int columns, size_t stride)
{
  void* bfr = malloc(sizeof(PiiMatrixData) + rows * stride);
  return new (bfr) PiiMatrixData(rows, columns, stride);
}

PiiMatrixData* PiiMatrixData::reallocate(PiiMatrixData* d, int rows)
{
  // This may move the contents of d into a new memory location
  d = static_cast<PiiMatrixData*>(realloc(d, sizeof(PiiMatrixData) + rows * d->iStride));
  // If the data buffer is internal, we need to fix the data pointer
  if (d->bufferType == InternalBuffer)
    d->pBuffer = d->bufferAddress();
  return d;
}

void PiiMatrixData::destroy()
{
  if (bufferType == ExternalOwnBuffer)
    free(pBuffer);
  else if (pSourceData != 0)
    pSourceData->release();
  free(this);
}

PiiMatrixData* PiiMatrixData::createUninitializedData(int rows, int columns, size_t bytesPerRow, size_t stride)
{
  if (stride < bytesPerRow)
    stride = alignedWidth(bytesPerRow);
  PiiMatrixData* pData = allocate(rows, columns, stride);
  pData->bufferType = InternalBuffer;
  if (rows*columns != 0)
    pData->pBuffer = pData->bufferAddress();
  else
    pData->pBuffer = 0;
  return pData;
}

PiiMatrixData* PiiMatrixData::createInitializedData(int rows, int columns, size_t bytesPerRow, size_t stride)
{
  PiiMatrixData* pData = createUninitializedData(rows, columns, bytesPerRow, stride);
  memset(pData->pBuffer, 0, pData->iStride * rows);
  return pData;
}

PiiMatrixData* PiiMatrixData::createReferenceData(int rows, int columns, size_t stride, void* buffer)
{
  PiiMatrixData* pData = allocate(0, columns, stride);
  pData->iRows = rows;
  pData->iCapacity = rows;
  pData->bufferType = ExternalBuffer;
  pData->pBuffer = buffer;
  return pData;
}

PiiMatrixData* PiiMatrixData::clone(int capacity, size_t bytesPerRow)
{
  PiiMatrixData* pData;
  int iNewRows = qMax(capacity, iRows);
  // If this is not a submatrix, retain the full width.
  if (pSourceData == 0)
    pData = createUninitializedData(iNewRows, iColumns, iStride, iStride);
  // Submatrices are truncated to minimum (aligned) width when cloning.
  else
    pData = createUninitializedData(iNewRows, iColumns, bytesPerRow);

  // Equal strides -> can copy the full contents at once
  if (pData->iStride == iStride)
    memcpy(pData->pBuffer, pBuffer, iStride * iRows);
  // Need to copy each row separately
  else
    {
      for (int i=0; i<iRows; ++i)
        memcpy(pData->row(i), row(i), bytesPerRow);
    }
  pData->iRows = iRows;
  return pData;
}
