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

#ifndef _PIILOOKUPTABLE_H
#define _PIILOOKUPTABLE_H

#include <PiiDefaultOperation.h>

/**
 * An operation that maps integers into arbitrary data.
 *
 * Inputs
 * ------
 *
 * @in index - a zero-based index into the look-up table. If there is
 * no [default value](defaultValue), overflows and underflows will
 * cause a run-time exception. Any primitive type is be accepted.
 *
 * Outputs
 * -------
 *
 * @out outputX - any number of outputs that emit arbitrary data. For
 * each incoming `index`, the corresponding look-up table entry will
 * be emitted.
 *
 */
class PiiLookupTable : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The look-up table. The look-up table is represented as a list of
   * variants. Each entry may be either a PiiVariant or a
   * QVariantList. In the first case, `table[index]` will be
   * emitted through all outputs whenever `index` is received. If the
   * list elements are QVariantLists, `table[0][index]` will be
   * emitted through `output0` etc.
   *
   * ~~~(c++)
   * PiiOperation* lut = engine.createOperation("PiiLookupTable");
   * // Map 0 to true and 1 to false. Emit the boolean value through all outputs.
   * lut->setProperty("table", QVariantList() << Pii::createQVariant(true) << Pii::createQVariant(false));
   *
   * QVariantList tbl;
   * // Map 0 to 0 and 1 to -1 in output0
   * tbl.append(QVariantList() << Pii::createQVariant(0) << Pii::createQVariant(-1));
   * // Map 0 to "zero" and 1 to "one" in output1
   * tbl.append(QVariantList() << Pii::createQVariant(QString("zero")) << Pii::createQVariant(QString("one")));
   * lut->setProperty("table", tbl);
   * ~~~
   *
   * If the number of look-up lists is less than the number of outputs
   * (see [outputCount]), the last valid value will be used for all
   * extra outputs. If multiple lists are provided, each must be of
   * the same length. Invalid lists will be ignored. If list values
   * are not PiiVariants, they will be substituted with zeros. This
   * will most likely cause a run-time exception, so be careful.
   *
   * By default, the look-up table contains nothing.
   */
  Q_PROPERTY(QVariantList table READ table WRITE setTable);

  /**
   * The number of outputs. Must be greater than zero. The default
   * value is one.
   */
  Q_PROPERTY(int dynamicOutputCount READ dynamicOutputCount WRITE setDynamicOutputCount);

  /**
   * Default value to be used for index over/underflows. If there is
   * no look-up table entry for an input index, this value will be
   * substituted. If the default value is not set and an
   * over/underflow occurs, a run-time exception will be generated.
   */
  Q_PROPERTY(PiiVariant defaultValue READ defaultValue WRITE setDefaultValue);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiLookupTable();

  void check(bool reset);

  void setTable(const QVariantList& table);
  QVariantList table() const;
  void setDynamicOutputCount(int count);
  int dynamicOutputCount() const;
  void setDefaultValue(const PiiVariant& defaultValue);
  PiiVariant defaultValue() const;

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    // A look-up list for all outputs
    QList<QList<PiiVariant> > lstOutputValues;
    // The actual property value
    QVariantList lstTable;
    // Last list index that overflows neither look-up list
    // (lstOutputValues) nor output list.
    int iMaxTableIndex;
    // Last index that doesn't overflow the look-up table itself
    // (lstOutputValues[0])
    int iMaxLookupIndex;
    PiiVariant varDefaultValue;
  };
  PII_D_FUNC;
};

#endif //_PIILOOKUPTABLE_H
