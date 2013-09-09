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

#ifndef _PIIMATRIXDECOMPOSER_H
#define _PIIMATRIXDECOMPOSER_H

#include <PiiDefaultOperation.h>

/**
 * Decomposes input matrix so that each row element corresponds one
 * output. So input matrix must be row matrix and size of columns must
 * be same as output count. You must set output counts with
 * outputCount-property first, default is 1.
 *
 * @inputs
 *
 * @in input - any row matrix
 * 
 * @outputs
 *
 * @out outputX - outputX is element (0,X) in input matrix.
 *
 * @ingroup PiiBasePlugin
 */
class PiiMatrixDecomposer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Specifies the number of the output sockets.
   */
  Q_PROPERTY(int dynamicOutputCount READ dynamicOutputCount WRITE setDynamicOutputCount);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiMatrixDecomposer();

  int dynamicOutputCount() const;
  void setDynamicOutputCount(int cnt);

protected:
  void process();

private:
  template<class T>  void operate(const PiiVariant& obj);
};


#endif //_PIIMATRIXDECOMPOSER_H
