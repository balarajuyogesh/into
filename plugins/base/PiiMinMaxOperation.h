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

#ifndef _PIIMINMAXOPERATION_H
#define _PIIMINMAXOPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>

/**
 * Calculates the minimum/maximum of two arguments, or the
 * minimum/maximum value in a matrix.
 *
 * Inputs
 * ------
 *
 * @in input0 - first input. Any matrix or scalar.
 *
 * @in input1 - Optional second input. Any matrix or scalar. If the
 * object in `input0` is a scalar, `input1` must be a scalar. If
 * `input0` is a matrix, `input1` can be either a scalar or a matrix.
 * In the latter case, the sizes of the matrices must match.
 *
 * Outputs
 * -------
 *
 * @out output - If only `input0` is connected, and the object read
 * from it is a matrix, outputs the minimum/maximum value of the
 * matrix as a scalar. If [minMaxCount] is larger than 1, outputs the N
 * smallest/largest values as a 1-by-N matrix. If both inputs are
 * connected and a) the objects are matrices, outputs a matrix that
 * stores the minimum/maximum of corresponding elements b) the objects
 * are scalars, outputs the minimum/maximum of them c) the object in
 * `input0` is a matrix and the object in `input1` a scalar, outputs
 * the minimum/maximum of the matrix and the scalar.
 *
 * @out coordinates - the coordinates of the minimum/maximum value in
 * a matrix. This output is active if and only if `input1` is not
 * connected. Emits a N-by-2 PiiMatrix<int> that stores the column and
 * row coordinates (x and y) of the located minima/maxima, in this
 * order.
 *
 */
class PiiMinMaxOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Operation mode. Default is `MaximumMode`.
   */
  Q_PROPERTY(Mode mode READ mode WRITE setMode);
  Q_ENUMS(Mode);

  /**
   * The number of smallest/largest values to find in a matrix. If the
   * number of elements in a matrix is smaller than this value, the
   * operation will stop running. Default is one.
   */
  Q_PROPERTY(int minMaxCount READ minMaxCount WRITE setMinMaxCount);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Operation modes.
   *
   * - `MinimumMode` - find minimum/mimina
   *
   * - `MaximumMode` - find maximum/maxima
   */
  enum Mode { MinimumMode, MaximumMode };

  PiiMinMaxOperation();

  void setMode(Mode mode);
  Mode mode() const;

  void setMinMaxCount(int minMaxCount);
  int minMaxCount() const;

protected:
  void process();

private:
  template <class T> void findMinMaxMatrix(const PiiVariant& obj);
  template <class T, class Comparator> PiiMatrix<T> findExtrema(const PiiMatrix<T>& mat,
                                                                PiiMatrix<int>& coords,
                                                                Comparator cmp,
                                                                T initialValue);
  template <class T> void findMinMaxScalar(const PiiVariant& obj);
  template <class T, class U> void findMinMaxMatrixMatrix(const PiiMatrix<U>&, const PiiVariant&);
  template <class T, class U> void findMinMaxMatrixScalar(const PiiMatrix<U>&, const PiiVariant&);
  template <class T, class U> void findMinMaxScalarScalar(U, const PiiVariant&);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    Mode mode;
    int iMinMaxCount;
  };
  PII_D_FUNC;
};


#endif //_PIIMINMAXOPERATION_H
