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
#define _PIIPERCEPTRON_H

#include <PiiClassifier.h>
#include <PiiLearningAlgorithm.h>

/**
 * Implementation of the Perceptron algorithm. The Perceptron is a
 * linear (hyperplane) classifier that maps an N-dimensional input
 * space into a binary value:
 *
 * \[
 * f(\mathbf{x}) = \begin{cases}
 * 1 & \text{if }\mathbf{w} \cdot \mathbf{x} + b > 0 \\
 * 0 & \text{otherwise}
 * \end{cases}
 * \]
 * 
 * The vector **w** in the equation is known as the *weight
 * vector*, which defines the direction of the hyperplane
 * separating the two classes. (The weight vector is perpendicular to
 * the plane, pointing to the positive side.) The *b* term is known
 * as the *bias*, and it can be seen as an offset to the decision
 * function. Geometrically, a non-zero bias term translates the
 * separating hyperplane along the plane's normal (weight vector) so
 * that the plane doesn't intersect the origin of the feature
 * space. The Perceptron algorithm iteratively adjusts the weights and
 * the bias to find the separating hyperplane.
 *
 * The input for the training algorithm is a sample set with binary
 * classifications: \(\mathbf{S} = \{ (\mathbf{x}_i, c_i)| c_i \in
 * \{0,1\}\}_{i=1}^M\). On each iteration, the weigh vector and the
 * bias are updated as follows:
 *
 * \[
 * \mathbf{w}_{t+1} = \mathbf{w}_t + \mu (c_i - f(\mathbf{x}_i))
 * \mathbf{x}_i
 * \]
 *
 * \[
 * b_{t+1} = b_t + \mu (c_i - f(\mathbf{x}_i))
 * \]
 *
 * Here, \(\mu\) is a learning constant and *t* denotes time. Note
 * that if the sample is classified correctly (*c* equals the output
 * of the decision function f(x)), the updates become zero. Another
 * thing to note is that scaling the decision function with a constant
 * value has no effect on the outcome. Therefore, the choice of
 * \(\mu\) becomes irrelevat with increasing number of
 * iterations. Usually, the learning rate is just set to one. In fact,
 * this implementation doesn't even allow you to specify the learning
 * rate for the sake of avoiding unnecessary parameterization.
 *
 * The linear Perceptron algorithm converges if the two classes are
 * linearly separable in the input space. If they are not, one can use
 * a [kernel function](classification_kernels) to separate the
 * classes with a non-linear mapping to a high-dimensional feature
 * space. This is called the [PiiKernelPerceptron] "Kernel
 * Perceptron" algorithm.
 *
 */
template <class SampleSet> class PiiPerceptron :
  public PiiLearningAlgorithm<SampleSet>,
  public PiiClassifier<SampleSet>
{
public:
  typedef typename PiiSampleSet::Traits<SampleSet>::ConstFeatureIterator ConstFeatureIterator;
  
  PiiPerceptron();
  ~PiiPerceptron();

  /**
   * Runs the Perceptron algorithm with the given *samples* and class
   * *labels*. Since the Perceptron is a binary classifier, the class
   * labels must be either ones or zeros. The *weights* will be
   * ignored.
   */
  void learn(const SampleSet& samples,
             const QVector<double>& labels,
             const QVector<double>& weights = QVector<double>());

  /**
   * Returns `true` if learn() was called, and the learning algorithm
   * converged to a solution, `false` otherwise. The learning
   * algorithm won't converge if the classes are not linearly
   * separable or if maxIterations() is reached before finding a
   * solution.
   */
  bool converged() const throw();

  /**
   * Returns the classification of `featureVector` (either 0 or 1),
   * or `NaN` if the Perceptron has not been trained yet.
   */
  double classify(ConstFeatureIterator featureVector) throw ();

  /**
   * Returns 0. The Perceptron is a supervised classifier with no
   * on-line learning capabilities.
   */
  PiiClassification::LearnerCapabilities capabilities() const;

  /**
   * Returns the number of features in each feature vector. If the
   * Perceptron has not been trained, zero will be returned.
   */
  int featureCount() const;

  /**
   * Returns the weight vector. The length of the weight vector is
   * equal to the number of feature space dimensions. If learn() has
   * not been called, an empty vector will be returned.
   */
  QVector<double> weights() const;

  /**
   * Sets the weight vector. The weighs are usually not assigned
   * manually, but using the learn() function. The number of weights
   * must match the number of feature space dimensions.
   */
  void setWeights(const QVector<double>& weights);

  /**
   * Returns the bias term. This value is initially zero and assigned
   * by the learning algorithm
   */
  double bias() const;

  /**
   * Sets the bias term. The bias is usually not assigned manually,
   * but using the learn() function.
   */
  void setBias(double bias);

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
    bool bConverged;
    double dBias;
    int iMaxIterations;
    QVector<double> vecWeights;
  };
  PII_D_FUNC;

  double adaptTo(ConstFeatureIterator featureVector, double label);
};

#include "PiiPerceptron-templates.h"

#endif //_PIIPERCEPTRON_H
