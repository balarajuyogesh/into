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

#ifndef _PIISAMPLERANDOMIZER_H
#define _PIISAMPLERANDOMIZER_H

#include <PiiDefaultOperation.h>

/**
 * An operation that stores names of samples belonging to N different
 * classes. On each iteration it randomly selects a sample from one of
 * the classes and outputs its name and class index.
 *
 * Inputs
 * ------
 *
 * @in trigger - an optional trigger input. (any)
 * 
 * Outputs
 * -------
 *
 * @out name - the name of a randomly selected sample (QString)
 *
 * @out label - the class label of the sample (int)
 *
 */
class PiiSampleRandomizer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The names of samples for each class. This list should hold one to
   * N-1 entries, each of which represents the names of samples that
   * belong to the class at that index. Class names are stored as a
   * QStringList.
   *
   * ~~~(c++)
   * randomizer->setProperty("sampleNames", QVariantList()
   *                         << (QStringList() << "Orange1" << "Orange2" << "Orange3")
   *                         << (QStringList() << "Apple1" << "Apple2"));
   * ~~~
   */
  Q_PROPERTY(QVariantList sampleNames READ sampleNames WRITE setSampleNames);
  
  /**
   * Relative weights for each of the classes. The random selection of
   * each class will be weighted according to its relative weight in
   * this list. To set class 1 twice as likely as class 0, do this:
   *
   * ~~~(c++)
   * randomizer->setProperty("classWeights", QVariantList() << 1.0 << 2.0);
   * ~~~
   *
   * If `classWeights` is an empty list, the operation goes
   * sequentially through all classes and emits one sample from each.
   */
  Q_PROPERTY(QVariantList classWeights READ classWeights WRITE setClassWeights);

  /**
   * Set this property to true if you want the *sample* selection to
   * be random. The operation still selects the class randomly, but
   * goes sequentially through the sample names by default.
   */
  Q_PROPERTY(bool randomSampling READ randomSampling WRITE setRandomSampling);

  /**
   * The maximum number of sample names the source will emit. Zero
   * means eternally. This property is ineffective if `trigger` is
   * connected. The default is zero.
   */
  Q_PROPERTY(int maxSamples READ maxSamples WRITE setMaxSamples);

  /**
   * The zero-based index of the next sample to be emitted.
   */
  Q_PROPERTY(int currentSampleIndex READ currentSampleIndex);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiSampleRandomizer();

  void setSampleNames(const QVariantList& sampleNames);
  QVariantList sampleNames() const;

  void setClassWeights(const QVariantList& classWeights);
  QVariantList classWeights() const;

  void setRandomSampling(bool randomSampling);
  bool randomSampling() const;

  void setMaxSamples(int maxSamples);
  int maxSamples() const;

  int currentSampleIndex() const;

protected:
  void process();
  void check(bool reset);

private:
  void emitFromClass(int classIndex);
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    QList<QStringList> lstSampleNames;
    QList<double> lstClassWeights;
    QVector<double> lstCumulativeWeights;
    QVector<int> lstSampleIndices;
    int iClassIndex;
    int iMaxSamples;
    int iCurrentSampleIndex;
    bool bRandomSampling;
  };
  PII_D_FUNC;
};


#endif //_PIISAMPLERANDOMIZER_H
