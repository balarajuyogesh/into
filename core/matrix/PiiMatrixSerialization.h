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

#ifndef _PIIMATRIXSERIALIZATION_H
#define _PIIMATRIXSERIALIZATION_H

#include "PiiMatrix.h"
#include <PiiSmartPtr.h>
#include <PiiSerializationTraits.h>
#include <PiiNameValuePair.h>
#include <PiiBinaryObject.h>
#include <PiiSerializationException.h>
#include <PiiSerialization.h>

/// @hide

namespace PiiSerialization
{
  /**** Variable-size matrices ****/

  template <class Archive, class T>
  void save(Archive& archive, const PiiMatrix<T>& mat, const unsigned int /*version*/)
  {
    int iRows = mat.rows(), iCols = mat.columns();
    archive << PII_NVP("rows", iRows);
    archive << PII_NVP("cols", iCols);
    unsigned int uiBytes = iCols*sizeof(T);
    for (int r=0; r<iRows; ++r)
      archive.writeRawData(mat[r], uiBytes);
  }

  template <class Archive, class T>
  void load(Archive& archive, PiiMatrix<T>& mat, const unsigned int /*version*/)
  {
    int iRows, iCols;
    archive >> PII_NVP("rows", iRows);
    archive >> PII_NVP("cols", iCols);

    if (iRows < 0 || iCols < 0)
      PII_SERIALIZATION_ERROR(InvalidDataFormat);

    mat.resize(iRows, iCols);

    unsigned int uiBytes = iCols*sizeof(T);
    for (int r=0; r<iRows; ++r)
      archive.readRawData(mat[r], uiBytes);
  }

  template <class Archive, class T>
  inline void serialize(Archive& archive, PiiMatrix<T>& mat, const unsigned int version)
  {
    separateFunctions(archive, mat, version);
  }

  /**** Fixed-size matrices ****/

  template <class Archive, class T, int rowCount, int columnCount>
  void save(Archive& archive,
            const PiiMatrix<T, rowCount, columnCount>& mat,
            const unsigned int /*version*/)
  {
    archive.writeRawData(mat.begin(), sizeof(T) * rowCount * columnCount);
  }

  template <class Archive, class T, int rowCount, int columnCount>
  void load(Archive& archive,
            PiiMatrix<T, rowCount, columnCount>& mat,
            const unsigned int /*version*/)
  {
    archive.readRawData(mat.begin(), sizeof(T) * rowCount * columnCount);
  }

  template <class Archive, class T, int rowCount, int columnCount>
  inline void serialize(Archive& archive,
                        PiiMatrix<T, rowCount, columnCount>& mat,
                        const unsigned int version)
  {
    separateFunctions(archive, mat, version);
  }
}

/// @endhide

#endif //_PIIMATRIXSERIALIZATION_H
