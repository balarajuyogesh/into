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

#ifndef _PIISTRINGIZER_H
#define _PIISTRINGIZER_H

#include <PiiDefaultOperation.h>

/**
 * Convert (almost) any type to a string.
 *
 * Inputs
 * ------
 *
 * @in input - any number or a matrix containing numbers
 * 
 * Outputs
 * -------
 *
 * @out output - the input value as a string. Matrices are represented
 * without any punctuation, as shown in the example below.
 *
 * ~~~
 * 0 1 2
 * 3 4 5
 * ~~~
 */
class PiiStringizer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Column separator for matrices. The default value is " ".
   */
  Q_PROPERTY(QString columnSeparator READ columnSeparator WRITE setColumnSeparator);

  /**
   * Row separator for matrices. Default value is "\n".
   */
  Q_PROPERTY(QString rowSeparator READ rowSeparator WRITE setRowSeparator);

  /**
   * This will be prepended to the output string. The default value is
   * an empty string.
   */
  Q_PROPERTY(QString startDelimiter READ startDelimiter WRITE setStartDelimiter);

  /**
   * This will be appended to the end of the string. The default value
   * is an empty string.
   */
  Q_PROPERTY(QString endDelimiter READ endDelimiter WRITE setEndDelimiter);
  
  /**
   * The maximum number of digits after the decimal point shown in a
   * floating-point number. The default value is 2.
   */
  Q_PROPERTY(int precision READ precision WRITE setPrecision);  

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiStringizer();

  void setColumnSeparator(const QString& columnSeparator);
  QString columnSeparator() const;
  
  void setRowSeparator(const QString& rowSeparator);
  QString rowSeparator() const;
  
  void setPrecision(int precision);
  int precision() const;
  
  void setStartDelimiter(const QString& startDelimiter);
  QString startDelimiter() const;
  
  void setEndDelimiter(const QString& endDelimiter);
  QString endDelimiter() const;
  
protected:
  void process();

private:
  template <class T> QString intToString(const PiiVariant& obj);
  template <class T> QString floatToString(const PiiVariant& obj);
  template <class T> QString matrixToString(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    QString strColumnSeparator;
    QString strRowSeparator;
    QString strStartDelimiter;
    QString strEndDelimiter;
    int iPrecision;
  };
  PII_D_FUNC;
};


#endif //_PIISTRINGIZER_H
