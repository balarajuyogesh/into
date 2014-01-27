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
#define _PIIFFTOPERATION_H

#include <PiiDefaultOperation.h>
#include "PiiFft.h"

using namespace PiiYdin;

/**
 * An operation that calculates the fast fourier transform for any
 * matrix.
 *
 * Inputs
 * ------
 *
 * @in input - the input matrix. If [direction] == `Inverse`, the type
 * of the input matrix must be a PiiMatrix<complex<float> > or a
 * PiiMatrix<complex<double> >. Otherwise, any numeric or complex
 * matrix will do.
 *
 * Outputs
 * -------
 *
 * @out output - the (forward or inverse) Fourier transform of the
 * input matrix (PiiMatrix<complex<float> > or
 * PiiMatrix<complex<double> >).
 *
 * @out power spectrum - real-valued power spectrum of the
 * transformed matrix. Used only in `Forward` transformation.
 *
 */
class PiiFftOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The direction of the transform. The default is `Forward`.
   */
  Q_PROPERTY(FftDirection direction READ direction WRITE setDirection);
  Q_ENUMS(FftDirection);

  /**
   * If this flag is set to `true`, the operation shifts the
   * zero-frequency component to the center of the spectrum. For
   * vectors, FftShift swaps the left and right halves. For matrices,
   * it swaps the first and fourth quadrants and the second and third
   * quadrants. FftShift is useful for visualizing the Fourier
   * transform with the zero- frequency component in the middle of the
   * spectrum. The default value is `false`.
   */
  Q_PROPERTY(bool shift READ shift WRITE setShift);

  /**
   * If this flag is `true`, the mean value of the input is
   * subtracted before applying the FFT. This makes the zero-frequency
   * component zero and helps in visualizing the result. The default
   * is `false`. This flag has no effect in inverse transform.
   */
  Q_PROPERTY(bool subtractMean READ subtractMean WRITE setSubtractMean);

public:

  PiiFftOperation();

  /**
   * FFT direction.
   *
   * - `Forward` - calculates FFT.
   *
   * - `Inverse` - calculates inverse FFT.
   */
  enum FftDirection { Forward, Inverse };

  FftDirection direction() const;
  void setDirection(FftDirection direction);

  bool shift() const;
  void setShift(bool shift);
  void setSubtractMean(bool subtractMean);
  bool subtractMean() const;

  template <class T> class Template;

protected:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    FftDirection direction;
    bool bShift;
    bool bSubtractMean;
  };
  PII_D_FUNC;

  PiiFftOperation(Data* data);

private:
  void init();
};

#include "PiiFftOperation-templates.h"


#endif //_PIIFFTOPERATION_H
