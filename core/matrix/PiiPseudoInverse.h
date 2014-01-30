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

#ifndef _PIIPSEUDOINVERSE_H
#define _PIIPSEUDOINVERSE_H

#include "PiiSvDecomposition.h"

namespace Pii
{
  /**
   * Returns the Moore-Penrose pseudoinverse of an m-by-n matrix.
   * Pseudoinverse is the optimal solution (in least squares sense)
   * for an over-determined system of linear equations (i.e. m > n).
   * The Moore-Penrose pseudoinverse of a matrix A has the following
   * properties:
   *
   * - \(AA^+A = A\)
   * - \(A^+AA^+ = A^+\)
   * - \((AA^+)^H = AA^+\) (H denotes a Hermitian transpose)
   * - \((A^+A)^H = A^+A\)
   *
   * This function uses [SVD](svDecompose()) to calculate the
   * pseudoinverse in a numerically stable way.
   */
  template <class Matrix>
  PiiMatrix<typename Matrix::value_type> pseudoInverse(const Matrix& A)
  {
    typedef typename Matrix::value_type T;
    // A = USV* -> A+ = VS+U*
    // A is m by n
    // U is m by m
    // S is m by n -> S+ is n by m
    // V is n by n
    // VS+ is n by m

    PiiMatrix<T> U, V;
    PiiMatrix<T> S = svDecompose(A, &U, &V);
    const T* pSingularValues = S[0];
    T zeroTolerance = epsilon<T>() * qMax(A.rows(), A.columns()) * max(S);
    int iNonZeroSVs = 0;

    // The pseudoinverse of a diagonal rectangular matrix is
    // calculated by taking the reciprocals of non-zero diagonal
    // entries and transposing the matrix. First, count non-zero
    // singular values.
    int iColumns = S.columns();
    for (; iNonZeroSVs<iColumns && pSingularValues[iNonZeroSVs] > zeroTolerance; ++iNonZeroSVs) ;

    // The rightmost part will be zeroed out in multiplication anyway.
    V.resize(V.rows(), iColumns);
    // Calculate V*S+
    for (int r=0; r<V.rows(); ++r)
      {
        T* pRow = V[r];
        int c=0;
        for (; c<iNonZeroSVs; ++c)
          pRow[c] /= pSingularValues[c];
        for (; c<iColumns; ++c)
          pRow[c] = T(0);
      }
    // The rightmost part will be zeroed out in multiplication anyway.
    U.resize(U.rows(), iColumns);
    return V * adjoint(U);
  }
}

#endif //_PIIPSEUDOINVERSE_H
