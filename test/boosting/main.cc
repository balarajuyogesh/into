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

#include <QtTest>
#include "TestBoosting.h"
#include <PiiBoostClassifier.h>
#include <PiiDecisionStump.h>
#include <PiiDefaultClassifierFactory.h>

void TestBoosting::decisionStump()
{
  // Six samples with 2 features in each
  PiiMatrix<int> features(6, 2,
                          4, 10,
                          2, 11,
                          3, 12,
                          5, 13,
                          0, 14,
                          1, 15);
  // Two different sets of training labels
  QVector<double> labels1;
  labels1 << 1 << 0 << 1 << 1 << 0 << 0;
  QVector<double> labels2;
  labels2 << 1 << 1 << 1 << 0 << 0 << 0;
  // Equal weights
  QVector<double> weights(6, 1.0/6);

  PiiDecisionStump<PiiMatrix<int> > stumps;

  stumps.learn(features, labels1, weights);
  QCOMPARE(stumps.selectedFeature(), 0);
  QCOMPARE(stumps.threshold(), 2);
  QCOMPARE(stumps.leftLabel(), 0.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,2, 2, 5).row(0)), 0.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,2, 1, 7).row(0)), 0.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,2, 5, 1).row(0)), 1.0);

  stumps.learn(features, labels2, weights);
  QCOMPARE(stumps.selectedFeature(), 1);
  QCOMPARE(stumps.threshold(), 12);
  QCOMPARE(stumps.leftLabel(), 1.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,2, 5, 17).row(0)), 0.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,2, 12, 13).row(0)), 0.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,2, 1, 12).row(0)), 1.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,2, -7, -7).row(0)), 1.0);

  // Six samples with one feature in each
  PiiMatrix<int> features2(6,1, 0, 1, 2, 3, 4, 5);
  QVector<double> labels3;
  labels3 << 1 << 1 << 0 << 1 << 0 << 0;
  // Sample 2 is weighted less than the others
  QVector<double> weights2(6, 2.0/11);
  weights2[2] = 1.0/11;

  stumps.learn(features2, labels3, weights2);
  QCOMPARE(stumps.selectedFeature(), 0);
  QCOMPARE(stumps.threshold(), 3);
  QCOMPARE(stumps.leftLabel(), 1.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,1, 4).row(0)), 0.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,1, 3).row(0)), 1.0);
  QCOMPARE(stumps.classify(PiiMatrix<int>(1,1, 2).row(0)), 1.0);
}

void TestBoosting::adaBoost()
{
  QFETCH(int, algorithm);

  PiiMatrix<int> features(8, 2,
                          1, 1,
                          5, 4,
                          -5, 5,
                          -4, 3,
                          3, -3,
                          7, -4,
                          -2, -6,
                          -3, -2);
  QVector<double> labels;
  labels << 0 << 0 << 1 << 1 << 0 << 0 << 1 << 0;
  PiiDefaultClassifierFactory<PiiDecisionStump<PiiMatrix<int> > > factory;
  PiiBoostClassifier<PiiMatrix<int> > classifier(&factory);
  classifier.setAlgorithm(PiiClassification::BoostingAlgorithm(algorithm));
  classifier.setMaxClassifiers(3);
  classifier.learn(features, labels);

  QList<PiiClassifier<PiiMatrix<int> >*> learners = classifier.classifiers();
  QCOMPARE(learners.size(), 3);

  /*for (int i=0; i<learners.size(); i++)
    qDebug("feature %d, threshold %d, inverted %d",
           ((PiiDecisionStump<int>*)learners[i])->selectedFeature(),
           ((PiiDecisionStump<int>*)learners[i])->threshold(),
           (int)((PiiDecisionStump<int>*)learners[i])->isInverted());
  */

  for (int i=0; i<features.rows(); ++i)
    QCOMPARE(classifier.classify(features[i]), labels[i]);
}

void TestBoosting::adaBoost_data()
{
  QTest::addColumn<int>("algorithm");
  QTest::newRow("AdaBoost") << int(PiiClassification::AdaBoost);
  QTest::newRow("RealBoost") << int(PiiClassification::RealBoost);
  QTest::newRow("SammeBoost") << int(PiiClassification::SammeBoost);
  //QTest::newRow("FloatBoost") << int(PiiClassification::FloatBoost);
}

QTEST_MAIN(TestBoosting)
