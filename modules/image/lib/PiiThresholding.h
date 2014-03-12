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

#ifndef _PIITHRESHOLDING_H
#define _PIITHRESHOLDING_H

#include <PiiMatrix.h>
#include "PiiHistogram.h"
#include "PiiLabeling.h"

namespace PiiImage
{
  /**
   * The Otsu method is used in automatically selecting an optimal
   * threshold. The Otsu method works best with histograms that have
   * two modes: dark objects on bright background or vice-versa.
   *
   * @param histogram normalized histogram
   *
   * @return optimal threshold for separating the two modes
   *
   * ~~~(c++)
   * using namespace PiiImage;
   * int iThreshold = otsuThreshold(normalize<float>(histogram(image)));
   * ~~~
   */
  template <class T> int otsuThreshold(const PiiMatrix<T>& histogram);

  /**
   * %ThresholdFunction compares two values and outputs either 0 or 1
   * based on the comparison result. You can use this struct for
   * example with PiiMatrix<T>::map():
   *
   * ~~~(c++)
   * int threshold = 5;
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.map<PiiImage::ThresholdFunction<int> >(threshold);
   *
   * // mat = 0 0 0 0 1 1 1 1 1
   *
   * const PiiMatrix<int> mat2(1,9,1,2,3,4,5,6,7,8,9);
   * PiiMatrix<bool> binary = mat2.mapped(PiiImage::ThresholdFunction<int,bool>(), 5);
   *
   * // binary = 0 0 0 0 1 1 1 1 1
   * ~~~
   */
  template <class V, class R = V, class T = V>
  struct ThresholdFunction : public Pii::BinaryFunction<V,T,R>
  {
    ThresholdFunction(R value=1) : _value(value) {}
    R operator() (V value, T threshold) const { return value < threshold ? R(0) : _value; }

  private:
    const R _value;
  };

  template <class V, class R, class T> struct ThresholdFunction<PiiColor<V>, PiiColor<R>, T > :
    public Pii::BinaryFunction<PiiColor<V>, T, PiiColor<R> >
  {
    ThresholdFunction(R value=1) : _value(value) {}
    PiiColor<R> operator() (const PiiColor<V>& value, T threshold) const
    {
      return PiiColor<R>((V)value < threshold ? R(0) : _value);
    }

  private:
    const R _value;
  };

  template <class V, class R, class T> struct ThresholdFunction<PiiColor4<V>, PiiColor4<R>, T > :
    public Pii::BinaryFunction<PiiColor4<V>, T, PiiColor4<R> >
  {
    ThresholdFunction(R value=1) : _value(value) {}
    PiiColor4<R> operator() (const PiiColor4<V>& value, T threshold) const
    {
      return PiiColor4<R>((V)value < threshold ? R(0) : _value);
    }

  private:
    const R _value;
  };

  /**
   * %InverseThresholdFunction compares two values and outputs either 0
   * or 1 based on the comparison result. Works analogously to
   * ThresholdFunction, but outputs one if `value` is smaller than or
   * equal to `threshold`. You can use this struct for
   * example with PiiMatrix<T>::binaryOp():
   *
   * ~~~(c++)
   * int threshold = 5;
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.binaryOp(PiiImage::InverseThresholdFunction<int>(), threshold);
   *
   * // mat = 1 1 1 1 0 0 0 0 0
   * ~~~
   */
  template <class V, class R = V, class T = V>
  struct InverseThresholdFunction : public Pii::BinaryFunction<V,T,R>
  {
    InverseThresholdFunction(R value=1) : _value(value) {}
    R operator() (V value, T threshold) const { return value >= threshold ? R(0) : _value; }

  private:
    const R _value;
  };

  template <class V, class R, class T> struct InverseThresholdFunction<PiiColor<V>, PiiColor<R>, T > :
    public Pii::BinaryFunction<PiiColor<V>, T, PiiColor<R> >
  {
    InverseThresholdFunction(R value=1) : _value(value) {}
    PiiColor<R> operator() (const PiiColor<V>& value, T threshold) const
    {
      return PiiColor<R>((V)value >= threshold ? R(0) : _value);
    }

  private:
    const R _value;
  };

  template <class V, class R, class T> struct InverseThresholdFunction<PiiColor4<V>, PiiColor4<R>, T > :
    public Pii::BinaryFunction<PiiColor4<V>, T, PiiColor4<R> >
  {
    InverseThresholdFunction(R value=1) : _value(value) {}
    PiiColor4<R> operator() (const PiiColor4<V>& value, T threshold) const
    {
      return PiiColor4<R>((V)value >= threshold ? R(0) : _value);
    }

  private:
    const R _value;
  };

  /**
   * %TwoLevelThresholdFunction compares its input argument to two
   * thresholds and returns one if the argument is in between them
   * (inclusive).
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.unaryOp(PiiImage::TwoLevelThresholdFunction<int>(5, 7));
   *
   * // mat = 0 0 0 0 1 1 1 0 0
   * ~~~
   */
  template <class T, class U = T> class TwoLevelThresholdFunction : public Pii::BinaryFunction<T,T,U>
  {
  public:
    /**
     * Contruct a two-level threshold function that requires returns
     * one iff the input value is between the given two values
     * (inclusive).
     */
    TwoLevelThresholdFunction(T lowThreshold, T highThreshold, U value=1) :
      _lowThreshold(lowThreshold), _highThreshold(highThreshold), _value(value) {}
    U operator() (T value) const { return value >= _lowThreshold && value <= _highThreshold ? _value : U(0); }

  private:
    T _lowThreshold, _highThreshold;
    U _value;
  };

  /**
   * %InverseTwoLevelThresholdFunction works analogously to
   * TwoLevelThresholdFunction, but inverses the result.
   *
   * ~~~(c++)
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.unaryOp(PiiImage::InverseTwoLevelThresholdFunction<int>(5, 7));
   *
   * // mat = 1 1 1 1 0 0 0 1 1
   * ~~~
   */
  template <class T, class U = T> class InverseTwoLevelThresholdFunction : public Pii::BinaryFunction<T,T,U>
  {
  public:
    /**
     * Contruct a two-level threshold function that requires returns
     * zero iff the input value is between the given two values
     * (inclusive).
     */
    InverseTwoLevelThresholdFunction(T lowThreshold, T highThreshold, U value=1) :
      _lowThreshold(lowThreshold), _highThreshold(highThreshold), _value(value) {}
    U operator() (T value) const { return value >= _lowThreshold && value <= _highThreshold ? U(0) : _value; }

  private:
    T _lowThreshold, _highThreshold;
    U _value;
  };

  /**
   * CutFunction compares `value` to a `threshold` and outputs either
   * `value` or `threshold` based on the comparison result. You can
   * use this struct for example with PiiMatrix<T>::binaryOp():
   *
   * ~~~(c++)
   * int threshold = 5;
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.binaryOp(PiiImage::CutFunction<int>(), threshold);
   *
   * // mat = 1 2 3 4 5 5 5 5 5
   * ~~~
   */
  template <class T, class U = T> struct CutFunction : public Pii::BinaryFunction<T,T,U>
  {
    U operator() (T value, T threshold) const { return value < threshold ? U(value) : U(threshold); }
  };

  /**
   * InverseCutFunction compares `value` to a `threshold` and
   * outputs either `value` or `threshold` based on the comparison
   * result. You can use this struct for example with
   * PiiMatrix<T>::binaryOp():
   *
   * ~~~(c++)
   * int threshold = 5;
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.binaryOp(PiiImage::InverseCutFunction<int>(), threshold);
   *
   * // mat = 5 5 5 5 5 6 7 8 9
   * ~~~
   */
  template <class T, class U = T> struct InverseCutFunction : public Pii::BinaryFunction<T,T,U>
  {
    U operator() (T value, T threshold) const { return value < threshold ? U(threshold) : U(value); }
  };

  /**
   * ZeroBelowFunction compares `value` to a `threshold` and outputs
   * either `value` or 0 based on the comparison result. You can use
   * this struct for example with PiiMatrix<T>::binaryOp():
   *
   * ~~~(c++)
   * int threshold = 5;
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.binaryOp(PiiImage::ZeroBelowFunction<int>(), threshold);
   *
   * // mat = 0 0 0 0 5 6 7 8 9
   * ~~~
   */
  template <class T, class U = T> struct ZeroBelowFunction : public Pii::BinaryFunction<T,T,U>
  {
    U operator() (T value, T threshold) const { return value < threshold ? U(0) : U(value); }
  };

  /**
   * ZeroAboveFunction works analogously to ZeroBelowFunction, but
   * outputs zero for values that are above or equal to the threshold.
   *
   * ~~~(c++)
   * int threshold = 5;
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.binaryOp(PiiImage::ZeroAboveFunction<int>(), threshold);
   *
   * // mat = 1 0 1 2 3 4 0 0 0 0 0
   * ~~~
   */
  template <class T, class U = T> struct ZeroAboveFunction : public Pii::BinaryFunction<T,T,U>
  {
    U operator() (T value, T threshold) const { return value < threshold ? U(value) : U(0); }
  };

  /**
   * DropFunction compares `value` to a `threshold` and outputs
   * either `value` - `threshold` or 0 based on the comparison
   * result. The function "drops" gray levels above a threshold to
   * zero. You can use this struct for example with
   * PiiMatrix<T>::binaryOp():
   *
   * ~~~(c++)
   * int threshold = 5;
   * PiiMatrix<int> mat(1,9,1,2,3,4,5,6,7,8,9);
   * mat.binaryOp(PiiImage::DropFunction<int>(), threshold);
   *
   * // mat = 0 0 0 0 0 1 2 3 4
   * ~~~
   */
  template <class T, class U = T> struct DropFunction : public Pii::BinaryFunction<T,T,U>
  {
    U operator() (T value, T threshold) const { return value < threshold ? U(0) : U(value-threshold); }
  };

  template <class ThresholdFunc>
  struct AdaptiveThresholdFunction :
    Pii::BinaryFunction<typename ThresholdFunc::first_argument_type,
                        float,
                        typename ThresholdFunc::result_type>
  {
    AdaptiveThresholdFunction(const ThresholdFunc& func,
                              float relativeThreshold,
                              float absoluteThreshold) :
      _func(func),
      _relativeThreshold(relativeThreshold),
      _absoluteThreshold(absoluteThreshold)
    {}

    typename ThresholdFunc::result_type operator() (typename ThresholdFunc::first_argument_type pixel, float mean) const
    {
      return _func(pixel, _relativeThreshold * mean + _absoluteThreshold);
    }

    ThresholdFunc _func;
    float _relativeThreshold, _absoluteThreshold;
  };

  template <class ThresholdFunc> AdaptiveThresholdFunction<ThresholdFunc>
  adaptiveThresholdFunction(const ThresholdFunc& func,
                            float relativeThreshold,
                            float absoluteThreshold)
  {
    return AdaptiveThresholdFunction<ThresholdFunc>(func, relativeThreshold, absoluteThreshold);
  }

  template <class Matrix, class Function>
  inline PiiMatrix<typename Matrix::value_type> threshold(const Matrix& input,
                                                          Function function,
                                                          typename Matrix::value_type level)
  {
    typedef typename Matrix::value_type T;
    PiiMatrix<T> matOutput(PiiMatrix<T>::uninitialized(input.rows(), input.columns()));
    transform(input, matOutput, std::bind2nd(function, level));
    return matOutput;
  }

  /**
   * Thresholds an image.
   *
   * @param image original image.
   *
   * @param level gray level value for thresholding the original
   * image. Note that the threshold value must be of the same type as
   * the image. Don't try to pass an `int` if the image is `uchar`.
   *
   * @return thresholded binary image
   *
   * @see ThresholdFunction
   */
  template <class T> inline PiiMatrix<T> threshold(const PiiMatrix<T>& image, T level)
  {
    return threshold(image, ThresholdFunction<T>(), level);
  }

  /**
   * Thresholds and inverts an image.
   *
   * @param image original image.
   *
   * @param threshold gray level value for thresholding the original image
   *
   * @return thresholded binary image
   *
   * @see InverseThresholdFunction
   */
  template <class T> inline PiiMatrix<T> inverseThreshold(const PiiMatrix<T>& image, T level)
  {
    return threshold(image, InverseThresholdFunction<T>(), level);
  }

  /**
   * Sets pixels above *level* to *level*.
   *
   * @see CutFunction
   */
  template <class T> inline PiiMatrix<T> cut(const PiiMatrix<T>& image, T level)
  {
    return threshold(image, CutFunction<T>(), level);
  }

  /**
   * Sets pixels below *level* to *level*.
   *
   * @see InverseCutFunction
   */
  template <class T> inline PiiMatrix<T> inverseCut(const PiiMatrix<T>& image, T level)
  {
    return threshold(image, InverseCutFunction<T>(), level);
  }

  /**
   * Sets pixels below *level* to zero.
   *
   * @see ZeroBelowFunction
   */
  template <class T> inline PiiMatrix<T> zeroBelow(const PiiMatrix<T>& image, T level)
  {
    return threshold(image, ZeroBelowFunction<T>(), level);
  }

  /**
   * Sets pixels above *level* to zero.
   *
   * @see ZeroAboveFunction
   */
  template <class T> inline PiiMatrix<T> zeroAbove(const PiiMatrix<T>& image, T level)
  {
    return threshold(image, ZeroAboveFunction<T>(), level);
  }

  /**
   * Performs hysteresis thresholding on image. This is a convenience
   * function that uses [labelImage()] to find connected components in
   * which the gray level of all pixels is greater than or equal to
   * `lowThreshold` and and at least one pixel is greater than or equal
   * to `highThreshold`.
   *
   * @param image a gray-level image
   *
   * @param lowThreshold low threshold value
   *
   * @param highThreshold high threshold value
   *
   * @param connectivity the type of connected components
   *
   * @return a matrix in which all thresholded pixels are ones and
   * others are zeros.
   */
  template <class Matrix>
  inline PiiMatrix<int> hysteresisThreshold(const Matrix& image,
                                            typename Matrix::value_type lowThreshold,
                                            typename Matrix::value_type highThreshold,
                                            Connectivity connectivity = Connect8)
  {
    typedef typename Matrix::value_type T;
    return labelImage(image,
                      std::bind2nd(std::greater_equal<T>(), lowThreshold),
                      std::bind2nd(std::greater_equal<T>(), highThreshold),
                      connectivity,
                      0);
  }

  /**
   * Inverse hysteresis thresholding. Works inversely to
   * hysteresisThreshold(): connected components in which all gray
   * levels are below `highThreshold` and at least one gray level is
   * below `lowThreshold` will be set to one. Note that the result is
   * **not** the same as inverting the result of hysteresisThreshold().
   *
   * @param image a gray-level image
   *
   * @param lowThreshold low threshold value
   *
   * @param highThreshold high threshold value
   *
   * @param connectivity the type of connected components
   *
   * @return a matrix in which all thresholded pixels are ones and
   * others are zeros.
   */
  template <class Matrix>
  inline PiiMatrix<int> inverseHysteresisThreshold(const Matrix& image,
                                                   typename Matrix::value_type lowThreshold,
                                                   typename Matrix::value_type highThreshold,
                                                   Connectivity connectivity = Connect8)
  {
    typedef typename Matrix::value_type T;
    return labelImage(image,
                      std::bind2nd(std::less<T>(), highThreshold),
                      std::bind2nd(std::less<T>(), lowThreshold),
                      connectivity,
                      0);
  }

  /**
   * Thresholds an image adaptively. The threshold is calculated
   * separately for each pixel based on the local mean. The local mean
   * \(\mu\) is calculated over an area determined by `windowRows`
   * and `windowColumns`. The final threshold *t* is obtained by
   * \(t = \mu * r + a\), where *r* denotes `relativeThreshold`
   * and *a* `absoluteThreshold`. If `windowColumns` is
   * non-positive, the same value will be used for both window
   * dimensions.
   *
   * @param image the input image
   *
   * @param func a binary function whose return value replaces the
   * pixel value in `image`. Invoked as \(\mathrm{func}(p,t)\),
   * where `is` the value of a pixel and `t` is a locally calculated
   * threshold. The type of the threshold is `float` for all other
   * image types except `double`, in which case a `double` will be
   * passed.
   *
   * @param relativeThreshold multiply local average with this value
   *
   * @param absoluteThreshold add this value to the multiplication
   * result
   *
   * @param windowRows the number of rows in the local averaging
   * window
   *
   * @param windowColumns the number of columns in the local averaging
   * window. If this value is non-positive, a `windowRows` - by -
   * `windowRows` square window will be used.
   *
   * ! Window size has no effect on processing time.
   *
   * ~~~(c++)
   * using namespace PiiImage;
   * PiiMatrix<float> img;
   * // Threshold at 0.8 times the local mean. Using
   * // InverseThresholdFunction as function sets values lower than
   * // the threshold to one.
   * img = adaptiveThreshold(img, InverseThresholdFunction<float>(), 0.8, 0.0, 65);
   * ~~~
   *
   * ! If the input image is integer-typed, you can avoid rounding
   * errors by providing a different input type for the thresholding
   * function. ThresholdFunction<float,int>, for example, will take in
   * a `fload` and return an `int`.
   */
  template <class Matrix, class BinaryFunction>
  PiiMatrix<typename BinaryFunction::result_type> adaptiveThreshold(const Matrix& image,
                                                                    BinaryFunction func,
                                                                    float relativeThreshold,
                                                                    float absoluteThreshold,
                                                                    int windowRows, int windowColumns);

  /**
   * the local mean as parameters.
   */
  template <class Matrix, class BinaryFunction>
  PiiMatrix<typename BinaryFunction::result_type> adaptiveThreshold(const Matrix& image,
                                                                    BinaryFunction func,
                                                                    int windowRows, int windowColumns);
  /**
   * entry in *roiMask* evaluates to `true`. The *image* and
   * *roiMask* matrices must match in size.
   */
  template <class Matrix, class BinaryFunction>
  PiiMatrix<typename BinaryFunction::result_type> adaptiveThreshold(const Matrix& image,
                                                                    const PiiMatrix<bool>& roiMask,
                                                                    BinaryFunction func,
                                                                    int windowRows, int windowColumns);

  /**
   * A function that calculates a local threshold *t* as \(t = \mu + r
   * \sigma\), where *r* stands for `relativeThreshold`. The output of
   * the function is *thresholdFunc(transformFunc(pixel), t)*.
   *
   * @see adaptiveThreshold()
   */
  template <class BinaryFunction, class UnaryFunction>
  struct MeanStdThresholdFunction :
    Pii::TernaryFunction<typename UnaryFunction::argument_type,
                         double,
                         double,
                         typename BinaryFunction::result_type>
  {
    MeanStdThresholdFunction(const BinaryFunction& thresholdFunc,
                             const UnaryFunction& transformFunc,
                             double relativeThreshold) :
      _thresholdFunc(thresholdFunc),
      _transformFunc(transformFunc),
      _dR(relativeThreshold)
    {}

    typename BinaryFunction::result_type operator() (typename UnaryFunction::argument_type pixel,
                                                     double mu,
                                                     double var) const
    {
      return _thresholdFunc(_transformFunc(pixel),
                            typename BinaryFunction::second_argument_type(mu + _dR * sqrt(var)));
    }

  private:
    const BinaryFunction& _thresholdFunc;
    const UnaryFunction& _transformFunc;
    double _dR;
  };

  /**
   * Creates a threshold function.
   *
   * @relates MeanStdThresholdFunction
   */
  template <class BinaryFunction, class UnaryFunction>
  MeanStdThresholdFunction<BinaryFunction, UnaryFunction> meanStdThresholdFunction(const BinaryFunction& threshold,
                                                                                   const UnaryFunction& transform,
                                                                                   double r)
  {
    return MeanStdThresholdFunction<BinaryFunction, UnaryFunction>(threshold, transform, r);
  }

  /**
   * A function that calculates a local threshold *t* as \(t = \mu (1
   * + r (\sigma/\sigma_{\mathrm max} - 1))\), where *r* stands for
   * `relativeThreshold` and \(\sigma_{\mathrm max}\) for the maximum
   * possible standard deviation. The output of the function is
   * *thresholdFunc(transformFunc(pixel), t)*.
   *
   * @see adaptiveThreshold()
   */
  template <class BinaryFunction, class UnaryFunction>
  struct SauvolaThresholdFunction :
    Pii::TernaryFunction<typename UnaryFunction::argument_type,
                         double,
                         double,
                         typename BinaryFunction::result_type>
  {
    SauvolaThresholdFunction(const BinaryFunction& thresholdFunc,
                             const UnaryFunction& transformFunc,
                             double relativeThreshold,
                             double maxStd) :
      _thresholdFunc(thresholdFunc),
      _transformFunc(transformFunc),
      _dR(relativeThreshold),
      _dMaxStd(maxStd)
    {}

    typename BinaryFunction::result_type operator() (typename UnaryFunction::argument_type pixel,
                                                     double mu,
                                                     double var) const
    {
      return _thresholdFunc(_transformFunc(pixel),
                            typename BinaryFunction::second_argument_type(mu * (1 + _dR * (sqrt(var)/_dMaxStd - 1))));
    }

  private:
    const BinaryFunction& _thresholdFunc;
    const UnaryFunction& _transformFunc;
    double _dR, _dMaxStd;
  };

  /// @internal
  template <class T> struct MaximumStd
  {
    struct Int { static double value() { return 128; } };
    struct Float { static double value() { return 0.5; } };

    static double value()
    {
      return Pii::IfClass<Pii::IsInteger<T>, Int, Float>::Type::value();
    }
  };

  /**
   * Creates a threshold function.
   *
   * @param threshold a thresholding function, such as
   * ThresholdFunction
   *
   * @param transform a function that transforms input data before
   * applying the threshold. If no transform is needed, use
   * Pii::Identity.
   *
   * @param r weight of local deviation
   *
   * @param maxStd maximum standar deviation. The default value is 0.5
   * for floating point types and 128 for others.
   *
   * @relates SauvolaThresholdFunction
   */
  template <class BinaryFunction, class UnaryFunction>
  SauvolaThresholdFunction<BinaryFunction, UnaryFunction>
  sauvolaThresholdFunction(const BinaryFunction& threshold,
                           const UnaryFunction& transform,
                           double r,
                           double maxStd = MaximumStd<typename UnaryFunction::argument_type>::value())
  {
    return SauvolaThresholdFunction<BinaryFunction, UnaryFunction>(threshold, transform, r, maxStd);
  }

  /**
   * Thresholds an image adaptively. This function calculates local
   * mean and local variance for each pixel, and uses `func` as the
   * thresholding function.
   *
   * @param image the input image
   *
   * @param func a ternary function whose return value replaces the
   * pixel value in `image`. Invoked as
   * \(\mathrm{func}(p,\mu,\sigma^2)\), where \(p, \mu, \sigma^2\)
   * stand for the pixel value, the local mean, and the local
   * variance, respectively.
   *
   * @param windowRows the number of rows in the local averaging
   * window
   *
   * @param windowColumns the number of columns in the local averaging
   * window. If this value is non-positive, a `windowRows` - by -
   * `windowRows` square window will be used.
   *
   * ~~~(c++)
   * // Document binarization
   * using namespace PiiImage;
   * PiiMatrix<int> image;
   * image = adaptiveThresholdVar(image,
   *                              sauvolaThresholdFunction(ThresholdFunction<int>(),
   *                                                       Pii::Identity<int>(),
   *                                                       0.34, 255),
   *                              31);
   * ~~~
   *
   * The [sauvolaThresholdFunction()] and [meanStdThresholdFunction()]
   * helper functions can be used to create suitable thresholding
   * functions.
   */
  template <class TernaryFunction, class Matrix>
  PiiMatrix<typename TernaryFunction::result_type> adaptiveThresholdVar(const Matrix& image,
                                                                        TernaryFunction func,
                                                                        int windowRows, int windowColumns = 0);
}

#include "PiiThresholding-templates.h"

#endif //_PIITHRESHOLDING_H
