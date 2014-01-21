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

#ifndef _PIIMATRIXNORMALIZER_H
#define _PIIMATRIXNORMALIZER_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>

/**
 * Scale the values in a matrix so that its values are limited to a
 * predefined range.
 *
 * Inputs
 * ------
 *
 * @in input - the input matrix. Any numeric matrix.
 *
 * Outputs
 * -------
 *
 * @out output - the output matrix. The output type is determined by
 * the [outputType] property.
 *
 */
class PiiMatrixNormalizer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The minimum value in the output matrix. Used in
   * `NormalizeMinMax` mode. Default is zero.
   */
  Q_PROPERTY(double min READ min WRITE setMin);
  /**
   * The minimum value in the output matrix. Used in
   * `NormalizeMinMax` mode. Default is one.
   */
  Q_PROPERTY(double max READ max WRITE setMax);
  /**
   * Mean of the output matrix. Used in `NormalizeMeanVar` mode.
   * Default is zero.
   */
  Q_PROPERTY(double mean READ mean WRITE setMean);
  /**
   * Variance of the output matrix. Used in `NormalizeMeanVar` mode
   * if non-zero. If `variance` is set to zero, only the mean value
   * will be changed. Default is one.
   */
  Q_PROPERTY(double variance READ variance WRITE setVariance);
  /**
   * The output type. See [PiiYdin::MatrixTypeId] for valid values.
   * Only numeric matrix types are allowed. The default is
   * `PiiYdin::DoubleMatrixType`.
   */
  Q_PROPERTY(int outputType READ outputType WRITE setOutputType);

  /**
   * Normalization mode. Default is `NormalizeMinMax`.
   */
  Q_PROPERTY(NormalizationMode normalizationMode READ normalizationMode WRITE setNormalizationMode);
  Q_ENUMS(NormalizationMode);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Normalization modes.
   *
   * - `NormalizeMinMax` - scale the input matrix to fixed minimum
   * and maximum values.
   *
   * - `NormalizeMeanVar` - scale the input matrix to fixed mean and
   * variance.
   */
  enum NormalizationMode { NormalizeMinMax, NormalizeMeanVar };

  PiiMatrixNormalizer();

  void check(bool reset);

  void setMin(double min);
  double min() const;
  void setMax(double max);
  double max() const;
  void setOutputType(int outputType);
  int outputType() const;
  void setMean(double mean);
  double mean() const;
  void setVariance(double variance);
  double variance() const;
  void setNormalizationMode(NormalizationMode normalizationMode);
  NormalizationMode normalizationMode() const;

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    double dMin;
    double dMax;
    double dMean;
    double dVariance;
    int iOutputType;
    NormalizationMode normalizationMode;
  };
  PII_D_FUNC;

  template <class T> void normalize(const PiiVariant& obj);
  template <class T> PiiMatrix<double> normalizeAs(const PiiMatrix<T>& matrix,
                                                   double preShift,
                                                   double scale,
                                                   double postShift);

  template <class T> inline void emitMatrix(PiiMatrix<double>& matrix)
  {
    emitObject(PiiMatrix<T>(matrix));
  }

  void throwOutputTypeError()
  {
    PII_THROW(PiiExecutionException, tr("Only numeric matrices are allowed as the output type."));
  }

};

#endif //_PIIMATRIXNORMALIZER_H
