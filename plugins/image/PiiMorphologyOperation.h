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

#ifndef _PIIMORPHOLOGYOPERATION_H
#define _PIIMORPHOLOGYOPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include <QSize>
#include "PiiImageGlobal.h"

/**
 * Basic binary morphology operations.
 *
 * @inputs
 *
 * @in image - the input image. Any gray-scale image. Zero is treated
 * as "false". Any value other than zero is considered "true".
 *
 * @outputs
 *
 * @out image - the image output. Output image is of the same data
 * type as the input image, but contains only ones and zeros.
 *
 * @ingroup PiiImagePlugin
 */
class PiiMorphologyOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Type of the structuring element. The default is @p RectangularMask.
   */
  Q_PROPERTY(PiiImage::MaskType maskType READ maskType WRITE setMaskType);
  
  /**
   * The type of a moprhological operation to be applied to input
   * images. The default value is @p Erode.
   */
  Q_PROPERTY(PiiImage::MorphologyOperation operation READ operation WRITE setOperation);

  /**
   * A flag that controls the behavior of border handling with
   * erosion. See PiiImage::erode() for details. The default value is
   * @p false.
   */
  Q_PROPERTY(bool handleBorders READ handleBorders WRITE setHandleBorders);
  
  /**
   * Size of the structuring element. Any size will do as far as the
   * mask is smaller than the input image. The default is 3x3.
   */
  Q_PROPERTY(QSize maskSize READ maskSize WRITE setMaskSize);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiMorphologyOperation();

  PiiImage::MorphologyOperation operation() const;
  void setOperation(PiiImage::MorphologyOperation operation);

  void setHandleBorders(bool handleBorders);
  bool handleBorders() const;

  PiiImage::MaskType maskType() const;
  void setMaskType(PiiImage::MaskType maskType);

  QSize maskSize() const;
  void setMaskSize(QSize maskSize);
  
  PiiMatrix<int> mask() const;
  template <class T> void setMask(PiiMatrix<T> mask);
  
protected:
  void process();
  
private:
  template <class T> void morphologyOperation(const PiiVariant& obj);

  void prepareMask();
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiImage::MorphologyOperation operation;
    PiiImage::MaskType maskType;
    bool bHandleBorders;
    QSize maskSize;
    
    PiiMatrix<int> matMask;
    
    PiiInputSocket* pImageInput;
    PiiOutputSocket* pBinaryImageOutput;
  };
  PII_D_FUNC;
};

#endif //_PIIMORPHOLOGYOPERATION_H
