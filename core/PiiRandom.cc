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

#include "PiiRandom.h"
#include "PiiMath.h"
#include <QDateTime>

namespace Pii
{
  void seedRandom()
  {
    QTime now(QTime::currentTime());
    seedRandom(QDateTime::currentDateTime().toTime_t() ^ now.msec());
  }

  PiiMatrix<double> uniformRandomMatrix(int rows, int columns)
  {
    PiiMatrix<double> result(PiiMatrix<double>::uninitialized(rows, columns));
    for (PiiMatrix<double>::iterator i=result.begin(); i != result.end(); ++i)
      *i = uniformRandom();
    return result;
  }

  PiiMatrix<double> uniformRandomMatrix(int rows, int columns,
                                        double min, double max)
  {
    PiiMatrix<double> result(PiiMatrix<double>::uninitialized(rows, columns));
    for (PiiMatrix<double>::iterator i=result.begin(); i != result.end(); ++i)
      *i = uniformRandom(min,max);
    return result;
  }

  double normalRandom()
  {
    return erf(uniformRandom());
  }

  PiiMatrix<double> normalRandomMatrix(int rows, int columns)
  {
    PiiMatrix<double> result(PiiMatrix<double>::uninitialized(rows, columns));
    for (PiiMatrix<double>::iterator i=result.begin(); i != result.end(); ++i)
      *i = normalRandom();
    return result;
  }
}
