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

#ifndef _PIIHISTOGRAMINTERSECTION_H
#define _PIIHISTOGRAMINTERSECTION_H

#include "PiiDistanceMeasure.h"

/**
 * Histogram intersection. Measures difference between two
 * distributions in a straightforward way: \( d = - \sum
 * \min(S_i,M_i) \) , where *S* and *M* represent the sample and
 * model distributions, respectively. Histogram intersection assumes
 * that the input samples are distributions. In classification, the
 * sum of the values in each should be the same, preferably one.
 *
 * The histogram intersection is optimized so that it produces a
 * negative distance. The minimum possible value (*S* and *M* are
 * equal) is the negation of the sum of input features. The maximum is
 * zero.
 *
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiHistogramIntersection)
{
  double diffSum = 0.0;
  for (int i=0; i<length; ++i)
    diffSum += double(qMin(sample[i], model[i]));

  return -diffSum;
}

#endif //_PIIHISTOGRAMINTERSECTION_H
