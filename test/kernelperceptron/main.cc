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

#include "TestPiiKernelPerceptron.h"

#include <PiiKernelPerceptron.h>
#include <PiiRandom.h>
#include <QtTest>

void TestPiiKernelPerceptron::learn()
{
  static const int iSamplesInSmallSet = 100, iRatio = 5,
    iSampleCount = (iRatio + 1) * iSamplesInSmallSet;

  PiiMatrix<double> matSamples;
  QVector<double> vecLabels;

  PiiKernelPerceptron<PiiMatrix<double> > kernelPerceptron;

  for (int i=0; i<10; ++i)
    {
      PiiClassification::createDartBoard(iSamplesInSmallSet,
                                         iRatio * iSamplesInSmallSet,
                                         matSamples, vecLabels);
      
      kernelPerceptron.learn(matSamples, vecLabels);

      QVERIFY(kernelPerceptron.converged());

      for (int r=0; r<iSampleCount; ++r)
        QCOMPARE(kernelPerceptron.classify(matSamples[r]), vecLabels[r]);
    }
}

QTEST_MAIN(TestPiiKernelPerceptron)
