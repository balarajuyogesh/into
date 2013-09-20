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

#ifndef _PIIGRAYFEATUREOPERATION_H
#define _PIIGRAYFEATUREOPERATION_H

#include <PiiDefaultOperation.h>

#define PII_GRAY_FEATURE_OPERATION_FEATURECNT 9

/**
 * Extracts a configurable set of simple gray-level features from
 * images.
 *
 * Inputs
 * ------
 *
 * @in image - input image. Any gray-level image.
 * 
 * Outputs
 * -------
 *
 * @out features - extracted features. (1xN PiiMatrix<float>)
 *
 */
class PiiGrayFeatureOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Enabled features. List here all features you want to enable. 
   * `Average`, `Minimum`, and `Maximum` are enabled by default. If
   * the list is empty, `Minimum` will be used.
   *
   * - `Minimum` - minimum gray level
   * - `Maximum` - maximum gray level
   * - `Average` - average gray level
   * - `Contrast` - difference between maximum and minimum
   * - `MaxDiff` - maximum absolute difference to the reference value
   * ([reference]).
   * - `Variance` - variance
   * - `Deviation` - standard deviation
   * - `HighAverage` - average of gray levels over the mean
   * - `LowAverage` - average of gray levels below the mean
   *
   * ~~~(c++)
   * pGrayFeatures->setProperty("features", QStringList() << "Average" << "Contrast");
   * ~~~
   *
   * Independent of the order of feature names in this list, the order
   * of features in the feature vector will be that listed above. If
   * the list contains "MaxDiff" and "Minimum", in this order, the
   * order of features in the resulting feature vector will be just
   * the opposite.
   */
  Q_PROPERTY(QStringList features READ features WRITE setFeatures);

  /**
   * Reference gray value. Maximum absolute difference is calculated
   * by subtracting this value from each pixel. The default value is
   * 127.
   */
  Q_PROPERTY(double reference READ reference WRITE setReference);
  
  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  PiiGrayFeatureOperation();
  ~PiiGrayFeatureOperation();
  
  void setFeatures(const QStringList& features);
  QStringList features() const;

  void setReference(double reference);
  double reference() const;

  QList<int> enabledFeatures() const;
  
  void check(bool reset);

protected:
  void process();

private:
  class FeatureExtractor;
  class MinimumExtractor;
  class MaximumExtractor;
  class MinMaxExtractor;
  class AverageExtractor;
  class MinAvgExtractor;
  class MaxAvgExtractor;
  class MinMaxAvgExtractor;
  class DefaultExtractor;
  friend class DefaultExtractor;
  
  void setExtractor(FeatureExtractor* extractor);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    double dReference;
    QList<int> lstEnabledFeatures;
    int iFeatureMask;
    FeatureExtractor* pExtractor;
  };
  PII_D_FUNC;

  static const char* _pFeatureNames[PII_GRAY_FEATURE_OPERATION_FEATURECNT];
};

#endif //_PIIGRAYFEATUREOPERATION_H
