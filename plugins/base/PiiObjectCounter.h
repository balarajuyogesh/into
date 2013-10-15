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

#ifndef _PIIOBJECTCOUNTER_H
#define _PIIOBJECTCOUNTER_H

#include <PiiDefaultOperation.h>

/**
 * Counts received objects.
 *
 * Inputs
 * ------
 *
 * @in input - any object
 * 
 * Outputs
 * -------
 *
 * @out count - the number of objects received so far.
 *
 */
class PiiObjectCounter : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of objects received. If you want zero-based indices
   * instead of object count, set the count to -1 before starting the
   * operation.
   */
  Q_PROPERTY(int count READ count WRITE setCount);

  /**
   * If this flag is `true`, the value of the counter will be reset
   * to zero when the operation is restarted after stopping. If the
   * flag is `false`, the previous count will be retained. The
   * default value is `true`.
   */
  Q_PROPERTY(bool autoReset READ autoReset WRITE setAutoReset);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiObjectCounter();

  void check(bool reset);

  void setCount(int count);
  int count() const;
  void setAutoReset(bool autoReset);
  bool autoReset() const;
  
protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    int iCount;
    bool bAutoReset;
  };
  PII_D_FUNC;
};


#endif //_PIIOBJECTCOUNTER_H
