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

#ifndef _PIICLASSINFOMAPPER_H
#define _PIICLASSINFOMAPPER_H

#include <PiiDefaultOperation.h>

/**
 * An operation that maps class indices to arbitrary data. Useful when
 * classification results need to be coupled with, say, image file
 * names. The [training] mode, the operation collects class indices and
 * the corresponding data into a look-up table. In normal operation,
 * the data corresponding to the input class index will be emitted.
 *
 * Inputs
 * ------
 *
 * @in class index - class index.
 *
 * @in data - data to be coupled with the class index. Optional if
 * [training] mode is `false`.
 *
 * Outputs
 * -------
 *
 * @out data - the data that is attached to the incoming class index.
 * If there is no data for the incoming class or the operation is in
 * training mode, the default value will be emitted.
 *
 */
class PiiClassInfoMapper : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * If `true`, the operation collects incoming data. If `false`,
   * the collected data is used as output.
   */
  Q_PROPERTY(bool training READ training WRITE setTraining);

  /**
   * Maximum number of classes allowed. Default value is 1024. The
   * operation will store data for class indices ranging from 0 to
   * maxClasses-1.
   */
  Q_PROPERTY(int maxClasses READ maxClasses WRITE setMaxClasses);

  /**
   * Default value for classes that we haven't learned yet. The
   * default value is `null`. Therefore, one must almost always set
   * the default value before running the operation.
   */
  Q_PROPERTY(PiiVariant defaultValue READ defaultValue WRITE setDefaultValue);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiClassInfoMapper();

  void setTraining(bool training);
  bool training() const;

  void setMaxClasses(int maxClasses);
  int maxClasses() const;

  void setDefaultValue(const PiiVariant& defaultValue);
  PiiVariant defaultValue() const;

  void check(bool reset);

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    bool bTraining;
    int iMaxClasses;
    PiiVariant defaultValue;
    QList<PiiVariant> lstData;
    PiiInputSocket* pIndexInput, *pDataInput;
  };
  PII_D_FUNC;
};


#endif //_PIICLASSINFOMAPPER_H
