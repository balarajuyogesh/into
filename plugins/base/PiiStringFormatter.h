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

#ifndef _PIISTRINGFORMATTER_H
#define _PIISTRINGFORMATTER_H

#include <PiiDefaultOperation.h>

/**
 * An operation that composes formatted strings out of data read from
 * a configurable number of inputs.
 *
 * Inputs
 * ------
 *
 * @in inputX - any type convertable to a QString. All primitive types
 * and QString are accepted.
 *
 * Outputs
 * -------
 *
 * @out output - the output
 *
 */
class PiiStringFormatter : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Format string. Use "%0", "%1", etc. to insert the data read from
   * `input0`, `input1`, etc. The default value is "%0". Setting
   * this property automatically changes the number of input sockets
   * to match the number of distinct argument places in the format
   * string. For example "%0 %3 %0" as the format string adds two
   * inputs to the operation. If the number of argument places is
   * zero, one input will still be retained.
   *
   * ~~~(c++)
   * PiiOperation* op = engine.createOperation("PiiStringFormatter");
   * // This will add two inputs to the operation.
   * op->setProperty("format", "%0 (%1)");
   *
   * otherOp->connectOutput("int output", op, "input0");
   * otherOp2->connectOutput("bool output", op, "input1");
   *
   * // output now emits something like
   * // 0 (false)
   * // 1 (true)
   * // 2 (false)
   * ~~~
   */
  Q_PROPERTY(QString format READ format WRITE setFormat);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiStringFormatter();

  void setFormat(const QString& format);
  QString format() const;
  
protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    QString strFormat;
  };
  PII_D_FUNC;
};


#endif //_PIISTRINGFORMATTER_H
