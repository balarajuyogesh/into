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

#ifndef _PIIDIFFOPERATION_H
#define _PIIDIFFOPERATION_H

#include <PiiDefaultOperation.h>

/**
 * Calculates the difference between two successive input objects.
 *
 * @inputs
 *
 * @in input - the input object. Any numeric or complex type, or a
 * matrix containing such types. Note that the difference between
 * unsigned integers may easily underflow.
 * 
 * @outputs
 *
 * @out difference - the difference between current object and the
 * last one. The type is equal to the input type. If there is no
 * previous object, the difference is calculated with the input object
 * itself. Thus, the first difference is always zero.
 *
 * @ingroup PiiBasePlugin
 */
class PiiDiffOperation : public PiiDefaultOperation
{
  Q_OBJECT

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiDiffOperation();

protected:
  void process();
  void check(bool reset);
  
private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiVariant lastObject;
  };
  PII_D_FUNC;

  template <class T> void diff(const PiiVariant& obj);
  template <class T> inline void matrixDiff(const PiiVariant& obj);
};


#endif //_PIIDIFFOPERATION_H
