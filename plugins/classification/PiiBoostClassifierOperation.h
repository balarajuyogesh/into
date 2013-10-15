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

#ifndef _PIIBOOSTCLASSIFIEROPERATION_H
#define _PIIBOOSTCLASSIFIEROPERATION_H

#include "PiiClassifierOperation.h"
#include "PiiBoostClassifier.h"
#include "PiiDefaultClassifierFactory.h"
#include "PiiDecisionStump.h"
#include "PiiSampleSetCollector.h"

/**
 * An operation that classifies samples using a boosted cascade of
 * weak classifiers. This operation supports all boosting algorithms
 * supported by PiiBoostClassifier. This operation always uses
 * PiiDecisionStump as the weak classifier. See PiiClassifierOperation
 * and PiiBoostClassifier for details.
 *
 */
class PiiBoostClassifierOperation : public PiiClassifierOperation
{
  Q_OBJECT

  /**
   * The boosting algorithm to use. Default is `RealBoost`.
   */
  Q_PROPERTY(PiiClassification::BoostingAlgorithm algorithm READ algorithm WRITE setAlgorithm);

  /**
   * The maximum number of weak classifiers to add to the classifier
   * tree. Generally, it is a good idea to keep this value lower than
   * the number of features. The default is 100.
   */
  Q_PROPERTY(int maxClassifiers READ maxClassifiers WRITE setMaxClassifiers);

  /**
   * Minimum trainining error. See PiiBoostClassifier for details. The
   * default value is 0.
   */
  Q_PROPERTY(double minError READ minError WRITE setMinError);

public:
  template <class SampleSet> class Template;

  ~PiiBoostClassifierOperation();
  
protected:
  /// @internal
  class Data : public PiiClassifierOperation::Data
  {
  public:
    Data();
    PiiClassification::BoostingAlgorithm algorithm;
    int iMaxClassifiers;
    double dMinError;
  };
  PII_D_FUNC;
  /// @internal
  PiiBoostClassifierOperation(Data* d);

  void setAlgorithm(PiiClassification::BoostingAlgorithm algorithm);
  PiiClassification::BoostingAlgorithm algorithm() const;
  void setMaxClassifiers(int maxClassifiers);
  int maxClassifiers() const;
  void setMinError(double minError);
  double minError() const;
};

template <class T> struct MsvcHack
{
  typedef T Type;
};


/// @internal
template <class SampleSet> class PiiBoostClassifierOperation::Template :
  public PiiBoostClassifierOperation,
  public PiiDefaultClassifierFactory<PiiDecisionStump<SampleSet> >
{
  friend struct PiiSerialization::Accessor;
  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION;
  template <class Archive> void serialize(Archive& archive, const unsigned int)
  {
    PII_D;
    PII_SERIALIZE_BASE(archive, PiiOperation);
    PiiSerialization::serializeProperties(archive, *this);
    if (Archive::InputArchive)
      delete d->pClassifier;
    archive & PII_NVP("classifier", d->pClassifier);
  }

public:
  Template();
  
  int bufferedSampleCount() const;
  int featureCount() const;

protected:
  void check(bool reset);
  double classify();
  bool learnBatch();
  void collectSample(double label, double weight);
  void replaceClassifier();
  void resizeBatch(int newSize);
  void resetClassifier();
  
private:
  /// @internal
  class Data : public PiiBoostClassifierOperation::Data
  {
  public:
    Data();
    ~Data();
    PiiBoostClassifier<SampleSet> *pClassifier, *pNewClassifier;
    PiiSampleSetCollector<SampleSet> collector;
  };
  PII_D_FUNC;
 
  // Had to put this inline due to a MSVC bug
  PiiBoostClassifier<SampleSet>* createClassifier()
  {
    PII_D;
    PiiBoostClassifier<SampleSet>* pClassifier = new PiiBoostClassifier<SampleSet>(this, d->algorithm);
    pClassifier->setMaxClassifiers(d->iMaxClassifiers);
    pClassifier->setMinError(d->dMinError);
    return pClassifier;
  }
};

template <class SampleSet> PiiBoostClassifierOperation::Template<SampleSet>::Data::Data() :
  pClassifier(0),
  pNewClassifier(0)
{
}

template <class SampleSet> PiiBoostClassifierOperation::Template<SampleSet>::Data::~Data()
{
  delete pClassifier;
  delete pNewClassifier;
}

template <class SampleSet> PiiBoostClassifierOperation::Template<SampleSet>::Template() :
  PiiBoostClassifierOperation(new Data)
{
  _d()->pClassifier = createClassifier();
}

template <class SampleSet> void PiiBoostClassifierOperation::Template<SampleSet>::check(bool reset)
{
  PiiBoostClassifierOperation::check(reset);
  PII_D;
  // PENDING what if weight is connected later?
  d->collector.setCollectWeights(this->weightInput()->isConnected());
  d->collector.setBatchSize(learningBatchSize());
  d->collector.setFullBufferBehavior(fullBufferBehavior());
}

template <class SampleSet> int PiiBoostClassifierOperation::Template<SampleSet>::bufferedSampleCount() const
{
  return _d()->collector.sampleCount();
}

template <class SampleSet> int PiiBoostClassifierOperation::Template<SampleSet>::featureCount() const
{
  const PII_D;
  return qMax(d->pClassifier->featureCount(), d->collector.featureCount());
}

template <class SampleSet> double PiiBoostClassifierOperation::Template<SampleSet>::classify()
{
  double dLabel = PiiClassifierOperation::classify(*_d()->pClassifier);
  classificationOutput()->emitObject(dLabel);
  return dLabel;
}

template <class SampleSet>
void PiiBoostClassifierOperation::Template<SampleSet>::collectSample(double label, double weight)
{
  PII_D;
  PiiClassifierOperation::learnOne(d->collector, label, weight);
}

template <class SampleSet> bool PiiBoostClassifierOperation::Template<SampleSet>::learnBatch()
{
  PII_D;
  d->pNewClassifier = createClassifier();
  bool bSuccess = PiiClassifierOperation::learnBatch(*d->pNewClassifier,
                                                     *d->collector.samples(),
                                                     *d->collector.classLabels(),
                                                     *d->collector.sampleWeights());
  if (!bSuccess)
    {
      delete d->pNewClassifier;
      d->pNewClassifier = 0;
    }
  
  return bSuccess;
}

template <class SampleSet> void PiiBoostClassifierOperation::Template<SampleSet>::replaceClassifier()
{
  PII_D;
  delete d->pClassifier;
  d->pClassifier = d->pNewClassifier;
  d->pNewClassifier = 0;
}

template <class SampleSet> void PiiBoostClassifierOperation::Template<SampleSet>::resetClassifier()
{
  PII_D;
  delete d->pClassifier;
  d->pClassifier = createClassifier();
}

template <class SampleSet> void PiiBoostClassifierOperation::Template<SampleSet>::resizeBatch(int newSize)
{
  _d()->collector.resize(newSize);
}

#endif //_PIIBOOSTCLASSIFIEROPERATION_H
