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

#ifndef _PIIKERNELADATRON_H
#define _PIIKERNELADATRON_H

#include "PiiClassifier.h"
#include "PiiLearningAlgorithm.h"
#include "PiiKernelFunction.h"

/**
 * An implementation of the Kernel Adatron algorithm. The Kernel
 * Adatron (KA) is a hyperplane classifier whose training algorithm
 * can be seen as a hybrid between [the Perceptron](PiiPerceptron)
 * and the [Support Vector Machine](PiiSvm). The objective of the
 * KA algorithm is to find a maximal margin hyperplane just like in
 * SVM. The solution algorithm does not however involve quadratic
 * programming, which makes the KA much faster to train than the SVM.
 * It has been proven that the KA converges toward the optimal
 * solution (the maximal margin hyperplane) with an exponentially fast
 * rate of convergence. The predictive power of the KA is practically
 * equivalent to that of the SVM, but its training algorithm is 10 to
 * 100 times faster.
 *
 * The decision function of the Kernel Adatron is
 *
 * \[
 * f(\mathbf{x}) = \begin{cases}
 * 1 & \text{if }\sum_{i=1}^M \alpha_i k(\mathbf{x}_i, \mathbf{x}) (y_i - 0.5) > \theta \\
 * 0 & \text{otherwise}
 * \end{cases}
 * \]
 *
 * In the equation, M is the number of *support vectors*, i.e.
 * the number of training samples with a non-zero weight &alpha;
 * after training. &theta; is a desicion threshold, *k*()
 * denotes the kernel function, and *y* is the binary classification
 * of a training sample (0 or 1).
 */
template <class SampleSet> class PiiKernelAdatron :
  public PiiLearningAlgorithm<SampleSet>,
  public PiiClassifier<SampleSet>
{
public:
  typedef typename PiiSampleSet::Traits<SampleSet>::ConstFeatureIterator ConstFeatureIterator;

  /// Constructs a new Kernel Adatron
  PiiKernelAdatron();
  /// Destroys the Kernel Adatron.
  ~PiiKernelAdatron();

  /**
   * Runs the Kernel Adatron algorithm with the given *samples* and
   * class *labels*. Since the Kernel Adatron is a binary classifier,
   * the class labels must be either ones or zeros. The *weights*
   * will be ignored.
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
   * or `NaN` if the Kernel Adatron has not been trained yet.
   */
  double classify(ConstFeatureIterator featureVector) throw ();

  /**
   * Returns 0. The Kernel Adatron is a supervised classifier with
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
   * adatron has not been trained, zero will be returned.
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
   * Set the &theta; parameter to *decisionThreshold*.
   */
  void setDecisionThreshold(double decisionThreshold);
  /**
   * Returns the current value of &theta;.
   */
  double decisionThreshold() const;

  /**
   * Sets the learning rate. The default value is 1.0, which usually
   * gives good results. There are certain theoretical bounds on the
   * learning rate, but only experimenting with this value will show
   * its real effect.
   */
  void setLearningRate(double learningRate);
  /**
   * Returns the learning rate.
   */
  double learningRate() const;

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

  /**
   * Sets the convergence threshold. The objective of the Kernel
   * Adatron is to maximize the margin between two classes.
   * Internally, the margin is calculated so that its maximum value is
   * one. The learning algorithm will stop once the margin is close
   * enough to the optimum, i.e. when (1-margin) <=
   * *convergenceThreshold*. The default value is 0.01.
   */
  void setConvergenceThreshold(double convergenceThreshold);
  /**
   * Returns the convergence threshold.
   */
  double convergenceThreshold() const;

private:
  class Data : public PiiLearningAlgorithm<SampleSet>::Data
  {
  public:
    Data();
    PiiKernelFunction<ConstFeatureIterator>* pKernel;
    bool bConverged;
    int iMaxIterations;
    double dTheta, dLearningRate, dConvergenceThreshold;
    QVector<double> vecWeights, vecLabels;
    SampleSet supportVectors;
  };
  PII_D_FUNC;
};

#include "PiiKernelAdatron-templates.h"

#endif //_PIIKERNELADATRON_H
