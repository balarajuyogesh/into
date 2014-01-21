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


#ifndef _TESTMARQUARDTFUNCTION_H
#define _TESTMARQUARDTFUNCTION_H

#include <PiiMatrix.h>
#include <PiiOptimization.h>
#include <QDebug>

/**
 * Params: x0, x1, x2
 *
 * Optimized functions:
 * y0 = x0^3 + x1^2 - x2 = 8 + 25 - 4 = 29
 * y1 = -x0^2 + x1 + x2/2 = -4 - 5 + 2 = -7
 * y2 = x0^4/4 - x1 - 2x2 = 4 + 5 - 8 = 1
 *
 * Known result:
 * x0 = 2, x1= -5, x2 = 4
 */
class TestMarquardtFunction : public PiiOptimization::ResidualFunction<double>
{
public:
  TestMarquardtFunction() : _bHasJacobian(false) {}
  ~TestMarquardtFunction() {}

  int functionCount() const;
  void residualValues(const double* params,
                      double* residuals) const;
  void jacobian(const double* params,
                PiiMatrix<double>& jacobian) const;

  bool hasJacobian() const { return _bHasJacobian; }
  void setHasJacobian(bool jacobian) { _bHasJacobian = jacobian; }

private:
  bool _bHasJacobian;
};

#endif
