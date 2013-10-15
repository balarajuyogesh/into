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

#ifndef _PIIABSOLUTEOPERATION_H
#define _PIIABSOLUTEOPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>

/**
 * An operation that calculates the absolute value of any numeric or
 * complex type or a matrix containing such types.
 *
 * Inputs
 * ------
 *
 * @in input - any number or a matrix containing numbers
 *
 * Outputs
 * -------
 *
 * @out output - absolute value(s)
 *
 */
class PiiAbsoluteOperation : public PiiDefaultOperation
{
  Q_OBJECT

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiAbsoluteOperation();
  
protected:
  void process();
  
private:
  template <class T> void operatePrimitive( const PiiVariant& obj );
  template <class T> void operateMatrix( const PiiVariant& obj );
};


#endif //_PIIABSOLUTEOPERATION_H
