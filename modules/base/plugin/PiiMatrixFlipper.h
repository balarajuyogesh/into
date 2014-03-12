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

#ifndef _PIIMATRIXFLIPPER_H
#define _PIIMATRIXFLIPPER_H

#include <PiiDefaultOperation.h>

/**
 * Flip a matrix vertically or horizontally.
 *
 * Inputs
 * ------
 *
 * @in input - any matrix
 *
 * Outputs
 * -------
 *
 * @out output - the input matrix flipped either vertically or
 * horizontally.
 *
 */
class PiiMatrixFlipper : public PiiDefaultOperation
{
  Q_OBJECT
  /**
   * Flip mode. Default is `FlipHorizontally`.
   */
  Q_PROPERTY(FlipMode flipMode READ flipMode WRITE setFlipMode);
  Q_ENUMS(FlipMode);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Flip modes.
   *
   * - `FlipHorizontally` - retain row order but turn data on each
   * row around.
   *
   * - `FlipVertically` - retain column order but turn rows around.
   *
   * - `FlipBoth` - flip horizontally and then vertically (same as
   * 180 degrees rotation)
   */
  enum FlipMode { FlipHorizontally, FlipVertically, FlipBoth };

  PiiMatrixFlipper();

protected:
  void process();

  void setFlipMode(const FlipMode& flipMode);
  FlipMode flipMode() const;

private:
  template <class T> void flip(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    FlipMode flipMode;
  };
  PII_D_FUNC;
};


#endif //_PIIMATRIXFLIPPER_H
