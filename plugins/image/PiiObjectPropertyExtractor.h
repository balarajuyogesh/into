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

#ifndef _PIIOBJECTPROPERTYEXTRACTOR_H
#define _PIIOBJECTPROPERTYEXTRACTOR_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include <QPair>

/**
 * Calculates geometric properties for labeled objects.
 *
 * @inputs
 *
 * @in image - a labeled image. Each object in the image must have a
 * unique integer label that distinguishes it from others. (any
 * integer-valued matrix)
 *
 * @in labels - the number of distinct objects. This input is
 * optional. (int)
 *
 * @outputs
 *
 * @out areas - the number of pixels on each object. 
 * PiiMatrix<int>(N,1).
 *
 * @out centroids - the center-of-mass point (x,y) for each object. 
 * PiiMatrix<int>(N,2).
 *
 * @out boundingboxes - The bounding boxes of each object
 * (x,y,width,height). PiiMatrix<int>(N,4).
 *
 * @ingroup PiiImagePlugin
 */
class PiiObjectPropertyExtractor : public PiiDefaultOperation
{
  Q_OBJECT

  PII_OPERATION_SERIALIZATION_FUNCTION
public:

  PiiObjectPropertyExtractor();

protected:
  void process();
  
private:
  template <class T> void operate(const PiiVariant& img, int labels);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiInputSocket* pLabeledImageInput;
    PiiInputSocket* pLabelsInput;
    PiiOutputSocket* pAreasOutput;
    PiiOutputSocket* pCentroidsOutput;
    PiiOutputSocket* pBoundingBoxOutput;
  };
  PII_D_FUNC;
};

#endif //_PIIOBJECTPROPERTYEXTRACTOR_H
