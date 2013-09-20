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

#ifndef _PIISAMPLEBALANCER_H
#define _PIISAMPLEBALANCER_H

#include <PiiDefaultOperation.h>

/**
 * An operation that balances training sets by giving more weight to
 * rare samples. The weighting is based on the distribution of
 * individual feature values. The balancer works in two modes: 
 * `ProbabilitySelection` and `WeightCalculation`. In the former mode,
 * the operation either passes feature vectors to the `features`
 * output or does nothing, based on the estimated weight of the
 * sample. In the latter mode, all features will be passed, and the
 * weight of the sample will be sent to the `weight` output.
 *
 * The graph below illustrates sample weighting on one-dimensional
 * Gaussian data. The (normalized) distribution of a feature value is
 * shown in blue. Its inverse (green) is used as a weight. The red
 * curve illustrates the effect of setting [emphasis] to three.
 *
 * @image html samplebalancer.png
 *
 * For multi-dimensional features, PiiSampleBalancer uses marginal
 * distributions, based on the assumption that all features are
 * independent. This is often not the case, but gives a reasonable
 * approximation without huge memory requirements.
 *
 * Inputs
 * ------
 *
 * @in features - feature vector. Each component must be quantized to
 * the number of quantization levels determined by [levels].
 * 
 * Outputs
 * -------
 *
 * @out features - the features. If [mode] is `ProbabilitySelection`,
 * the features will be emitted only if a generated random number is
 * less than `weight`. The `select` output will indicate whether the
 * sample was selected or not. In `WeightCalculation` mode, this
 * output will always pass the incoming features.
 *
 * @out weight - the weight of the sample, 0.0-1.0 (double). 0.0 means
 * not selected and 1.0 means definitely selected.
 *
 * @out select - a boolean value indicating whether the sample was
 * randomly selected or not. In `WeightCalculation` mode, this is
 * output will always emit `true`.
 *
 */
class PiiSampleBalancer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Operation mode.
   */
  Q_PROPERTY(Mode mode READ mode WRITE setMode);
  Q_ENUMS(Mode);
  
  /**
   * A list of quantization levels for each feature value. For
   * three-dimensional feature vectors, the default can be changed as
   * follows:
   *
   * ~~~(c++)
   * balancer->setProperty("levels", QVariantList() << 128 << 256 << 64);
   * ~~~
   *
   * The minimum number of quantization levels is one.
   */
  Q_PROPERTY(QVariantList levels READ levels WRITE setLevels);

  /**
   * The default number of quantization levels. This value is used for
   * all features whose quantization levels have not been explicitly
   * set by [levels]. The default value is 256.
   */
  Q_PROPERTY(int defaultLevels READ defaultLevels WRITE setDefaultLevels);

  /**
   * By default, the operation tries to flatten out the variations in
   * feature distribution. If the common samples need to be given even
   * less weight, `emphasis` can be set to a larger value. The
   * operation will raise the weight estimate to this power.
   */
  Q_PROPERTY(int emphasis READ emphasis WRITE setEmphasis);
  
  /**
   * The speed of adaptation to changing conditions. The operation
   * initially assumes a uniform feature distribution. The estimate of
   * the distribution is updated once every [learningBatchSize] samples. The
   * adaptation ratio tells how much the new measurements affect the
   * learnt model. 0 means that the initial uniform approximation will
   * never be changed. 1 means that the new estimate will fully
   * replace the old one. The default value is 0.1.
   */
  Q_PROPERTY(double adaptationRatio READ adaptationRatio WRITE setAdaptationRatio);

  /**
   * The number of features required for a reliable estimate. The
   * estimate is updated every `learningBatchSize` samples. The default value
   * is 25600 (100 samples / histogram bin).
   */
  Q_PROPERTY(int learningBatchSize READ learningBatchSize WRITE setLearningBatchSize);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Operation modes.
   *
   * - `ProbabilitySelection` - pass those feature vectors that are
   * likely to be important with a higher probability than the others.
   *
   * - `WeightCalculation` - pass every incoming vector accompanied
   * with selection probability.
   */
  enum Mode { ProbabilitySelection, WeightCalculation };
  
  PiiSampleBalancer();
  ~PiiSampleBalancer();

  void setLevels(const QVariantList& levels);
  QVariantList levels() const;
  void setDefaultLevels(int defaultLevels);
  int defaultLevels() const;
  void setEmphasis(int emphasis);
  int emphasis() const;
  void setMode(Mode mode);
  Mode mode() const;
  void setAdaptationRatio(double adaptationRatio);
  double adaptationRatio() const;
  void setLearningBatchSize(int learningBatchSize);
  int learningBatchSize() const;

  void check(bool reset);

protected:
  void process();

private:
  class Histogram;
  inline double weight(int feature, int index);
  void allocateHistograms();
  template <class T> void balance(const PiiVariant& obj);
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    Mode mode;
    int iDefaultLevels;
    int iEmphasis;
    int iFeatureCnt;
    Histogram* pHistograms;
    double dAdaptationRatio;
    int iLearningBatchSize;
    QList<int> lstLevels;
  };
  PII_D_FUNC;
};


#endif //_PIISAMPLEBALANCER_H
