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

#ifndef _PIIARITHMETICOPERATION_H
#define _PIIARITHMETICOPERATION_H

#include <PiiDefaultOperation.h>
#include <QPair>
#include <PiiMatrix.h>

/**
 * An operation that performs simple arithmetic like adding and
 * subtracting values.
 *
 * @inputs
 *
 * @in input0 - first operand. Any matrix or scalar type.
 *
 * @in input1 - second operand (optional). If this input is not
 * connected, the second operand will initialized to a #constant
 * value. The default constant value is a zero (double). Note that if
 * this input is a matrix, @p input0 must also be a matrix.
 * 
 * @outputs
 *
 * @out output - the type of the output is a combination of the
 * inputs. For example, if @p input0 is PiiMatrix<int> and @p input1
 * is @p double, output type is PiiMatrix<double>. If both types are
 * the same, no type change will be made.
 *
 * @ingroup PiiBasePlugin
 */
class PiiArithmeticOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * A constant value for the input1. If input1 is not connected,
   * the second operand is this constant value.
   */
  Q_PROPERTY(PiiVariant constant READ constant WRITE setConstant);

  /**
   * The arithmetic operator to be used. Default value is @p Plus.
   */
  Q_PROPERTY(Function function READ function WRITE setFunction);
  Q_ENUMS(Function);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiArithmeticOperation();

  /**
   * Known arithmetic functions. We can't use "operator", because it
   * is reserved by C++.
   *
   * @lip Plus - addition
   *
   * @lip Minus - subtraction
   *
   * @lip Division - division
   *
   * @lip Multiplication - multiplication
   *
   * @lip ElementMultiplication - multiply matrices element-by-element
   * (see Pii::multiply()). This operator applies only to two
   * matrices. For other types, ordinary multiplication will be used.
   *
   * @lip ElementDivision - divide matrices element-by-element (see
   * Pii::divide()). This operator applies only to two matrices. 
   * For other types, ordinary division will be used.
   */
  enum Function { Plus, Minus, Division, Multiplication, ElementMultiplication, ElementDivision };
  
  PiiVariant constant() const;
  void setConstant(const PiiVariant& constant);

  Function function() const;
  void setFunction(Function function);
  
  void check(bool reset);

protected:
  void process();

private:
  template <class T> void calculate(const PiiVariant& obj0, const PiiVariant& obj1);
  template <class T> void calculate(const PiiMatrix<T>& obj0,
                                    const PiiMatrix<T>& obj1);
  template <class T, class U> void calculate(const T& obj0, const U& obj1);

  inline void wrongTypes(int type0, int type1);
  template <class T> inline void send(const T& value);
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    
    PiiInputSocket* pInput0, *pInput1;
    PiiOutputSocket* pOutput;
    PiiVariant pConstant;
    bool bInput1Connected;
    Function function;
  };
  PII_D_FUNC;
};

#endif //_PIIARITHMETICOPERATION_H
