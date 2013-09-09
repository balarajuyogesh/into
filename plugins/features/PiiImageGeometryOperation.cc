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

#include "PiiImageGeometryOperation.h"

#include <PiiYdinTypes.h>

const char* PiiImageGeometryOperation::_pFeatureNames[PII_IMAGE_GEOMETRY_OPERATION_FEATURECNT] = {
  "Width",
  "Height",
  "Area",
  "AspectRatio"
};

PiiImageGeometryOperation::PiiImageGeometryOperation() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("features"));
}

QStringList PiiImageGeometryOperation::features() const
{
  QStringList result;
  for (int i=0; i<PII_IMAGE_GEOMETRY_OPERATION_FEATURECNT; i++)
    result << _pFeatureNames[i];
  return result;
}


void PiiImageGeometryOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_ALL_MATRIX_CASES(calculateFeatures, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiImageGeometryOperation::calculateFeatures(const PiiVariant& obj)
{
  const PiiMatrix<T> img = obj.valueAs<PiiMatrix<T> >();
  double h = img.rows(), w = img.columns();

  emitObject(PiiMatrix<double>(1,4, w, h, w*h, w/h));
}
