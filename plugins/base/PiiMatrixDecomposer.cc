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

#include "PiiMatrixDecomposer.h"
#include <PiiYdinTypes.h>

PiiMatrixDecomposer::PiiMatrixDecomposer() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  setDynamicOutputCount(1);
}

void PiiMatrixDecomposer::setDynamicOutputCount(int cnt)
{
  if (cnt < 1)
    return;
  setNumberedOutputs(cnt);
}

int PiiMatrixDecomposer::dynamicOutputCount() const { return outputCount(); }

void PiiMatrixDecomposer::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_NUMERIC_MATRIX_CASES(operate, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template<class T>  void PiiMatrixDecomposer::operate(const PiiVariant& obj)
{
  const PiiMatrix<T> matrix = obj.valueAs<PiiMatrix<T> >();
  if (matrix.rows() != 1)
    PII_THROW_WRONG_SIZE(inputAt(0), matrix, 1, tr("N"));
  if (matrix.columns() != outputCount())
    PII_THROW_WRONG_SIZE(inputAt(0), matrix, 1, outputCount());

  for (int i=outputCount(); i--; )
    emitObject(matrix(0,i), i);
}
