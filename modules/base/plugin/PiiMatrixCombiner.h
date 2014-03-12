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

#ifndef _PIIMATRIXCOMBINER_H
#define _PIIMATRIXCOMBINER_H

#include <PiiDefaultOperation.h>
#include <QSize>

/**
 * An operation that puts multiple matrices into a grid that produces
 * a larger matrix.
 *
 * Inputs
 * ------
 *
 * @in inputX - any number of input matrices. X is a zero-based index.
 *
 * Outputs
 * -------
 *
 * @out compound - a compound matrix on which the input matrices are
 * placed as denoted by the [rows] an [columns] properties.
 *
 */
class PiiMatrixCombiner : public PiiDefaultOperation
{
  Q_OBJECT
  /**
   * The number of rows in the grid. If the number of rows is limited
   * (non-zero), the grid will grow horizontally. Default is 1.
   */
  Q_PROPERTY(int rows READ rows WRITE setRows);
  /**
   * The number of columns in the grid. If the number of columns is
   * limited (non-zero), the grid will grow vertically. Default is 0.
   */
  Q_PROPERTY(int columns READ columns WRITE setColumns);
  /**
   * The number of input sockets. Default is 2.
   */
  Q_PROPERTY(int dynamicInputCount READ dynamicInputCount WRITE setDynamicInputCount);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiMatrixCombiner();

  void setRows(int rows);
  int rows() const;
  void setColumns(int columns);
  int columns() const;
  void setDynamicInputCount(int count);
  int dynamicInputCount() const;

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    int iRows;
    int iColumns;
  };
  PII_D_FUNC;

  template <class T> QSize matrixSize(const PiiVariant& obj);
  template <class T> void buildCompound(QSize maxSize);

  struct PrimitiveBuilder;
  struct ColorBuilder;
  friend struct PrimitiveBuilder;
  friend struct ColorBuilder;
};


#endif //_PIIMATRIXCOMBINER_H
