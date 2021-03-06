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

#ifndef _PIICOLORCHANNELSETTER_H
#define _PIICOLORCHANNELSETTER_H

#include <PiiDefaultOperation.h>

/**
 * Sets indidivual color channels in images.
 *
 * Inputs
 * ------
 *
 * @in image - a color image to which color channels are to be set.
 * Optional. If this input is not connected, the output will be
 * composed of the individual color channels.
 *
 * @in channelX - individual color channels as intensity images. X
 * ranges from 0 to 3 (0 = red, 1 = blue, 2 = green, 3 = alpha in
 * RGBA).
 *
 * All inputs are optional, but at least one of them must be
 * connected.
 *
 * Outputs
 * -------
 *
 * @out image - result image. If `image` is connected, the output
 * will be the same type. Otherwise, if `channel3` is connected, or
 * if the [defaultValue3] property is set to a valid number, the output
 * image will have four color channels. Otherwise, there will be three
 * channels. The data type of the output channels is the same as that
 * of the first connected channel input.
 *
 */
class PiiColorChannelSetter : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Default value for color channel 0. If the `channel0` input is
   * not connected and this value is not `NaN`, the whole channel
   * will be set to this value. The default value is `NaN`.
   */
  Q_PROPERTY(double defaultValue0 READ defaultValue0 WRITE setDefaultValue0);

  /**
   * Default value for color channel 1. See [defaultValue0].
   */
  Q_PROPERTY(double defaultValue1 READ defaultValue1 WRITE setDefaultValue1);

  /**
   * Default value for color channel 2. See [defaultValue0].
   */
  Q_PROPERTY(double defaultValue2 READ defaultValue2 WRITE setDefaultValue2);

  /**
   * Default value for color channel 3. See [defaultValue0].
   */
  Q_PROPERTY(double defaultValue3 READ defaultValue3 WRITE setDefaultValue3);


  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiColorChannelSetter();

  void check(bool reset);

protected:
  void process();

  void setDefaultValue0(double defaultValue0);
  double defaultValue0() const;
  void setDefaultValue1(double defaultValue1);
  double defaultValue1() const;
  void setDefaultValue2(double defaultValue2);
  double defaultValue2() const;
  void setDefaultValue3(double defaultValue3);
  double defaultValue3() const;

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    PiiColor4<float> defaultColor;
    int iFirstConnectedInput;
  };
  PII_D_FUNC;

  template <class Clr> void setChannels(const PiiVariant& obj);
  template <class T> void setChannels();
  template <class Clr> void setChannels(PiiMatrix<Clr>& img);
  template <class T, class Clr> void setChannel(PiiMatrix<Clr>& img, int index, const PiiVariant& channel);
};


#endif //_PIICOLORCHANNELSETTER_H
