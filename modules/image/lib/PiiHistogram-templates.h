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

#ifndef _PIIHISTOGRAM_H
# error "Never use <PiiHistogram-templates.h> directly; include <PiiHistogram.h> instead."
#endif

#include <PiiAlgorithm.h>
#include <PiiFunctional.h>

namespace PiiImage
{
  template <class T, class U, class Roi> PiiMatrix<T> histogram(const PiiMatrix<U>& image, const Roi& roi, unsigned int levels)
  {
    if (levels < 1)
      levels = unsigned(Pii::max(image)) + 1;
    PiiMatrix<T> result(1, int(levels));
    T* vector = result.row(0);

    const int iRows = image.rows(), iCols = image.columns();
    for (int r=0; r<iRows; ++r)
      {
        const U* row = image.row(r);
        for (int c=0; c<iCols; ++c)
          if (unsigned(row[c]) < levels && roi(r,c)) ++vector[unsigned(row[c])];
      }
    return result;
  }

  template <class T, class U, class Roi> PiiMatrix<T> histogram(const PiiMatrix<U>& image, const Roi& roi, const PiiQuantizer<U>& quantizer)
  {
    PiiMatrix<T> result(1, quantizer.levels());
    T* vector = result.row(0);

    const int iRows = image.rows(), iCols = image.columns();
    for (int r=0; r<iRows; ++r)
      {
        const U* row = image.row(r);
        for (int c=0; c<iCols; ++c)
          if (roi(r,c)) ++vector[quantizer.quantize(row[c])];
      }
    return result;
  }

  template <class T, class U> PiiMatrix<T> normalize(const PiiMatrix<U>& histogram)
  {
    PiiMatrix<T> result(PiiMatrix<T>::uninitialized(histogram.rows(), histogram.columns()));
    const int iRows = histogram.rows(), iColumns = histogram.columns();
    for (int r=0; r<iRows; ++r)
      {
        const U* pRow = histogram.row(r);
        T sum = Pii::accumulateN(pRow, iColumns, std::plus<T>(), T(0));
        if (sum != 0)
          Pii::transformN(pRow, iColumns, result.row(r),
                          std::bind2nd(std::multiplies<T>(), 1.0/sum));
        else
          memset(result.row(r), 0, sizeof(T)*iColumns);
      }
    return result;
  }

  template <class T> int percentile(const PiiMatrix<T>& cumulative, T value)
  {
    int start = 0, end = cumulative.columns();
    const T* values = cumulative.row(0);
    // Values in a cumulative distribution are monotonically
    // increasing -> use binary search
    while (start < end)
      {
        int half = (start + end) >> 1;
        if (value <= values[half])
          end = half;
        else
          start = half+1;
      }
    return end < cumulative.columns() ? end : -1;
  }

  template <class T, class U> PiiMatrix<U> backProject(const PiiMatrix<T>& img, const PiiMatrix<U>& histogram)
  {
    PiiMatrix<U> result(PiiMatrix<U>::uninitialized(img.rows(), img.columns()));
    Pii::transform(img.begin(), img.end(), result.begin(), Pii::arrayLookup(histogram[0]));
    return result;
  }

  template <class T, class U> PiiMatrix<U> backProject(const PiiMatrix<T>& ch1, const PiiMatrix<T>& ch2,
                                                       const PiiMatrix<U>& histogram)
  {
    PiiMatrix<U> result(PiiMatrix<U>::uninitialized(ch1.rows(), ch1.columns()));
    const int iRows = ch1.rows(), iCols = ch1.columns();
    for (int r=0; r<iRows; ++r)
      {
        const T* ch1Row = ch1.row(r);
        const T* ch2Row = ch2.row(r);
        U* targetRow = result.row(r);
        for (int c=0; c<iCols; ++c)
          targetRow[c] = histogram((int)ch1Row[c], (int)ch2Row[c]);
      }
    return result;
  }

  template <class T> PiiMatrix<T> equalize(const PiiMatrix<T>& img, unsigned int levels)
  {
    PiiMatrix<T> result(img.rows(), img.columns());

    unsigned int maxValue = (unsigned int)Pii::max(img);
    if (levels < maxValue)
      levels = maxValue + 1;
    if (levels == 0)
      return img;

    PiiMatrix<int> dist = cumulative(histogram(img, levels));
    int* pDist = dist.row(0);

    PiiMatrix<T> newDist(1, levels);
    T* pNewDist = newDist.row(0);
    int sum = pDist[levels-1]; // total number of pixels
    // If the distribution was really equalized, this would be the
    // ideal frequency for each gray level.
    double idealFreq = double(sum)/levels;

    int j = 0;
    // Find out the new value for each pixel.
    for(unsigned int i=0; i<levels; i++)
      {
        int expectedShare = int(idealFreq * (i+1) + 0.5);
        while (pDist[j] < expectedShare)
          pNewDist[j++] = i;
        pNewDist[j] = i;
      }
    return backProject(img, newDist);
  }
}
