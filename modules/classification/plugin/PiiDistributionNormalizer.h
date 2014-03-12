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

#ifndef _PIIDISTRIBUTIONNORMALIZER_H
#define _PIIDISTRIBUTIONNORMALIZER_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>

/**
 * An operation that divides incoming feature vectors by their sum.
 * This allows one to use differently scaled distributions in
 * classifying statistical features.
 *
 * Inputs
 * ------
 *
 * @in features - a feature vector. Any numeric matrix.
 *
 * @in boundaries - an optional input that marks the boundaries of
 * multiple feature vectors in a compound feature vector. This input
 * is mostly used in conjunction with PiiFeatureCombiner. If this
 * input is connected, each part of the compound feature vector will
 * be separately normalized.
 *
 * Outputs
 * -------
 *
 * @out features - a normalized feature vector. PiiMatrix<double> or
 * PiiMatrix<float>, depending on the `doubleMode` property. If the
 * sum of the input vector is zero, it will not be normalized.
 *
 */
class PiiDistributionNormalizer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * A flag that determines the format of output vectors. The
   * operation always outputs floating-point vectors. By default, the
   * values are floats. If this flag is true, double accuracy is used.
   */
  Q_PROPERTY(bool doubleMode READ doubleMode WRITE setDoubleMode);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiDistributionNormalizer();

  bool doubleMode() const;
  void setDoubleMode(bool mode);

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    bool bDoubleMode;

    PiiInputSocket* pFeatureInput, *pBoundaryInput;
    PiiOutputSocket* pFeatureOutput;
  };
  PII_D_FUNC;

  template <class T> void normalize(const PiiVariant& obj);
  template <class U, class T> void normalizePieces(const PiiMatrix<T>& vector, const PiiMatrix<int>& boundaries)
;
};

#endif //_PIIDISTRIBUTIONNORMALIZER_H
