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

#ifndef _PIIIMAGE_H
#define _PIIIMAGE_H

#include "PiiImageGlobal.h"
#include <PiiMath.h>
#include <PiiMatrixUtil.h>
#include <PiiDsp.h>
#include <PiiColor.h>
#include <PiiPoint.h>

/**
 * Definitions and functions for image processing.
 *
 * This namespace contains functions and definitions for creating
 * digital image filters and for applying them to images,
 * thresholding, labeling binary images, connected component analysis,
 * edge detection, and histogram handling.
 */
namespace PiiImage
{
  /**
   * Default region-of-interest function object for feature extraction
   * and image analysis methods that support ROI processing. This
   * function returns `true` for all pixels, and reduces to a no-op
   * with compiler optimizations turned on.
   */
  struct DefaultRoi
  {
    /**
     * Check if a pixel is within a region-of-interest. Each ROI
     * functor must implement a function with this signature
     * (inline/const are optional). This function returns `true`.
     *
     * @param r the row coordinate of a pixel
     *
     * @param c the column coordinate of a pixel
     */
    inline bool operator() (int r, int c) const { Q_UNUSED(r); Q_UNUSED(c); return true; }
  };

  /**
   * A region-of-interest function object that returns `true` if the
   * alpha channel has a non-zero value at (r,c) and `false`
   * otherwise. If there is no alpha channel in the color type `T`,
   * returns always `true`.
   */
  template <class T> struct AlphaRoi : DefaultRoi
  {
    AlphaRoi(const PiiMatrix<T>&) {}
  };

  template <class T> struct AlphaRoi<PiiColor4<T> >
  {
    AlphaRoi(const PiiMatrix<PiiColor4<T> >& img) : _image(img) {}

    inline bool operator() (int r, int c) const { return _image(r,c).rgbaA != 0; }

  private:
    const PiiMatrix<PiiColor4<T> > _image;
  };

  /**
   * Returns the alpha channel of `image` as a boolean mask. Non-zero
   * entries in the alpha channel will be `true` in the returned
   * mask.
   */
  PiiMatrix<bool> alphaToMask(const PiiMatrix<PiiColor4<> >& image);

  /**
   * Creates a binary ROI mask out of a set of rectangular regions.
   *
   * @param rows the number of rows in the result image
   *
   * @param columns the number of columns in the result image
   *
   * @param rectangles a N-by-4 matrix in which each row represents a
   * rectangle (x, y, width, height). If a rectangle exceeds the
   * boundaries of the result image, it will be ignored.
   *
   * @return a *rows* -by- *columns* binary image in which all
   * pixels that are in any of the *rectangles* will be set to
   * `true`.
   */
  PII_IMAGE_EXPORT PiiMatrix<bool> createRoiMask(int rows, int columns,
                                                 const PiiMatrix<int>& rectangles);

  /**
   * Returns `true` if any two of the given *rectangles* overlap
   * each other and `false` otherwise.
   */
  PII_IMAGE_EXPORT bool overlapping(const PiiMatrix<int>& rectangles);

  typedef PiiMatrix<PiiPoint<int> > IntCoordinateMap;
  typedef PiiMatrix<PiiPoint<double> > DoubleCoordinateMap;

  template <class T> struct GrayToGray
  {
    typedef T Type;
  };

  template <class Clr> struct ColorToGray
  {
    typedef typename Clr::Type Type;
  };

  template <class T> struct ToGray :
    Pii::IfClass<Pii::IsColor<T>, ColorToGray<T>, GrayToGray<T> >::Type
  {};

  template <class T> inline const PiiMatrix<T>& toGray(const PiiMatrix<T>& image)
  {
    return image;
  }

  template <class T> inline PiiMatrix<T> toGray(const PiiMatrix<PiiColor<T> >& image)
  {
    return PiiMatrix<T>(image);
  }

  template <class T> inline PiiMatrix<T> toGray(const PiiMatrix<PiiColor4<T> >& image)
  {
    return PiiMatrix<T>(image);
  }

  /**
   * Convert the gray values of an image to floating point. This
   * function divides 8-bit gray levels by 255.
   */
  template <class T> inline PiiMatrix<float> toFloat(const PiiMatrix<T>& image)
  {
    PiiMatrix<float> result(image);
    result /= 255.0f;
    return result;
  }

  /**
   * An explicit specialization of the toFloat() function. Returns the
   * argument without modifications.
   */
  inline PiiMatrix<float> toFloat(const PiiMatrix<float>& image)
  {
    return image;
  }

  /**
   * Convert the gray values of an image to 8-bit gray. The default
   * implementation just casts the type.
   */
  template <class T> inline PiiMatrix<unsigned char> to8Bit(const PiiMatrix<T>& image)
  {
    return PiiMatrix<unsigned char>(image);
  }

  inline PiiMatrix<unsigned char> to8Bit(const PiiMatrix<unsigned char>& image)
  {
    return image;
  }

  /**
   * Convert the gray values of an image to 8-bit gray. This function
   * multiplies floating-point gray values by 255.
   */
  inline PiiMatrix<unsigned char> to8Bit(const PiiMatrix<float>& image)
  {
    return PiiMatrix<unsigned char>(image * 255.0);
  }

  /**
   * Extract a channel from a color image. This is a generic template
   * function that works with any color type.
   *
   * ~~~(c++)
   * PiiMatrix<PiiColor4<> > image(5,5);
   * PiiMatrix<unsigned char> ch1(PiiImage::colorChannel(image, 1)); // Green channel
   *
   * PiiMatrix<PiiColor<unsigned short> > image2(5,5);
   * PiiMatrix<unsigned short> ch2(PiiImage::colorChannel(image2, 2)); // Blue channel
   * ~~~
   *
   * @param image the input image
   *
   * @param channel the zero-based index of the color channel
   *
   * @return the channel as a matrix whose dimensions equal to the
   * input image.
   */
  template <class ColorType> PiiMatrix<typename ColorType::Type> colorChannel(const PiiMatrix<ColorType>& image,
                                                                              int channel);

  /**
   * Set a color channel. This is a generic template that works with
   * any color type. If the sizes of `image` and `value` do not
   * match, the function does nothing.
   *
   * ~~~(c++)
   * PiiMatrix<PiiColor4<> > image(5,5);
   * PiiMatrix<unsigned char> red(5,5);
   * red = 255;
   * PiiImage::setColorChannel(image, 0, red);
   * ~~~
   *
   * @param image the color image
   *
   * @param channel index of the color channel to modify
   *
   * @param values new values for the channel
   */
  template <class ColorType> void setColorChannel(PiiMatrix<ColorType>& image,
                                                  int channel,
                                                  const PiiMatrix<typename ColorType::Type>& values);

  /**
   * Set a color channel to a constant value. Works analogously to the
   * above function but uses the same value for each pixel.
   *
   * ~~~(c++)
   * PiiMatrix<PiiColor4<> > image(5,5);
   * PiiImage::setColorChannel(image, 0, unsigned char(255));
   * ~~~
   */
  template <class ColorType> void setColorChannel(PiiMatrix<ColorType>& image,
                                                  int channel,
                                                  typename ColorType::Type value);

  /**
   * Split a color image into channels. This function is equivalent to
   * but faster than calling colorChannel() *channels* times in
   * sequence.
   *
   * @param image the color image to be split into channels
   *
   * @param channelImages an array of at least *channels* images. The
   * images will be automatically resized to the size of the input
   * image.
   *
   * @param channels the number of color channels to extract. Either
   * three or four.
   */
  template <class ColorType> void separateChannels(const PiiMatrix<ColorType>& image,
                                                   PiiMatrix<typename ColorType::Type>* channelImages,
                                                   int channels = 3);

  /**
   * Predefined filter masks for the x and y components of the Sobel
   * edge finder.
   */
  PII_IMAGE_EXPORT extern PiiMatrix<int> sobelX, sobelY;
  /**
   * Predefined filter masks for the x and y components of the Roberts
   * edge finder.
   */
  PII_IMAGE_EXPORT extern PiiMatrix<int> robertsX, robertsY;
  /**
   * Predefined filter masks for the x and y components of the Prewitt
   * edge finder.
   */
  PII_IMAGE_EXPORT extern PiiMatrix<int> prewittX, prewittY;

  /**
   * Create a size-by-size Gaussian low-pass filter. Size must be odd.
   * The filter is cut at approximately \(3\sigma\) and normalized
   * so that it sums up to unity.
   */
  PII_IMAGE_EXPORT PiiMatrix<double> makeGaussian(unsigned int size);

  /**
   * Create a size-by-size Laplacian-of-Gaussian filter. Size must be
   * odd. The filter is cut at approximately \(3\sigma\) and
   * normalized so that it sums up to zero.
   */
  PII_IMAGE_EXPORT PiiMatrix<double> makeLoGaussian(unsigned int size);

  /**
   * Create an image filter. This function can be used to create
   * well-known filters for many image processing tasks.
   *
   * @param type the type of the filter
   *
   * @param size the size of the filter. Some filters (Sobel, Roberts,
   * Prewitt) are fixed in size, and this value will be ignored.
   *
   * @return a square-shaped image filter matrix
   *
   * ! `UniformFilter`, `GaussianFilter`, and `LoGFilter`
   * cannot be represented as integers. Use `float` or `double` as
   * the data type.
   *
   * ~~~(c++)
   * // Create a 5-by-5 gaussian filter
   * PiiMatrix<double> filter = PiiImage::makeFilter<double>(PiiImage::GaussianFilter, 5);
   * ~~~
   */
  template <class T> PiiMatrix<T> makeFilter(PrebuiltFilterType type, unsigned int size=3);

  /**
   * Decomposes a two-dimensional filter into two one-dimensional
   * filters. This function decomposes a matrix A into a row vector H
   * and column vector V so that A = VH.
   *
   * Decomposition is useful in speeding up convolution because
   * one-dimensional convolution is much faster than two-dimensional,
   * but not all filters are separable. Prominent examples of
   * separable filters are the two-dimensional Gaussian function (the
   * only separable, circularly symmetric filter), the Sobel filters,
   * and the moving average filter. These can be created with the
   * [makeFilter()] function using `GaussianFilter`, `SobelXFilter`,
   * `SobelYFilter`, or `UniformFilter` as the filter type,
   * respectively.
   *
   * @param filter a two-dimensional filter mask
   *
   * @param horizontalFilter the horizontal component of the
   * decomposition, a row vector. Output-value parameter.
   *
   * @param verticalFilter the vertical component of the
   * decomposition, a column vector. Output-value parameter.
   *
   * @return `true` if the decomposition was successful, `false`
   * otherwise.
   *
   * ~~~(c++)
   * PiiMatrix<int> filter = PiiImage::makeFilter<int>(PiiImage::SobelXFilter);
   * PiiMatrix<int> h, v;
   * PiiImage::separateFilter(filter, h, v);
   * QVERIFY(filter.equals(v*h));
   * ~~~
   *
   * ! A filter with floating-point coefficients may not be
   * separable if it is not stored accurately enough. This applies
   * especially to the Gaussian filter.
   *
   * ~~~(c++)
   * PiiMatrix<float> h, v;
   * // Returns false, must use double as the data type
   * PiiImage::separateFilter(PiiImage::makeFilter<float>(PiiImage::GaussianFilter), h, v);
   * ~~~
   */
  template <class T> bool separateFilter(const PiiMatrix<T>& filter,
                                         PiiMatrix<T>& horizontalFilter,
                                         PiiMatrix<T>& verticalFilter);

  /**
   * Calculate gradient angle from gradient magnitude components.
   *
   * @param gradX horizontal gradient magnitude
   *
   * @param gradY vertical gradient magnitude
   *
   * @return gradient angle for each pixel (\([-\pi,\pi]\)), or an
   * empty matrix if `gradX` and `gradY` are of different size.
   *
   * ~~~(c++)
   * using namespace PiiImage;
   * PiiMatrix<float> directions = gradientDirection(filter(image, SobelXFilter),
   *                                                 filter(image, SobelYFilter));
   * ~~~
   */
  template <class T> PiiMatrix<float> gradientDirection(const PiiMatrix<T>& gradX, const PiiMatrix<T>& gradY)
  {
    return Pii::matrix(gradY.mapped(Pii::FastAtan2<T>(), gradX));
  }

  /**
   * Calculate total gradient magnitude from gradient magnitude
   * components.
   *
   * @param gradX horizontal gradient magnitude
   *
   * @param gradY vertical gradient magnitude
   *
   * @param fast a flag that determines whether a fast approximation
   * is used (default is true). The real magnitude is
   * \(\sqrt{x^2+y^2}\) (`fast` == `false`). In `fast` mode, the
   * magnitude is approximated with \(|x|+|y|\).
   *
   * @return gradient magnitude for each pixel, or an
   * empty matrix if `gradX` and `gradY` are of different size.
   *
   * ~~~(c++)
   * using namespace PiiImage;
   * PiiMatrix<int> image; // construct somewhere
   * // Use built-in filter masks directly
   * PiiMatrix<int> magnitude = gradientMagnitude(filter<int>(image, sobelX), filter<int>(image, sobelY));
   * ~~~
   */
  template <class T> PiiMatrix<T> gradientMagnitude(const PiiMatrix<T>& gradX, const PiiMatrix<T>& gradY, bool fast = true)
  {
    if (fast)
      return Pii::matrix(gradX.mapped(Pii::AbsSum<T>(), gradY));
    return Pii::matrix(gradX.mapped(Pii::Hypotenuse<T>(), gradY));
  }

  /**
   * A unary function that converts radians to points of the compass.
   * Angles in \([-\pi/8, \pi/8)\) are point 0, angles in \([\pi/8,
   * 3\pi/8)\) point 1 and so on.
   */
  template <class T> struct RadiansToPoints : public Pii::UnaryFunction<T,int>
  {
    // Quantize angle to 0-7. Binary AND adds 8 to negative numbers.
    int operator()(T radians) const { return int(Pii::round(4.0/M_PI*radians - 0.25)) & 7; }
  };

  /**
   * Thin edge detection result. This function checks each pixel in
   * `magnitude` to see if it is a local maximum. It searches up to
   * `radius` pixels to the direction of the gradient. Only the maximum
   * pixels are retained in the result image; others are set to zero.
   *
   * @param magnitude gradient magnitude image. (see
   * gradientMagnitude())
   *
   * @param direction gradient direction image (see
   * gradientDirection()).
   *
   * @param quantizer a unary function that converts the angles stored
   * in `direction` into points of the compass (0-7, 0 = east, 1 =
   * north-east, ..., 7 = south-east).
   *
   * ~~~(c++)
   * // Typical case: thin edges using a gradient direction image (in radians)
   * PiiMatrix<int> gradientX, gradientY, edges;
   * edges =
   * PiiImage::suppressNonMaxima(PiiImage::gradientMagnitude(gradientX, gradienY),
   *                             PiiImage::gradientDirection(gradientX, gradientY),
   *                             PiiImage::RadiansToPoints<float>());
   *
   * // If you have already quantized the gradient direction image into 8 levels:
   * PiiMatrix<int> gradientDirection;
   * edges =
   * PiiImage::suppressNonMaxima(PiiImage::gradientMagnitude(gradientX, gradienY),
   *                             gradientDirection,
   *                             Pii::Identity<int>());
   * ~~~
   *
   * @relates RadiansToPoints
   */
  template <class T, class U, class Quantizer> PiiMatrix<T> suppressNonMaxima(const PiiMatrix<T>& magnitude,
                                                                              const PiiMatrix<U>& direction,
                                                                              Quantizer quantizer);

  /**
   * Detect edges in a gray-level image. This function implements the
   * Canny edge detector.
   *
   * @param image a gray-level image in which edges are to be found
   *
   * @param smoothWidth the width of a Gaussian smoothing window
   *
   * @param lowThreshold the low threshold value for hysteresis
   * thresholding. If zero, 0.4 * highThreshold will be used.
   *
   * @param highThreshold the high threshold value for hysteresis
   * thresholding. If zero, mean+2*std of the gradient magnitude will
   * be used.
   *
   * @return a binary image in which detected edges are ones and other
   * pixels zeros.
   */
  template <class T> PiiMatrix<int> detectEdges(const PiiMatrix<T>& image,
                                                int smoothWidth = 0,
                                                T lowThreshold = 0, T highThreshold = 0);

  /**
   * Filter an image with the given filter. This is equivalent to
   * PiiDsp::filter(), except for the `mode` parameter.
   *
   * @param image the image to be filtered
   *
   * @param filter the filter
   *
   * @param mode "extension" mode, i.e. the way of handling border
   * effects.
   *
   * ~~~(c++)
   * // Gaussian low-pass filtering assuming zeros outside of the image.
   * PiiMatrix<float> filtered = PiiImage::filter<float>(image,
   *                                                     PiiImage::makeFilter<float>(PiiImage::GaussianFilter),
   *                                                     Pii::ExtendZeros);
   * ~~~
   *
   * ! It is not a good idea to use `unsigned char` as the
   * result type. If the filters are `double`, use `double` as the
   * output type.
   */
  template <class ResultType, class T, class U>
  PiiMatrix<ResultType> filter(const PiiMatrix<T>& image,
                               const PiiMatrix<U>& filter,
                               Pii::ExtendMode mode = Pii::ExtendReplicate)
  {
    if (mode == Pii::ExtendZeros)
      return PiiDsp::filter<ResultType>(image, filter, PiiDsp::FilterOriginalSize);
    const int rows = filter.rows() >> 1, cols = filter.columns() >> 1;
    return PiiDsp::filter<ResultType>(Pii::extend(image, rows, rows, cols, cols, mode), filter, PiiDsp::FilterValidPart);
  }

  /**
   * Filter an image with two one-dimensional filters. If a
   * two-dimensional filter can be decomposed into two one-dimensional
   * ones, the filtering operation is much faster. Use
   * [separateFilter()] to find the decomposition.
   *
   * @param image the image to be filtered
   *
   * @param horizontalFilter a row matrix representing the horizontal
   * component of a decomposed filter
   *
   * @param verticalFilter a column matrix representing the vertical
   * component of a decomposed filter
   *
   * @param mode "extension" mode, i.e. the way of handling border
   * effects.
   *
   * @return filtered image. If `horizontalFilter` is not a row
   * vector or `verticalFilter` is not a column vector, a clone of
   * the input image will be returned.
   *
   * ~~~(c++)
   * // Convolution with a 23-by-23 filter is (informally) O(N * 23^2)
   * // Convolution with two 23-by-1 filters is O(N * 23*2)
   * // In theory, the processing time can go down to one 11th
   * PiiMatrix<int> filter = PiiImage::makeFilter(PiiImage::UniformFilter, 23);
   * PiiMatrix<int> hFilter, vFilter;
   * PiiImage::separateFilter(filter, hFilter, vFilter);
   * PiiMatrix<int> filtered = PiiImage::filter<int>(image, hFilter, vFilter);
   * ~~~
   *
   * ! It is not a good idea to use `unsigned` `char` as the
   * result type. If the filters are `double`, use `double` as the
   * output type.
   */
  template <class ResultType, class T, class U>
  PiiMatrix<ResultType> filter(const PiiMatrix<T>& image,
                               const PiiMatrix<U>& horizontalFilter,
                               const PiiMatrix<U>& verticalFilter,
                               Pii::ExtendMode mode = Pii::ExtendReplicate);

  /**
   * Same as above, but filters the image with a named filter. See
   * [makeFilter()] for information about filter names. This function
   * determines suitable data types for the filtering operation based
   * on the filter type and decomposes the filter if possible.
   *
   * @param image input image
   *
   * @param type filter type
   *
   * @param mode how to handle image borders
   *
   * @param filterSize the size of the filter, ignored by some filter
   * types
   *
   * @see makeFilter()
   *
   * ~~~(c++)
   * // First template parameter is the type of the result
   * PiiMatrix<int> smoothed = PiiImage::filter<int>(image,
   *                                                 PiiImage::GaussianFilter,
   *                                                 Pii::ExtendSymmetric);
   * ~~~
   */
  template <class ResultType, class ImageType>
  PiiMatrix<ResultType> filter(const PiiMatrix<ImageType>& image,
                               PrebuiltFilterType type,
                               Pii::ExtendMode mode = Pii::ExtendReplicate,
                               int filterSize = 3);

  /**
   * Filters an integer image by a double-valued filter. The filter is
   * first scaled and rounded to integers. The image is then filtered
   * with the integer-valued filter, and the result is rescaled. This
   * is more efficient (and less accurate) than performing the
   * convolution with doubles.
   *
   * @param image input image
   *
   * @param doubleFilter the filter
   *
   * @param mode how to deal with borders
   *
   * @param scale a scaling factor for the filter. If `scale` is
   * zero, the function uses 256/max(abs(filter)).
   *
   * ~~~(c++)
   * PiiMatrix<int> filtered = PiiImage::intFilter(image, PiiImage::makeGaussian(5));
   * ~~~
   *
   * ! Beware of overflows! If the input image contains large
   * entries and `scale` is not set carefully, the operation may
   * easily overflow `int`.
   */
  PII_IMAGE_EXPORT PiiMatrix<int> intFilter(const PiiMatrix<int>& image,
                                            const PiiMatrix<double>& doubleFilter,
                                            Pii::ExtendMode mode = Pii::ExtendReplicate,
                                            double scale = 0);

  /**
   * Filters an integer image by a double-valued separable filter. See
   * [intFilter()] and [filter()] for details.
   *
   * @param image input image
   *
   * @param horizontalFilter horizontal component of a decomposed
   * filter
   *
   * @param verticalFilter vertical component of a decomposed filter
   *
   * @param mode how to deal with borders
   *
   * @param scale a scaling factor for the filters. If `scale` is
   * zero, the function uses 64/max(abs(filter)) for both.
   *
   * ! Beware of overflows! If the input image contains large
   * entries and `scale` is not set carefully, the operation may
   * easily overflow `int`.
   */
  PII_IMAGE_EXPORT PiiMatrix<int> intFilter(const PiiMatrix<int>& image,
                                            const PiiMatrix<double>& horizontalFilter,
                                            const PiiMatrix<double>& verticalFilter,
                                            Pii::ExtendMode mode = Pii::ExtendReplicate,
                                            double scale = 0);

  /**
   * Filters an image with a median filter.
   *
   * @param image the input image
   *
   * @param windowRows filter size in vertical direction
   *
   * @param windowColumns filter size in horizontal direction. If this
   * value is less than one, `windowRows` will be used instead.
   *
   * @param mode the method of handling image borders
   */
  template <class T> PiiMatrix<T> medianFilter(const PiiMatrix<T>& image,
                                               int windowRows = 3, int windowColumns = 0,
                                               Pii::ExtendMode mode = Pii::ExtendZeros);

  /**
   * Filters *image* with a maximum filter. The maximum filter is a
   * non-linear filter that produces an image in which each pixel is
   * replaced by the local maximum.
   *
   * @param image the input image
   *
   * @param windowRows the size of the local window in vertical
   * direction.
   *
   * @param windowColumns the size of the local window in horizontal
   * direction. If this value is less than one, `windowRows` will be
   * used instead.
   */
  template <class T> PiiMatrix<T> maxFilter(const PiiMatrix<T>& image,
                                            int windowRows, int windowColumns = -1);

  /**
   * Filters *image* with a minimum filter. The minimum filter is
   * similar to the maximum filter, but calculates the local minimum
   * instead.
   *
   * @see maxFilter()
   */
  template <class T> PiiMatrix<T> minFilter(const PiiMatrix<T>& image,
                                            int windowRows, int windowColumns = -1);
  /**
   * Scales image to a specified size.
   *
   * @param image input image
   *
   * @param rows the number of pixel rows in the result image
   *
   * @param columns the number of pixel columns in the result image
   *
   * @param interpolation interpolation mode
   */
  template <class T> PiiMatrix<T> scale(const PiiMatrix<T>& image, int rows, int columns,
                                        Pii::Interpolation interpolation = Pii::LinearInterpolation);

  /**
   * Scales image according to a scale ratio.
   *
   * @param image input image
   *
   * @param scaleRatio scale ratio, which must be larger than zero.
   *
   * @param interpolation interpolation mode
   */
  template <class T> PiiMatrix<T> scale(const PiiMatrix<T>& image, double scaleRatio, Pii::Interpolation interpolation = Pii::LinearInterpolation)
  {
    return scale(image, int(scaleRatio*image.rows()), int(scaleRatio*image.columns()), interpolation);
  }

  /**
   * Quickly scales an image to a quarter of its original size. This
   * function halves both dimensions of the image by calculating the
   * average of four neighboring pixels. If the input image has an odd
   * number of rows/columns, the last one will be ignored.
   */
  template <class T> PiiMatrix<T> quarterSize(const PiiMatrix<T>& image);

  /**
   * Quickly scales an image to one sixteenth of its original size.
   * This function divides both dimensions of the image by four and
   * calculates the average og each 4-by-4 block of pixels. If the
   * size of the input image is not divisible by four, extra pixels
   * will be ignored, and the scaled portion will be taken from the
   * center of the image.
   */
  template <class T> PiiMatrix<T> oneSixteenthSize(const PiiMatrix<T>& image);

  /**
   * Transforms a 2D point using *transform*. The source point is
   * represented in homogeneous coordinates; it is assumed that the
   * third coordinate is one.
   *
   * @param transform a 3-by-3 transformation matrix
   *
   * @param sourceX the x coordinate of the source point
   *
   * @param sourceY the y coordinate of the source point
   *
   * @param transformedX an output value argument that will store the
   * transformed x coordinate.
   *
   * @param transformedY an output value argument that will store the
   * transformed y coordinate.
   */
  template <class T> void transformHomogeneousPoint(const PiiMatrix<T>& transform,
                                                    T sourceX, T sourceY,
                                                    T* transformedX, T* transformedY);

  /**
   * Transforms 2D point coordinates using *transform*. This function
   * applies transformHomogeneousPoint() to all rows in *points* and
   * returns the transformed points in a new matrix. *points* must
   * have two columns.
   */
  template <class T, class U> PiiMatrix<U> transformHomogeneousPoints(const PiiMatrix<T>& transform,
                                                                      const PiiMatrix<U>& points);

  /**
   * Creates a transform that rotates a coordinate system `theta`
   * radians around its origin.
   *
   * ! In the image coordinate system, angles grow clockwise.
   *
   * @see transform()
   */
  PII_IMAGE_EXPORT PiiMatrix<float> createRotationTransform(float theta);

  /**
   * Creates a transform that rotates a coordinate system `theta`
   * radians around the specified center point.
   *
   * ~~~(c++)
   * // Rotate 45 degrees around image center
   * PiiMatrix<int> img(100,100);
   * PiiMatrix<float> matRotation = createRotationTransform(M_PI/4,
   *                                                        img.columns()/2.0,
   *                                                        img.rows()/2.0);
   * img = transform(img, matRotation);
   * ~~~
   *
   * ! In the image coordinate system, angles grow clockwise.
   *
   * @see transform()
   */
  PII_IMAGE_EXPORT PiiMatrix<float> createRotationTransform(float theta, float centerX, float centerY);

  /**
   * Creates a transform that shears a coordinate system by
   * `shearX` horizontally and by `shearY` vertically.
   *
   * @see transform()
   */
  PII_IMAGE_EXPORT PiiMatrix<float> createShearingTransform(float shearX, float shearY);

  /**
   * Creates a transform that translates a coordinate system by `x`
   * horizontally and by `y` vertically.
   *
   * @see transform()
   */
  PII_IMAGE_EXPORT PiiMatrix<float> createTranslationTransform(float x, float y);

  /**
   * Creates a transform that scales a coordinate system by `scaleX`
   * horizontally and by `scaleY` vertically. If you just want to
   * scale an image, it is a good idea to use the [scale()]
   * function instead. It is faster than [transform()] and produces
   * better results.
   *
   * @see transform()
   * @see scale()
   */
  PII_IMAGE_EXPORT PiiMatrix<float> createScalingTransform(float scaleX, float scaleY);

  /**
   * Applies an arbitrary geometric transform to `image`.
   *
   * @param image the image to be transformed
   *
   * @param transform a 3-by-3 matrix specifying a geometric transform
   * of original image coordinates to a new domain.
   *
   * @param handling clipping style.
   *
   * @param backgroundColor the color used for the parts of the output
   * image that are outside of the input image.
   *
   * @return the transformed image
   *
   * @exception PiiMathException& if the transform matrix is invalid
   * (singular) and cannot be inverted.
   *
   * Homogeneous coordinates are used in calculations to allow
   * simultaneous translations. Image coordinates are represented as a
   * column vector \(p = [x\ y\ 1]^T\). The transformed coordinates
   * are obtained by \(p' = Ap\), where *A* is the transformation
   * matrix. For example, a transformation that rotates the image
   * \(\theta\) radians, is specified as follows:
   *
   * \[
   * \left( \begin{array}{c} x' \\ y' \\ 1 \end{array} \right) =
   * \left( \begin{array}{ccc}
   * \cos \theta & -\sin\theta & 0 \\
   * \sin \theta & \cos \theta & 0 \\
   * 0 & 0 & 1
   * \end{array} \right)
   * \left( \begin{array}{c} x \\ y \\ 1 \end{array} \right)
   * \]
   *
   * Transformation matrices are most easily created by
   * [createRotationTransform()], [createScalingTransform()],
   * [createTranslationTransform], and [createShearingTransform()].
   * Transformations can be chained by simply multiplying the
   * transformation matrices. Assume *R* is a rotation transform and
   * *S* is a shear transform. Shear after rotate transform is
   * obtained with \(T = SR\).
   */
  template <class T> PiiMatrix<T> transform(const PiiMatrix<T>& image,
                                            const PiiMatrix<float>& transform,
                                            TransformedSize handling = ExpandAsNecessary,
                                            T backgroundColor = T(0));

  /**
   * Rotates image clockwise `theta` radians around its center.
   *
   * @param image the image to be rotated
   *
   * @param theta rotation angle in radians.
   *
   * @param handling clipping style.
   *
   * @param backgroundColor the color used for the parts of the output
   * image that are outside of the input image.
   *
   * @return rotated image
   */
  template <class T> PiiMatrix<T> rotate(const PiiMatrix<T>& image,
                                         double theta,
                                         TransformedSize handling = ExpandAsNecessary,
                                         T backgroundColor = T(0));

  /**
   * Crop a rectangular area out of a transformed image, in which the
   * target may not appear as a rectangular object. This function
   * makes it possible to cut parts rectangular pars of an image even
   * if the image was transformed with an affine transform.
   *
   * @param image the source image
   *
   * @param x the x coordinate of the original upper left corner of
   * the rectangle
   *
   * @param y the y coordinate of the original upper left corner of
   * the rectangle
   *
   * @param width the width of the rectangle to cut out of the image
   *
   * @param height the height of the rectangle to cut out of the image
   *
   * @param transform a 3-by-3 transformation matrix A that relates
   * the coordinates of the result r to the image i: Ar = i. The
   * function uses homogeneous coordinates.
   *
   * ~~~(c++)
   * PiiMatrix<int> image(5, 5,
   *                      0, 0, 1, 0, 0,
   *                      0, 0, 2, 0, 0,
   *                      0, 0, 3, 0, 0,
   *                      0, 0, 4, 0, 0,
   *                      0, 0, 5, 0, 0);
   *
   * // This transformation rotates 90 degrees clockwise around
   * // origin (in image coordinates, y down) and translates
   * // 3 steps to the right and one step down.
   * PiiMatrix<float> matTransform(3, 3,
   *                               0.0, -1.0, 3.0,
   *                               1.0, 0.0, 1.0,
   *                               0.0, 0.0, 1.0);
   *
   * // The non-transformed area was a 3-by-3 square at the upper left corner.
   * PiiMatrix<int> matCropped = PiiImage::crop(image,
   *                                            0, 0, 3, 3,
   *                                            matTransform);
   *
   * // matCropped = 0 0 0
   * //              2 3 4
   * //              0 0 0
   * ~~~
   */
  template <class T> PiiMatrix<T> crop(const PiiMatrix<T>& image,
                                       int x, int y,
                                       int width, int height,
                                       const PiiMatrix<float>& transform);
  /**
   * Detects corners in *image* using the FAST corner detector.
   *
   * @param image the input image
   *
   * @param threshold detection threshold. This value affects both the
   * number of detections and the detection speed. A high value
   * accepts only strong corners. The lower the value, the more
   * corners are detected, which also means lower processing speed.
   *
   * @return a N-by-2 matrix in which each row stores the (x,y)
   * coordinates of a detected corner.
   */
  template <class T> PiiMatrix<int> detectFastCorners(const PiiMatrix<T>& image, T threshold=25);

  /**
   * Transforms *image* according to the given coordinate *map*. The
   * size of the resulting image will be equal to the size of the
   * *map*. Each pixel in the result image will be sampled from *image*
   * according to map. For example, if map(0,0) is (1,2), the pixel at
   * (0,0) in the result image will be taken from image(1,2). If the
   * mapping would result in accessing *image* outside of its
   * boundaries, the corresponding pixel in the result image will be
   * left black. If the map coordinates are given as `doubles`, this
   * function samples *image* using bilinear interpolation.
   */
  template <class T, class U> PiiMatrix<T> remap(const PiiMatrix<T>& image, const PiiMatrix<PiiPoint<U> >& map);

  template <class T, class Matrix, class UnaryFunction>
  PiiMatrix<T> collectCoordinates(const Matrix& image,
                                  UnaryFunction decisionRule)
  {
    const int iRows = image.rows();
    const int iCols = image.columns();

    // Collect matching points
    PiiMatrix<T> matXY(0, 2);
    matXY.reserve(128);

    for (typename Matrix::const_iterator i=image.begin();
         i != image.end(); ++i)
      if (decisionRule(*i))
        matXY.appendRow(i.column(), i.row());

    return matXY;
  }

  /**
   * Matches *templ* to *image* with a xor correlation technique.
   * Unlike standard correlation, xor correlation gives negative
   * weight to non-matching pixels. If both image and template are
   * binary, the operation is functionally equivalent to
   * (image-0.5)*(templ-0.5), where * denotes correlation. The
   * function returns a value in [0,1], where 1 means a perfect match.
   * If *templ* is larger than *image*, zero will be returned.
   */
  template <class T> double xorMatch(const PiiMatrix<T>& image, const PiiMatrix<T>& templ);

  /**
   * Transforms *input* to *function(output)*. This function calls
   * *function* for each pixel except if the type of the input matrix
   * is `unsigned char`. For such input, an optimized version based on
   * table look-ups is used.
   *
   * @param input an input image
   *
   * @param output an output image. The output image must be
   * initialized to the same size as the input
   *
   * @function an adaptable unary function to be applied to the input.
   */
  template <class Matrix, class UnaryFunction>
  inline void transform(const Matrix& input,
                        PiiMatrix<typename UnaryFunction::result_type>& output,
                        UnaryFunction function,
                        typename Pii::OnlyIf<!Pii::IsSame<typename Matrix::value_type, uchar>::boolValue>::Type = 0)
  {
    Pii::transform(input.begin(), input.end(), output.begin(), function);
  }

  template <class Matrix, class UnaryFunction>
  inline void transform(const Matrix& input,
                        PiiMatrix<typename UnaryFunction::result_type>& output,
                        UnaryFunction function,
                        typename Pii::OnlyIf<Pii::IsSame<typename Matrix::value_type, uchar>::boolValue>::Type = 0)
  {
    typedef typename UnaryFunction::result_type R;
    R map[256];
    for (int i=0; i<256; ++i)
      map[i] = function(uchar(i));

    Pii::transform(input.begin(), input.end(), output.begin(),
                   Pii::arrayLookup(map));
  }

}

#include "PiiImage-templates.h"

#endif //_PIIIMAGE_H
