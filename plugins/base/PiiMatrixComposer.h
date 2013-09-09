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

#ifndef _PIIMATRIXCOMPOSER_H
#define _PIIMATRIXCOMPOSER_H

#include <PiiDefaultOperation.h>

/**
 * Composes an output matrix from N inputs of primitive data types. 
 * The amount of inputs is set with #dynamicInputCount.
 *
 * At least one dynamic input must be connected. The output matrix
 * will be generated once all the connected inputs have received an
 * object. If the @p sync input is connected, the output will be
 * emitted after all data has been received.
 *
 * The type of the output matrix is determined by the type of the
 * object in the first connected input.
 *
 * @inputs
 *
 * @in sync - an optional synchronization input. If this input is
 * connected, it is assumed that each synchronization object
 * corresponds to M objects in the other inputs, and the operation
 * will add M rows/columns to the result matrix before emitting it.
 *
 * @in inputX - inputX is element (0, X) in output matrix. Any
 * primitive type.
 *
 * @outputs
 *
 * @out output - composed matrix. The type of the matrix is determined
 * by the type of the first connected input. If type @p T is read, a
 * PiiMatrix<T> will be emitted. If #direction is @p
 * Pii::Horizontally, the matrix is M-by-#dynamicInputCount, where M
 * is the number of objects related to @p sync. If #direction is @p
 * Pii::Vertically, the matrix is #dynamicInputCount-by-M. If @p sync
 * is not connected, M is always one. If @p sync is connected and no
 * objects are received in the other inputs during one sync object, an
 * empty matrix (PiiMatrix<int>) will be emitted.
 *
 * @ingroup PiiBasePlugin
 */
class PiiMatrixComposer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of inputs. Defines also the amount of columns (or
   * rows, if #direction = @p Pii::Vertically) in the output matrix. 
   * The default is one.
   */
  Q_PROPERTY(int dynamicInputCount READ dynamicInputCount WRITE setDynamicInputCount);

  /**
   * The direction of the composition. @p Pii::Horizontally (the
   * default) creates #dynamicInputCount columns, @p Pii::Vertically
   * creates the same number of rows.
   */
  Q_PROPERTY(Pii::MatrixDirection direction READ direction WRITE setDirection);
  
  /**
   * A default value for each input as a @p double. The default value
   * will be used if the corresponding input is not connected. Default
   * values will be casted to the type of the output matrix. For
   * exampe if the type of the output matrix is PiiMatrix<int> and the
   * default value for an output is 1.1, the value is rounded to value
   * 1 in the matrix.
   *
   * If the number of default values is smaller than
   * #dynamicInputCount, extra zeroes will be automatically added at
   * the end of the list when the operation is started.
   *
   * @code
   * composer->setProperty("dynamicInputCount", 4);
   * composer->setProperty("defaultValues", QVariantList() << 1.0 << 2.0);
   * // ...
   * engine.execute();
   * // defaultValues is now (1.0, 2.0, 0.0, 0.0)
   * @endcode
   */
  Q_PROPERTY(QVariantList defaultValues READ defaultValues WRITE setDefaultValues);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiMatrixComposer();
  ~PiiMatrixComposer();
  
  void check(bool reset);
  
protected:
  void process();
  void syncEvent(SyncEvent* event);

  void setDynamicInputCount(int cnt);
  int dynamicInputCount() const;

  void setDirection(const Pii::MatrixDirection& direction);
  Pii::MatrixDirection direction() const;

  void setDefaultValues(const QVariantList& defaultValues);
  QVariantList defaultValues() const;

private:
  template <class T> void operate();
  template <class T> void emitMatrix();
  template <class T> void resize(PiiMatrix<T>& matrix);

  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    Pii::MatrixDirection direction;
    QVector<double> lstDefaultValues;
    QVector<bool> lstConnectedInputs;
    PiiInputSocket *pSyncInput, *pFirstConnectedInput;
    unsigned int uiPreviousType;
    PiiTypelessMatrix* pMatResult;
  };
  PII_D_FUNC;
};

#endif //_PIIMATRIXCOMPOSER_H
