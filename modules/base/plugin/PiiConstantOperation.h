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

#ifndef _PIICONSTANTOPERATION_H
#define _PIICONSTANTOPERATION_H

#include <PiiDefaultOperation.h>

/**
 * Emits a constant value.
 *
 * Inputs
 * ------
 *
 * @in trigger - an optional input for triggering the constant value.
 *
 * Outputs
 * -------
 *
 * @out value - a constant value each time `trigger` is received or
 * continuously if `trigger` is not connected.
 *
 */
class PiiConstantOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The value to emit. The default value is 0 (int).
   */
  Q_PROPERTY(PiiVariant value READ value WRITE setValue);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiConstantOperation();

protected:
  void process();

  void setValue(const PiiVariant& value);
  PiiVariant value() const;

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiVariant value;
  };
  PII_D_FUNC;
};


#endif //_PIICONSTANTOPERATION_H
