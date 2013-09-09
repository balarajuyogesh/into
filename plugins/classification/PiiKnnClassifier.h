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
#define _PIIKNNCLASSIFIER_H

#include "PiiVectorQuantizer.h"
#include <PiiMatrix.h>

/**
 * K nearest neighbors classifier. The k-NN classifier considers the k
 * code vectors closest to an unknown sample. The winning class index
 * is chosen by voting among the k closest neighbors.
 *
 * @ingroup PiiClassificationPlugin
 */
template <class SampleSet> class PiiKnnClassifier : public PiiVectorQuantizer<SampleSet>
{
public:
  typedef typename PiiSampleSet::Traits<SampleSet>::ConstFeatureIterator ConstFeatureIterator;

  /**
   * Creates a new k-NN classifier with the default distance measure
   * (PiiSquaredGeometricDistance). The value for k is initialized to
   * 5.
   */
  PiiKnnClassifier();
  /**
   * Creates a new k-NN classifier that uses @a measure to measure
   * distances between samples. The value for k is initialized to 5.
   */
  PiiKnnClassifier(PiiDistanceMeasure<SampleSet>* measure);
  
  ~PiiKnnClassifier();

  /**
   * Returns the class label of the closest model sample. If the
   * distance to the closest sample is too large (see
   * PiiVectorQuantizer::setRejectTreshold()), or there is no class
   * label for the closest sample, @p NaN will be returned.
   */
  double classify(ConstFeatureIterator featureVector) throw();

  /**
   * Returns the index of the closest model sample in the winning
   * class selected by the k nearest neighbors rule.
   */
  int findClosestMatch(ConstFeatureIterator featureVector, double* distance) const throw();

  /**
   * Returns a modifiable reference to the class labels.
   */
  QVector<double>& classLabels();

  /**
   * Returns the class labels.
   */
  QVector<double> classLabels() const;

  /**
   * Sets the class labels.
   */
  void setClassLabels(const QVector<double>& labels);

  /**
   * Sets the number of closest neighbors to find when classifying an
   * unknown sample. If @a k is set to one, the classifier works as a
   * nearest-neighbor classifier.
   */
  void setK(int k);
  /**
   * Returns the the current value for k.
   */
  int getK() const;

private:
  class Data : public PiiVectorQuantizer<SampleSet>::Data
  {
  public:
    Data();
    Data(PiiDistanceMeasure<SampleSet>* measure);
    
    QVector<double> vecClassLabels;
    int k;
  };
  PII_D_FUNC;
  PII_DISABLE_COPY(PiiKnnClassifier);
};

#include "PiiKnnClassifier-templates.h"

#endif //_PIIKNNCLASSIFIER_H
