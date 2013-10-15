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

#ifndef _PIISQUAREDGEOMETRICDISTANCE_H
#define _PIISQUAREDGEOMETRICDISTANCE_H

#include "PiiDistanceMeasure.h"

/**
 * Squared geometric distance. The squared geometric distance is
 * calculated as the the sum of squared differences of the vectors:
 * \( d = (S-M)(S-M)^T \), where S and M represent the sample and
 * model feature vectors, respectively. It is equivalent to the
 * geometric distance in most classification tasks, because square
 * root is a monotonic function. Squared distance however works much
 * faster because no square root needs to be taken.
 *
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiSquaredGeometricDistance)
{
  double sum = 0.0, tmp;
  for (int i=0; i<length; ++i)
    {
      tmp = double(sample[i] - model[i]);
      sum += tmp*tmp;
    }
  return sum;
}

#endif //_PIISQUAREDGEOMETRICDISTANCE_H
