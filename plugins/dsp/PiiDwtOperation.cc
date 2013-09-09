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

#include "PiiDwtOperation.h"

#include <PiiYdinTypes.h>

PiiDwtOperation::Data::Data() :
  waveletFamily(Haar), iFamilyMember(1)
{
}

PiiDwtOperation::PiiDwtOperation() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("approximation"));
  addSocket(new PiiOutputSocket("horizontal"));
  addSocket(new PiiOutputSocket("vertical"));
  addSocket(new PiiOutputSocket("diagonal"));
}

void PiiDwtOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_UNSIGNED_MATRIX_CASES(integerTransform, obj);
      PII_INTEGER_MATRIX_CASES(integerTransform, obj);
      PII_FLOAT_MATRIX_CASES(floatTransform, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiDwtOperation::floatTransform(const PiiVariant& obj)
{
  transform(obj.valueAs<PiiMatrix<T> >());
}

template <class T> void PiiDwtOperation::integerTransform(const PiiVariant& obj)
{
  transform(PiiMatrix<float>(obj.valueAs<PiiMatrix<T> >()));
}

template <class T> void PiiDwtOperation::transform(const PiiMatrix<T>& mat)
{
  PII_D;
  QList<PiiMatrix<T> > lstTransforms(PiiDsp::dwt(mat, (PiiDsp::WaveletFamily)d->waveletFamily, d->iFamilyMember));
  for (int i=0; i<4; ++i)
    emitObject(lstTransforms[i], i);
}

void PiiDwtOperation::setWaveletFamily(const WaveletFamily& waveletFamily) { _d()->waveletFamily = waveletFamily; }
PiiDwtOperation::WaveletFamily PiiDwtOperation::waveletFamily() const { return _d()->waveletFamily; }
void PiiDwtOperation::setFamilyMember(int familyMember) { _d()->iFamilyMember = familyMember; }
int PiiDwtOperation::familyMember() const { return _d()->iFamilyMember; }
