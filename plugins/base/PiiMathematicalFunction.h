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

#ifndef _PIIMATHEMATICALFUNCTION_H
#define _PIIMATHEMATICALFUNCTION_H

#include <PiiDefaultOperation.h>
#include <PiiMath.h>

/**
 * An operation that applies a mathematical function to its input.
 *
 * Inputs
 * ------
 *
 * @in input - any scalar or matrix
 * 
 * Outputs
 * -------
 *
 * @out output - the result of applying a mathematical function to the
 * input. The output type depends both on the input and the function. 
 * For example, the absolute value for a complex number is real. The
 * input type will be preserved whenever possible.
 *
 */
class PiiMathematicalFunction : public PiiDefaultOperation
{
  Q_OBJECT
  /**
   * The function to apply. The default value is `NoFunction`.
   */
  Q_PROPERTY(Function function READ function WRITE setFunction);
  Q_ENUMS(Function);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Supported mathematical functions.
   *
   * - `NoFunction` - just pass the input object
   *
   * Functions that will be applied element-wise to matrices and
   * scalars:
   *
   * - `Abs` - absolute value. Output type equals input type for all
   * but complex numbers.
   *
   * - `Log` - natural logarithm. Output type is `double`. Complex
   * numbers cause run-time exception.
   *
   * - `Sqrt` - square root. Output type is `double`. Complex
   * numbers cause run-time exception.
   *
   * - `Square` - square. Output type equals input type. Beware of
   * overflows!
   *
   * - `Sin` - sine of angle. Output type is `double` except for 
   * `float` input, for which it is `float`. Complex numbers cause
   * run-time exception.
   *
   * - `Cos` - cosine of angle. Output type is `double` except for
   * `float` input, for which it is `float`. Complex numbers cause
   * run-time exception.
   *
   * - `Tan` - tangent of angle. Output type is `double` except for
   * `float` input, for which it is `float`. Complex numbers cause
   * run-time exception.
   *
   * Functions that calculate a value over all elements in a matrix. 
   * In all these cases the output type is `double`. Scalars and
   * complex-valued matrices as input cause run-time exception:
   *
   * - `Var` - variance
   * - `Std` - standard deviation
   * - `Mean` - mean
   */
  enum Function
  {
    NoFunction,
    Abs, Log, Sqrt, Square, Sin, Cos, Tan,
    Var, Std, Mean
  };
  
  PiiMathematicalFunction();

  void setFunction(Function function);
  Function function() const;

protected:
  void process();

private:
#define EMIT(obj) emitObject(obj)
#define FUNC_DEF(func) \
  template <class T> void func(const PiiVariant& obj) { EMIT(Pii::func(obj.valueAs<T>())); } \
  template <class T> void func##Mat(const PiiVariant& obj) { EMIT(Pii::func(obj.valueAs<PiiMatrix<T> >())); }

  FUNC_DEF(abs)
  FUNC_DEF(log)
  FUNC_DEF(sqrt)
  FUNC_DEF(square)
  FUNC_DEF(sin)
  FUNC_DEF(cos)
  FUNC_DEF(tan)

  template <class T> void std(const PiiVariant& obj) { EMIT(Pii::std<double>(obj.valueAs<PiiMatrix<T> >())); }
  template <class T> void var(const PiiVariant& obj) { EMIT(Pii::var<double>(obj.valueAs<PiiMatrix<T> >())); }
  template <class T> void mean(const PiiVariant& obj) { EMIT(Pii::mean<double>(obj.valueAs<PiiMatrix<T> >())); }

#undef EMIT
#undef FUNC_DEF
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    Function function;
  };
  PII_D_FUNC;
};


#endif //_PIIMATHEMATICALFUNCTION_H
