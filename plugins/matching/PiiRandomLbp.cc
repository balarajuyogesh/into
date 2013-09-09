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

#include "PiiRandomLbp.h"

#include <cstdlib>

PiiRandomLbp::Data::Data() :
  iPatterns(50),
  iPairs(11)
{}

PiiMatrix<int> PiiRandomLbp::initializeHistogram() const
{
  PiiMatrix<int> matResult(1, d->iPatterns * (1 << d->iPairs));
  matResult = 1;
  return matResult;
}

void PiiRandomLbp::setParameters(int patterns, int pairs, int rows, int columns)
{
  d->iPatterns = patterns;
  d->iPairs = pairs;

  d->vecPointPairs.clear();
  d->vecPointPairs.reserve(patterns*pairs);

  for (int i=0; i<patterns*pairs; ++i)
    d->vecPointPairs << qMakePair(PiiPoint<int>(rand() % rows, rand() % columns),
                                  PiiPoint<int>(rand() % rows, rand() % columns));
  /* PENDING
   * The pairs could be reordered to optimize cache usage.
   */
}
