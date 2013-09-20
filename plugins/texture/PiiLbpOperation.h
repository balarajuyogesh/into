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

#ifndef _PIILBPOPERATION_H
#define _PIILBPOPERATION_H

#include <PiiDefaultOperation.h>
#include <QVector>
#include "PiiLbp.h"

/**
 * The LBP texture feature.
 *
 * Inputs
 * ------
 *
 * @in image - an image the LBP texture feature is extracted from. 
 * The image may be of any type. Color images are automatically
 * converted to gray scale before processing.
 *
 * @in roi - region-of-interest. See [PiiImagePlugin] for a
 * description. Optional.
 *
 * Outputs
 * -------
 *
 * @out features - all extracted features as a concatenated feature
 * vector. In `ImageOutput` mode, this output will emit the same
 * feature image as `features0`.
 *
 * @out featuresX - a feature vector that contains the extracted LBP
 * features or a feature image that contains the LBP feature code for
 * each pixel, depending on [outputType]. X stands for the Xth
 * parameter set (zero-based index). The size of the output image is
 * smaller than the size of the input image. By default, one output is
 * created and named features0. It outputs the 256-bin feature vector
 * for LBP 8,1 in Standard mode with nearest neighbor interpolation.
 *
 */
class PiiLbpOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * A list of parameters used in extracting the LBP features. If
   * multiple parameters are given, multiple feature vectors are
   * extracted for each input image. The parameters are represented
   * with
   * `samples,radius[,threshold][,mode][,interpolation][,smoothing]`. 
   * Samples is the number of samples taken around each pixel (int),
   * radius is the sampling radius (float), and threshold a "noise
   * canceller threshold" (float). Valid values for mode are 
   * `Uniform`, `RotationInvariant`, `UniformRotationInvariant`, and
   * `Symmetric`. Interpolation can be either `LinearInterpolation`
   * or `NearestNeighborInterpolation`. Smoothing can be either 
   * `Smoothed` or `NonSmoothed`. Everything but samples and radius can
   * be omitted. Default values are "Standard",
   * "NearestNeighborInterpolation", and "NonSmoothed". Threshold,
   * mode, interpolation, and smoothing can be given in any order.
   *
   * Notes
   * -----
   *
   * - If mode is "Symmetric", a "symmetric" LBP is used instead of
   * the traditional one. The symmetric version compares opposing
   * pairs of pixels in a neighborhood and doesn't consider the
   * center. See PiiLbp::genericSymmetricLbp() for details. The amount
   * of samples for the symmetric LBP must be even. If an odd number
   * is given, samples-1 will be used. The symmetric version cannot
   * make use of the threshold parameter.
   *
   * - If smoothing is set to "Smoothed", the input image will be
   * smoothed prior to applying the LBP operation. The smoothing is
   * performed with Pii::fastMovingAverage() using the distance
   * between neighborhood samples as the size of the smoothing window.
   * Formally, the window size is calculated as @f$s = \ceil{2 \pi R /
   * N}@f$, where *N* is the number of samples and *N* is the
   * neighborhood radius. Multi-scale LBP with smoothing can be seen
   * as a pyramid description of image texture.
   *
   * - The standard LBP, albeit being largely invariant against
   * illumination changes, is quite sensitive to noise on uniformly
   * colored surfaces. LBP's noise tolerance can be increased by
   * adding a constant value ("noise canceller threshold") to each
   * pixel before comparing it to the neighbors. This way, small
   * variations don't get captured. The downside is that one needs to
   * select a suitable threshold value, which is yet another parameter
   * to decide. Furthermore, the threshold makes LBP more dependent on
   * illumination changes.
   *
   * Examples of valid parameters:
   *
   * - 8,1
   * - 8,1,RotationInvariant
   * - 16,2,4,UniformRotationInvariant,LinearInterpolation
   * - 12,1.5,Uniform,Smoothed
   * - 8,1,LinearInterpolation
   * - 10,1.7,LinearInterpolation,Symmetric
   *
   * ~~~(c++)
   * lbpOperation.setProperty("parameters", QStringList() << "8,1" << "16,2,Uniform");
   * ~~~
   */
  Q_PROPERTY(QStringList parameters READ parameters WRITE setParameters);

  /**
   * A type that determines whether histograms or feature images will
   * be produced. The default value is `HistogramOutput`.
   */
  Q_PROPERTY(OutputType outputType READ outputType WRITE setOutputType);
  Q_ENUMS(OutputType);

  /**
   * The type or the `roi` input, if connected. The default value is
   * `AutoRoi`.
   */
  Q_PROPERTY(PiiImage::RoiType roiType READ roiType WRITE setRoiType);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:

  /**
   * Output types.
   *
   * - `HistogramOutput` - LBP codes are collected to a histogram and
   * output as a row vector (PiiMatrix<int>). In `HistogramOutput`
   * mode, a separate histogram operation is not needed.
   *
   * - `NormalizedHistogramOutput` - same as `HistogramOutput`, but
   * the histogram is normalized to sum up to unity. The output type
   * will be PiiMatrix<float>.
   *
   * - `ImageOutput` - the output will be an image (PiiMatrix<int>)
   * in which each pixel is replaced with its LBP code.
   */
  enum OutputType { HistogramOutput, NormalizedHistogramOutput, ImageOutput };

  /**
   * Create a new LBP operation.
   */
  PiiLbpOperation();
  ~PiiLbpOperation();

  QStringList parameters() const;
  void setParameters(const QStringList& parameters);

  OutputType outputType() const;
  void setOutputType(OutputType type);
  
  void setRoiType(PiiImage::RoiType roiType);
  PiiImage::RoiType roiType() const;

  void check(bool reset);

protected:
  void process();
  void aboutToChangeState(State state);

private:
  class AnyLbp;
  template <class T, class LbpType> class Lbp;
  friend class AnyLbp;
  template <class T, class LbpType> friend class Lbp;
  
  void createOutput(int samples, double radius, PiiLbp::Mode mode = PiiLbp::Standard,
                    Pii::Interpolation interpolation = Pii::NearestNeighborInterpolation,
                    bool smoothed = false, double threshold = 0);
  template <class T> inline void calculate(const PiiVariant& obj);
  template <class LbpType, class T> void calculate(const PiiMatrix<T>& image);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    ~Data();
    OutputType outputType;
    
    QStringList lstParameters;
    QList<PiiLbp*> lstOperators;
    QList<int> lstSmoothingWindows;
    QList<double> lstThresholds;
    bool bMustSmooth;
    
    QVector<bool> vecMustCalculate;
    bool bCompoundConnected;
    int iStaticOutputCount;
    
    PiiInputSocket* pRoiInput;
    PiiImage::RoiType roiType;
    AnyLbp* pLbp;
    unsigned int uiPreviousType;
  };
  PII_D_FUNC;
};


#endif //_PIILBPOPERATION_H
