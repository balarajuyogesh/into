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

#ifndef _PIIQUANTIZEROPERATION_H
#define _PIIQUANTIZEROPERATION_H

#include <PiiDefaultOperation.h>
#include "PiiQuantizer.h"

/**
 * Quantizes any numeric matrix to discrete levels.
 *
 * @inputs
 *
 * @in image - input image. Any numeric matrix.
 * 
 * @outputs
 *
 * @out image - input image quantized to discrete levels. 
 * PiiMatrix<int>.
 *
 * @ingroup PiiImagePlugin
 */
class PiiQuantizerOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of quantization levels (for training). This value has
   * no effect is limits are manually set. If limits are learned, this
   * value specifies the number of quantization levels. The default is
   * 16.
   */
  Q_PROPERTY(int levels READ levels WRITE setLevels);

  /**
   * Quantization limits. Limits must be represented as a
   * monotonically increasing series of double values. The number of
   * quantization levels equals to limit.size() + 1. See PiiQuantizer
   * for details.
   */
  Q_PROPERTY(QVariantList limits READ limits WRITE setLimits);

  /**
   * training description
   */
  Q_PROPERTY(bool training READ training WRITE setTraining);
  
  /**
   * trainingPixels description
   */
  Q_PROPERTY(int trainingPixels READ trainingPixels WRITE setTrainingPixels);
  
  /**
   * selectionProbability description
   */
  Q_PROPERTY(double selectionProbability READ selectionProbability WRITE setSelectionProbability);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiQuantizerOperation();
  ~PiiQuantizerOperation();
  
  void setLevels(int levels);
  int levels() const;

  void setLimits(const QVariantList& limits);
  QVariantList limits() const;

  void setTraining(bool training);
  bool training() const;

  void setTrainingPixels(int trainingPixels);
  int trainingPixels() const;

  void setSelectionProbability(double selectionProbability);
  double selectionProbability() const;

  void check(bool reset);

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    int iLevels;
    bool bTraining;
    int iTrainingPixels, iCollectionIndex;
    double dSelectionProbability;
    PiiQuantizer<double> quantizer;
    double* pCollectedData;
  };
  PII_D_FUNC;
  
  template <class T> void quantize(const PiiVariant& obj);
  template <class T, class U> void quantize(const PiiMatrix<U>& img);
  void learnBoundaries();
};


#endif //_PIIQUANTIZEROPERATION_H
