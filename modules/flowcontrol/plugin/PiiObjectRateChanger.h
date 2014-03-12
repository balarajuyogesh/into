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

#ifndef _PIIOBJECTRATECHANGER_H
#define _PIIOBJECTRATECHANGER_H

#include <PiiDefaultOperation.h>

/**
 * Replicate or sample input objects. Depending on the value of
 * [rateChange], this operation either replicates each input object N
 * times or passes just every Nth object.
 *
 * Inputs
 * ------
 *
 * @in input - any object
 *
 * Outputs
 * -------
 *
 * @out output - input objects, replicated or sampled
 *
 */
class PiiObjectRateChanger : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Change in object rate. The default value is 0 which causes every
   * input object to be passed as such. If this value is greater than
   * zero, every object will be sent `rateChange` times. If the value
   * is less than zero, `abs`(rateChange) objects will be skipped
   * after each passed object. For example, a rate change of -1 means
   * that every second object will be passed, starting from the second
   * one.
   */
  Q_PROPERTY(int rateChange READ rateChange WRITE setRateChange);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiObjectRateChanger();

  void check(bool reset);

protected:
  void process();

  void setRateChange(int rateChange);
  int rateChange() const;

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    int iRateChange, iCounter;
  };
  PII_D_FUNC;
};


#endif //_PIIOBJECTRATECHANGER_H
