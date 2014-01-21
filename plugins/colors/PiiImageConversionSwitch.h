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

#ifndef _PIIIMAGECONVERSIONSWITCH_H
#define _PIIIMAGECONVERSIONSWITCH_H

#include <PiiDefaultOperation.h>

/**
 * PiiImageConversionSwitch is an operation which convert a receiving
 * image from the color/gray to the gray/color type if necessary.
 *
 * Inputs
 * ------
 *
 * @in image - color or gray scale image
 *
 * Outputs
 * -------
 *
 * @out gray image - gray scale image
 *
 * @out color image - color image
 *
 * @see PiiColorConverter
 *
 */
class PiiImageConversionSwitch : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The type of color conversion. The default value is
   * `RgbToGrayMean`.
   */
  Q_PROPERTY(ColorConversion colorConversion READ colorConversion WRITE setColorConversion);
  Q_ENUMS(ColorConversion);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Supported color conversion schemes.
   *
   * - `RgbToGrayMean` - calculate the mean of three color channels.
   * Retains the type of color channels, but outputs a gray-level
   * image.
   */
  enum ColorConversion
  {
    RgbToGrayMean
  };

  PiiImageConversionSwitch();

protected:
  void check(bool reset);
  void process();

  void setColorConversion(ColorConversion colorConversion);
  ColorConversion colorConversion() const;

private:
  /**
   * Emit the gray image forward to gray image output and convert it
   * to PiiUnsignedCharColor4MatrixType for color output if it is connected.
   */
  void operateGrayImage(const PiiVariant& obj);
  /**
   * Emit the color image forward to color image output and convert it
   * to PiiUnsignedCharMatrixType for gray output if it is connected.
   */
  void operateColorImage(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiInputSocket *pImageInput;
    PiiOutputSocket *pGrayImageOutput, *pColorImageOutput;
    bool bGrayImageOutputConnected, bColorImageOutputConnected;

    ColorConversion colorConversion;
  };
  PII_D_FUNC;
};


#endif //_PIIIMAGECONVERSIONSWITCH_H
