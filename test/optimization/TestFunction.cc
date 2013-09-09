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


#include "TestFunction.h"


void TestFunction::functionGradient(const double* params, double* gradient) const
{
  //Now test function is x^2 so gradient function is 2x
  gradient[0] = 2.0 * params[0];
}

double TestFunction::functionValue(const double* params) const
{
  //Now test function is x^2 
  double res = params[0]*params[0];
  //qDebug()<<"Function Val res: "<<res<<" params: "<<params(0,0);
  return res;
}
