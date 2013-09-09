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
    const U* pHistogram = histogram.row(0);
    for (int r=img.rows(); r--; )
      {
        const T* sourceRow = img.row(r);
        U* targetRow = result.row(r);
        for (int c=img.columns(); c--; )
          targetRow[c] = pHistogram[(int)sourceRow[c]];
      }
    return result;
  }

  template <class T, class U> PiiMatrix<U> backProject(const PiiMatrix<T>& ch1, const PiiMatrix<T>& ch2,
                                                       const PiiMatrix<U>& histogram)
  {
    PiiMatrix<U> result(PiiMatrix<U>::uninitialized(ch1.rows(), ch1.columns()));
    for (int r=ch1.rows(); r--; )
      {
        const T* ch1Row = ch1.row(r);
        const T* ch2Row = ch2.row(r);
        U* targetRow = result.row(r);
        for (int c=ch1.columns(); c--; )
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
  
  template <class T> void GrayHistogramHandler<T>::operator() (const PiiMatrix<T>& image)
  {
    iPixelCount += image.rows() * image.columns();
    addToVariant(varHistogram, histogram(image, iLevels));
  }

  template <class T> template <class Roi>
  void GrayHistogramHandler<T>::operator() (const PiiMatrix<T>& image, const Roi& roi)
  {
    PiiMatrix<int> matHistogram(histogram(image, roi, iLevels));
    if (bNormalized)
      iPixelCount += Pii::sum<int>(matHistogram);
    addToVariant(varHistogram, matHistogram);
  }

  template <class T> void GrayHistogramHandler<T>::normalize()
  {
    if (iPixelCount != 0)
      varHistogram = PiiVariant(varHistogram.valueAs<const PiiMatrix<int> >()
                                .mapped(std::multiplies<float>(), 1.0 / iPixelCount));
  }

  template <class T> void GrayHistogramHandler<T>::initialize(int levels, bool normalized)
  {
    HistogramHandler::initialize(levels, normalized);
    varHistogram = PiiVariant();
  }

  template <class Clr> void ColorHistogramHandler<Clr>::operator() (const PiiMatrix<Clr>& image)
  {
    PiiImage::separateChannels(image, channelImages);
    iPixelCount += image.rows() * image.columns();
    for (int i=0; i<3; ++i)
      if (baCalculate[i])
        addToVariant(varHistograms[i], histogram(channelImages[i], iLevels));
  }

  template <class Clr> template <class Roi>
  void ColorHistogramHandler<Clr>::operator() (const PiiMatrix<Clr>& image, const Roi& roi)
  {
    PiiImage::separateChannels(image, channelImages);
    bool bMustCount = bNormalized;
    for (int i=0; i<3; ++i)
      {
        if (baCalculate[i])
          {
            PiiMatrix<int> matHistogram(histogram(channelImages[i], roi, iLevels));
            if (bMustCount)
              {
                iPixelCount += Pii::sum<int>(matHistogram);
                bMustCount = false;
              }
            addToVariant(varHistograms[i], matHistogram);
          }
      }
  }

  template <class Clr> void ColorHistogramHandler<Clr>::normalize()
  {
    if (iPixelCount != 0)
      for (int i=0; i<3; ++i)
        if (baCalculate[i])
          varHistograms[i] = PiiVariant(varHistograms[i].valueAs<const PiiMatrix<int> >()
                                        .mapped(std::multiplies<float>(), 1.0 / iPixelCount));
  }

  template <class Clr> void ColorHistogramHandler<Clr>::initialize(int levels, bool normalized)
  {
    HistogramHandler::initialize(levels, normalized);
    for (int i=0; i<3; ++i)
      varHistograms[i] = PiiVariant();
  }
}
