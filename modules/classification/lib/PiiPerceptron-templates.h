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

#ifndef _PIIPERCEPTRON_H
# error "Never use <PiiPerceptron-templates.h> directly; include <PiiPerceptron.h> instead."
#endif

#include <PiiMath.h>
#include <PiiAlgorithm.h>
#include <cstring>

template <class SampleSet> PiiPerceptron<SampleSet>::Data::Data() :
  bConverged(false),
  dBias(0),
  iMaxIterations(100)
{}

template <class SampleSet> PiiPerceptron<SampleSet>::PiiPerceptron() :
  PiiLearningAlgorithm<SampleSet>(new Data)
{}

template <class SampleSet> PiiPerceptron<SampleSet>::~PiiPerceptron()
{}

template <class SampleSet> void PiiPerceptron<SampleSet>::learn(const SampleSet& samples,
                                                                const QVector<double>& labels,
                                                                const QVector<double>& /*weights*/)
{
  PII_D;
  const int iSamples = PiiSampleSet::sampleCount(samples),
    iFeatures = PiiSampleSet::featureCount(samples);
  d->vecWeights.fill(0, iFeatures);
  d->dBias = 0;
  d->bConverged = false;
  int iErrorCount, iIterations = 0;
  do
    {
      // Count incorrect classifications on each round.
      iErrorCount = 0;
      for (int i=0; i<iSamples; ++i)
        {
          if (adaptTo(PiiSampleSet::sampleAt(samples,i), labels[i]) != labels[i])
            ++iErrorCount;
          // Check if the user interrupted. NAN means we don't know
          // the amount of work remaining.
          PII_TRY_CONTINUE(d->pController, NAN);
        }
    }
  // Stop if everything was correctly classified or when we reach the
  // maximum number of iterations.
  while (iErrorCount > 0 &&
         ++iIterations < d->iMaxIterations);

  d->bConverged = (iErrorCount == 0);
}

template <class SampleSet> int PiiPerceptron<SampleSet>::featureCount() const { return _d()->vecWeights.size(); }

template <class SampleSet>
double PiiPerceptron<SampleSet>::adaptTo(ConstFeatureIterator featureVector,
                                         double label)
{
  PII_D;
  double dPrediction = classify(featureVector);
  if (!Pii::isNan(dPrediction) && dPrediction != label)
    {
      double dMu = label - dPrediction;
      const int iFeatures = d->vecWeights.size();
      for (int i=0; i<iFeatures; ++i)
        d->vecWeights[i] += dMu * featureVector[i];
      d->dBias += dMu;
    }
  return dPrediction;
}

template <class SampleSet>
double PiiPerceptron<SampleSet>::classify(ConstFeatureIterator featureVector) throw()
{
  PII_D;
  if (!d->vecWeights.isEmpty())
    return Pii::innerProductN(d->vecWeights.constData(),
                              d->vecWeights.size(),
                              featureVector,
                              0.0) + d->dBias > 0 ? 1 : 0;
  return NAN;
}

template <class SampleSet> PiiClassification::LearnerCapabilities PiiPerceptron<SampleSet>::capabilities() const { return 0; }
template <class SampleSet> void PiiPerceptron<SampleSet>::setMaxIterations(int maxIterations) { _d()->iMaxIterations = maxIterations; }
template <class SampleSet> int PiiPerceptron<SampleSet>::maxIterations() const { return _d()->iMaxIterations; }
template <class SampleSet> void PiiPerceptron<SampleSet>::setWeights(const QVector<double>& weights) { _d()->vecWeights = weights; }
template <class SampleSet> QVector<double> PiiPerceptron<SampleSet>::weights() const { return _d()->vecWeights; }
template <class SampleSet> void PiiPerceptron<SampleSet>::setBias(double bias) { _d()->dBias = bias; }
template <class SampleSet> double PiiPerceptron<SampleSet>::bias() const { return _d()->dBias; }
template <class SampleSet> bool PiiPerceptron<SampleSet>::converged() const throw() { return _d()->bConverged; }
