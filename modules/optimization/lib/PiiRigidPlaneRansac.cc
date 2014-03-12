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

#include "PiiRigidPlaneRansac.h"
#include <PiiMath.h>

namespace PiiOptimization
{
  PiiMatrix<float> rigidPlaneModelToTransform(const PiiMatrix<double>& model)
  {
    double dCos = model(0) * Pii::cos(model(1)), dSin = model(0) * Pii::sin(model(1));
    return PiiMatrix<float>(3,3,
                            dCos, -dSin, model(2),
                            dSin,  dCos, model(3),
                            0.0,   0.0,  1.0);
  }
}

