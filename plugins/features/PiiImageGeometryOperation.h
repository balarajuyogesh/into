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

#ifndef _PIIIMAGEGEOMETRYOPERATION_H
#define _PIIIMAGEGEOMETRYOPERATION_H

#include <PiiDefaultOperation.h>

#define PII_IMAGE_GEOMETRY_OPERATION_FEATURECNT 4

/**
 * Calculates simple features based on image geometry.
 *
 * @inputs
 *
 * @in image - input image. Any matrix.
 * 
 * @outputs
 *
 * @out features - a feature vector (1x4 PiiMatrix<double>) containing
 * the width, height, area (width*height) and aspect ratio
 * (width/height) of the image.
 *
 * @ingroup PiiFeaturesPlugin
 */
class PiiImageGeometryOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Return all the features that are supported by this operation. In
   * the current version of the operation the supported features can only be
   * read, not set.
   */
  Q_PROPERTY(QStringList features READ features);  

  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiImageGeometryOperation();
  
  QStringList features() const;
 
protected:
  void process();

private:
  template <class T> void calculateFeatures(const PiiVariant& obj);

  static const char* _pFeatureNames[PII_IMAGE_GEOMETRY_OPERATION_FEATURECNT];
};

#endif //_PIIIMAGEGEOMETRYOPERATION_H
