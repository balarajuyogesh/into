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

#ifndef _PIIMASKGENERATOR_H
#define _PIIMASKGENERATOR_H

#include <PiiDefaultOperation.h>
#include <QPolygon>
#include <PiiQImage.h>
#include <PiiColor.h>
#include "PiiImageGlobal.h"

/**
 * An operation that creates binary mask images. The masks can be
 * used, for example, for ROI processing.
 *
 * Inputs
 * ------
 *
 * @in trigger - mask matrix is emitted whenever any object is
 * received in this input. The input value is ignored. Either this or
 * `size` needs to be connected.
 *
 * @in size - the size of the mask matrix as a PiiMatrix<int>. If the
 * size of the matrix is 1-by-4, the input is treated as a rectangle
 * (x,y,w,h) whose width and height are used as the mask size. If the
 * size is 1-by-2, the input is treated as a size (w,h). Either this
 * or `trigger` needs to be connected.
 *
 * Outputs
 * -------
 *
 * @out mask - a binary mask (PiiMatrix<unsigned char>)
 *
 */
class PiiMaskGenerator : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The type of mask to generate. Default is `EllipticalMask`.
   */
  Q_PROPERTY(PiiImage::MaskType maskType READ maskType WRITE setMaskType);

  /**
   * The size of the mask to be generated. If the `size` input is
   * connected, this value will be ignored. If the size is set to
   * (0,0) (the default), the size of the mask image will be
   * automatically determined. With polygons, a mask will be generated
   * that just encloses all the polygons. With other mask types, a
   * 1-by-1 mask with a single one will be created.
   */
  Q_PROPERTY(QSize maskSize READ maskSize WRITE setMaskSize);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiMaskGenerator();

  void setMaskType(PiiImage::MaskType maskType);
  PiiImage::MaskType maskType() const;
  void setMaskSize(QSize maskSize);
  QSize maskSize() const;

  void check(bool reset);

protected:
  void process();

private:
  PiiMatrix<unsigned char> createMask(int rows, int columns);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiMatrix<unsigned char> maskMatrix;
    PiiImage::MaskType maskType;
    QSize maskSize;
  };
  PII_D_FUNC;
};


#endif //_PIIMASKGENERATOR_H
