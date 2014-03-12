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

#ifndef _PIITHRESHOLDING_TEMPLATES_H
#define _PIITHRESHOLDING_TEMPLATES_H

#include <PiiMatrix.h>
#include "PiiHistogram.h"

namespace PiiImage
{
  template <class T> int otsuThreshold(const PiiMatrix<T>& histogram)
  {
    PiiMatrix<double> matVariances(1,histogram.columns()-1);
    PiiMatrix<double> matCumulative(cumulative(histogram));
    double* varptr = matVariances.row(0);

    for (int k=0; k<matVariances.columns(); k++)
      {
        double sum1 = 0, sum2 = 0, mu1 = 0, mu2 = 0, var1 = 0, var2 = 0;
        sum1 = matCumulative(0,k);

        if (sum1 != 0)
          {
            for ( int i=0; i<=k; i++ )
              mu1 += i*histogram(i);
            mu1 /= sum1;

            for ( int i=0; i<=k; i++ )
              {
                double temp = (i-mu1);
                var1 += temp*temp*histogram(i);
              }
          }

        if (sum1 != 1)
          {
            sum2 = 1.0-sum1;
            for (int i=k+1; i<histogram.columns(); i++)
              mu2 += i*histogram(i);
            mu2 /= sum2;

            for (int i=k+1; i<histogram.columns(); i++)
              {
                double temp = (i - mu2);
                var2 += temp*temp*histogram(i);
              }
          }
        varptr[k] = var1 + var2;
      }

    int th = 0;
    double min = matVariances(0);
    for (int c=1; c<matVariances.columns(); c++)
      if (varptr[c] < min)
        {
          min = varptr[c];
          th = c;
        }

    return th;
  }

  template <class Image, class IntegralImage, class PixelCounter, class BinaryFunction>
  PiiMatrix<typename BinaryFunction::result_type> adaptiveThresholdImpl(const Image& image,
                                                                        const IntegralImage& integral,
                                                                        const PixelCounter& counter,
                                                                        BinaryFunction func,
                                                                        int windowRows, int windowColumns)
  {
    typedef typename BinaryFunction::result_type T;
    typedef typename Image::const_row_iterator ImageRow;
    typedef typename IntegralImage::const_row_iterator IntegralRow;
    // Use at least float for the mean.
    typedef typename Pii::Combine<typename Image::value_type,float>::Type M;

    // Initialize result image
    PiiMatrix<T> matThresholded(PiiMatrix<T>::uninitialized(image.rows(), image.columns()));
    if (windowColumns <= 0)
      windowColumns = windowRows;
    int iHalfRows = windowRows/2, iHalfCols = windowColumns/2;
    const int iRows = image.rows(), iCols = image.columns();
    int c1, c2, r1, r2;

    IntegralRow prevRow, nextRow;
    T* pTarget;
    for (int r=0; r<iRows; ++r)
      {
        // Check image boundaries
        r1 = qMax(r-iHalfRows, 0);
        r2 = qMin(r+iHalfRows+1, iRows);
        prevRow = integral[r1];
        nextRow = integral[r2];
        pTarget = matThresholded[r];
        ImageRow pSource = image[r];
        for (int c=0; c<iCols; ++c)
          {
            c1 = qMax(c-iHalfCols, 0);
            c2 = qMin(c+iHalfCols+1, iCols);
            // Use the integral image to calculate moving average.
            pTarget[c] = func(M(pSource[c]),
                              M(nextRow[c2] + prevRow[c1] - nextRow[c1] - prevRow[c2]) /
                              counter.countPixels(r1, c1, r2, c2));
          }
      }
    return matThresholded;
  }

  template <class Image, class PixelCounter, class BinaryFunction>
  inline PiiMatrix<typename BinaryFunction::result_type> adaptiveThresholdImpl(const Image& image,
                                                                               const PixelCounter& counter,
                                                                               BinaryFunction func,
                                                                               int windowRows,
                                                                               int windowColumns)
  {
    typedef typename Pii::Combine<typename Image::value_type,int>::Type I;
    return adaptiveThresholdImpl(image,
                                 Pii::cumulativeSum<I,Image>(image, Pii::ZeroBorderCumulativeSum),
                                 counter,
                                 func,
                                 windowRows,
                                 windowColumns);
  }

  template <class T, class U=bool> struct RoiMaskFunction : Pii::BinaryFunction<T,U,T>
  {
    T operator() (T value, U mask) const { return mask ? value : T(0); }
  };

  // Uses cumulative sum of mask matrix to quickly count handled
  // pixels in a local window.
  template <class T=bool> struct RoiMaskPixelCounter
  {
    RoiMaskPixelCounter(const PiiMatrix<T>& mask) :
      matMaskSum(Pii::cumulativeSum<int>(mask & 1, Pii::ZeroBorderCumulativeSum))
    {}

    inline int countPixels(int r1, int c1, int r2, int c2) const
    {
      return matMaskSum(r2,c2) + matMaskSum(r1,c1) - matMaskSum(r2,c1) - matMaskSum(r1,c2);
    }

    PiiMatrix<int> matMaskSum;
  };

  struct DefaultPixelCounter
  {
    inline int countPixels(int r1, int c1, int r2, int c2) const { return (c2-c1) * (r2-r1); }
  };

  template <class Matrix, class BinaryFunction>
  PiiMatrix<typename BinaryFunction::result_type> adaptiveThreshold(const Matrix& image,
                                                                    BinaryFunction func,
                                                                    float relativeThreshold,
                                                                    float absoluteThreshold,
                                                                    int windowRows, int windowColumns)
  {
    return adaptiveThreshold(image,
                             adaptiveThresholdFunction(func, relativeThreshold, absoluteThreshold),
                             windowRows,
                             windowColumns);
  }

  template <class Matrix, class BinaryFunction>
  PiiMatrix<typename BinaryFunction::result_type> adaptiveThreshold(const Matrix& image,
                                                                    BinaryFunction func,
                                                                    int windowRows, int windowColumns)
  {
    return adaptiveThresholdImpl(image,
                                 DefaultPixelCounter(),
                                 func,
                                 windowRows,
                                 windowColumns);
  }

  template <class Matrix, class BinaryFunction>
  PiiMatrix<typename BinaryFunction::result_type> adaptiveThreshold(const Matrix& image,
                                                                    const PiiMatrix<bool>& roiMask,
                                                                    BinaryFunction func,
                                                                    int windowRows, int windowColumns)
  {
    return adaptiveThresholdImpl(Pii::binaryMatrixTransform(image.selfRef(),
                                                            roiMask,
                                                            RoiMaskFunction<typename Matrix::value_type>()),
                                 RoiMaskPixelCounter<bool>(roiMask),
                                 func,
                                 windowRows,
                                 windowColumns);
  }

  template <class TernaryFunction, class Matrix>
  PiiMatrix<typename TernaryFunction::result_type> adaptiveThresholdVar(const Matrix& image,
                                                                        TernaryFunction func,
                                                                        int windowRows, int windowColumns)
  {
    /* Variance s² = 1/N sum(x - u)²
     *
     * Ns² = sum(x - u)²
     *     = sum(x² - 2xu + u²)
     *     = sum(x²) - 2u sum(x) + N u²
     *     = sum(x²) - 2 sum(x)/N sum(x) + N (sum(x) / N)²
     *     = sum(x²) - 2 sum²(x) / N + sum²(x) / N
     *     = sum(x²) - sum²(x)/N
     *  s² = sum(x²)/N - sum²(x)/N² = sum(x²)/N - u²
     */

    typedef typename TernaryFunction::result_type T;
    typedef typename Matrix::const_row_iterator ImageRow;
    // Use at least int accuracy for the integral image
    typedef typename Pii::Combine<typename Matrix::value_type,int>::Type I;
    // Use at least long long accuracy for the integral image of squares
    typedef typename Pii::Combine<typename Matrix::value_type,long long>::Type I2;

    // Calculate an integral image
    PiiMatrix<I> matIntegral = Pii::cumulativeSum<I,Matrix>(image, Pii::ZeroBorderCumulativeSum);
    // Integral image of squares
    PiiMatrix<I2> matIntegral2 = Pii::cumulativeSum(image, Pii::Square<I2>(), Pii::ZeroBorderCumulativeSum);

    // Initialize result image
    PiiMatrix<T> matThresholded(PiiMatrix<T>::uninitialized(image.rows(), image.columns()));
    if (windowColumns <= 0)
      windowColumns = windowRows;
    int iHalfRows = windowRows/2, iHalfCols = windowColumns/2;
    const int iRows = image.rows(), iCols = image.columns();
    int c1, c2, r1, r2;

    const I* pPrevRow, *pNextRow;
    const I2* pPrevRow2, *pNextRow2;
    ImageRow source;
    T* pTarget;
    for (int r=0; r<iRows; ++r)
      {
        // Check image boundaries
        r1 = qMax(r-iHalfRows, 0);
        r2 = qMin(r+iHalfRows+1, iRows);
        pPrevRow = matIntegral[r1];
        pNextRow = matIntegral[r2];
        pPrevRow2 = matIntegral2[r1];
        pNextRow2 = matIntegral2[r2];
        pTarget = matThresholded[r];
        source = image[r];
        for (int c=0; c<iCols; ++c)
          {
            c1 = qMax(c-iHalfCols, 0);
            c2 = qMin(c+iHalfCols+1, iCols);
            int iCount = (c2-c1) * (r2-r1);
            double dMean = double(pNextRow[c2] + pPrevRow[c1] - pNextRow[c1] - pPrevRow[c2]) / iCount;
            double dVar = double(pNextRow2[c2] + pPrevRow2[c1] - pNextRow2[c1] - pPrevRow2[c2]) / iCount // sum(x²)/N
              - Pii::square(dMean);

            pTarget[c] = func(source[c],
                              dMean,
                              dVar);
          }
      }
    return matThresholded;
  }
}

#endif //_PIITHRESHOLDING_TEMPLATES_H
