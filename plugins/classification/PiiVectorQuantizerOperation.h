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

#ifndef _PIIVECTORQUANTIZEROPERATION_H
#define _PIIVECTORQUANTIZEROPERATION_H

#include "PiiVectorQuantizer.h"
#include "PiiMultiFeatureDistance.h"
#include "PiiClassifierOperation.h"


/**
 * A superclass for classifier operations that use classifiers derived
 * from PiiVectorQuantizer. This class adds support for run-time
 * changeable distance measures to the classifier operation.
 *
 * Inputs
 * ------
 *
 * @in boundaries - an optional input that marks the boundaries of
 * multiple feature vectors in a compound feature vector. This input
 * is usually used in conjunction with PiiFeatureCombiner. If this
 * input is connected, a multi-feature distance measure can used in
 * classification (PiiMatrix<int>).
 *
 * Outputs
 * -------
 *
 * @out model index - the index of the closest code vector (int). 
 * This value equals `classification` if [classLabels] are not set.
 *
 * @out distance - distance to the closest code vector. (double)
 *
 */
class PII_CLASSIFICATION_EXPORT PiiVectorQuantizerOperation : public PiiClassifierOperation
{
  Q_OBJECT

  /**
   * The class name of a distance measure. Use the resource name in
   * the [resource database](PiiYdin::resourceDatabase()) as a
   * key. Note that distance measures are registered to the resource
   * database as template instances, but template arguments should not
   * be explicitly given here. If no resource matching the given name
   * is found in the resource database, setting this property does
   * nothing. The default distance measure is
   * PiiSquaredGeometricDistance.
   *
   * ~~~(c++)
   * PiiOperation* pClassifier = engine.createOperation("PiiKnnClassifier<float>");
   * // This will create an instance of PiiHistogramIntersection<float>
   * classifier->setProperty("distanceMeasure", "PiiHistogramIntersection");
   * // Explicitly creating a one-element list has the same same effect
   * classifier->setProperty("distanceMeasures", QStringList() << "PiiHistogramIntersection");
   * ~~~
   */
  Q_PROPERTY(QString distanceMeasure READ distanceMeasure WRITE setDistanceMeasure STORED false);

  /**
   * Set many distance measures. Multiple different distance measures
   * may be needed if input samples are composed of many feature
   * vectors. Assume your feature vectors actually consist of two
   * concatenated vectors (see PiiFeatureCombiner). You intend to use
   * histogram intersection for the first and log-likelihood for the
   * other. Here's how:
   *
   * ~~~(c++)
   * classifier->setProperty("distanceMeasures",
   *                         QStringList() << "PiiHistogramIntersection" << "PiiLogLikelihood");
   * ~~~
   *
   * If multiple distance measures are given, [distanceMeasure] will be
   * set to "PiiMultiFeatureDistance".
   */
  Q_PROPERTY(QStringList distanceMeasures READ distanceMeasures WRITE setDistanceMeasures);

  /**
   * Weights assigned to distance measures, if many are used. See
   * PiiMultiFeatureDistance.
   */
  Q_PROPERTY(QVariantList distanceWeights READ distanceWeights WRITE setDistanceWeights);

  /**
   * The mode of combining the distances calculated by different
   * distance measures. See PiiMultiFeatureDistance.
   */
  Q_PROPERTY(PiiClassification::DistanceCombinationMode distanceCombinationMode
             READ distanceCombinationMode
             WRITE setDistanceCombinationMode);
  
  /**
   * Set a distance threshold for rejecting samples. If the distance
   * of an unknown sample to the closest code vector is above this
   * threshold, the sample will be rejected and classified as minus
   * one. The default value is INFINITY.
   */
  Q_PROPERTY(double rejectThreshold READ rejectThreshold WRITE setRejectThreshold);

  /**
   * The model samples as a PiiVariant. The variant will usually
   * hold a PiiMatrix whose data type equals the type of the
   * operation. For example, PiiSomOperation<double> will have a
   * PiiMatrix<double> as its code book. Subclasses are however free
   * to use any suitable data structure to represent the code book.
   */
  Q_PROPERTY(PiiVariant models READ models WRITE setModels);

  /**
   * Class labels for code vectors. If this list is non-empty, the 
   * `classification` output will emit the label corresponding to the
   * closest code vector instead of the index of the code vector. 
   * Otherwise the `classification` and `vector` `index` outputs
   * will both emit the index of the closest code vector.
   */
  Q_PROPERTY(QVariantList classLabels READ classLabels WRITE setClassLabels);
  
public:
  ~PiiVectorQuantizerOperation();

protected:
  /// @internal
  class PII_CLASSIFICATION_EXPORT Data : public PiiClassifierOperation::Data
  {
  public:
    Data(PiiClassification::LearnerCapabilities capabilities);

    PiiInputSocket* pBoundaryInput;
    PiiOutputSocket *pVectorIndexOutput, *pDistanceOutput;
    QStringList lstDistanceMeasures;
    QVariantList lstDistanceWeights;
    PiiClassification::DistanceCombinationMode distanceCombinationMode;
    double dRejectThreshold;
    QVector<double> vecClassLabels;
    bool bMultiFeatureMeasure;
    bool bMustConfigureBoundaries;
    PiiVariant varModels;
  };
  PII_D_FUNC;

  PiiVectorQuantizerOperation(PiiClassification::LearnerCapabilities capabilities);

  /// @internal
  PiiVectorQuantizerOperation(Data* data);

  void setDistanceMeasure(const QString& name);
  QString distanceMeasure() const;

  void setDistanceMeasures(const QStringList& names);
  QStringList distanceMeasures() const;

  double rejectThreshold() const;
  void setRejectThreshold(double rejectThreshold);

  void setModels(const PiiVariant& models);
  PiiVariant models() const;

  void setDistanceWeights(const QVariantList& distanceWeights);
  QVariantList distanceWeights() const;

  void setDistanceCombinationMode(PiiClassification::DistanceCombinationMode distanceCombinationMode);
  PiiClassification::DistanceCombinationMode distanceCombinationMode() const;

  void setClassLabels(const QVariantList& labels);
  QVariantList classLabels() const;

  /**
   * Returns a pointer to the `boundary` input.
   */
  PiiInputSocket* boundaryInput();
  /**
   * Returns a pointer to the `vector` `index` output.
   */
  PiiOutputSocket* vectorIndexOutput();
  /**
   * Returns a pointer to the `distance` output.
   */
  PiiOutputSocket* distanceOutput();

  /**
   * Configures *classifier* for running. This function must be
   * called by a subclass' implementation of the check() function. 
   * This function configures *classifier* with the samples given as
   * the [models] property, and creates an instance of the requested
   * distance measure.
   *
   * @exception PiiExecutionException& if setting the model samples or
   * the distance measure fails.
   */
  template <class SampleSet> void check(PiiVectorQuantizer<SampleSet>& classifier, bool reset);
  /**
   * Creates an instance of *name* as `Measure`.
   *
   * @exception PiiExecutionException& if the resource database
   * doesn't contain the named resource, or it is not instantiable as
   * `Measure`.
   */
  template <class Measure> Measure* createDistanceMeasure(const QString& name);
  /**
   * Creates an instance of a distance measure as specified by the
   * [distanceMeasure] property. If there are many distance measures,
   * creates a PiiMultiFeatureDistance and appends all requrested
   * distance measures to it.
   *
   * @exception PiiExecutionException& if the distance measures cannot
   * be created.
   */
  template <class SampleSet> void setDistanceMeasure(PiiVectorQuantizer<SampleSet>& classifier);
  /**
   * Configures *classifier* so that its distance measure is aware of
   * multi-feature boundaries. Does nothing if the distance measure is
   * not a PiiMultiFeatureDistance or the `boundaries` input is not
   * connected.
   *
   * @exception PiiExecutionException& if the boundary input doesn't
   * contain a PiiMatrix<int>.
   */
  template <class SampleSet> void setFeatureBoundaries(PiiVectorQuantizer<SampleSet>& classifier);
  /**
   * Reads features from the `features` input, configures
   * multi-feature boundaries if needed, finds the closest match for
   * the features just read, and returns the index of the closest
   * vector in the model sample set. If [classLabels] is non-empty,
   * returns the class label corresponding to the index of the closest
   * sample. This function also sends the distance to the closest
   * sample and its index.
   *
   * @exception PiiExecutionException& if the features are of
   * incorrect type or size.
   */
  template <class SampleSet> double classify(PiiVectorQuantizer<SampleSet>& classifier,
                                             int* vectorIndex = 0, double* distance = 0);

  /**
   * Returns the class label corresponding to the sample at *index*. 
   * If the label list is empty, returns *index* (or `NaN` if 
   * *index* is -1). If there is no label for *index*, returns *NAN*.
   */
  double labelForIndex(int index) const;

  /**
   * Checks that the [models] variant is of correct type, and
   * reconfigures *classifier* with them.
   *
   * @exception PiiExecutionException& if the type of the variant is incorrect
   */
  template <class SampleSet> void setModels(PiiVectorQuantizer<SampleSet>& classifier);

private:
  void init();
};

#include "PiiVectorQuantizerOperation-templates.h"

#endif //_PIIVECTORQUANTIZEROPERATION_H
