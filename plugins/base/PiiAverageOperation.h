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

#ifndef _PIIAVERAGEOPERATION_H
#define _PIIAVERAGEOPERATION_H

#include <PiiDefaultOperation.h>

/**
 * Calculate the average of all values in a matrix.
 *
 * Inputs
 * ------
 *
 * @in matrix - any numeric or complex matrix
 *
 * Outputs
 * -------
 *
 * @out average - the mean value, either `double` or
 * `complex`<double>. If an average type is meanRows or meanColumns,
 * then output type is PiiMatrix<double> or PiiMatrix<complex<double> >.
 *
 */
class PiiAverageOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * CalculationMode is a type of the average calculation. Default is
   * MeanAll.
   */
  Q_PROPERTY(CalculationMode calculationMode READ calculationMode WRITE setCalculationMode);
  Q_ENUMS(CalculationMode);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiAverageOperation();

  /**
   * Calculation modes.
   *
   * - `MeanAll` - output the mean of all values (double or double
   * complex)
   *
   * - `MeanRows` - output the means of all rows (column matrix with
   * double or double complex data)
   *
   * - `MeanColumns` - output the means of all columns (row matrix
   * with double or double complex data)
   */
  enum CalculationMode
    {
      MeanAll,
      MeanRows,
      MeanColumns
    };

  void setCalculationMode(const CalculationMode& calculationMode);
  CalculationMode calculationMode() const;

protected:
  void process();

private:
  template <class T> void average(const PiiVariant& obj);
  template <class T> void complexAverage(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    CalculationMode calculationMode;
  };
  PII_D_FUNC;
};


#endif //_PIIAVERAGEOPERATION_H
