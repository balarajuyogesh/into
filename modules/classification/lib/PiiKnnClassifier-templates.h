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

#ifndef _PIIKNNCLASSIFIER_H
# error "Never use <PiiKnnClassifier-templates.h> directly; include <PiiKnnClassifier.h> instead."
#endif

#include <QtAlgorithms>

template <class SampleSet> PiiKnnClassifier<SampleSet>::Data::Data() :
  k(5)
{}

template <class SampleSet> PiiKnnClassifier<SampleSet>::Data::Data(PiiDistanceMeasure<SampleSet>* measure) :
  PiiVectorQuantizer<SampleSet>::Data(measure),
  k(5)
{}

template <class SampleSet> PiiKnnClassifier<SampleSet>::PiiKnnClassifier() :
  PiiVectorQuantizer<SampleSet>(new Data)
{}

template <class SampleSet> PiiKnnClassifier<SampleSet>::PiiKnnClassifier(PiiDistanceMeasure<SampleSet>* measure) :
  PiiVectorQuantizer<SampleSet>(new Data(measure))
{}

template <class SampleSet> PiiKnnClassifier<SampleSet>::~PiiKnnClassifier()
{}

template <class SampleSet> int PiiKnnClassifier<SampleSet>::getK() const { return _d()->k; }

template <class SampleSet> void PiiKnnClassifier<SampleSet>::setK(int k) { if (k > 0) _d()->k = k; }

template <class SampleSet>
QVector<double> PiiKnnClassifier<SampleSet>::classLabels() const
{
  return _d()->vecClassLabels;
}

template <class SampleSet>
QVector<double>& PiiKnnClassifier<SampleSet>::classLabels()
{
  return _d()->vecClassLabels;
}

template <class SampleSet>
void PiiKnnClassifier<SampleSet>::setClassLabels(const QVector<double>& labels)
{
  _d()->vecClassLabels = labels;
}

template <class SampleSet>
double PiiKnnClassifier<SampleSet>::classify(ConstFeatureIterator featureVector) throw()
{
  PII_D;
  double dDistance;
  int iLabelIndex = findClosestMatch(featureVector, &dDistance);
  if (dDistance > d->dRejectThreshold || iLabelIndex < 0 || iLabelIndex >= d->vecClassLabels.size())
    return NAN;
  return d->vecClassLabels[iLabelIndex];
}

template <class SampleSet> int PiiKnnClassifier<SampleSet>::findClosestMatch(ConstFeatureIterator featureVector,
                                                                             double* distance) const throw()
{
  const PII_D;
  int iClosestIndex;
  if (d->k == 1)
    iClosestIndex = PiiClassification::findClosestMatch(featureVector,
                                                        d->modelSet,
                                                        *d->pMeasure,
                                                        distance);
  else
    PiiClassification::knnClassify(featureVector,
                                   d->modelSet,
                                   d->vecClassLabels,
                                   *d->pMeasure,
                                   d->k,
                                   distance,
                                   &iClosestIndex);
  return iClosestIndex;
}
