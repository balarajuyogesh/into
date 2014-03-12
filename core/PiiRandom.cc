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
#include <algorithm>

namespace Pii
{
  void seedRandom()
  {
    QDateTime now(QDateTime::currentDateTime());
    seedRandom(now.toTime_t() ^ now.time().msec());
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

  void selectRandomly(QVector<int>& indices, int n, int max)
  {
    indices.clear();
    if (n <= 0)
      return;
    else if (n < max/2)
      {
        indices.reserve(n);
        int iRandom = std::rand() % max;
        // The first one cannot already be there
        indices.append(iRandom);
        // The rest can. Generate n-1 distinct indices.
        while (--n)
          {
            QVector<int>::iterator i;
            do
              {
                iRandom = std::rand() % max;
                // Binary search
                i = std::lower_bound(indices.begin(),
                                     indices.end(),
                                     iRandom);
              }
            while (*i == iRandom);
            indices.insert(i, iRandom);
          }
      }
    else
      {
        indices.resize(max);
        Pii::generateN(indices.begin(), max, Pii::CountFunction<int>());
        if (n < max)
          {
            shuffle(indices);
            indices.resize(n);
          }
      }
  }

  QVector<int> selectRandomly(int n, int max)
  {
    QVector<int> vecResult;
    selectRandomly(vecResult, n, max);
    return vecResult;
  }
}
