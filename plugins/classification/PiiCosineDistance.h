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

#ifndef _PIICOSINEDISTANCE_H
#define _PIICOSINEDISTANCE_H

#include "PiiDistanceMeasure.h"

/**
 * Cosine distance calculates the cosine of the angle between feature
 * vectors.
 *
 * @ingroup PiiClassificationPlugin
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiCosineDistance)
{
  double sum = 0.0, len1 = 0.0, len2 = 0.0, tmp1, tmp2;
  for (int i=0; i<length; ++i)
    {
      tmp1 = double(sample[i]);
      tmp2 = double(model[i]);
      sum += tmp1*tmp2;
      len1 += tmp1*tmp1;
      len2 += tmp2*tmp2;
    }
  len1 *= len2;
  if (len1 != 0)
    return -sum/::sqrt(len1);
  return 0;
}

#endif //_PIICOSINEDISTANCE_H
