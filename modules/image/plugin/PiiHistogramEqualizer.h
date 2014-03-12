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

#ifndef _PIIHISTOGRAMEQUALIZER_H
#define _PIIHISTOGRAMEQUALIZER_H

#include <PiiDefaultOperation.h>

/**
 * Histogram equalizer. Enhances the contrast of input images by
 * making their gray-level distributions as uniform as possible.
 *
 * Inputs
 * ------
 *
 * @in image - The input image. Any integer-valued gray-level image.
 *
 * Outputs
 * -------
 *
 * @out image - Equalized image. Type equals that of the input.
 *
 */
class PiiHistogramEqualizer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of discrete gray levels in the output image. The
   * default value is 256. Setting this value to zero makes the
   * operation use the maximum value of the input image.
   */
  Q_PROPERTY(int levels READ levels WRITE setLevels);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiHistogramEqualizer();

  void setLevels(int levels);
  int levels() const;

protected:
  void process();

private:
  template <class T> void equalize(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    int iLevels;};
  PII_D_FUNC;

};


#endif //_PIIHISTOGRAMEQUALIZER_H
