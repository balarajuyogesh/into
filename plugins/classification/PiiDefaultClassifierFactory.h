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

#ifndef _PIIDEFAULTCLASSIFIERFACTORY_H
#define _PIIDEFAULTCLASSIFIERFACTORY_H

#include "PiiBoostClassifier.h"

/**
 * Default implementation of PiiBoostClassifier::Factory. The type of
 * the classifier to be created by this factory is given by the @p
 * Classifier template parameter. @p Classifier must implement the
 * PiiLearningAlgorithm and PiiClassifier interfaces and it must
 * support weighted learning.
 *
 * @code
 * // Creates decision stumps
 * PiiDefaultClassifierFactory<PiiDecisionStump<PiiMatrix<double> > > factory;
 * PiiBoostClassifier<PiiMatrix<double> > classifier(&factory);
 * @endcode
 *
 * @see PiiBoostClassifier
 *
 * @ingroup PiiClassificationPlugin
 */
template <class Classifier> class PiiDefaultClassifierFactory :
  public PiiBoostClassifier<typename Classifier::SampleSetType>::Factory
{
public:
  typedef typename Classifier::SampleSetType SampleSet;
  
  /**
   * Creates a new instance of @p Classifier (given as a template
   * parameter) using its default constructor and trains it using the
   * given @a samples, @a labels, and @a weights.
   */
  Classifier* create(PiiBoostClassifier<SampleSet>* classifier,
                     const SampleSet& samples,
                     const QVector<double>& labels,
                     const QVector<double>& weights);
};

template <class Classifier>
Classifier* PiiDefaultClassifierFactory<Classifier>::create(PiiBoostClassifier<SampleSet>* classifier,
                                                            const SampleSet& samples,
                                                            const QVector<double>& labels,
                                                            const QVector<double>& weights)
{
  Q_UNUSED(classifier);
  Classifier* pClassifier = new Classifier;
  pClassifier->learn(samples, labels, weights);
  return pClassifier;
}

#endif //_PIIDEFAULTCLASSIFIERFACTORY_H
