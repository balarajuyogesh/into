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

#include "PiiImage.h"
#include <PiiMatrixUtil.h>

namespace PiiImage
{
  PiiMatrix<int> sobelX(3, 3,
                        -1, 0, 1,
                        -2, 0, 2,
                        -1, 0, 1);
  PiiMatrix<int> sobelY(3, 3,
                        -1,-2,-1,
                        0, 0, 0,
                        1, 2, 1);

  PiiMatrix<int> robertsX(2, 2,
                          1, 0,
                          0, -1);
  PiiMatrix<int> robertsY(2, 2,
                          0, 1,
                          -1, 0);

  PiiMatrix<int> prewittX(3, 3,
                          -1, 0, 1,
                          -1, 0, 1,
                          -1, 0, 1);
  PiiMatrix<int> prewittY(3, 3,
                          -1,-1,-1,
                          0, 0, 0,
                          1, 1, 1);

  PiiMatrix<double> makeGaussian(unsigned int size)
  {
    size |= 1; // make odd
    PiiMatrix<double> matResult(PiiMatrix<double>::uninitialized(size,size));
    int iCenter = size / 2;
    // Cut the filter at 2 times std
    double dSigma = double(iCenter) / 2.0;
    double dExponentScale = -0.5 / (dSigma * dSigma);
    double dSum = 0;
    for (int r = -iCenter; r <= iCenter; ++r)
      for (int c = -iCenter; c <= iCenter; ++c)
        {
          double dVal = exp((r * r + c * c) * dExponentScale);
          dSum += dVal;
          matResult(r + iCenter, c + iCenter) = dVal;
        }
    matResult /= dSum;
    return matResult;
  }

  PiiMatrix<double> makeLoGaussian(unsigned int size)
  {
    size |= 1; // make odd
    PiiMatrix<double> matResult(PiiMatrix<double>::uninitialized(size,size));
    int iCenter = size / 2;
    // Cut the filter at 3 times std
    double dSigma = double(iCenter) / 3.0;
    double dExponentScale = -0.5 / (dSigma * dSigma);
    double dMean = 0;
    for (int r = -iCenter; r <= iCenter; ++r)
      for (int c = -iCenter; c <= iCenter; ++c)
        {
          double dExponent = dExponentScale * (c * c + r * r);
          double dVal = (1 + dExponent) * exp(dExponent);
          dMean += dVal;
          matResult(r + iCenter, c + iCenter) = dVal;
        }
    dMean /= size * size;
    // The maximum value is now 1 at the center.
    matResult -= dMean;
    // Max is now 1 - dMean -> scale so that the center is -4.
    matResult *= -4.0 / (1.0 - dMean);
    return matResult;
  }

  static inline bool nonZeroSums(int iSum, double dSum)
  {
    return iSum != 0 && !Pii::almostEqualRel(dSum, 0.0) &&
      Pii::sign(iSum) == Pii::sign(dSum);
  }

  PiiMatrix<int> intFilter(const PiiMatrix<int>& image,
                           const PiiMatrix<double>& doubleFilter,
                           Pii::ExtendMode mode,
                           double scale)
  {
    if (scale == 0)
      {
        double dMax = Pii::maxAbs(doubleFilter);
        if (dMax != 0)
          scale = 256.0 / dMax;
      }

    PiiMatrix<int> integerFilter(Pii::round<int>(doubleFilter * scale));
    int iSum = Pii::sum<int>(integerFilter);
    double dSum = Pii::sum<double>(doubleFilter);
    //qDebug("%d %lg", iSum, dSum);
    if (nonZeroSums(iSum, dSum))
      scale = iSum / dSum;
    PiiMatrix<int> filtered = filter<int>(image, integerFilter, mode);
    filtered.map(Pii::unaryCompose(Pii::Round<double,int>(),
                                   std::bind2nd(std::multiplies<double>(),
                                                1.0/scale)));
    return filtered;
  }

  PiiMatrix<int> intFilter(const PiiMatrix<int>& image,
                           const PiiMatrix<double>& horizontalFilter,
                           const PiiMatrix<double>& verticalFilter,
                           Pii::ExtendMode mode,
                           double scale)
  {
    if (horizontalFilter.rows() != 1 || verticalFilter.columns() != 1)
      return image;

    double hMax = Pii::maxAbs(horizontalFilter), vMax = Pii::maxAbs(verticalFilter);
    if (hMax == 0) hMax = 1;
    if (vMax == 0) vMax = 1;

    double dHScale = scale == 0 ? 64.0 / hMax : scale;
    double dVScale = scale == 0 ? 64.0 / vMax : scale;

    // Scale the filters and round to int
    PiiMatrix<int> horizontalIntegerFilter(Pii::round<int>(horizontalFilter * dHScale));
    PiiMatrix<int> verticalIntegerFilter(Pii::round<int>(verticalFilter * dVScale));

    // Recalculate the real scale (after rounding)
    int iHSum = Pii::sum<int>(horizontalIntegerFilter),
      iVSum = Pii::sum<int>(verticalIntegerFilter);
    double dHSum = Pii::sum<double>(horizontalFilter),
      dVSum = Pii::sum<double>(verticalFilter);

    if (nonZeroSums(iHSum, dHSum))
      dHScale = double(iHSum) / dHSum;
    if (nonZeroSums(iVSum, dVSum))
      dVScale = double(iVSum) / dVSum;

    if (mode == Pii::ExtendZeros)
      {
        PiiMatrix<int> filtered = PiiDsp::filter<int>(PiiDsp::filter<int>(image,
                                                                          horizontalIntegerFilter,
                                                                          PiiDsp::FilterOriginalSize),
                                                      verticalIntegerFilter,
                                                      PiiDsp::FilterOriginalSize);
        // Readable? Not. Scales each element as doubles and rounds
        // the result to an int.
        filtered.map(Pii::unaryCompose(Pii::Round<double,int>(),
                                       std::bind2nd(std::multiplies<double>(),
                                                    1.0/(dVScale*dHScale))));
        return filtered;
      }

    const int rows = verticalFilter.rows() >> 1, cols = horizontalFilter.columns() >> 1;
    PiiMatrix<int> filtered = PiiDsp::filter<int>(PiiDsp::filter<int>(Pii::extend(image, rows, rows, cols, cols, mode),
                                                                      horizontalIntegerFilter,
                                                                      PiiDsp::FilterValidPart),
                                                  verticalIntegerFilter, PiiDsp::FilterValidPart);
    // Scale back, see above
    filtered.map(Pii::unaryCompose(Pii::Round<double,int>(),
                                   std::bind2nd(std::multiplies<double>(),
                                                1.0/(dVScale*dHScale))));
    return filtered;
  }

  PiiMatrix<float> createRotationTransform(float theta)
  {
    float fSin = Pii::sin(theta), fCos = Pii::cos(theta);
    return PiiMatrix<float>(3,3,
                            fCos, -fSin, 0.0,
                            fSin, fCos, 0.0,
                            0.0, 0.0, 1.0);
  }

  PiiMatrix<float> createRotationTransform(float theta, float centerX, float centerY)
  {
    // Move origin to the new center, rotate, and move back
    return createTranslationTransform(centerX, centerY) *
      createRotationTransform(theta) *
      createTranslationTransform(-centerX, -centerY);
  }

  PiiMatrix<float> createShearingTransform(float shearX, float shearY)
  {
    return PiiMatrix<float>(3,3,
                            1.0, shearX, 0.0,
                            shearY, 1.0, 0.0,
                            0.0, 0.0, 1.0);
  }

  PiiMatrix<float> createTranslationTransform(float x, float y)
  {
    return PiiMatrix<float>(3,3,
                            1.0, 0.0, x,
                            0.0, 1.0, y,
                            0.0, 0.0, 1.0);
  }

  PiiMatrix<float> createScalingTransform(float scaleX, float scaleY)
  {
    return PiiMatrix<float>(3,3,
                            scaleX, 0.0, 0.0,
                            0.0, scaleY, 0.0,
                            0.0, 0.0, 1.0);
  }

  PiiMatrix<bool> createRoiMask(int rows, int columns,
                                const PiiMatrix<int>& rectangles)
  {
    PiiMatrix<bool> result(rows, columns);
    for (int r=0; r<rows; ++r)
      {
        const PiiRectangle<int>& rect = rectangles.rowAs<PiiRectangle<int> >(r);
        if (rect.x >=0 && rect.x < columns &&
            rect.y >=0 && rect.y < rows &&
            rect.width > 0 &&
            rect.height > 0 &&
            rect.x + rect.width <= columns &&
            rect.y + rect.height <= rows)
          result(rect.y, rect.x, rect.height, rect.width) = true;
      }
    return result;
  }

  bool overlapping(const PiiMatrix<int>& rectangles)
  {
    const int iRows = rectangles.rows();
    if (iRows <= 1)
      return false;
    for (int r1=0; r1<iRows; ++r1)
      {
        const PiiRectangle<int>& rect = rectangles.rowAs<PiiRectangle<int> >(r1);
        for (int r2=r1+1; r2<iRows; ++r2)
          if (rect.intersects(rectangles.rowAs<PiiRectangle<int> >(r2)))
            return true;
      }
    return false;
  }

  PiiMatrix<bool> alphaToMask(const PiiMatrix<PiiColor4<> >& image)
  {
    const int iRows = image.rows(), iColumns = image.columns();
    PiiMatrix<bool> matMask(PiiMatrix<bool>::uninitialized(iRows, iColumns));
    for (int r=0; r<iRows; ++r)
      {
        const PiiColor4<>* pSourceRow = image[r];
        bool* pTargetRow = matMask[r];
        for (int c=0; c<iColumns; ++c)
          pTargetRow[c] = !!pSourceRow[c].rgbaA;
      }
    return matMask;
  }
}
