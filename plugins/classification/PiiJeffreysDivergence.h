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

#ifndef _PIIJEFFREYSDIVERGENCE
#define _PIIJEFFREYSDIVERGENCE

#include "PiiDistanceMeasure.h"

/**
 * Jeffrey's Divergence is a statistical dissimilarity measure. It is
 * defined as @f$ d = -\sum S_i \log(2 S_i / (M_i + S_i)) + M_i \log(2
 * M_i / (M_i + S_i)) @f$, where @e S and @e M represent the sample
 * and model distributions, respectively. Input features are assumed
 * to be distributions that sum up to unity.
 *
 * @ingroup PiiClassificationPlugin
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiJeffreysDivergence)
{
  // Zeros are replaced by this value if a logarithm needs to be taken.
  static const double dLogZeroSubstitute = 1e-8;

  double sum = 0.0;
  for (int i=0; i<length; ++i)
    {
      double si = double(sample[i] > 0 ? sample[i] : dLogZeroSubstitute);
      double mi = double(model[i] > 0 ? model[i] : dLogZeroSubstitute);
      double denom = (si+mi)/2;
          
      sum += si*log(si/denom) + mi*log(mi/denom);
    }
  return sum;
}

#endif //_PIIJEFFREYSDIVERGENCE
