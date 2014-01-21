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

#include "PiiClassification.h"
#include <PiiMath.h>
#include <PiiAlgorithm.h>
#include <PiiRandom.h>

namespace PiiClassification
{
  double calculateError(const QVector<double>& knownLabels, const QVector<double>& hypothesis, const QVector<double>& weights)
  {
    if (weights.size() == 0)
      {
        int iCnt = Pii::min(knownLabels.size(), hypothesis.size());
        int iValidLabels = 0;
        int iErrors = 0;
        for (int i=0; i<iCnt; ++i)
          {
            if (!Pii::isNan(hypothesis[i]))
              {
                if (knownLabels[i] != hypothesis[i])
                  ++iErrors;
                ++iValidLabels;
              }
          }
        return double(iErrors) / qMax(iValidLabels, 1);
      }
    else
      {
        int iCnt = qMin(qMin(knownLabels.size(), hypothesis.size()), weights.size());
        double dError = 0, dWeightSum = 0;
        for (int i=0; i<iCnt; ++i)
          {
            if (!Pii::isNan(hypothesis[i]))
              {
                if (knownLabels[i] != hypothesis[i])
                  dError += weights[i];
                dWeightSum += weights[i];
              }
          }
        return dWeightSum != 0 ? dError / dWeightSum : dError;
      }
  }

  PiiMatrix<int> createConfusionMatrix(const QVector<double>& knownLabels, const QVector<double>& hypothesis)
  {
    if (knownLabels.size() != hypothesis.size())
      return PiiMatrix<int>();

    int maxLabel = int(qMax(Pii::max(knownLabels), Pii::max(hypothesis)));
    int iCols = maxLabel+1;
    if (Pii::min(hypothesis) < 0) //need to add discard class
      ++iCols;
    PiiMatrix<int> result(maxLabel+1, iCols);
    for (int c=knownLabels.size(); c--; )
      {
        if (knownLabels[c] >= 0)
          ++result(int(knownLabels[c]), int(hypothesis[c]) >= 0 ? int(hypothesis[c]) : iCols-1);
      }
    return result;
  }

  QList<QPair<double,int> > countLabels(const QVector<double>& labels)
  {
    QList<QPair<double,int> > lstResult;
    if (labels.size() == 0)
      return lstResult;
    QVector<double> vecSorted(labels);
    qSort(vecSorted);
    double dCurrentLabel = vecSorted[0];
    int iCurrentCount = 1;
    for (int i=1; i<vecSorted.size(); ++i)
      {
        double dNextLabel = vecSorted[i];
        if (dNextLabel == dCurrentLabel)
          ++iCurrentCount;
        else
          {
            lstResult << qMakePair(dCurrentLabel, iCurrentCount);
            dCurrentLabel = dNextLabel;
            iCurrentCount = 1;
          }
      }
    lstResult << qMakePair(dCurrentLabel, iCurrentCount);
    return lstResult;
  }

  QVector<int> countLabelsInt(const QVector<double>& labels)
  {
    int iMaxLabel = int(Pii::maxIn(labels.begin(), labels.end()));
    QVector<int> vecResult(iMaxLabel+1, 0);
    for (int i=0; i<labels.size(); ++i)
      ++vecResult[qMax(0, int(labels[i]))];
    return vecResult;
  }

  void createDoubleSpiral(int samplesPerSet,
                          double rounds,
                          PiiMatrix<double>& samples,
                          QVector<double>& labels)
  {
    PiiMatrix<double> matAlpha(Pii::uniformRandomMatrix(samplesPerSet, 1) * rounds * 2 * M_PI);
    PiiMatrix<double> matD0(matAlpha + Pii::uniformRandomMatrix(samplesPerSet, 1)/5);
    PiiMatrix<double> matD1(matAlpha + M_PI_2 + Pii::uniformRandomMatrix(samplesPerSet, 1)/5);

    samples.resize(2*samplesPerSet, 2);
    samples(0,0,samplesPerSet,1) << Pii::multiplied(matD0, Pii::cos(matAlpha));
    samples(0,1,samplesPerSet,1) << Pii::multiplied(matD0, Pii::sin(matAlpha));
    matAlpha -= M_PI_2;
    samples(samplesPerSet,0,-1,1) << Pii::multiplied(matD1, Pii::cos(matAlpha));
    samples(samplesPerSet,1,-1,1) << Pii::multiplied(matD1, Pii::sin(matAlpha));

    labels.fill(1.0, 2*samplesPerSet);
    Pii::fillN(labels.begin(), samplesPerSet, 0.0);
  }

  void createDartBoard(int samples1, int samples2,
                       PiiMatrix<double>& samples,
                       QVector<double>& labels)
  {
    samples.resize(samples1 + samples2, 2);
    // Generate two linearly non-separable, non-linearly separable classes
    // 1: normally distributed around (0,0)
    samples(0,0,samples1,2) << Pii::normalRandomMatrix(samples1, 2);

    // 2: normally distributed on a circle around the first set (r = 7)
    PiiMatrix<double> matD(Pii::normalRandomMatrix(samples2, 1) / 2 + 7),
      matAlpha(Pii::uniformRandomMatrix(samples2, 1) * M_PI * 2);
    samples(samples1,0,-1,1) << Pii::multiplied(matD, Pii::cos(matAlpha));
    samples(samples1,1,-1,1) << Pii::multiplied(matD, Pii::sin(matAlpha));

    labels.fill(1.0, samples1 + samples2);
    Pii::fillN(labels.begin(), samples1, 0.0);
  }
}
