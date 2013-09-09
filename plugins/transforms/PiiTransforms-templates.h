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
# error "Never use <PiiTransforms-templates.h> directly; include <PiiTransforms.h> instead."
#endif

#include <PiiMatrixUtil.h>
#include <PiiImage.h>

namespace PiiTransforms
{
  inline void addPixel(int magnitude, PiiMatrix<int>& result, double r, double c)
  {
    int iR = Pii::round<int>(r), iC = Pii::round<int>(c);
    if (iR >= 0 && iR < result.rows() && iC >=0 && iC < result.columns())
      result(iR, iC) += magnitude;
  }

  template <class T> inline void addPixel(T magnitude, PiiMatrix<T>& result, double r, double c,
                                          typename Pii::OnlyFloatingPoint<T>::Type = 0)
  {
    if (r >= 0 && r < result.rows() && c >=0 && c < result.columns())
      Pii::setValueAt(magnitude, result, r, c);
  }
  
 
  template <class T, class Selector, class U>
  PiiMatrix<U> circularHough(const PiiMatrix<T>& gradientX,
                             const PiiMatrix<T>& gradientY,
                             Selector select,
                             U radius,
                             double angleError,
                             GradientSign sign)
  {
    const int iRows =  gradientX.rows();
    const int iCols =  gradientX.columns();
    const bool bPositive = sign member_of (PositiveGradient, IgnoreGradientSign);
    const bool bNegative = sign member_of (NegativeGradient, IgnoreGradientSign);

    int iArcLength = 1;
    double dAngleStep = 0, dSinAlpha = 0, dCosAlpha = 0;

    if (angleError > 0)
      {
        dAngleStep = 1.0 / radius;
        iArcLength = Pii::round<int>(angleError * radius);
        // Rotation matrix coefficients
        dSinAlpha = sin(dAngleStep);
        dCosAlpha = cos(dAngleStep);
      }
    
    PiiMatrix<U> matResult(iRows, iCols);

    for (int r=0; r<iRows; ++r)
      {
        const T* pX = gradientX[r];
        const T* pY = gradientY[r];
        
        for (int c=0; c<iCols; ++c)
          {
            double dMagnitude = Pii::hypotenuse<double>(pX[c], pY[c]);
            U magnitude = Pii::round<U>(dMagnitude);
            if (!select(magnitude))
              continue;

            // Form direction vector.
            double dX = pX[c] / dMagnitude * radius;
            double dY = pY[c] / dMagnitude * radius;

            // Each edge point in the input adds two points to the
            // transform. (In the gradient direction and its
            // opposite.)
            if (bPositive)
              addPixel(magnitude, matResult, r + dY, c + dX);
            if (bNegative)
              addPixel(magnitude, matResult, r - dY, c - dX);
            
            // If an estimate of the gradient error is given, draw two
            // arcs to the transformation domain.
            if (iArcLength > 1)
              {
                double dX1 = dX, dX2 = dX, dY1 = dY, dY2 = dY;
                for (int i=1; i<iArcLength; ++i)
                  {
                    // Rotate the direction vector clockwise ...
                    double dXTmp = dCosAlpha * dX1 - dSinAlpha * dY1;
                    dY1 = dSinAlpha * dX1 + dCosAlpha * dY1;
                    dX1 = dXTmp;

                    // ... and counter-clockwise
                    dXTmp = dCosAlpha * dX2 + dSinAlpha * dY2;
                    dY2 = dCosAlpha * dY2 - dSinAlpha * dX2;
                    dX2 = dXTmp;

                    if (bPositive)
                      {
                        addPixel(magnitude, matResult, r + dY1, c + dX1);
                        addPixel(magnitude, matResult, r + dY2, c + dX2);
                      }
                    if (bNegative)
                      {
                        addPixel(magnitude, matResult, r - dY1, c - dX1);
                        addPixel(magnitude, matResult, r - dY2, c - dX2);
                      }
                  }
              }
          }
      }
    return matResult;
  }

  template <class T, class Selector, class U>
  QList<PiiMatrix<U> > circularHough(const PiiMatrix<T>& image,
                                     Selector select,
                                     U startRadius,
                                     U endRadius,
                                     U radiusStep,
                                     GradientSign sign)
  {
    typedef typename Pii::Combine<int,T>::Type GradType;
    if (startRadius > endRadius)
      qSwap(startRadius, endRadius);
    if (radiusStep <= 0)
      radiusStep = endRadius - startRadius;
    
    QList<PiiMatrix<U> > lstResult;
    PiiMatrix<GradType> matX(PiiImage::filter<GradType>(image, PiiImage::SobelXFilter));
    PiiMatrix<GradType> matY(PiiImage::filter<GradType>(image, PiiImage::SobelYFilter));
    while (startRadius <= endRadius)
      {
        lstResult << circularHough(matX, matY, select, startRadius, M_PI/64, sign);
        if (radiusStep == 0)
          break;
        startRadius += radiusStep;
      }
    return lstResult;
  }

  template <class T> inline void putValue(PiiHeap<T>& heap, T value) { heap.put(value); }
  template <class T> inline void putValue(QList<T>& lst, T value) { lst.append(value); }
  
  template <class T, class PeakList, class Threshold>
  void findPeaks(const PiiMatrix<T>& mat, PeakList& lst, Threshold threshold)
  {
    const int iRows = mat.rows(), iCols = mat.columns();
    const T *pPrevRow = 0, *pRow = 0, *pNextRow = mat[0];
    for (int r=0; r<iRows; ++r)
      {
        pRow = pNextRow;
        if (r < iRows-1)
          pNextRow = mat[r+1];
        for (int c=0; c<iCols; ++c)
          {
            // Accept only local maxima (4-connected)
            if (threshold(pRow[c]) &&
                (c == 0 || pRow[c] >= pRow[c-1]) &&
                (c == iCols-1 || pRow[c] >= pRow[c+1]) &&
                (r == 0 || pRow[c] >= pPrevRow[c]) &&
                (r == iRows-1 || pRow[c] >= pNextRow[c]))
              putValue(lst, PiiMatrixValue<T>(pRow[c], r, c));
          }
        pPrevRow = pRow;
      }
  }

  template <class T>
  void removeDuplicates(QList<PiiMatrixValue<T> >& peaks, double minDistance)
  {
    minDistance *= minDistance;

    // O(N²) algorithm.
    // PENDING With a very large number of peaks, a k-d tree would be faster.

    // NOTE to keep things simple, this doesn't handle the case where
    // neighboring peaks have equal magnitude "correctly". In such a
    // case it could be possible to find a combination of retained
    // peaks that would fill the minDistance criterium. Instead, the
    // peak with a smaller list index is retained.

    // Check peaks from largest to smallest
    for (int i=0; i<peaks.size(); ++i)
      {
        // Remove every peak that is too close to this one.
        for (int j=peaks.size()-1; j>i; --j)
          {
            if (Pii::square(peaks[j].row - peaks[i].row) +
                Pii::square(peaks[j].column - peaks[i].column) < minDistance)
              peaks.removeAt(j);
          }
      }
  }

  template <class T>
  QList<PiiMatrixValue<T> > findPeaks(const PiiMatrix<T>& mat, double minDistance, int maxCnt, T threshold)
  {
    QList<PiiMatrixValue<T> > lstResult;

    if (mat.isEmpty())
      return lstResult;

    // If the maximum number of peaks is limited, use a priority queue
    // as the list type.
    if (maxCnt > 0)
      {
        // Fill the heap initially with the smallest possible value.
        // If minDistance is specified, reserve some room for the
        // special case where a neighborhood has two peaks with the
        // same magnitude.
        PiiHeap<PiiMatrixValue<T> > heap(minDistance > 1 ? maxCnt * 2 : maxCnt,
                                         PiiMatrixValue<T>(Pii::Numeric<T>::minValue()),
                                         Pii::InverseHeap);
        // Find local maxima in the input matrix
        PiiMatrix<T> matFiltered(PiiImage::maxFilter(mat, qMax(int(minDistance*2+1), 3)));
        // Retain only local maxima
        matFiltered(matFiltered != mat) = 0;
        
        if (threshold > 0)
          findPeaks(matFiltered, heap, std::bind2nd(std::greater<T>(), threshold));
        else
          findPeaks(matFiltered, heap, Pii::YesFunction<T>());
        heap.sort();
        
        // Copy values to the result list
        lstResult.reserve(heap.size());
        for (int i=0; i<heap.size(); ++i)
          {
            if (heap[i].value != Pii::Numeric<T>::minValue())
              lstResult << heap[i];
            else
              break;
          }
        if (minDistance > 1)
          {
            removeDuplicates(lstResult, minDistance);
            if (lstResult.size() > maxCnt)
              lstResult.erase(lstResult.begin() + maxCnt, lstResult.end());
          }
      }
    // Find every peak that is larger than the given threshold
    else
      {
        // If neither maxCnt nor threshold is given, select threshold automatically
        if (threshold <= 0)
          threshold = Pii::max(mat) / 2;
        // In a degenerate case the algorithm would treat every pixel
        // as a peak.
        if (threshold >= 0)
          {
            findPeaks(mat, lstResult, std::bind2nd(std::greater<T>(), threshold));
            qSort(lstResult);
          }
        if (minDistance > 1)
          removeDuplicates(lstResult, minDistance);
      }
    
    return lstResult;
  }

  // circle is always larger than any of the existing circles
  template <class T> void addToList(const HoughCircle<T>& circle,
                                    double tolerance,
                                    QList<HoughCircle<T> >& circles,
                                    int cnt)
  {
    for (int i=0; i<cnt; ++i)
      {
        T rDiff = circle.radius - circles[i].radius;
        // If these two circles are almost the same...
        if (rDiff < tolerance &&
            Pii::hypotenuse(circle.x - circles[i].x, circle.y - circles[i].y) + rDiff < tolerance)
          {
            // If the existing one is weaker, replace it
            if (circles[i].magnitude < circle.magnitude)
              circles[i] = circle;
            return;
          }
      }
    // No other circle was close enough -> append to list
    circles << circle;
  }

  // Sorts circles in descending order based on their magnitude
  template <class T> struct CircleGreater
  {
    bool operator() (const HoughCircle<T>& c1, const HoughCircle<T>& c2) const { return c1.magnitude > c2.magnitude; }
  };

  template <class T>
  QList<HoughCircle<T> > findCircles(const QList<PiiMatrix<T> >& transformations,
                                     T startRadius,
                                     T radiusStep,
                                     double tolerance,
                                     int maxCnt,
                                     T threshold)
  {
    QList<HoughCircle<T> > lstCircles;
    /* Counts the total number of accepted circles. Circles are added
       directly to the result list, but only those added from a
       previous transformation domain need to be taken into account,
       because removeDuplicates() takes care of the ones with the same
       radius.
    */
    int iAcceptedCircleCnt = 0;
    for (int i=0; i<transformations.size(); ++i, startRadius += radiusStep)
      {
        QList<PiiMatrixValue<T> > lstPeaks(findPeaks(transformations[i], tolerance, maxCnt, threshold));
        for (int j=0; j<lstPeaks.size(); ++j)
          {
            HoughCircle<T> circle(lstPeaks[j].column, lstPeaks[j].row, startRadius, lstPeaks[j].value);
            // PENDING could be optimized to avoid comparisons to
            // circles whose radius is known to be out of reach
            // anyway.
            if (i == 0 || radiusStep > tolerance)
              lstCircles << circle;
            else
              addToList(circle, tolerance, lstCircles, iAcceptedCircleCnt);
          }
        iAcceptedCircleCnt = lstCircles.size();
      }
    // Ensure at most maxCnt circles will be returned
    if (maxCnt != 0 && lstCircles.size() > maxCnt)
      {
        qSort(lstCircles.begin(), lstCircles.end(), CircleGreater<T>());
        lstCircles.erase(lstCircles.begin() + maxCnt, lstCircles.end());
      }
    // If neither maxCnt nor threshold is given, make sure no peak
    // lower than max/2 is accepted.
    else if (threshold == 0 && maxCnt == 0)
      {
        T maxMagnitude(0);
        for (int i=0; i<lstCircles.size(); ++i)
          if (lstCircles[i].magnitude > maxMagnitude)
            maxMagnitude = lstCircles[i].magnitude;
        maxMagnitude /= 2;
        for (int i=lstCircles.size(); i--; )
          if (lstCircles[i].magnitude < maxMagnitude)
            lstCircles.removeAt(i);
      }
    return lstCircles;
  }
}
