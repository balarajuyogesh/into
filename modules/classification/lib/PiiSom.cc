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

#include <cmath>

#include "PiiSom.h"

namespace PiiClassification
{
  //squared distance between two nodes in a hexagonal topology
  double somHexagonalDistance(int bx, int by, int tx, int ty)
  {
    double ret, diff;
    diff = bx - tx;
    if (((by - ty) & 1) != 0)
      {
        if ((by & 1) == 0)
          diff -= 0.5;
        else
          diff += 0.5;
      }
    ret = diff * diff;
    diff = by - ty;
    ret += 0.75 * diff * diff;
    return(ret);
  }

  //squared distance between two nodes in a rectangular topology
  double somSquareDistance(int bx, int by, int tx, int ty)
  {
    double ret, diff;
    diff = bx - tx;
    ret = diff * diff;
    diff = by - ty;
    ret += diff * diff;
    return(ret);
  }
}
