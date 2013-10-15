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

#ifndef _PIICHISQUAREDDISTANCE_H
#define _PIICHISQUAREDDISTANCE_H

#include "PiiDistanceMeasure.h"

/**
 * Chi squared distance. The chi squared distance between two vectors
 * S and M is defined as \( d = \frac{1}{2} \sum (S_i - M_i)^2 / (S_i
 * + M_i) \). This distance measure doesn't however divide the sum by
 * two.
 *
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiChiSquaredDistance)
{
  double sum = 0.0, tmp;
  for (int i=0; i<length; ++i)
    {
      tmp = double(sample[i] - model[i]);
      sum += tmp*tmp / (sample[i] + model[i]);
    }
  return sum;
}

#endif //_PIICHISQUAREDDISTANCE_H
