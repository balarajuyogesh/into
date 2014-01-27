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

#include "TestPiiPerceptron.h"

#include <PiiPerceptron.h>
#include <PiiRandom.h>
#include <QtTest>

/*#include <PiiMatrixUtil.h>
 #include <iostream>
 using std::cout;
*/

void TestPiiPerceptron::learn()
{
  static const int iSamplesPerSet = 100;

  PiiMatrix<double> matSamples(2*iSamplesPerSet,2);
  QVector<double> vecLabels(2*iSamplesPerSet, 1);
  Pii::fillN(vecLabels.begin(), iSamplesPerSet, 0);

  PiiPerceptron<PiiMatrix<double> > perceptron;

  QVERIFY(Pii::isNan(perceptron.classify(matSamples[0])));

  for (int i=0; i<10; ++i)
    {
      // Generate two linearly separable classes
      // 1: normally distributed around (3,3)
      matSamples(0,0,iSamplesPerSet,2) << Pii::normalRandomMatrix(iSamplesPerSet, 2) + 3;
      // 2: normally distributed around (-3,-3)
      matSamples(iSamplesPerSet,0,-1,2) << Pii::normalRandomMatrix(iSamplesPerSet, 2) - 3;

      perceptron.learn(matSamples, vecLabels);

      QVERIFY(perceptron.converged());

      //qDebug() << perceptron.weights();
      //qDebug() << perceptron.bias();

      for (int r=0; r<2*iSamplesPerSet; ++r)
        QCOMPARE(perceptron.classify(matSamples[r]), double(r/iSamplesPerSet));
    }
}

QTEST_MAIN(TestPiiPerceptron)
