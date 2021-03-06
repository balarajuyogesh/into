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

#ifndef _PIICOLORCONVERTER_H
#define _PIICOLORCONVERTER_H

#include <PiiDefaultOperation.h>

/**
 * Convert color spaces. This operation performs color conversions
 * between RGB, HSV, Y'CbCr, XYZ, CIELAB and other color spaces.
 *
 * Inputs
 * ------
 *
 * @in image - a color image.
 *
 * Outputs
 * -------
 *
 * @out image - a color image. The type of the output is that of the
 * input, if possible. See [ColorConversion] for detailed information
 * of type changes.
 *
 */
class PiiColorConverter : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The type of color conversion. The default value is
   * `GenericConversion`.
   */
  Q_PROPERTY(ColorConversion colorConversion READ colorConversion WRITE setColorConversion);
  Q_ENUMS(ColorConversion);

  /**
   * A 3-by-3 conversion matrix (PiiMatrix<float> or
   * PiiMatrix<double>) for the generic conversion. Let us assume the
   * color to be converted is represented by \(C_i = [ c_1 c_2 c_3
   * ]^T\). The converted color is obtained by \(C_o = A C_i\), where
   * A is the conversion matrix. The default value is a 3-by-3
   * identity matrix.
   */
  Q_PROPERTY(PiiVariant conversionMatrix READ conversionMatrix WRITE setConversionMatrix);

  /**
   * The XYZ coordinates of the white point of imaging equipment in
   * prevailing illumination. This information is needed for the XYZ
   * to LAB conversion. The white point is represented as a 1-by-3
   * matrix (PiiMatrix<float> or PiiMatrix<double>) that stores the X,
   * Y, and Z coordinates, in this order. The default value is [95.05
   * 100 108.88], which is the white point of a camera with ITU-R
   * BT.709 primaries for a scene illuminated with a standard D65
   * light source. Since cameras never have ITU-R BT.709 primaries and
   * the illumination is not likely to be D65, you had better find the
   * real white point yourself.
   */
  Q_PROPERTY(PiiVariant whitePoint READ whitePoint WRITE setWhitePoint);

  /**
   * Correction factor for gamma correction. Default is 1/2.2, which
   * inverts the typical gamma of a monitor.
   */
  Q_PROPERTY(double gamma READ gamma WRITE setGamma);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Supported color conversion schemes.
   *
   * - `GenericConversion` - convert the colors by multiplying the
   * three-component color vector on each pixel with a generic
   * conversion matrix. The result type will always be
   * PiiMatrix<PiiColor<float> >.
   *
   * - `RgbToGrayMean` - calculate the mean of three color channels.
   * Retains the type of color channels, but outputs a gray-level
   * image.
   *
   * - `RgbToGrayMeanFloat` - calculate the mean of three color
   * channels. The type of the output will be PiiMatrix<float>.
   *
   * - `RgbToGraySum` - calculate the sum of three color channels.
   * The type of the output will be PiiMatrix<int> for all types
   * except PiiMatrix<PiiColor<float> >, for which it equals the
   * input.
   *
   * - `RgbToHsv` - convert RGB to HSV. Retains the input type. See
   * PiiColors::rgbToHsv().
   *
   * - `HsvToRgb` - convert HSV to RGB. Retains the input type. See
   * PiiColors::hsvToRgb().
   *
   * - `BgrToRgb` - reverse the order of color channels. Retains the
   * input type.
   *
   * - `XyzToLab` - convert (linear) CIE XYZ colors to (non-linear,
   * perceptually uniform) CIE L*a*b*. This conversion requires
   * knowledge of the white point of imaging equipment. See
   * [whitePoint]. The input must be (or it will converted to) a
   * PiiMatrix<PiiColor<float> >, and so will be the output. **Please
   * note that there is no universal conversion from RGB to XYZ or
   * from XYZ to L*a*b*.** You need to know the primaries of your
   * camera and the white point to make the conversion really work.
   * See PiiColors::xyzToLab().
   *
   * - `LabToXyz` - the inverse of XyzToLab. Same restrictions apply.
   * See PiiColors::labToXyz().
   *
   * - `RgbToLab` - convert RGB colors to CIE L*a*b*. This conversion
   * requires knowledge of the primaries of the imaging equipment and
   * its white point in prevailing illumination. This conversion works
   * like `GenericConversionFloat` followed by `XyzToLab`. And no,
   * there is no standard way to convert RGB to XYZ. Make sure your
   * conversion matrix is correct.
   *
   * - `RgbToOhtaKanade` - perform a linear conversion from RGB to
   * the maximally independent color components found by Ohta and
   * Kanade back in 1980. Retains the input type.
   *
   * - `RgbToY719` - convert non-linear RGB to luminance as defined
   * in CIE Rec. 709. The type of the output will be PiiMatrix<float>.
   * See PiiColors::rgbToY719().
   *
   * - `RgbToYpbpr` - convert non-linear RGB to Y'PbPr. Retains input
   * type. Note that the color type must be able to store negative
   * values. The preferable color channel type for this conversion is
   * `float`. See PiiColors::rgbToYpbpr().
   *
   * - `YpbprToRgb` - convert Y'PbPr to non-linear RGB. Retains input
   * type. See PiiColors::ypbprToRgb().
   *
   * - `RgbToYcbcr` - convert non-linear RGB to Y'CbCr. Retains input
   * type. See PiiColors::rgbToYcbcr().
   *
   * - `YcbcrToRgb` - convert Y'CbCr to non-linear RGB. Retains input
   * type. See PiiColors::ycbcrToRgb().
   *
   * - `GammaCorrection` - apply gamma correction to each color
   * channel. Uses [gamma] as the correction factor. Retains input
   * type. See PiiColors::correctGamma().
   *
   * @see PiiColors
   */
  enum ColorConversion
  {
    GenericConversion,
    RgbToGrayMean,
    RgbToGrayMeanFloat,
    RgbToGraySum,
    RgbToHsv,
    HsvToRgb,
    BgrToRgb,
    XyzToLab,
    LabToXyz,
    RgbToLab,
    RgbToOhtaKanade,
    RgbToYpbpr,
    YpbprToRgb,
    RgbToYcbcr,
    YcbcrToRgb,
    RgbToY719,
    GammaCorrection
  };

  PiiColorConverter();

  void setColorConversion(ColorConversion colorConversion);
  ColorConversion colorConversion() const;
  void setConversionMatrix(const PiiVariant& conversionMatrix);
  PiiVariant conversionMatrix() const;
  void setWhitePoint(const PiiVariant& whitePoint);
  PiiVariant whitePoint() const;

  void check(bool reset);

  void setGamma(double gamma);
  double gamma() const;

protected:
  void process();

private:
  template <class T> struct SumTraits;

  template <class T> void correctGamma(const PiiVariant& obj);
  template <class Clr> void convertImage(const PiiVariant& obj);
  template <class T, class UnaryFunction>
  void sumColors(const PiiMatrix<T>& image, UnaryFunction func);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    ColorConversion colorConversion;
    PiiVariant pConversionMatrix;
    PiiMatrix<float> matGenericConversion;
    PiiVariant pWhitePoint;
    PiiColor<float> clrWhitePoint;
    double dGamma;
  };
  PII_D_FUNC;
};

template <class T> struct PiiColorConverter::SumTraits { typedef int Type; };
template <> struct PiiColorConverter::SumTraits<float> { typedef float Type; };

#endif //_PIICOLORCONVERTER_H
