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

#include "PiiAverageOperation.h"
#include <PiiYdinTypes.h>
#include <PiiMath.h>

PiiAverageOperation::Data::Data() :
  calculationMode(MeanAll)
{
}

PiiAverageOperation::PiiAverageOperation() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("matrix"));
  addSocket(new PiiOutputSocket("average"));
}

void PiiAverageOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_NUMERIC_MATRIX_CASES(average, obj);
      PII_COMPLEX_MATRIX_CASES(complexAverage, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiAverageOperation::average(const PiiVariant& obj)
{
  switch (_d()->calculationMode)
    {
    case MeanRows:
      emitObject(Pii::mean<double>(obj.valueAs<PiiMatrix<T> >(), Pii::Horizontally));
      break;
    case MeanColumns:
      emitObject(Pii::mean<double>(obj.valueAs<PiiMatrix<T> >(), Pii::Vertically));
      break;
    default:
      emitObject(Pii::mean<double>(obj.valueAs<PiiMatrix<T> >()));
      break;
    }
}

template <class T> void PiiAverageOperation::complexAverage(const PiiVariant& obj)
{
  switch (_d()->calculationMode)
    {
    case MeanRows:
      emitObject(Pii::mean<std::complex<double> >(obj.valueAs<PiiMatrix<T> >(), Pii::Horizontally));
      break;
    case MeanColumns:
      emitObject(Pii::mean<std::complex<double> >(obj.valueAs<PiiMatrix<T> >(), Pii::Vertically));
      break;
    default:
      emitObject(Pii::mean<std::complex<double> >(obj.valueAs<PiiMatrix<T> >()));
      break;
    }
}

void PiiAverageOperation::setCalculationMode(const CalculationMode& calculationMode) { _d()->calculationMode = calculationMode; }
PiiAverageOperation::CalculationMode PiiAverageOperation::calculationMode() const { return _d()->calculationMode; }
