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

#ifndef _PIILOGOPERATION_H
#define _PIILOGOPERATION_H

#include <PiiDefaultOperation.h>

/**
 * An operation that calculates the logarithm of its input.
 *
 * Inputs
 * ------
 *
 * @in input - any number or numeric matrix.
 * 
 * Outputs
 * -------
 *
 * @out output - the logarithm as a double scalar or a double matrix.
 *
 */
class PiiLogOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The base of the logarithm. The default value is *e*.
   */
  Q_PROPERTY(double base READ base WRITE setBase);

  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  PiiLogOperation();

  void setBase(double base);
  double base() const;

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    double dBase, dOnePerLogBase;
  };
  PII_D_FUNC;

  template <class T> void logarithm(const PiiVariant& obj);
  template <class T> void matrixLogarithm(const PiiVariant& obj);
};

#endif //_PIILOGOPERATION_H
