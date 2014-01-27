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


#include "TestMarquardtFunction.h"
#include <cmath>

int TestMarquardtFunction::functionCount() const
{
  return 3;
}

void TestMarquardtFunction::residualValues(const double* x,
                                           double* residuals) const
{
  /* y0 = x0^3 + x1^2 - x2 = 8 + 25 - 4 = 29
   * y1 = -x0^2 + x1 + x2/2 = -4 - 5 + 2 = -7
   * y2 = x0^4/4 - x1 - 2x2 = 4 + 5 - 8 = 1
   */

  residuals[0] = 29 - (std::pow(x[0],3) + std::pow(x[1],2) - x[2]);
  residuals[1] = -7 - (-std::pow(x[0],2) + x[1] + x[2]/2);
  residuals[2] = 1 - (std::pow(x[0],4)/4 - x[1] - 2*x[2]);
}

void TestMarquardtFunction::jacobian(const double* x,
                                     PiiMatrix<double>& jacobian) const
{
  /* Jacobian:
   * -dy0/dx0 -dy1/dx0 -dy2/dx0
   * -dy1/dx1 -dy1/dx1 -dy2/dx1
   * -dy0/dx2 -dy1/dx2 -dy2/dx2
   */
  jacobian(0,0) = -3*std::pow(x[0],2);
  jacobian(1,0) = -2*x[1];
  jacobian(2,0) = 1;

  jacobian(0,1) = 2*x[0];
  jacobian(1,1) = -1;
  jacobian(2,1) = -0.5;

  jacobian(0,2) = -std::pow(x[0],3);
  jacobian(1,2) = 1;
  jacobian(2,2) = 2;
}
