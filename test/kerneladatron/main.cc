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

#include "TestPiiKernelAdatron.h"

#include <PiiKernelAdatron.h>
#include <PiiClassification.h>
#include <QtTest>

void TestPiiKernelAdatron::learn()
{
  static const int iSamplesPerSet = 500;

  PiiMatrix<double> matSamples;
  QVector<double> vecLabels;

  PiiKernelAdatron<PiiMatrix<double> > kernelAdatron;

  for (int i=0; i<4; ++i)
    {
      PiiClassification::createDoubleSpiral(iSamplesPerSet,
                                            3,
                                            matSamples,
                                            vecLabels);

      kernelAdatron.setLearningRate(i*0.25);
      kernelAdatron.learn(matSamples, vecLabels);

      QVERIFY(kernelAdatron.converged());

      for (int r=0; r<iSamplesPerSet*2; ++r)
        QCOMPARE(kernelAdatron.classify(matSamples[r]), vecLabels[r]);
    }
}

QTEST_MAIN(TestPiiKernelAdatron)
