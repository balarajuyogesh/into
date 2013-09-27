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

#ifndef _PIIKERNELPERCEPTRON_H
#define _PIIKERNELPERCEPTRON_H

#include "PiiClassifier.h"
#include "PiiLearningAlgorithm.h"
#include "PiiKernelFunction.h"

/**
 * An implementation of the Kernel Perceptron algorithm. This class
 * implements a more general version of the Perceptron algorithm known
 * as the Kernel Perceptron. The Kernel Perceptron uses a dual form of
 * the [Perceptron algorithm](PiiPerceptron) to find the
 * separating hyperplane in a high-dimensional feature space. The
 * algorithm is computationally less efficient than the ordinary
 * Perceptron, but can solve problems that are not linearly separable
 * in the input space. See [classification_kernels] for details on
 * kernel functions.
 *
 * The dual form of the Perceptron algoritm is based on the fact that
 * (assuming zero initial weights) the weight vector is a linear
 * combination of the training vectors:
 *
 * \[
 * \mathbf{w} = \sum_{i=1}^M \alpha_i \mathbf{x}_i
 * \]
 *
 * Therefore, finding the coefficients \(\alpha_i\) is equivalent to
 * finding the weight vector. One cannot just solve the weight vector
 * as in the normal case, because the solution must be found in the
 * feature space, not in the input space. Depending on the kernel
 * function, we may even not know the dimensionality of the feature
 * space. In the case of a Gaussian kernel, the feature space is
 * infinite-dimensional.
 *
 * The dual form of the decision threshold is @f$\sum_{i=1}^M(\alpha_i
 * \mathbf{x}_i) \cdot \mathbf{x} = \sum_{i=1}^M(\alpha_i \mathbf{x}_i
 * \cdot \mathbf{x})@f$. Now, if the weight vector and the feature
 * vector are mapped into a high-dimensional feature space, the
 * threshold becomes @f$\sum_{i=1}^M(\alpha_i \Phi(\mathbf{x}_i) \cdot
 * \Phi(\mathbf{x}))@f$. Thus, the decision function for the Kernel
 * Perceptron is
 *
 * \[
 * f(\mathbf{x}) = \begin{cases}
 * 1 & \text{if }\sum_{i=1}^M \alpha_i k(\mathbf{x}_i, \mathbf{x}) > 0 \\
 * 0 & \text{otherwise}
 * \end{cases}
 * \]
 *
 * Here, *k*() denotes the kernel function. The original perceptron
 * algorithm uses dot product as the kernel, resulting in a linear
 * hyperplane classifier. Thus, using PiiDotProductKernel as the
 * kernel function for PiiKernelPerceptron is functionally equivalent
 * to (but less efficient than) using PiiPerceptron, except for the
 * bias term.
 *
 * After training, the Kernel Perceptron only keeps the support
 * vectors (training samples with a non-zero weight) in memory. The
 * number of retained support vectors varies widely depending on
 * application, but sometimes only very few samples need to be stored.
 *
 * ! The derivation above neglects the bias term. See 
 * [classification_kernels] for a justification.
 *
 */
template <class SampleSet> class PiiKernelPerceptron :
  public PiiLearningAlgorithm<SampleSet>,
  public PiiClassifier<SampleSet>
{
public:
  typedef typename PiiSampleSet::Traits<SampleSet>::ConstFeatureIterator ConstFeatureIterator;

  /// Constructs a new Kernel Perceptron.
  PiiKernelPerceptron();
  /// Destroys the Kernel Perceptron.
  ~PiiKernelPerceptron();

  /**
   * Runs the Kernel Perceptron algorithm with the given *samples*
   * and class *labels*. Since the Kernel Perceptron is a binary
   * classifier, the class labels must be either ones or zeros. The
   * weights will be ignored.
   */
  void learn(const SampleSet& samples,
             const QVector<double>& labels,
             const QVector<double>& weights = QVector<double>());

  /**
   * Returns `true` if learn() was called, and the learning algorithm
   * converged to a solution, `false` otherwise. The learning
   * algorithm won't converge if the classes are not linearly
   * separable in the feature space or if [maxIterations()] is reached
   * before finding a solution.
   */
  bool converged() const throw();

  /**
   * Returns the classification of `featureVector` (either 0 or 1),
   * or `NaN` if the perceptron has not been trained yet.
   */
  double classify(ConstFeatureIterator featureVector) throw ();

  /**
   * Returns 0. The Kernel Perceptron is a supervised classifier with
   * no on-line learning capabilities.
   */
  PiiClassification::LearnerCapabilities capabilities() const;

  /**
   * Returns the kernel function.
   */
  PiiKernelFunction<ConstFeatureIterator>* kernelFunction() const;

  /**
   * Sets the kernel function. This class will take the ownership of
   * *kernel*. The old kernel function will be deleted. The default
   * kernel is a Gaussian kernel with unit variance.
   */
  void setKernelFunction(PiiKernelFunction<ConstFeatureIterator>* kernel);
  
  /**
   * Returns the number of features in each feature vector. If the
   * perceptron has not been trained, zero will be returned.
   */
  int featureCount() const;

  /**
   * Returns the weight vector. The number of weights is equal to the
   * number of support vectors.
   */
  QVector<double> weights() const;

  /**
   * Sets the weight vector. The weighs are usually not assigned
   * manually, but using the learn() function.
   */
  void setWeights(const QVector<double>& weights);

  /**
   * Returns the support vectors.
   */
  SampleSet supportVectors() const;

  /**
   * Sets the support vectors. The support vectors are usually not set
   * manually, but using the learn() function.
   */
  void setSupportVectors(const SampleSet& supportVectors);
  
  /**
   * Returns the maximum number of learning iterations. The default
   * value is 100.
   */
  int maxIterations() const;
  /**
   * Sets the maximum number of iterations the algorithm will take. 
   * This value limits the number of times the whole sample set is
   * iterated over.
   */
  void setMaxIterations(int maxIterations);
  
private:
  class Data : public PiiLearningAlgorithm<SampleSet>::Data
  {
  public:
    Data();
    PiiKernelFunction<ConstFeatureIterator>* pKernel;
    bool bConverged;
    int iMaxIterations;
    QVector<double> vecWeights;
    SampleSet supportVectors;
  };
  PII_D_FUNC;
};

#include "PiiKernelPerceptron-templates.h"

#endif //_PIIKERNELPERCEPTRON_H
