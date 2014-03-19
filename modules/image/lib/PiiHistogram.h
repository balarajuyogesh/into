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
#define _PIIHISTOGRAM_H

#include "PiiQuantizer.h"
#include "PiiImage.h"
#include <PiiMath.h>

namespace PiiImage
{
  /**
   * Calculate the histogram of a one-channel image. The result will
   * be a row matrix containing the frequencies of all values in the
   * input image. The return type is determined by the `T` template
   * parameter.
   *
   * @param image the input image. All integer types are supported. If
   * another type is used, each element is casted to an int in
   * processing. The minimum value of the image must not be negative.
   *
   * @param roi region-of-interest. See PiiImage.
   *
   * @param levels the number of distinct levels in the image. If zero
   * is given, the maximum value of the image will be found. For 8 bit
   * gray-scale images, use 256.
   *
   * @return the histogram as a PiiMatrix<T>
   */
  template <class T, class U, class Roi> PiiMatrix<T> histogram(const PiiMatrix<U>& image, const Roi& roi, unsigned int levels);

  /**
   * Calculate the histogram of a one-channel image. This is a
   * shorthand for `histogram<int>(image, roi, levels)`.
   */
  template <class T, class Roi> inline PiiMatrix<int> histogram(const PiiMatrix<T>& image, const Roi& roi, unsigned int levels)
  {
    return histogram<int,T,Roi>(image, roi, levels);
  }

  /**
   * Calculate the histogram of a one-channel image. This is a
   * shorthand for `histogram<int>(image, PiiImage::DefaultRoi(),
   * levels)`.
   */
  template <class T> inline PiiMatrix<int> histogram(const PiiMatrix<T>& image, unsigned int levels=0)
  {
    return histogram<int,T>(image, PiiImage::DefaultRoi(), levels);
  }


  /**
   * Calculate the histogram of a one-channel image. The result will
   * be a row matrix containing the frequencies of all values in the
   * input image. The return type is determined by the `T` template
   * parameter.
   *
   * @param image the input image. All integer types are supported. If
   * another type is used, each element is casted to an int in
   * processing. The minimum value of the image must not be negative.
   *
   * @param roi region-of-interest. See PiiImage.
   *
   * @param quantizer a quantizer that converts image pixels into
   * quantized values.
   */
  template <class T, class U, class Roi> PiiMatrix<T> histogram(const PiiMatrix<U>& image, const Roi& roi, const PiiQuantizer<U>& quantizer);

  /**
   * Calculate the histogram of a one-channel image. This is a
   * shorthand for `histogram<int>(image, quantizer)`.
   */
  template <class T, class Roi> inline PiiMatrix<int> histogram(const PiiMatrix<T>& image, const Roi& roi, const PiiQuantizer<T>& quantizer)
  {
    return histogram<int>(image, roi, quantizer);
  }

  /**
   * Calculate the histogram of a one-channel image. This is a
   * shorthand for `histogram<int>(image, quantizer,
   * PiiImage::DefaultRoi())`.
   */
  template <class T> inline PiiMatrix<int> histogram(const PiiMatrix<T>& image, const PiiQuantizer<T>& quantizer)
  {
    return histogram<int>(image, PiiImage::DefaultRoi(), quantizer);
  }

  /**
   * Calculate the cumulative frequency distribution of the given
   * frequency distribution (histogram). The histogram must be
   * represented as a row vector. If the input matrix has many rows,
   * the cumulative histogram for each row is calculated.
   *
   * ~~~(c++)
   * PiiMatrix<int> histogram(1,5, 1,2,3,4,5);
   * PiiMatrix<int> cum(PiiImage::cumulative(histogram)); // sic!
   * // cum = (1,3,6,10,15)
   * ~~~
   *
   * @see Pii::cumulativeSum()
   */
  template <class T> inline PiiMatrix<T> cumulative(const PiiMatrix<T>& histogram)
  {
    return Pii::cumulativeSum<T,PiiMatrix<T> >(histogram, Pii::Horizontally);
  }

  /**
   * Normalize the given histogram so that its elements sum up to one.
   * If the matrix has many rows, each row is normalized. The return
   * type may be different from the input type. Typically, float or
   * double is used as the return type. If all values in a row equal
   * to zero, they are left as such.
   *
   * ~~~(c++)
   * PiiMatrix<int> histogram(1,4, 1,2,3,4);
   * PiiMatrix<double> normalized(PiiImage::normalize<double>(histogram));
   * //normalized = (0.1, 0.2, 0.3, 0.4)
   * ~~~
   */
  template <class T, class U> PiiMatrix<T> normalize(const PiiMatrix<U>& histogram);

  /**
   * Find the index of the first entry in a cumulative frequency
   * distribution that exceeds or equals to the given value. For
   * normalized cumulative distributions, `value` should be between 0
   * and 1.
   *
   * ~~~(c++)
   * PiiMatrix<double> cumulative(1,4, 0.1, 0.3, 0.6, 1.0);
   * int p = percentile(cumulative, 0.5);
   * //p = 2
   * ~~~
   *
   * @param cumulative a cumulative frequency distribution. A row
   * matrix with monotonically increasing values.
   *
   * @param value the percentile value, which should be smaller than
   * or equal to the maximum value in `cumulative`.
   *
   * @return the index of the first element exceeding or equal to
   * `value`, or -1 if no such element was found
   */
  template <class T> int percentile(const PiiMatrix<T>& cumulative, T value);

  /**
   * Histogram backprojection. In backprojection, each pixel in `img`
   * is replaced by the corresponding value in `histogram`. Despite
   * histogram backprojection this function can be used to convert
   * indexed images to color images.
   *
   * @param img input image
   *
   * @param histogram the histogram. A 1-by-N matrix.
   *
   * The function makes no boundary checks for performance reasons. If
   * you aren't sure about your data, you must check that
   * `histogram`.columns() is larger than the maximum value in `img`
   * and that there are no negative values in `img`.
   *
   * Since pixels in `img` are used as indices in `histogram`, they
   * will be converted to integers. Using floating-point types in the
   * `img` parameter is not suggested.
   *
   * ~~~(c++)
   * // Normal backprojection
   * PiiMatrix<int> histogram(1,256);
   * PiiMatrix<unsigned char> img(100,100);
   * PiiMatrix<int> backProjected(PiiImage::backProject(img,histogram));
   *
   * // Color mapping
   * PiiMatrix<PiiColor<> > colorMap(1,256);
   * PiiMatrix<unsigned char> indexedImg(100,100);
   * PiiMatrix<PiiColor<> > colorImg(PiiImage::backProject(indexedImg, colorMap);
   * ~~~
   */
  template <class T, class U> PiiMatrix<U> backProject(const PiiMatrix<T>& img, const PiiMatrix<U>& histogram);

  /**
   * Two-dimensional histogram backprojection. This function is
   * analogous to the previous one, but uses a two-dimensional
   * histogram for backprojection. It is provided just for convenience
   * as two-dimensional distributions can be converted to one
   * dimension.
   *
   * @param ch1 first channel (indexes rows in `histogram`, maximum
   * value N-1)
   *
   * @param ch2 second channel (indexes columns in `histogram`,
   * maximum value M-1)
   *
   * @param histogram two-dimensional histogram (N-by-M)
   *
   * The sizes of `ch1` and `ch2` must be equal.
   *
   * ~~~(c++)
   * // Backproject a two-dimensional RG histogram
   * PiiMatrix<int> histogram(256,256);
   * PiiMatrix<unsigned char> redChannel(100,100);
   * PiiMatrix<unsigned char> greenChannel(100,100);
   * PiiMatrix<int> backProjected(PiiImage::backProject(redChannel, greenChannel, histogram));
   * ~~~
   */
  template <class T, class U> PiiMatrix<U> backProject(const PiiMatrix<T>& ch1, const PiiMatrix<T>& ch2,
                                                       const PiiMatrix<U>& histogram);

  /**
   * Histogram equalization. Enhances the contrast of `img` by making
   * its gray levels as uniformly distributed as possible.
   *
   * @param img the input image
   *
   * @param levels the number of quantization levels. If this value is
   * omitted, the maximum value found in `image` will be used. If
   * `levels` is smaller than the maximum value, the latter will be used.
   *
   * @return an image with enhanced contrast
   */
  template <class T> PiiMatrix<T> equalize(const PiiMatrix<T>& img, unsigned int levels = 0);
};

#include "PiiHistogram-templates.h"

#endif //_PIIHISTOGRAM_H
