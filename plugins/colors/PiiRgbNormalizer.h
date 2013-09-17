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

#ifndef _PIIRGBNORMALIZER_H
#define _PIIRGBNORMALIZER_H

#include <PiiDefaultOperation.h>

/**
 * An operation that converts images to normalized RGB. See @ref
 * PiiColors::normalizedRgb() for details.
 *
 * Inputs
 * ------
 *
 * @in image - any color image
 * 
 * Outputs
 * -------
 *
 * @out channel0 - first normalized color channel (red, by default)
 *
 * @out channel1 - second normalized color channel (green, by default)
 *
 */
class PiiRgbNormalizer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The maximum value a normalized color channel can attain. The
   * default is 255. This allows simultaneous quantization and
   * normalization.
   */
  Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue);

  /**
   * The normalized color channels the operation will emit through 
   * `channel0` and `channel1`.
   */
  Q_PROPERTY(ChannelPair channels READ channels WRITE setChannels);
  Q_ENUMS(ChannelPair);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Allowed channel pairs. 
   *
   * - `RedGreen` - normalized RG. `channel0` will emit the red
   * channel and `channel1` the green channel.
   *
   * - `RedBlue` - normalized RB
   *
   * - `GreenBlue` - normalized GB
   */
  enum ChannelPair { RedGreen, RedBlue, GreenBlue };
  
  PiiRgbNormalizer();

  void setMaxValue(double maxValue);
  double maxValue() const;

  void setChannels(const ChannelPair& channels);
  ChannelPair channels() const;

protected:
  void process();

private:
  template <class T> void normalizedRgb(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    double dMaxValue;
    ChannelPair channels;
  };
  PII_D_FUNC;
};


#endif //_PIIRGBNORMALIZER_H
