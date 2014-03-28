/* This file is part of Into.
 * Copyright (C) Intopii 2014.
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

#ifndef _PIIHISTOGRAMHANDLER_H
#define _PIIHISTOGRAMHANDLER_H

#include <PiiVariant.h>
#include <PiiMatrix.h>
#include <PiiHistogram.h>
#include <functional>

struct PiiHistogramHandler
{
  PiiHistogramHandler() :
    iPixelCount(0),
    iLevels(256),
    bNormalized(false)
  {}

  virtual ~PiiHistogramHandler() {}

  static inline bool acceptsManyRegions() { return true; }

  static void addToVariant(PiiVariant& variant, const PiiMatrix<int>& histogram)
  {
    if (!variant.isValid())
      variant = PiiVariant(histogram);
    else
      variant.valueAs<PiiMatrix<int> >() += histogram;
  }

  void initialize(int levels, bool normalized)
  {
    iPixelCount = 0;
    iLevels = levels;
    bNormalized = normalized;
  }

  int iPixelCount;
  int iLevels;
  int bNormalized;
};

template <class T> struct PiiGrayHistogramHandler : PiiHistogramHandler
{
  void initialize(int levels, bool normalized);
  void operator() (const PiiMatrix<T>& image);
  template <class Roi> void operator() (const PiiMatrix<T>& image, const Roi& roi);
  void normalize();

  PiiVariant varHistogram;
};

template <class Clr> struct PiiColorHistogramHandler : PiiHistogramHandler
{
  PiiColorHistogramHandler()
  {
    baCalculate[0] = baCalculate[1] = baCalculate[2] = true;
  }

  void initialize(int levels, bool normalized);
  void operator() (const PiiMatrix<Clr>& image);
  template <class Roi> void operator() (const PiiMatrix<Clr>& image, const Roi& roi);
  void normalize();

  PiiVariant varHistograms[3];
  PiiMatrix<typename Clr::Type> channelImages[3];
  bool baCalculate[3];
};

template <class T> void PiiGrayHistogramHandler<T>::operator() (const PiiMatrix<T>& image)
{
  iPixelCount += image.rows() * image.columns();
  addToVariant(varHistogram, PiiImage::histogram(image, iLevels));
}

template <class T> template <class Roi>
void PiiGrayHistogramHandler<T>::operator() (const PiiMatrix<T>& image, const Roi& roi)
{
  PiiMatrix<int> matHistogram(PiiImage::histogram(image, roi, iLevels));
  if (bNormalized)
    iPixelCount += Pii::sum<int>(matHistogram);
  addToVariant(varHistogram, matHistogram);
}

template <class T> void PiiGrayHistogramHandler<T>::normalize()
{
  if (iPixelCount != 0)
    varHistogram = PiiVariant(Pii::matrix(varHistogram.valueAs<const PiiMatrix<int> >()
                                          .mapped(std::multiplies<float>(), 1.0 / iPixelCount)));
}

template <class T> void PiiGrayHistogramHandler<T>::initialize(int levels, bool normalized)
{
  PiiHistogramHandler::initialize(levels, normalized);
  varHistogram = PiiVariant();
}

template <class Clr> void PiiColorHistogramHandler<Clr>::operator() (const PiiMatrix<Clr>& image)
{
  PiiImage::separateChannels(image, channelImages);
  iPixelCount += image.rows() * image.columns();
  for (int i=0; i<3; ++i)
    if (baCalculate[i])
      addToVariant(varHistograms[i], PiiImage::histogram(channelImages[i], iLevels));
}

template <class Clr> template <class Roi>
void PiiColorHistogramHandler<Clr>::operator() (const PiiMatrix<Clr>& image, const Roi& roi)
{
  PiiImage::separateChannels(image, channelImages);
  bool bMustCount = bNormalized;
  for (int i=0; i<3; ++i)
    {
      if (baCalculate[i])
        {
          PiiMatrix<int> matHistogram(PiiImage::histogram(channelImages[i], roi, iLevels));
          if (bMustCount)
            {
              iPixelCount += Pii::sum<int>(matHistogram);
              bMustCount = false;
            }
          addToVariant(varHistograms[i], matHistogram);
        }
    }
}

template <class Clr> void PiiColorHistogramHandler<Clr>::normalize()
{
  if (iPixelCount != 0)
    for (int i=0; i<3; ++i)
      if (baCalculate[i])
        varHistograms[i] = PiiVariant(Pii::matrix(varHistograms[i].valueAs<const PiiMatrix<int> >()
                                                  .mapped(std::multiplies<float>(), 1.0 / iPixelCount)));
}

template <class Clr> void PiiColorHistogramHandler<Clr>::initialize(int levels, bool normalized)
{
  PiiHistogramHandler::initialize(levels, normalized);
  for (int i=0; i<3; ++i)
    varHistograms[i] = PiiVariant();
}


#endif //_PIIHISTOGRAMHANDLER_H
