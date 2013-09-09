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

#ifndef _PIIGEOMETRICDISTANCE_H
#define _PIIGEOMETRICDISTANCE_H

#include "PiiDistanceMeasure.h"

/**
 * Geometric distance. The geometric distance is calculated as the
 * square root of the sum of squared differences of the vectors: @f$ d
 * = \sqrt{(S-M)(S-M)^T} @f$, where @e S and @e M represent the sample
 * and model feature vectors, respectively.
 *
 * @ingroup PiiClassificationPlugin
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiGeometricDistance)
{
  double sum = 0.0, tmp;
  for (int i=0; i<length; ++i)
    {
      tmp = double(sample[i] - model[i]);
      sum += tmp*tmp;
    }
  return sqrt(sum);
}

#endif //_PIIGEOMETRICDISTANCE_H
