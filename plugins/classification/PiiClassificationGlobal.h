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

#ifndef _PIICLASSIFICATIONGLOBAL_H
#define _PIICLASSIFICATIONGLOBAL_H

#include <PiiGlobal.h>
#include <QObject>

#ifdef PII_BUILD_CLASSIFICATION
#  define PII_CLASSIFICATION_EXPORT PII_DECL_EXPORT
#  define PII_BUILDING_CLASSIFICATION 1
#else
#  define PII_CLASSIFICATION_EXPORT PII_DECL_IMPORT
#  define PII_BUILDING_CLASSIFICATION 0
#endif

#define PII_CLASSIFICATION_NAME_ALIAS_AS_T(CLASS_NAME, REAL_TYPE) \
namespace PiiSerializationTraits { template <class T> struct ClassName<CLASS_NAME<REAL_TYPE> > { \
  static const char* get() { \
    static PiiSmartPtr<char[]> pName(createTemplateName(PII_STRINGIZE(CLASS_NAME), ClassName<T>::get())); \
    return static_cast<char*>(pName); } }; }

#ifdef Q_MOC_RUN
class PiiClassification
#else
namespace PiiClassification
#endif
{
#ifdef Q_MOC_RUN
  Q_GADGET
    
  Q_ENUMS(BoostingAlgorithm
          FullBufferBehavior
          DistanceCombinationMode
          SomTopology
          SomRateFunction
          SomNeighborhood
          SomInitMode
          SomLearningAlgorithm);
  Q_FLAGS(LearnerCapability LearnerCapabilities);
public:
#endif
  /// @internal
  extern PII_CLASSIFICATION_EXPORT const QMetaObject staticMetaObject;

  /**
   * Implemented boosting algorithms.
   *
   * - `AdaBoost` - the original (discrete) AdaBoost as introduced by
   * Schapire & Freund. This algorithm has mostly historical value,
   * use RealBoost instead.
   *
   * - `RealBoost` - AdaBoost with confidence-rated predictions
   * (a.k.a RealAdaBoost). Usually more accurate than AdaBoost.
   *
   * - `FloatBoost` - RealBoost supplemented by ideas from
   * floating search methods (after Stan Z. Li et al.)
   *
   * - `SammeBoost` - Stagewise Adaptive Modeling using a Multi-class
   * Exponential loss function (after Ji Zhu et al.) A multi-class
   * generalization of AdaBoost.
   */
  enum BoostingAlgorithm
    {
      AdaBoost,
      RealBoost,
      FloatBoost,
      SammeBoost
    };

  /**
   * Possible actions when a sample buffer is full.
   *
   * - `OverwriteRandomSample` - the sample to be overwritten will be
   * picked at random
   *
   * - `OverwriteOldestSample` - the oldest sample currently in the
   * buffer will be overwritten.
   *
   * - `DiscardNewSample` - perform no action. Once the buffer is
   * full, new samples will no longer be buffered.
   */
  enum FullBufferBehavior
  {
    OverwriteRandomSample,
    OverwriteOldestSample,
    DiscardNewSample
  };

  /**
   * Different ways of combining sub-vector distances in
   * PiiMultiFeatureDistance.
   *
   * - `DistanceSum` - sub-vector distances are summed up
   *
   * - `DistanceProduct` - sub-vector distances are multiplied by
   * each other
   *
   * - `DistanceMin` - the minimum sub-vector distance is returned
   *
   * - `DistanceMax` - the maximum sub-vector distance is returned
   */
  enum DistanceCombinationMode
    {
      DistanceSum,
      DistanceProduct,
      DistanceMin,
      DistanceMax
    };

  /**
   * Learning algorithm capabilities.
   *
   * - `NonSupervisedLearner` - the algorithm can be trained with no
   * a priori knowledge of sample labels.
   *
   * - `OnlineLearner` - the classifier is capable of learning
   * on-line, one sample at a time.
   *
   * - `WeightedLearner` - the classifier is able to learn weighted
   * samples.
   */
  enum LearnerCapability
    {
      NonSupervisedLearner = 0x1,
      OnlineLearner = 0x2,
      WeightedLearner = 0x4
    };

  Q_DECLARE_FLAGS(LearnerCapabilities, LearnerCapability);
  Q_DECLARE_OPERATORS_FOR_FLAGS(LearnerCapabilities);

  /**
   * Different topologies types for the arrangement of neighboring
   * nodes in a SOM.
   *
   * - `SomHexagonal` - with each node, six closest neighbors have a
   * distance of one
   * - `SomSquare` - four neighbors have a distance of one
   *
   * The following picture illustrates the arrangement of neighbors
   * with different topologies. With hexagonal arrangement, distance
   * to the six closest neighbors is one. With squares, the corners
   * have a distance of sqrt(2).
   *
   * ~~~
   *      ___        ___ ___ ___
   *  ___/   \___   |   |   |   |
   * /   \___/   \  |___|___|___|
   * \___/   \___/  |   |   |   |
   * /   \___/   \  |___|___|___|
   * \___/   \___/  |   |   |   |
   *     \___/      |___|___|___|
   *     
   * ~~~
   */
  enum SomTopology { SomHexagonal, SomSquare };

  /**
   * SOM learning rate functions.
   *
   * - `SomLinearAlpha` - learning rate decreases linearly
   * - `SomInverseAlpha` - learning rate is inversely proportional to
   * training interation index
   */
  enum SomRateFunction { SomLinearAlpha, SomInverseAlpha };

  /**
   * Different types of SOM neighborhoods. When updating nodes in a
   * SOM, the amount of vector movement is determined by the
   * neighborhood function.
   *
   * - `SomBubble` - each node within the current radius is updated
   * with a weight of one. Others are not updated.
   *
   * - `SomGaussian` - the neighbors are weighted according to a
   * Gaussian function that decreases with distance.
   *
   * - `SomCutGaussian` - the neighbors are weighted according to a
   * Gaussian function that decreases with distance, if they fall
   * within the radius. This is practically a combination of the two
   * other modes.
   */
  enum SomNeighborhood { SomBubble, SomGaussian, SomCutGaussian };

  /**
   * Initialization modes for a SOM code book.
   *
   * - `SomRandomInit` - initialize the code book randomly. The
   * limits of the random values are taken from the first incoming
   * feature vector.
   *
   * - `SomSampleInit` - initialize the code book by selecting
   * incoming samples as initial code vectors. In on-line learning,
   * the first w*h samples will be used (w and h denote SOM width and
   * height). In batch learning, initial code vectors will be randomly
   * selected from the training samples.
   */
  enum SomInitMode { SomRandomInit, SomSampleInit };

  /**
   * Learning algorithms for training a SOM.
   *
   * - `SomSequentialAlgorithm` - the traditional sequential learning
   * algorithm. Monotonically decreasing learning constant and
   * neighborhood size.
   *
   * - `SomBalancedAlgorithm` - the balanced SOM algorithm. Each
   * input sample is weighted based on its disparity. This algorithm
   * better captures small clusters in the input space while
   * maintaining the topographic properties of the original SOM
   * algorithm.
   *
   * - `SomQErrAlgorithm` - a modification of the "parameterless" SOM
   * algorithm. Each input sample is weighted based on its
   * quantization error. This algorithm is the most "elastic" of the
   * three. It tries to cover the whole input space independent of
   * data density.
   */
  enum SomLearningAlgorithm { SomSequentialAlgorithm, SomBalancedAlgorithm, SomQErrAlgorithm };
};

#endif //_PIICLASSIFICATIONGLOBAL_H
