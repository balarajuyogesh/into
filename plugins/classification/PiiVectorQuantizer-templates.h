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

#ifndef _PIIVECTORQUANTIZER_H
# error "Never use <PiiVectorQuantizer-templates.h> directly; include <PiiVectorQuantizer.h> instead."
#endif

#include <PiiMatrix.h>
#include "PiiClassification.h"
#include "PiiSquaredGeometricDistance.h"

template <class SampleSet>
PiiVectorQuantizer<SampleSet>::Data::Data() :
  pMeasure(new PII_POLYMORPHIC_MEASURE(PiiSquaredGeometricDistance)),
  dRejectThreshold(INFINITY)
{
}

template <class SampleSet>
PiiVectorQuantizer<SampleSet>::Data::Data(PiiDistanceMeasure<ConstFeatureIterator>* measure) :
  dRejectThreshold(INFINITY),
  pMeasure(measure)
{
}

template <class SampleSet>
PiiVectorQuantizer<SampleSet>::PiiVectorQuantizer() :
  d(new Data)
{
}

template <class SampleSet>
PiiVectorQuantizer<SampleSet>::PiiVectorQuantizer(Data* data) :
  d(data)
{
}

template <class SampleSet>
PiiVectorQuantizer<SampleSet>::PiiVectorQuantizer(PiiDistanceMeasure<ConstFeatureIterator>* measure) :
  d(new Data(measure))
{
}

template <class SampleSet> PiiVectorQuantizer<SampleSet>::~PiiVectorQuantizer()
{
  delete d->pMeasure;
  delete d;
}

template <class SampleSet> void PiiVectorQuantizer<SampleSet>::setRejectThreshold(double rejectThreshold)
{
  d->dRejectThreshold = rejectThreshold;
}

template <class SampleSet> double PiiVectorQuantizer<SampleSet>::rejectThreshold() const
{
  return d->dRejectThreshold;
}
  
template <class SampleSet>
PiiDistanceMeasure<typename PiiSampleSet::Traits<SampleSet>::ConstFeatureIterator>* PiiVectorQuantizer<SampleSet>::distanceMeasure() const
{
  return d->pMeasure;
}

template <class SampleSet> void PiiVectorQuantizer<SampleSet>::setModels(const SampleSet& models)
{
  d->modelSet = models;
}

template <class SampleSet> SampleSet& PiiVectorQuantizer<SampleSet>::models()
{
  return d->modelSet;
}

template <class SampleSet> SampleSet PiiVectorQuantizer<SampleSet>::models() const
{
  return d->modelSet;
}

template <class SampleSet>
typename PiiSampleSet::Traits<SampleSet>::FeatureIterator PiiVectorQuantizer<SampleSet>::modelAt(int index)
{
  return PiiSampleSet::sampleAt(d->modelSet, index);
}

template <class SampleSet>
typename PiiSampleSet::Traits<SampleSet>::ConstFeatureIterator PiiVectorQuantizer<SampleSet>::modelAt(int index) const
{
  return PiiSampleSet::sampleAt(d->modelSet, index);
}

template <class SampleSet> int PiiVectorQuantizer<SampleSet>::modelCount() const
{
  return PiiSampleSet::sampleCount(d->modelSet);
}

template <class SampleSet> int PiiVectorQuantizer<SampleSet>::featureCount() const
{
  return PiiSampleSet::featureCount(d->modelSet);
}


template <class SampleSet> void PiiVectorQuantizer<SampleSet>::setDistanceMeasure(PiiDistanceMeasure<ConstFeatureIterator>* measure)
{
  delete d->pMeasure;
  d->pMeasure = measure;
}

template <class SampleSet> double PiiVectorQuantizer<SampleSet>::classify(ConstFeatureIterator features) throw()
{
  double distance;
  int iBestMatch = findClosestMatch(features, &distance);
  return iBestMatch >= 0 ? iBestMatch : NAN;
}

template <class SampleSet> int PiiVectorQuantizer<SampleSet>::findClosestMatch(ConstFeatureIterator features,
                                                                               double* distance) const throw()
{
  *distance = INFINITY;
  int iBestMatch = PiiClassification::findClosestMatch(features,
                                                       const_cast<const SampleSet&>(d->modelSet),
                                                       *d->pMeasure,
                                                       distance);
  // Return the index of the closest code vector or -1, if the sample
  // is rejected.
  return *distance <= d->dRejectThreshold ? iBestMatch : -1;
}
