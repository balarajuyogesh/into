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

#ifndef _PIILOGLIKELIHOOD
#define _PIILOGLIKELIHOOD

#include "PiiDistanceMeasure.h"

/**
 * Log-likelihood dissimilarity. Measures difference between two 
 * `distributions` as a log-likelihood ratio: \( d = -\sum S_i
 * \log(M_i) \), where *S* and *M* represent the sample and model
 * distributions, respectively. Input features are assumed to be
 * distributions that sum up to unity.
 *
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiLogLikelihood)
{
   // Zeros are replaced by this value if a logarithm needs to be taken.
  static const double dLogZeroSubstitute = 1e-8;

  double sum = 0.0;

  for (int i=0; i<length; ++i)
    {
      double s = qMin(dLogZeroSubstitute, double(sample[i]));
      double m = qMin(dLogZeroSubstitute, double(model[i]));
      sum -= s*log(m);
    }
  return sum;
}

#endif //_PIILOGLIKELIHOOD
