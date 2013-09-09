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

#include "PiiMathematicalFunction.h"

#include <PiiYdinTypes.h>

PiiMathematicalFunction::Data::Data() :
  function(NoFunction)
{
}

PiiMathematicalFunction::PiiMathematicalFunction() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
}

void PiiMathematicalFunction::process()
{
  PiiVariant obj = readInput();
  
  switch (_d()->function)
    {
    case NoFunction:
      emitObject(obj);
      break;

    case Abs:
      switch (obj.type())
        {
          PII_NUMERIC_CASES(abs, obj);
          PII_NUMERIC_MATRIX_CASES(absMat, obj);
          PII_COMPLEX_CASES(abs, obj);
          PII_COMPLEX_MATRIX_CASES(absMat, obj);
        }
      return;

    case Log:
      switch (obj.type())
        {
          PII_NUMERIC_CASES(log, obj);
          PII_NUMERIC_MATRIX_CASES(absMat, obj);
        }
      return;

    case Sqrt:
      switch (obj.type())
        {
          PII_NUMERIC_CASES(sqrt, obj);
          PII_NUMERIC_MATRIX_CASES(sqrtMat, obj);
        }
      return;

    case Square:
      switch (obj.type())
        {
          PII_NUMERIC_CASES(square, obj);
          PII_NUMERIC_MATRIX_CASES(squareMat, obj);
          PII_COMPLEX_CASES(square, obj);
          PII_COMPLEX_MATRIX_CASES(squareMat, obj);
        }
      return;

    case Sin:
      switch (obj.type())
        {
          PII_NUMERIC_CASES(sin, obj);
          PII_NUMERIC_MATRIX_CASES(sinMat, obj);
        }
      return;

    case Cos:
      switch (obj.type())
        {
          PII_NUMERIC_CASES(cos, obj);
          PII_NUMERIC_MATRIX_CASES(cosMat, obj);
        }
      return;

    case Tan:
      switch (obj.type())
        {
          PII_NUMERIC_CASES(tan, obj);
          PII_NUMERIC_MATRIX_CASES(tanMat, obj);
        }
      return;

    case Std:
      switch (obj.type())
        {
          PII_NUMERIC_MATRIX_CASES(std, obj);
        }
      return;

    case Var:
      switch (obj.type())
        {
          PII_NUMERIC_MATRIX_CASES(var, obj);
        }
      return;

    case Mean:
      switch (obj.type())
        {
          PII_NUMERIC_MATRIX_CASES(mean, obj);
        }
      return;
    }
  
  PII_THROW_UNKNOWN_TYPE(inputAt(0));
}

void PiiMathematicalFunction::setFunction(Function function) { _d()->function = function; }
PiiMathematicalFunction::Function PiiMathematicalFunction::function() const { return _d()->function; }
