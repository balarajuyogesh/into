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

#ifndef _PIITYPECASTINGOPERATION_H
#define _PIITYPECASTINGOPERATION_H

#include <PiiDefaultOperation.h>
#include <QPair>
#include <PiiMatrix.h>

/**
 * An operation that converts the data type of a matrix.
 *
 * Inputs
 * ------
 *
 * @in input - any matrix.
 *
 * Outputs
 * -------
 *
 * @out output - a matrix whose type is specified by [outputType].
 *
 */
class PiiTypeCastingOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The type id of the output. See PiiYdinTypes for valid type id
   * numbers. The default is 0x48 (PiiMatrix<unsigned char>).
   */
  Q_PROPERTY(int outputType READ outputType WRITE setOutputType);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiTypeCastingOperation();
  ~PiiTypeCastingOperation();

protected:
  void process();

  int outputType() const;
  void setOutputType(int outputType);

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    int outputType;
  };
  PII_D_FUNC;

  template <class T> void operate( const PiiVariant& obj );
  template <class T> void operateC( const PiiVariant& obj );
  template <class T> void operateCI( const PiiVariant& obj );
  template <class T, class U> void operateMatrix( const PiiMatrix<U>& matrix );
  template <class T, class U> void operateComplex( const PiiMatrix<U>& matrix );
  template <class T, class U> void operateColorToGray( const PiiMatrix<U>& matrix );
};

#endif //_PIITYPECASTINGOPERATION_H
