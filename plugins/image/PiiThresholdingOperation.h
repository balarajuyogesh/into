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

#ifndef _PIITHRESHOLDINGOPERATION_H
#define _PIITHRESHOLDINGOPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>

/**
 * A threshold operation with one threshold value. Input image
 * must be grayscale image and output image has same type as input
 * image, but value of the output image can only be 0 or 1.
 *
 * Inputs
 * ------
 *
 * @in image - an image to be thresholded. Any image type. Color
 * images will be converted to gray scale before applying the
 * threshold.
 *
 * @in threshold - an optional threshold input. Any numeric type. The
 * final threshold will be calculated by multiplying the input value
 * by [relativeThreshold] and adding [absoluteThreshold], except if 
 * `HysteresisThreshold` is used. In this case the input will be used as
 * the upper threshold, [absoluteThreshold] will be ignored and
 * [relativeThreshold] will be added to the input to get the lower
 * threshold.
 *
 * Outputs
 * -------
 *
 * @out image - the image output. Emits output image which has same
 * type as input image, but values can only be 0 or 1. However, if the
 * input is a color image, the output will be a gray-level image. If
 * `HysteresisThreshold` is in use, the output will always be a
 * PiiMatrix<int>.
 *
 * @out threshold - the value of the threshold. (`double`)
 *
 */
class PiiThresholdingOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Absolute threshold. The effect of this value is affected by
   * [thresholdType]. Default value is 0.
   */
  Q_PROPERTY(double absoluteThreshold READ absoluteThreshold WRITE setAbsoluteThreshold);
  /**
   * Relative threshold. The effect of this value is affected by
   * [thresholdType]. Default value is 1.0.
   */
  Q_PROPERTY(double relativeThreshold READ relativeThreshold WRITE setRelativeThreshold);
  
  /**
   * How to determine the threshold. Default is `StaticThreshold`. 
   * This value has no effect if the threshold input is connected.
   */
  Q_PROPERTY(ThresholdType thresholdType READ thresholdType WRITE setThresholdType);
  Q_ENUMS(ThresholdType);

  /**
   * Inversion flag. If this value is set to `true`, the result will
   * be inverted. In all cases but with `HysteresisThreshold` the
   * "inverted" result will be same as the normal thresholding result
   * except that zeros and ones are exchanged. See @ref
   * PiiImage::inverseHysteresisThreshold() for details on hysteresis
   * thresholding.
   */
  Q_PROPERTY(bool inverse READ isInverse WRITE setInverse);

  /**
   * The size of a local window for adaptive treshold calculation. The
   * default is 15-by-15.
   */
  Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize);
 
  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  /**
   * Supported threshold calculation techniques.
   *
   * - `StaticThreshold` - the gray levels are thresholded using
   * [absoluteThreshold] as such. [relativeThreshold] will be ignored.
   *
   * - `OtsuThreshold` - initial threshold value is automatically
   * determined from histogram of the input image with the Otsu
   * method. The initial value will be multiplied by
   * [relativeThreshold], and [absoluteThreshold] will be added to the
   * resulting value before applying the operation. This thresholding
   * technique works only if the gray levels are quantized. Floating
   * point input will be rounded to integers, and the result will most
   * likely be totally black.
   *
   * - `RelativeToMeanThreshold` - the threshold is calculated by
   * multiplying the mean gray level of the input image by
   * [relativeThreshold] and adding [absoluteThreshold] to the result.
   *
   * - `RelativeToMaxThreshold` - the threshold is calculated by
   * multiplying the maximum gray level of the input image by
   * [relativeThreshold] and adding [absoluteThreshold] to the result.
   
   * - `RelativeToMinThreshold` - the threshold is calculated by
   * multiplying the minimum gray level of the input image by
   * [relativeThreshold] and adding [absoluteThreshold] to the result.
   *
   * - `MeanStdThreshold` - the threshold is calculated by adding
   * [relativeThreshold] times the standard deviation of the input gray
   * levels to the mean gray level. [absoluteThreshold] will be added to
   * the result.
   *
   * - `PercentageThreshold` - The image will be thresholded so that
   * a fixed fraction of pixels will always be ones. If
   * [relativeThreshold] is set to 0.7, 30 % of pixels will be ones
   * etc. [absoluteThreshold] will be added to the result. This
   * thresholding technique works only if the gray levels are
   * quantized. Floating point input will be rounded to integers, and
   * the result will most likely be totally black.
   *
   * - `TwoLevelThreshold` - Everything between [absoluteThreshold]
   * and [absoluteThreshold] + [relativeThreshold] (inclusive) will be
   * set to one. The `threshold` output will emit the value of
   * [absoluteThreshold].
   *
   * - `HysteresisThreshold` - [absoluteThreshold] will be used as an
   * upper threshold. All pixels with a gray level higher than this
   * value will work as seeds. The result will contain all pixels with
   * a gray level higher than [absoluteThreshold] - [relativeThreshold]
   * that are connected to at least one seed pixel. The `threshold`
   * output will emit the value of [absoluteThreshold]. The 
   * `threshold` input will be ignored.
   *
   * - `RelativeToMeanAdaptiveThreshold` - same as 
   * `RelativeToMeanThreshold`, but the threshold is calculated
   * separately for each pixel in a local window. The size of the
   * local window is determined by [windowSize]. The `threshold`
   * output will emit the value of [absoluteThreshold]. The 
   * `threshold` input will be ignored.
   *
   * - `MeanStdAdaptiveThreshold` - same as `MeanStdThreshold`, but
   * the threshold is calculated separately for each pixel in a local
   * window. The size of the local window is determined by
   * [windowSize]. The `threshold` output will emit the value of
   * [absoluteThreshold]. The `threshold` input will be ignored.
   *
   * - `SauvolaAdaptiveThreshold` - and adaptive thresholding
   * technique that takes local variance into account. The local
   * threshold *t* is calculated as @f$t = \mu (1 + r
   * (\sigma/\sigma_{\mathrm max} - 1)) + a@f$, where *r* stands for
   * [relativeThreshold], *a* for [absoluteThreshold], and
   * \(\sigma_{\mathrm max}\) for the maximum possible standard
   * deviation, which is assumed to be 128 for integer-typed images
   * and 0.5 for floating-point types. In document image binarization,
   * a good value for [relativeThreshold] is 0.34. [absoluteThreshold]
   * is typically zero. The `threshold` output will emit the value of
   * [absoluteThreshold]. The `threshold` input will be ignored.
   */
  enum ThresholdType
  {
    StaticThreshold,
    OtsuThreshold,
    RelativeToMeanThreshold,
    RelativeToMinThreshold,
    RelativeToMaxThreshold,
    MeanStdThreshold,
    PercentageThreshold,
    TwoLevelThreshold,
    HysteresisThreshold,
    RelativeToMeanAdaptiveThreshold,
    MeanStdAdaptiveThreshold,
    SauvolaAdaptiveThreshold
  };
  
  PiiThresholdingOperation();

  double absoluteThreshold() const;
  void setAbsoluteThreshold(double absoluteThreshold);

  void setRelativeThreshold(double relativeThreshold);
  double relativeThreshold() const;

  ThresholdType thresholdType() const;
  void setThresholdType(ThresholdType thresholdType);

  void check(bool reset);

  void setInverse(bool inverse);
  bool isInverse() const;
  void setWindowSize(const QSize& windowSize);
  QSize windowSize() const;
  
protected:
  void process();
  
private:
  template <class T> void thresholdColor(const PiiVariant& obj);
  template <class T> void thresholdGray(const PiiVariant& obj);
  template <class T> void threshold(const PiiMatrix<T>& image);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    double dAbsoluteThreshold;
    double dRelativeThreshold;
    ThresholdType thresholdType;
    bool bThresholdConnected;
    
    PiiInputSocket* pImageInput, *pThresholdInput;
    PiiOutputSocket* pBinaryImageOutput, *pThresholdOutput;
    bool bInverse;
    QSize windowSize;
  };
  PII_D_FUNC;
};

#endif //_PIITHRESHOLDINGOPERATION_H
