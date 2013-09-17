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

#ifndef _PIIFEATURECOMBINER_H
#define _PIIFEATURECOMBINER_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include "PiiDistanceMeasure.h"

/**
 * Feature combiner is an operation that combines feature vectors into
 * a larger compound feature vector. This is useful when one wants to
 * use many different features together in classification. The
 * operation reads from 2 (the default) up to 64 feature vectors,
 * concatenates them into one compound vector and emits the result. 
 * Since the incoming vectors may vary in length, the operation also
 * emits a vector that indicates the boundaries of the input vectors.
 *
 * Feature combiner has the capability of learning scaling factors for
 * otherwise incompatible distance measures. Given a list of distance
 * measures and a batch of samples, PiiFeatureCombiner estimates the
 * variances of distance measure outcomes by calculating pairwise
 * distances between samples in the batch. It provides the inverse
 * values of the variances as scaling factors that can be used as
 * weights for a PiiMultiFeatureDistance (see
 * PiiVectorQuantizerOperation::distanceWeights).
 *
 * To use the learning facilities one needs to set the
 * [distanceMeasures] property so that each incoming feature vector has
 * a distance measure. The [learningBatchSize] property must be set to a value
 * larger than one, or to -1 to start buffering samples. Once enough
 * samples have been buffered, learning is started by calling
 * [startLearningThread()]. While learning, the operation regularly
 * emits the [progressed()] signal. When the learning thread quits, the
 * [distanceWeights] property contains scaling factors that should make
 * the individual distance measures commensurable.
 *
 * Inputs
 * ------
 *
 * @in featuresX - a feature vector. X varies from 0 to N, where N is
 * the number of different feature vectors to combine. Any primitive
 * matrix type. Only row matrices are accepted.
 *
 * Outputs
 * -------
 *
 * @out features - a concatenated feature vector. If any input is
 * double, the result is double, then float, then int. If AAAA is read
 * from `input0` and BBBBB from `input1`, `features` will emit
 * AAAABBBBB.
 *
 * @out boundaries - the indices of vector ends (PiiMatrix<int>). The
 * first vector is always at index 0, and the first boundary value is
 * the index of the start of the second one. In the previous example,
 * the boundaries would contain two values: 4 and 9.
 *
 * An example
 * ----------
 *
 * Let us assume that [dynamicInputCount] is set to three, and the
 * following inputs are received:
 *
 * - features0: PiiMatrix<double> [ 0.1, 0.2, 0.3 ]
 * - features1: PiiMatrix<int> [ 4, 5, 6, 7 ]
 * - features2: a `float` 80.0
 *
 * The compound feature vector will be a PiiMatrix<double> [ 0.1, 0.2,
 * 0.3, 4, 5, 6, 7, 80.0 ]. The `boundaries` output will emit a
 * PiiMatrix<int> [ 3, 7, 8 ].
 *
 */
class PiiFeatureCombiner : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of inputs, i.e. the number of feature vectors to
   * combine. Valid values are 1-64. The default is two.
   */
  Q_PROPERTY(int dynamicInputCount READ dynamicInputCount WRITE setDynamicInputCount);
  
  /**
   * The maximum number of training samples collected for learning. 
   * Zero means that no training samples will be collected, and the
   * operation will only join incoming feature vectors. If 
   * `learningBatchSize` is set to N (N > 1), at most N samples will be kept in
   * memory. If `learningBatchSize` is -1, all incoming samples will be
   * buffered without limit. The buffered samples will be used to
   * estimate the variance of inter-sample distances. The default
   * value is 0.
   *
   * Note that the time it takes to learn distance distributions is
   * proportional to [learningBatchSize] squared. The total number of distance
   * measure evaluations is equal to \(\fraq{N}{2}(M^2 - M)\), where
   * N is the number of feature vectors and M is the number of
   * samples.
   */
  Q_PROPERTY(int learningBatchSize READ learningBatchSize WRITE setLearningBatchSize);

  /**
   * The action to perform with new samples when [learningBatchSize] has been
   * exceeded. The default is `OverwriteRandomSample`.
   */
  Q_PROPERTY(FullBufferBehavior fullBufferBehavior READ fullBufferBehavior WRITE setFullBufferBehavior);
  Q_ENUMS(FullBufferBehavior);
  
  /**
   * The names of distance measures used in measuring inter-sample
   * distances in classification. See
   * PiiVectorQuantizerOperation::distanceMeasures. In this list is
   * non-empty, its length must match [dynamicInputCount].
   */
  Q_PROPERTY(QStringList distanceMeasures READ distanceMeasures WRITE setDistanceMeasures);

  /**
   * Calculated scaling factors for the distance measures. Once this
   * operation is trained, the scaling factors can be copied to
   * PiiVectorQuantizerOperation to make distance measures
   * commensurable. The list contains `doubles`, and its length
   * equals [featureCount]. If the operation has not been trained, the
   * list is empty.
   */
  Q_PROPERTY(QVariantList distanceWeights READ distanceWeights WRITE setDistanceWeights);

  /**
   * The total number of features to be scaled, or zero if not known
   * yet. This value will be reset to zero at check() and set again
   * when the first feature vector arrives.
   */
  Q_PROPERTY(int featureCount READ featureCount);

  /**
   * A read-only property whose value is `true` when the learning
   * thread is running, and `false` otherwise.
   */
  Q_PROPERTY(bool learningThreadRunning READ learningThreadRunning);

  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  /**
   * See PiiClassification::FullBufferBehavior.
   */
  enum FullBufferBehavior
  {
    OverwriteRandomSample,
    OverwriteOldestSample,
    DiscardNewSample
  };
  
  PiiFeatureCombiner();
  ~PiiFeatureCombiner();

  void check(bool reset);
  
  void setDynamicInputCount(int cnt);
  int dynamicInputCount() const;

  void setLearningBatchSize(int size);
  int learningBatchSize() const;

  void setFullBufferBehavior(FullBufferBehavior fullBufferBehavior);
  FullBufferBehavior fullBufferBehavior() const;

  void setDistanceMeasures(const QStringList& names);
  QStringList distanceMeasures() const;

  void setDistanceWeights(const QVariantList& distanceWeights);
  QVariantList distanceWeights() const;

  int featureCount() const;
  bool learningThreadRunning() const;

public slots:
  /**
   * Start the learning thread. If the number of buffered samples is
   * less than two or the learning thread is already running, this
   * function does nothing. Otherwise, it starts a thread that
   * calculates the variances of distances between buffered samples. 
   * The thread can be interrupted by calling [stopLearningThread()].
   * The [progressed()] signal will be emitted every once in a while
   * during the calculation.
   */
  void startLearningThread();

  /**
   * Stop the learning thread. After this function has been called,
   * [canContinue()] will return `false`, which interrupts the
   * learning algorithm.
   */
  void stopLearningThread();

signals:
  /**
   * Emitted time to time as the learning algorithm calculates
   * distance measure variances.
   *
   * @param percentage the current progress of the learning algorithm.
   * 0 means uninitialized, 1.0 means fully converged.
   */
  void progressed(double percentage);

protected:
  void process();

private:
  typedef PiiDistanceMeasure<const double*> MeasureType;
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiOutputSocket *pFeatureOutput, *pBoundaryOutput;
    PiiVariant varBoundaries;
    int iTotalLength;
    unsigned int uiMaxType;
    QStringList lstDistanceMeasureNames;
    QList<MeasureType*> lstDistanceMeasures;
    QList<double> lstDistanceWeights;
    bool bThreadRunning;
    int iLearningBatchSize;
    FullBufferBehavior fullBufferBehavior;
    QThread* pLearningThread;
    int iSampleIndex;
    PiiMatrix<double> matBuffer;
    PiiMatrix<int> matStoredBoundaries;
    mutable QMutex learningMutex;
  };
  PII_D_FUNC;

  void initializeBoundaries();
  template <class T> void emitCompound(int totalLength);
  template <class T, class U> void copyMatrixAs(const PiiVariant& obj, U* row, int* columns);
  template <class T, class U> void copyScalarAs(const PiiVariant& obj, U* row, int* columns);

  void learnBatch();
};

#endif //_PIIFEATURECOMBINER_H
