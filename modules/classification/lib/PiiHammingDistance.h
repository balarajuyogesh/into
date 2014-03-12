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

#ifndef _PIIHAMMINGDISTANCE_H
#define _PIIHAMMINGDISTANCE_H

#include "PiiDistanceMeasure.h"
#include <PiiBits.h>

/**
 * Calculates the Hamming distance between two integer-valued feature
 * vectors. The Hamming distance is the number of different bits in
 * two numbers.
 *
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiHammingDistance)
{
  double distance = 0;
  for (int i=0; i<length; ++i)
    distance += Pii::hammingDistance(sample[i], model[i],
                                     sizeof(typename std::iterator_traits<FeatureIterator>::value_type));
  return distance;
}

#endif //_PIIHAMMINGDISTANCE_H
