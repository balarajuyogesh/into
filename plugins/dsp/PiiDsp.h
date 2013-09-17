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

#ifndef _PIIDSP_H
#define _PIIDSP_H

#include <PiiMatrix.h>
#include <PiiMath.h>
#include <QList>
#include <complex>
#include "PiiDspGlobal.h"

namespace PiiDsp
{
  /**
   * Filtering modes for convolution and relatives.
   *
   * - `FilterFull` - full convolution/correlation. The size of the
   * convolution between matrices A and B whose sizes are denoted by a
   * and b is a+b-1 (A*B -> a+b-1).
   *
   * - `FilterOriginalSize` - retain the size of the original matrix. 
   * (A*B -> a)
   *
   * - `FilterValidPart` - return only those parts that can be
   * computed without zero-padding. (A*B -> a-b+1, if a>b. Otherwise,
   * an empty matrix is returned.)
   */
  enum FilterMode { FilterFull, FilterOriginalSize, FilterValidPart };

  /*
   * Methods for performing operations related to convolution and
   * correlation.
   */

  /**
   * Two-dimensional convolution of two matrices. The size of
   * the resulting matrix will be (M+O-1)x(N+P-1) if the input
   * matrices are of sizes MxN and OxP. Make sure b is not larger
   * than a.
   *
   * @param a source matrix
   *
   * @param b convolution filter
   *
   * @param mode filtering mode
   *
   * ~~~
   * PiiMatrix<int> a(1,5, 1,2,3,4,5);
   * PiiMatrix<int> b(1,3, -1, 0, 1);
   * PiiMatrix<int> result = PiiDsp::convolution<int>(a, b);
   * // result = -1 -2 -2 -2 -2 4 5
   * ~~~
   */
  template <class ResultType, class T, class U>
  PiiMatrix<ResultType> convolution(const PiiMatrix<T>& a,
                                    const PiiMatrix<U>& b,
                                    FilterMode mode = FilterFull);
  
  
  /**
   * Two-dimensional correlation of two matrices. The
   * size of the resulting matrix will be (M+O-1)x(N+P-1) if
   * the input matrices are of sizes MxN and OxP. Make sure
   * that b is smaller than a. 
   *
   * @param a source matrix
   *
   * @param b correlation filter
   *
   * @param mode filtering mode
   *
   * @see convolution()
   */
  template <class ResultType, class T, class U>
  PiiMatrix<ResultType> correlation(const PiiMatrix<T>& a,
                                    const PiiMatrix<U>& b,
                                    FilterMode mode = FilterFull);

  /**
   * Normalized cross-correlation of two-dimensional signals. Works
   * analogously to [correlation()], but normalizes the source signal
   * `a` so that its local sum over the area covered by the
   * correlation mask *b* is always one. In normalized correlation,
   * changing the level or scaling of the source signal will not
   * affect the correlation result.
   */
  template <class ResultType, class T, class U>
  PiiMatrix<ResultType> normalizedCorrelation(const PiiMatrix<T>& a,
                                              const PiiMatrix<U>& b,
                                              FilterMode mode = FilterFull);


  /**
   * Two-dimensional filter of two matrices. The size of the resulting
   * matrix will be (M+O-1)x(N+P-1) if the input matrices are of sizes
   * MxN and OxP. Make sure that b is smaller than a. The type of the
   * filter determines output type.
   *
   * @param a source matrix
   *
   * @param b convolution filter
   *
   * @param mode filtering mode
   */ 
  template <class ResultType, class T, class U>
  PiiMatrix<ResultType> filter(const PiiMatrix<T>& a,
                               const PiiMatrix<U>& b,
                               FilterMode mode = FilterFull);

  /**
   * A structure that stores information about a peak detected by
   * [findPeaks()].
   */
  struct Peak
  {
    /**
     * Create a default peak structure with all values initialized to
     * zero.
     */
    Peak() :
      dataIndex(0), position(0), height(0), width(0)
    {}
    
    /**
     * Construct a new peak with the given data index, estimate
     * position, height, and width.
     */
    Peak(int i, double pos, double h, double w) :
      dataIndex(i), position(pos), height(h), width(w)
    {}

    /**
     * The index of the measurement in input data that triggered peak
     * detection.
     */
    int dataIndex;
    /**
     * The estimated real position of the peak (x coordinate).
     */
    double position;
    /**
     * The estimated real height of the peak (y coordinate).
     */
    double height;
    /**
     * The estimated width of the peak.
     */
    double width;
  };
  
  /**
   * Find peaks in noisy one-dimensional data. This function locates
   * and measures positive peaks in a noisy sequence of measurements
   * by looking for sign changes in the smoothed first derivative of
   * the input data. The exact location of the peak and its width are
   * estimated by fitting a parabola to measurements around the zero
   * crossing.
   *
   * @param data a row vector containing the measurements (e.g. a time
   * series). If `data` is a 2-by-N matrix, the first row will be
   * treated as the x coordinate, and the second row as the y
   * coordinate. If the data only contains the y coordinate (1-by-N
   * row vector), the x is assumed to be in [0,N-1].
   *
   * @param levelThreshold the mimimum value for a measurement to be
   * regarded as a peak. This value cuts off peaks that are not high
   * enough.
   *
   * @param sharpnessThreshold the minimum relative value of the
   * second derivative that will trigger peak detection. The relative
   * derivative is calculated by dividing the second derivative by the
   * corresponding measurement. This value cuts off peaks that are not
   * sharp enough.
   *
   * @param smoothWidth the width of a smoothing window that will be
   * applied to the derivative before peak detection. Set to 1 or
   * smaller to disable smoothing.
   *
   * @param windowWidth the number of measurements considered in
   * fitting the local parabola for accurate peak position. If this
   * value is smaller than five, no fitting will be performed and no
   * width estimate will be stored in the results. The width estimate
   * becomes unreliable with a small window width.
   *
   * @return a list of detected peaks or an empty list if no peaks
   * were found. An empty list will be returned also if the peaks
   * cannot be detected with the given parameters.
   */
  PII_DSP_EXPORT QList<Peak> findPeaks(const PiiMatrix<double>& data,
                                       double levelThreshold,
                                       double sharpnessThreshold = 1e-3,
                                       int smoothWidth = 5,
                                       int windowWidth = 7);

  /**
   * Swap opposing halves or quadrands of a matrix. This effectively
   * moves the origin of a Fourier spectrum to the center of the
   * matrix. Two successive calls restore the original matrix. If the
   * matrix has an odd number of rows or columns, the original state
   * will only be restored if the `inverse` flag is set to true when
   * shifting back. The figure below shows the matrix quadrants before
   * (left) and after (right) shifting.
   *
@verbatim
+---+---+      +---+---+
| 1 | 2 |      | 4 | 3 |
+---+---+  ->  +---+---+
| 3 | 4 |      | 2 | 1 |
+---+---+      +---+---+
@endverbatim
   *
   * @param matrix the matrix to be shifted
   *
   * @param inverse a flag that tells the direction of the shift. This
   * is needed for matrices with an odd number of rows and columns.
   *
   * @return shifted matrix
   */
  template <class T> PiiMatrix<T> fftShift(const PiiMatrix<T>& matrix, bool inverse = false);
}

#include "PiiDsp-templates.h"

#endif //_PIIDSP_H
