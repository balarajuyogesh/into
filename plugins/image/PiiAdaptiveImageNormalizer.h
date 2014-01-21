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

#ifndef _PIIADAPTIVEIMAGENORMALIZER_H
#define _PIIADAPTIVEIMAGENORMALIZER_H

#include <PiiDefaultOperation.h>
#include <PiiFunctional.h>

/**
 * Normalizes an image so that its local average is approximately
 * constant everywhere. This is done by calculating the local average
 * around each pixel and adjusting the pixel's gray level so that it
 * is relatively as far from the target mean as it originally was from
 * the real mean.
 *
 * The operation prevents overflows by cutting the gray levels at 255
 * in 8-bit images. Other data types will not be cut.
 *
 * Inputs
 * ------
 *
 * @in image - any image. Color images will be normalized
 * channel-wise.
 *
 * Outputs
 * -------
 *
 * @out image - normalized image, same type as input
 *
 */
class PiiAdaptiveImageNormalizer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The size of the adaptation window. The smaller the window, the
   * more sensitive the operation is to local changes. The default
   * value is 64-by-64.
   */
  Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize);

  /**
   * The wanted local mean value. If this value is NAN (the default),
   * 127 will be used for integer images and 0.5 for floating-point
   * images.
   */
  Q_PROPERTY(double targetMean READ targetMean WRITE setTargetMean);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiAdaptiveImageNormalizer();

protected:
  void process();

  void setWindowSize(const QSize& windowSize);
  QSize windowSize() const;

  void setTargetMean(double targetMean);
  double targetMean() const;

private:
  template <class T> void normalizeColor(const PiiVariant& obj);
  template <class T> void normalizeGray(const PiiVariant& obj);
  template <class T> PiiMatrix<T> normalize(const PiiMatrix<T>& obj);
  template <class T> struct Normalizer;

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    QSize windowSize;
    double dTargetMean;
  };
  PII_D_FUNC;
};


#endif //_PIIADAPTIVEIMAGENORMALIZER_H
