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

#ifndef _PIIIDGENERATOR_H
#define _PIIIDGENERATOR_H

#include <PiiDefaultOperation.h>
#include <QString>

/**
 * On operation for generating running IDs. The operation can be used
 * for example for generating file names. The generated id has a
 * format [PREFIX]INDEX[SUFFIX], where [PREFIX] is a string set by the
 * property [prefix], INDEX is a running index converted to string
 * generated internally in the operation and [SUFFIX] is a string set
 * by the property [suffix]. `INDEX` has at least the width set by
 * [indexWidth]. The extra zeroes are added to fullfill this
 * requirement. The default value for the width is 6. For example if
 * the value of the property [prefix] were "image", value of the
 * property [suffix] ".jpg" and the current value of the internal
 * index is 11, the generated id will be "image000011.jpg"
 * 
 *
 * Inputs
 * ------
 *
 * @in trigger - any object. Informs that a new id must be generated.
 *
 * Outputs
 * -------
 *
 * @out id - generated id as a string having a format [PREFIX]INDEX[SUFFIX] 
 *
 */

class PiiIdGenerator : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Prefix for the id.
   */
  Q_PROPERTY(QString prefix READ prefix WRITE setPrefix);

  /**
   * Suffix for the id.
   */
  Q_PROPERTY(QString suffix READ suffix WRITE setSuffix);

  /**
   * Defines the next index for the id. The default value is zero. The
   * value is not zeroed, when the operation is interrupted, and re-executed.
   * executing the operation after it has been interrupted, doesn't
   * zero it.
   */
  Q_PROPERTY(int nextIndex READ nextIndex WRITE setNextIndex);

  /**
   * Defines the minimum width for the INDEX section of the id. Extra
   * zeroes  are added before other numbers in order to fullfill the
   * minimum width requirement. If for example the value of 
   * `indexWidth` were 4, and the value of the current index were 11,
   * the value of the property [prefix] "image" and value of the
   * property [suffix] ".jpg" the generated id would be
   * "image0011.jpg". The default value is 6. If the value is zero, no
   * extra zeroes are added in the index section. The negative values
   * can cause indeterminate results and should be avoided.
   */
  Q_PROPERTY(int indexWidth READ indexWidth WRITE setIndexWidth);
 
  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  PiiIdGenerator();

  void setPrefix(const QString& prefix);
  QString prefix() const;
  void setSuffix(const QString& suffix);
  QString suffix() const;
  void setNextIndex(int nextIndex);
  int nextIndex() const;
  void setIndexWidth(int indexWidth);
  int indexWidth() const;
  
protected:
  void process();
  void check(bool reset);
  
private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    QString strPrefix;
    QString strSuffix;
    int iNextIndex;
    PiiInputSocket *pTriggerInput;
    PiiOutputSocket *pIdOutput;
    int iIndexWidth;
  };
  PII_D_FUNC;  
};

#endif //_PIIIDGENERATOR_H
