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

#ifndef _PIIHOUGHTRANSFORMOPERATION_H
#define _PIIHOUGHTRANSFORMOPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiYdinTypes.h>
#include <PiiVariant.h>
#include "PiiHoughTransform.h"

/** 
 * Operation interface for Hough transform. Hough transform is a 
 * technique which can be used to find imperfect instances of
 * objects within a certain class of shapes by a voting procedure.
 *
 * This voting procedure is carried out in a parameter space,
 * from which object candidates are obtained as local maxima in a
 * so-called accumulator space that is explicitly constructed by the
 * choosed algorithm (for computing the Hough transform).
 *
 * This operation implements the classical Hough transform, which is
 * concerned with the identification of lines in the image. Hough
 * transform has been extended to identifying positions of arbitrary
 * shapes, most commonly circles or ellipses. See @ref
 * PiiHoughTransform for details.
 *
 * Inputs
 * ------
 *
 * @in image - an image to be transformed. Typically, the input
 * image is binary, but any gray-level image can be used. All non-zero
 * values in the input image will add to the transform. Higher values
 * have higher weight.
 *
 * Outputs
 * -------
 *
 * @out accumulator - the accumulator array. A PiiMatrix<int>, unless
 * the input is a floating-point type, in which case the accumulator
 * will be a PiiMatrix<float>.
 *
 * @out peaks - detected peaks in the transformation domain. The
 * output value is a PiiMatrix<double> with at most [maxPeakCount]
 * rows. Each row stores the location of a peak \((d, \theta)\),
 * where *d* is the distance to the center of the image and
 * \(\theta\) is the slope of the line. If there are less than
 * [maxPeakCount] peaks, the number of rows in the matrix matches the
 * number of peaks found. The rows of the matrix are ordered so that
 * the highest peak comes first.
 * 
 * @out coordinates - a PiiMatrix<int> containing the end points of
 * detected lines at image boundaries [maxPeakCount] highest peaks in
 * the transformation domain are considered. The matrix has 4 columns
 * containing the begin and end points of a detected line (x1, y1, x2,
 * y2). This output can be directly connected to the `property` input
 * of [PiiImageAnnotator].
 *
 */
class PII_TRANSFORMS_EXPORT PiiHoughTransformOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The angleResolution defines number of degrees each column
   * represents. The default value is 1.0 which produces 180 columns
   * in the result matrix.
   */
  Q_PROPERTY(double angleResolution READ angleResolution WRITE setAngleResolution);
  
  /**
   * The distanceResolution defines distance (in number of pixels)
   * what each row represents in the result matrix. Default value is 1.0
   */
  Q_PROPERTY(double distanceResolution READ distanceResolution WRITE setDistanceResolution);
  
  /**
   * The number of highest peaks detected in the accumulator. See the
   * `maxCnt` parameter of [PiiTransforms::findPeaks()]. Default
   * value is 1.
   */
  Q_PROPERTY(int maxPeakCount READ maxPeakCount WRITE setMaxPeakCount);

  /**
   * The minimum magnitude for a peak in the parameter space to be
   * considered a peak. See the `threshold` parameter of @ref
   * PiiTransforms::findPeaks(). Default value is 0. Getting this
   * value right usually requires experimenting.
   */
  Q_PROPERTY(double minPeakMagnitude READ minPeakMagnitude WRITE setMinPeakMagnitude);

  /**
   * The minimum distance between detected peaks in the transformation
   * domain. The default value is 1.
   *
   * @see PiiTransforms::findPeaks()
   */
  Q_PROPERTY(double minPeakDistance READ minPeakDistance WRITE setMinPeakDistance);  

  Q_PROPERTY(int startAngle READ startAngle WRITE setStartAngle);
  Q_PROPERTY(int endAngle READ endAngle WRITE setEndAngle);
  Q_PROPERTY(int startDistance READ startDistance WRITE setStartDistance);
  Q_PROPERTY(int endDistance READ endDistance WRITE setEndDistance);

  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiHoughTransformOperation();
  ~PiiHoughTransformOperation();

  void check(bool reset);

protected:
  void process();

  void setAngleResolution(double resolution);
  double angleResolution() const;

  void setDistanceResolution(double resolution);
  double distanceResolution() const;
  
  void setMaxPeakCount(int value);
  int maxPeakCount() const;
  void setStartAngle(int startAngle);
  int startAngle() const;
  void setEndAngle(int endAngle);
  int endAngle() const;
  void setStartDistance(int startDistance);
  int startDistance() const;
  void setEndDistance(int endDistance);
  int endDistance() const;
  void setMinPeakMagnitude(double minPeakMagnitude);
  double minPeakMagnitude() const;
  void setMinPeakDistance(double minPeakDistance);
  double minPeakDistance() const;

private:
  template <class T> void transform(const PiiVariant& obj);
  template <class T> void findPeaks(const PiiMatrix<T>& objects);

  template <class T> struct TransformTraits;
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    int iMaxPeakCount;
    double dMinPeakMagnitude;
    bool bPeaksConnected;
    PiiHoughTransform hough;
    double dMinPeakDistance;
  };
  PII_D_FUNC;
};

template <class T> struct PiiHoughTransformOperation::TransformTraits { typedef int Type; };
template <> struct PiiHoughTransformOperation::TransformTraits<float> { typedef float Type; };

#endif //_PIIHOUGHTRANSFORMOPERATION_H
