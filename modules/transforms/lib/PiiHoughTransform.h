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

#ifndef _PIIHOUGHTRANSFORM_H
#define _PIIHOUGHTRANSFORM_H

#include "PiiTransformsGlobal.h"
#include <PiiMatrix.h>
#include <PiiMathDefs.h>
#include <PiiFunctional.h>
#include <PiiSharedD.h>

/**
 * Linear Hough transform. The linear Hough transform is used in
 * detecting lines in images. Lines are paramerized by their slope and
 * distance to the origin, resulting in a two-dimensional
 * transformation domain. In the transformation domain, columns
 * represent slopes and rows distances to the origin. The origin of
 * the domain is at the center of the image. The quantization of the
 * transformation domain is user-specifiable.
 *
 * The number of rows in the result will always be odd. The row at the
 * middle always represents lines that intersect the origin (i.e.
 * distance zero). If `distanceResolution` is 1.0 (the default),
 * moving up or down from the middle row increases distance to the
 * origin by one pixel. The first column represents lines with a zero
 * slope. If `angleResolution` is 1.0 (the default), the next column
 * represents lines with a 1 degree slope and so on. Zero angle points
 * downwards, and the angle grows clockwise. This peculiar choice was
 * made to make it easy to calculate the resulting lines in image
 * coordinates (x is columns and y is rows, positive y axis
 * downwards).
 *
 * Formally, the detected lines are parametrized with the following
 * equation:
 *
 * \[
 * d = x\cos \theta + y\sin \theta
 * \]
 *
 * where \(\theta\) stands for the angle, and d is the "distance"
 * from origin (d can be negative). If you pick a value from the
 * transform domain, its row and column coordinates tell the values
 * of d and \(\theta\), respectively. Once these are known, it
 * is straightforward to solve x with respect to y or vice versa.
 *
 * Note that the transform adds the value of a transformed pixel to
 * the transformation domain (a.k.a. the accumulator). If the input
 * image is binary, each pixel will have an equal effect. Higher
 * values can be used in giving higher significance to certain
 * pixels.
 *
 * @param angleResolution the number of degrees each column
 * represents. The default value is 1.0 which produces 180 columns
 * in the result matrix.
 *
 * @param distanceResolution the distance (in pixels) each row
 * represents.
 *
 * @param angleStart the start angle of the transformation domain.
 * The default value is zero, which places zero angle to the
 * leftmost column. This value can be negative.
 *
 * @param angleEnd the end angle of the transformation domain. This
 * value with `angleStart` can be used to limit the line search to
 * a certain range of angles. Note that the last angle will not be
 * present in the transformation. That is, if `angleEnd` is 180
 * (the default) and `angleResolution` is 1, the last angle will be
 * 179.
 *
 * @param distanceStart the smallest (signed) distance to the origin
 * considered in the transformation. If the distance is smaller than
 * the minimum possible distance, the theoretical minimum will be
 * used.
 *
 * @param distanceEnd the largest (signed) distance to the origin
 * considered in the transformation. If the distance is larger than
 * the maximum possible distance, the theoretical maximum will be
 * used.
 *
 *
 * ~~~(c++)
 * PiiMatrix<int> img; // fill somehow...
 * // Make the Hough transform. The result will be a PiiMatrix<int>,
 * // and each pixel with a value higher than or equal to three will
 * // be transformed. The angles will be quantized to 90 discrete
 * // levels (2 degrees each)
 *
 * double distanceResolution = 1.0;
 * double angleResolution = 2.0;
 * PiiMatrix<int> result =
 *   PiiHoughTransform(distanceResolution,
 *                     angleResolution).transform(img,
 *                                                std::bind2nd(std::greater<int>(), 3));
 * ~~~
 */
class PII_TRANSFORMS_EXPORT PiiHoughTransform
{
public:
  PiiHoughTransform();
  PiiHoughTransform(double angleResolution,
                    double distanceResolution,
                    int startAngle = 0,
                    int endAngle = 180,
                    int startDistance = Pii::Numeric<int>::minValue(),
                    int endDistance = Pii::Numeric<int>::maxValue());

  PiiHoughTransform(const PiiHoughTransform& other);

  ~PiiHoughTransform();

  PiiHoughTransform& operator= (const PiiHoughTransform& other);

  void setAngleResolution(double angleResolution);
  double angleResolution() const;

  void setDistanceResolution(double distanceResolution);
  double distanceResolution() const;

  void setStartAngle(int startAngle);
  int startAngle() const;

  void setEndAngle(int endAngle);
  int endAngle() const;

  void setStartDistance(int startDistance);
  int startDistance() const;

  void setEndDistance(int endDistance);
  int endDistance() const;

  /**
   * Returns the distance from the center of the input image, given
   * the index of a *row* in the transformation result.
   */
  double distance(int row) const;
  /**
   * Returns the angle (degrees) corresponding to the given *column*
   * in the transformation result.
   */
  double angle(int column) const;

  /**
   * Returns the points at which the line represented by the given
   * point in the accumulator hits the input image's borders.
   *
   * @param d the distance of the line to the transform's origin
   *
   * @param theta the angle of the line, in degrees.
   *
   * @return a 1-by-4 matrix storing the start and end points of a
   * line segment (x1, y1, x2, y2). The returned value is suitable for
   * use with PiiImageAnnotator's `property` input.
   *
   * ~~~(c++)
   * // Transform an image
   * PiiHoughTransform hough;
   * PiiMatrix<int> matTransformed(hough.transform(img));
   * // Find 10 highest peaks in the transformation domain
   * PiiHeap<PiiMatrixValue<int> > maxima = Pii::findMaxima(matTransformed, 10);
   * PiiMatrix<int> matPoints(0,4);
   * for (int i=0; i<maxima.size(); ++i)
   *   matPoints.insertRow(hough.lineEnds(maxima[i].row, maxima[i].column));
   * ~~~
   *
   * @see Pii::findMaxima()
   */
  PiiMatrix<int> lineEnds(int row, int column) const;

  /**
   * @param img the input image. The image will be scanned, and each
   * pixel that makes `rule` evaluate `true` will be added to
   * the parameter space. Typically, the input image is binary.
   *
   * @param rule the is unary function which is used to determine if a
   * pixel may be part of a line. The function takes the pixel value
   * as a parameter and returns a `bool`.
   *
   * @return the accumulator array. The size of the returned matrix
   * depends on angle and distance limits and their resolution.
   */
  template <class T, class Matrix, class UnaryOp>
  PiiMatrix<T> transform(const Matrix& img, UnaryOp rule);

  template <class T, class Matrix>
  PiiMatrix<T> transform(const Matrix& img)
  {
    return transform<T>(img, Pii::Identity<typename Matrix::value_type>());
  }

protected:
  /// @internal
  class Data : public PiiSharedD<Data>
  {
  public:
    Data();
    Data(double angleResolution,
         double distanceResolution,
         int startAngle,
         int endAngle,
         int startDistance,
         int endDistance);
    Data(const Data& other);
    ~Data();

    double dAngleResolution;
    double dDistanceResolution;
    int iStartAngle;
    int iEndAngle;
    int iStartDistance;
    int iEndDistance;
    int iRows, iColumns;
    double dMaxDistance;
    double *pSinTable, *pCosTable;
    int iPreviousAngles, iPreviousStartAngle;
    double dPreviousAngleResolution;
  } *d;
  PII_SHARED_D_FUNC;

private:
  void setSize(int rows, int columns);
  void initSinCosTables(int angles);
  const double* sinTable() const;
  const double* cosTable() const;
};

#include "PiiHoughTransform-templates.h"

#endif //_PIIHOUGHTRANSFORM_H
