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

#ifndef _PIICOLORCHANNELSPLITTER_H
#define _PIICOLORCHANNELSPLITTER_H

#include "PiiDefaultOperation.h"

/**
 * An operation that splits color images into channels. The operation
 * reads in any color image type and outputs three to four separated
 * channel images on different outputs.
 *
 * Inputs
 * ------
 *
 * @in image - a color image
 *
 * Outputs
 * -------
 *
 * @out channelX - color channel X as an intensity image. 
 * `channel0` is for the red color channel, `channel1` for green, and
 * `channel2` for blue. If the input image has an alpha channel, it
 * is emitted from `channel3`.
 *
 */
class PiiColorChannelSplitter : public PiiDefaultOperation
{
  Q_OBJECT

  PII_OPERATION_SERIALIZATION_FUNCTION;

public:
  PiiColorChannelSplitter();
  ~PiiColorChannelSplitter();
  
protected:
  void process();

private:
  template <class Color, int channels> void splitChannels(const PiiVariant& obj);
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiInputSocket* pImageInput;
  };
  PII_D_FUNC;

};

#endif //_PIICOLORCHANNELSPLITTER_H
