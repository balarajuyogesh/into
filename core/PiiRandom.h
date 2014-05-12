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

#ifndef _PIIRANDOM_H
#define _PIIRANDOM_H

#include "PiiGlobal.h"
#include <cstdlib>
#include <PiiMatrix.h>
#include <QVector>

namespace Pii
{
  /**
   * @group random Random Number Generation
   *
   * Functions for generating different types of random numbers.
   */

  /**
   * Returns a uniformly distributed random number in [0,1].
   */
  inline double uniformRandom()
  {
#if defined(Q_OS_WIN) || defined(__TI_COMPILER_VERSION__)
    return double(std::rand()) * 1.0/RAND_MAX;
#else
    return drand48();
#endif
  }

  /**
   * Returns a *rows* x *columns* matrix filled with uniformly
   * distributed random numbers in [0,1].
   */
  PII_CORE_EXPORT PiiMatrix<double> uniformRandomMatrix(int rows, int columns);

  /**
   * Returns a uniformly distributed random number in [*min*,
   * *max*].
   */
  inline double uniformRandom(double min, double max)
  {
    return uniformRandom() * (max-min) + min;
  }

  /**
   * Returns a *rows* x *columns* matrix filled with uniformly
   * distributed random numbers in [*min*, *max*].
   */
  PII_CORE_EXPORT PiiMatrix<double> uniformRandomMatrix(int rows, int columns,
                                                        double min, double max);

  /**
   * Returns a random number from a distribution that follows N(0,1)
   * (zero mean, unit variance Gaussian distribution). To convert x in
   * N(0,1) to N(m,v), where m is a non-zero mean and v a non-unit
   * variance, calculate x*v+m.
   */
  PII_CORE_EXPORT double normalRandom();

  /**
   * Returns a *rows* x *columns* matrix filled with normally
   * distributed random numbers.
   */
  PII_CORE_EXPORT PiiMatrix<double> normalRandomMatrix(int rows, int columns);

  /**
   * Initializes the random number generator from system clock. Note
   * that successive inits within the same millisecond have no effect.
   * Note also that this function must be called before any random
   * numbers are generated if you don't want a similar sequence each
   * time the program is run.
   */
  PII_CORE_EXPORT void seedRandom();

  /**
   * Seeds the random number generator with your favourite value.
   */
  inline void seedRandom(long value)
  {
#if defined(Q_OS_WIN) || defined(__TI_COMPILER_VERSION__)
    std::srand(static_cast<unsigned int>(value));
#else
    srand48(value);
#endif
  }
  /**
   * Randomize the order of elements in a collection.
   *
   * ~~~(c++)
   * QList<int> lst;
   * lst << 1 << 2 << 3;
   * Pii::shuffle(lst);
   * ~~~
   */
  template <class Collection> inline void shuffle(Collection& lst)
  {
    shuffle(lst.begin(), lst.end());
  }


  /**
   * Select randomly *n* distinct integers that are smaller than
   * *max*. This function can be used to take a random sample of a
   * collection.
   *
   * ~~~(c++)
   * QStringList lstNames;
   * lstNames << "foo" << "bar" << "etc";
   * QVector<int> indices = Pii::selectRandomly(2, lstNames.size());
   * for (int i=0; i<indices.size(); ++i)
   *   qDebug(qPrintable(lstNames[indices[i]]));
   * ~~~
   *
   * @return randomly selected indices. If *n* is larger than *max*,
   * *max* indices will returned.
   *
   * @see selectRandomly(Collection&, const Collection&, int)
   */
  QVector<int> PII_CORE_EXPORT selectRandomly(int n, int max);

  /**
   *
   * Randomly selects *n* distinct integers in [0, max-1] and stores
   * them to *indices*. This version is better suited for selecting a
   * small number of indices in tight loops.
   */
  void PII_CORE_EXPORT selectRandomly(QVector<int>& indices, int n, int max);

  /**
   * Select randomly *n* elements from *source* and insert them to
   * *target*. The `Collection` type must have size() and append()
   * member functions defined.
   *
   * ~~~(c++)
   * QStringList lstNames;
   * lstNames << "foo" << "bar" << "etc";
   * QStringList selected;
   * Pii::selectRandomly(selected, lstNames, 2);
   * ~~~
   */
  template <class Collection>
  void selectRandomly(Collection& target, const Collection& source, int n)
  {
   QVector<int> indices = selectRandomly(n, source.size());
   for (int i=0; i<indices.size(); ++i)
     target.append(source[indices[i]]);
  }
  /// @endgroup
}

#endif //_PIIRANDOM_H
