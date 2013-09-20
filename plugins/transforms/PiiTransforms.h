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

#ifndef _PIITRANSFORMS_H
#define _PIITRANSFORMS_H

#include <PiiMatrix.h>
#include <PiiMath.h>
#include <PiiMatrixValue.h>
#include <PiiHeap.h>

#include "PiiTransformsGlobal.h"

/**
 * A namespace for functions that transform images into different
 * types of domains.
 *
 */
namespace PiiTransforms
{
  /**
   * Finds peaks in the Hough transform domain. This function differs
   * from Pii::findMaxima() in that it considers only local maxima and
   * doesn't return peaks that are too close to each other in the
   * transformation domain. It also gives the possibility to specify a
   * threshold for detecting the peaks.
   *
   * @param transformation the result of a Hough transform
   *
   * @param minDistance the minimum distance between peaks. If peaks
   * are closer than this value, only the larger will survive. Setting
   * *minDistance* less than 1 has no effect.
   *
   * @param maxCnt the maximum number of highest peaks to return. If
   * this value is zero, every peak exceeding *threshold* will be
   * returned.
   *
   * @param threshold the minimum magnitude for a peak. If this value
   * is non-positive, and maxCnt is zero, max(transformation)/2 will
   * be used as the threshold.
   *
   * @return a list of detected peaks
   */
  template <class T>
  QList<PiiMatrixValue<T> > findPeaks(const PiiMatrix<T>& transformation,
                                      double minDistance,
                                      int maxCnt,
                                      T threshold = 0);

  /**
   * A function object that selects pixels whose gradient magnitude is
   * higher than or equal to *threshold*.
   *
   * @see circularHough()
   */
  struct ThresholdSelector
  {
    ThresholdSelector(double threshold) : _dThreshold(threshold) {}

    template <class T> bool operator() (T magnitude) const { return magnitude >= _dThreshold; }
    
  private:
    double _dThreshold;
  };

  /**
   * A function object that randomly selects pixels whose gradient
   * magnitude is higher than or equal to *threshold*.
   *
   * @see circularHough()
   */
  struct RandomSelector : ThresholdSelector
  {
    RandomSelector(double threshold, double selectionProbability) :
      ThresholdSelector(threshold),
      _iLimit(Pii::round<int>(selectionProbability * RAND_MAX))
    {}
    
    template <class T> bool operator() (T magnitude) const
    {
      return ThresholdSelector::operator() (magnitude) &&
        rand() < _iLimit;
    }
    
  private:
    int _iLimit;
  };

  enum GradientSign { PositiveGradient = 1, NegativeGradient = 2, IgnoreGradientSign = 3 };
  
  /**
   * Circular Hough transform. The circular Hough transform detects
   * circles in images. Circles are parametrized by their center (x,y)
   * and radius (r), resulting in a three-dimensional transform
   * domain. This function calculates a slice of the three-dimensional
   * domain specific to a known radius r.
   *
   * @param gradientX estimated magnitude of horizontal image
   * gradient. (e.g. image filtered with the `SobelX` filter)
   *
   * @param gradientX estimated magnitude of vertical image gradient
   * (e.g. image filtered with the `SobelX` filter).
   *
   * @param selector a function that is used to accept or reject
   * pixels. Use ThresholdSelector to choose pixels based on the
   * gradient magnitude only. Use RandomSelector if every pixel is not
   * necessarily needed.
   *
   * @param radius the radius of the circles to be found. If this
   * value is a `double`, the returned matrix will also be a double,
   * and a floating-point (more accurate) algorithm will be used. If
   * this value is an `int`, a faster integer algorithm will be used.
   *
   * @param angleError an esimate of angular error in the calculated
   * gradients. Setting `angleError` to a non-zero value will
   * increase accuracy but also slow down the algorithm.
   *
   * @return the transformation domain. Each pixel in the
   * transformation domain stores the number of times it was accessed
   * during the algorithm. With the floating point version, the counts
   * are weighted by gradient magnitude. A high value means that a
   * circle with the given *radius* is centered at that pixel with a
   * high likelihood.
   *
   * ~~~(c++)
   * PiiMatrix<unsigned char> matInput; // input image
   * PiiMatrix<GradType> matGradX(PiiImage::filter<int>(matInput, PiiImage::SobelXFilter));
   * PiiMatrix<GradType> matGradY(PiiImage::filter<int>(matInput, PiiImage::SobelYFilter));
   * using namespace PiiTransforms;
   * PiiMatrix<int> matTransform(circularHough(matGradX, matGradY,
   *                                           ThresholdSelector(5.0),
   *                                           5,
   *                                           0.01));
   * ~~~
   */
  template <class T, class Selector, class U>
  PiiMatrix<U> circularHough(const PiiMatrix<T>& gradientX,
                             const PiiMatrix<T>& gradientY,
                             Selector select,
                             U radius,
                             double angleError,
                             GradientSign sign = IgnoreGradientSign);
  
  /**
   * This version uses the Sobel edge detector to first estimate
   * gradient in *image*. Then, it applies circularHough() to all
   * radii in [*startRadius*, *endRadius*] in *radiusStep* steps.
   *
   * @return a transformation domain for each inspected radius
   *
   * ~~~(c++)
   * using namespace PiiTransforms;
   * QList<PiiMatrix<int> > lstMatrices(circularHough(matInput,
   *                                                  RandomSelector(1.0, 0.1), // Selects one tenth of pixels
   *                                                  10, 20, 2)); // Scan radii from 10 to 20 in 2 pixel steps
   * ~~~
   */
  template <class T, class Selector, class U>
  QList<PiiMatrix<U> > circularHough(const PiiMatrix<T>& image,
                                     Selector select,
                                     U startRadius,
                                     U endRadius,
                                     U radiusStep = 1,
                                     GradientSign sign = IgnoreGradientSign);

  template <class T> struct HoughCircle
  {
    HoughCircle(T xVal = 0, T yVal = 0, T r = 0, T m = 0) :
      x(xVal), y(yVal), radius(r), magnitude(m)
    {}

    /**
     * The x coordinate of the center.
     */
    T x;
    /**
     * The y coordinate of the center.
     */
    T y;
    /**
     * The radius of the circle.
     */
    T radius;
    /**
     * The magnitude of the circle in Hough transform domain.
     */
    T magnitude;
  };

  /**
   * Finds separate circles in a list of circular Hough transform
   * result.
   *
   * @param transformations a list of transformation matrices
   * calculated with circularHough().
   *
   * @param startRadius the radius of the first transformation matrix
   *
   * @param radiusStep the difference in radii between successive
   * transformation matrices.
   *
   * @param tolerance a minimum distance between circles. If two
   * circles have the same radius, their centers must be this far
   * apart. In general, the distance between centers plus the
   * difference between radii is used as the measure of difference.
   *
   * @param maxCnt the maximum number of circles to find. If this
   * value is zero, all circles exceeding threshold will be returned. 
   * A non-zero value means at most maxCnt strongest circles will be
   * returned.
   *
   * @param threshold the minimum magnitude for a circle.
   */
  template <class T>
  QList<HoughCircle<T> > findCircles(const QList<PiiMatrix<T> >& transformations,
                                     T startRadius,
                                     T radiusStep,
                                     double tolerance,
                                     int maxCnt,
                                     T threshold);
}

#include "PiiTransforms-templates.h"

#endif //_PIITRANSFORMS_H
