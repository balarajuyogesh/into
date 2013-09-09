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

#ifndef _PIIIMAGESCALEOPERATION_H
#define _PIIIMAGESCALEOPERATION_H

#include <PiiDefaultOperation.h>

/**
 * Scale images to arbitrary sizes. The operation supports linear and
 * nearest neighbor interpolation.
 *
 * @inputs
 *
 * @in image - Input image. Any image type.
 * 
 * @outputs
 *
 * @out image - Scaled image. Same type as the input.
 *
 * @ingroup PiiImagePlugin
 */
class PiiImageScaleOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Scaling mode. Default is @p ScaleAccordingToFactor.
   */
  Q_PROPERTY(ScaleMode scaleMode READ scaleMode WRITE setScaleMode);
  Q_ENUMS(ScaleMode);

  /**
   * Scaling factor. Default is 1.0. The scale ratio also works as the
   * target aspect ratio, depending on @ref scaleMode. The scale ratio
   * is calculated as width/height. Thus, 2 means an image whose width
   * is two times its height.
   */
  Q_PROPERTY(double scaleRatio READ scaleRatio WRITE setScaleRatio);
  /**
   * Defines the size of output image. Default is (100,100).
   */
  Q_PROPERTY(QSize scaledSize READ scaledSize WRITE setScaledSize);

  /**
   * Interpolation mode. The default is @p LinearInterpolation. @p
   * NearestNeighborInterpolation is faster, but less accurate.
   */
  Q_PROPERTY(Interpolation interpolation READ interpolation WRITE setInterpolation);
  Q_ENUMS(Interpolation);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * A copy of Pii::Interpolation. (Stupid moc.)
   */
  enum Interpolation { NearestNeighborInterpolation, LinearInterpolation };
  
  /**
   * Scaling modes:
   *
   * @lip ScaleAccordingToFactor - scale both dimensions according to
   * the specified scale ratio (@ref scaleRatio). Retains original
   * aspect ratio.
   *
   * @lip ScaleToSize - scales to the specified size (@ref
   * scaledSize), ignoring original aspect ratio.
   *
   * @lip ScaleToAspectRatioX - scales to a specified aspect ratio
   * (@ref scaleRatio). Stretches/shrinks the image horizontally to
   * obtain this aspect ratio.
   *
   * @lip ScaleToAspectRatioY - same as above, but stretches image
   * vertically.
   *
   * @lip ScaleDownToAspectRatio - same as above, but scales the
   * direction that results in a smaller output image.
   *
   * @lip ScaleUpToAspectRatio - same as above, but scales the
   * direction that results in a larger output image.
   *
   * @lip ScaleToBox - scales either up or down so the the result fits
   * to a box whose size is determined by #scaledSize. Retains aspect
   * ratio.
   *
   * @lip ScaleToFillBox - scales either up or down so the the result
   * fills a box whose size is determined by #scaledSize. Retains
   * aspect ratio.
   *
   * @lip ScaleToPixelCount - scales the image so that it has about
   * #scaledSize.width() * #scaledSize.height() pixels. Retains aspect
   * ratio, which may cause variations in resulting pixel count.
   *
   * Aspect ratio is always calculated as width/height.
   */
  enum ScaleMode
    {
      ScaleAccordingToFactor,
      ScaleToSize,
      ScaleToAspectRatioX,
      ScaleToAspectRatioY,
      ScaleDownToAspectRatio,
      ScaleUpToAspectRatio,
      ScaleToBox,
      ScaleToFillBox,
      ScaleToPixelCount
    };
  
  PiiImageScaleOperation();

protected:
  void process();

  ScaleMode scaleMode() const;
  void setScaleMode(ScaleMode scaleMode);
  
  double scaleRatio() const;
  void setScaleRatio(double scaleRatio);

  QSize scaledSize() const;
  void setScaledSize(QSize scaledSize);

  Interpolation interpolation() const;
  void setInterpolation(Interpolation interpolation);

private:
  template <class T> void scaleImage(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    ScaleMode scaleMode;
    double dScaleRatio;
    QSize scaledSize;
    Interpolation interpolation;
  };
  PII_D_FUNC;
};

#endif //_PIIIMAGESCALEOPERATION_H
