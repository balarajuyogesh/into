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

#ifndef _PIIDEBUGOPERATION_H
#define _PIIDEBUGOPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiOneInputFlowController.h>

/**
 * An operation that prints all objects passing through it to the
 * debug console. It is useful in debugging connections.
 *
 * Inputs
 * ------
 *
 * @in input - any object
 *
 * @input
 *
 * @out output - the object read from `input`
 *
 */
class PiiDebugOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Where to write the debug output? The default is `StdOut`.
   */
  Q_PROPERTY(OutputStream outputStream READ outputStream WRITE setOutputStream);
  Q_ENUMS(OutputStream);

  /**
   * Output format. The format may contain one or more variables.
   * Recognized variables are:
   *
   * - $count - the number of objects received since reset
   * - $type - the type ID of the object in hexadecimal
   * - $objectName - the objectName of the operation
   * - $value - the value of the object. The operation recognizes
   * primitive types, strings and matrices.
   * - $symbol - a one-character symbol for the object. Normal,
   * processable objects are denoted with a dot (.), and
   * synchronization primitives with the following symbols: < = start,
   * > = end, S = stop, P = pause, R = resume.
   *
   * The default format is "$objectName: 0x$type received ($count since
   * reset)\n".
   */
  Q_PROPERTY(QString format READ format WRITE setFormat);

  /**
   * Enables/disables printing of control objects. The default is
   * `false`.
   */
  Q_PROPERTY(bool showControlObjects READ showControlObjects WRITE setShowControlObjects);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Output streams.
   *
   * - `StdOut` - standard output
   * - `StdErr` - standard error
   * - `Debug` - piiDebug() output. Note that in this mode newlines
   * will be automatically appended and you may need to change [format]
   * accordingly.
   */
  enum OutputStream { StdOut, StdErr, Debug };

  PiiDebugOperation();

  void check(bool reset);

protected:
  void process();

  PiiFlowController* createFlowController();

  void setOutputStream(OutputStream outputStream);
  OutputStream outputStream() const;
  void setFormat(const QString& format);
  QString format() const;
  void setShowControlObjects(bool showControlObjects);
  bool showControlObjects() const;

private:
  class Controller : public PiiOneInputFlowController
  {
  public:
    Controller(PiiDebugOperation* parent) :
      PiiOneInputFlowController(parent->inputAt(0), parent->outputSockets()),
      _pParent(parent)
    {}

    FlowState prepareProcess()
    {
      PII_D;
      if (PiiYdin::isControlType(d->pInput->queuedType(0)))
        _pParent->printControlObject(d->pInput->queuedObject(0));
      return PiiOneInputFlowController::prepareProcess();
    }
  private:
    PiiDebugOperation* _pParent;
  };

  friend class Controller;

  template <class T> QString printMatrix(const PiiVariant& obj);
  void printControlObject(const PiiVariant& obj);
  void print();

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    int iCnt;
    QString strFormat;
    OutputStream outputStream;
    QMap<QString,QString> mapVariables;
    bool bShowControlObjects;
  };
  PII_D_FUNC;
};

#endif //_PIIDEBUGOPERATION_H
