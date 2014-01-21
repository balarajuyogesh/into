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

#ifndef _PIIFFTOPERATION_H
# error "Never use <PiiFftOperation-templates.h> directly; include <PiiFftOperation.h> instead."
#endif

#include <PiiYdinTypes.h>
#include <PiiMath.h>
#include "PiiDsp.h"

using namespace PiiYdin;

template <class T> class PiiFftOperation::Template : public PiiFftOperation
{
  PII_OPERATION_SERIALIZATION_FUNCTION;

public:
  Template() : PiiFftOperation(new Data) {}

  void check(bool reset)
  {
    PiiFftOperation::check(reset);

    _d()->bPowerSpectrumConnected = outputAt(1)->isConnected();
  }

protected:
  void process()
  {
    PiiVariant obj = readInput();

    if (_d()->direction == Forward)
      {
        switch (obj.type())
          {
            PII_NUMERIC_MATRIX_CASES(operateForward, obj);
          case DoubleComplexMatrixType:
            operateForward<std::complex<double> >(obj);
            break;
          case FloatComplexMatrixType:
            operateForward<std::complex<float> >(obj);
            break;
          default:
            PII_THROW_UNKNOWN_TYPE(inputAt(0));
          }
      }
    else
      {
        if (obj.type() == DoubleComplexMatrixType)
          operateInverse<double>(obj);
        else if (obj.type() == FloatComplexMatrixType)
          operateInverse<float>(obj);
        else
          PII_THROW_UNKNOWN_TYPE(inputAt(0));
      }
  }

private:
  template <class S> void operateForward(const PiiVariant& obj);
  template <class S> void operateInverse(const PiiVariant& obj);

  class Data : public PiiFftOperation::Data
  {
  public:
    Data() :  bPowerSpectrumConnected(false) {}

    PiiFft<T> fft;
    bool bPowerSpectrumConnected;
  };
  PII_D_FUNC;
};

template <class T>
template <class S> void PiiFftOperation::Template<T>::operateForward(const PiiVariant& obj)
{
  PII_D;
  typedef PiiMatrix<std::complex<T> > ResultType;
  typedef typename Pii::ToFloatingPoint<S>::Type FloatType;

  const PiiMatrix<S>& image = obj.valueAs<PiiMatrix<S> >();

  ResultType result = d->bSubtractMean ?
    d->fft.forwardFft(image.mapped(std::minus<FloatType>(), Pii::mean<FloatType>(image))) :
    d->fft.forwardFft(image);

  if (d->bShift)
    result = PiiDsp::fftShift(result);

  emitObject(result);

  if (d->bPowerSpectrumConnected)
    emitObject(result.mapped(Pii::AbsPow<std::complex<T> >(), 2), 1);
}

template <class T>
template <class S> void PiiFftOperation::Template<T>::operateInverse(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<std::complex<S> > image = obj.valueAs<PiiMatrix<std::complex<S> > >();
  emitObject(d->fft.inverseFft(d->bShift ? PiiDsp::fftShift(image, true) : image));
}
