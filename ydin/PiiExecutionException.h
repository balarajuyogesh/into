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

#ifndef _PIIEXECUTIONEXCEPTION_H
#define _PIIEXECUTIONEXCEPTION_H

#include <PiiException.h>
#include <QList>
#include <QPair>
#include "PiiYdin.h"

class PiiOperation;

/**
 * Thrown when an operation cannot be started, or an error is detected
 * during execution.
 *
 */
class PII_YDIN_EXPORT PiiExecutionException : public PiiException
{
  PII_DEFAULT_SERIALIZATION_FUNCTION(PiiException)
  PII_VIRTUAL_METAOBJECT_FUNCTION;
public:
  /**
   * Codes for different exception types.
   *
   * - `Error` - the exception was caused by an error. The operation
   * must be terminated.
   *
   * - `Finished` - the operation that threw the exception finished
   * execution due to end of data (or similar reason) or upon
   * reception of a stop tag.
   *
   * - `Interrupted` - the operation was abruptly interrupted by an
   * external signal.
   *
   * - `Paused` - the operation was paused upon reception of a pause
   * tag.
   */
  enum Code { Error, Finished, Interrupted, Paused };
  
  /**
   * Constructs a new `Error` with the given message and location.
   */
  PiiExecutionException(const QString& message = "", const QString& location = "");

  /**
   * Constructs a new PiiExecutionException with the given code.
   */
  PiiExecutionException(Code code);

  /**
   * Constructs a new PiiExecutionException with the given code, error
   * message and location.
   */
  PiiExecutionException(Code code, const QString& message, const QString& location = "");

  /**
   * Copies another exception.
   */
  PiiExecutionException(const PiiExecutionException& other);

  ~PiiExecutionException();
  
  /**
   * Get the cause of the exception.
   */
  Code code() const;

  /**
   * Set the error code.
   */
  void setCode(Code code);

  virtual bool isCompound() const;

  /**
   * Returns a textual representation of the given *code*.
   */
  static const char* errorName(Code code);

protected:
  /// @cond null
  class Data : public PiiException::Data
  {
  public:
    Data(const QString& message, const QString& location, Code code);
    Data(const QString& message, const QString& location);
    Data(const Data& other);
    Code code;
  };
  PII_D_FUNC;

  PiiExecutionException(Data* d);
  void throwThis();
  /// @endcond
};

#define PII_SERIALIZABLE_CLASS PiiExecutionException
#define PII_VIRTUAL_METAOBJECT
#define PII_BUILDING_LIBRARY PII_BUILDING_YDIN

#include "PiiSerializableRegistration.h"

/**
 * Thrown by PiiOperationCompound when errors occur during check(). 
 * This exception combines the possibly many error messages of all
 * child operations.
 *
 */
class PII_YDIN_EXPORT PiiCompoundExecutionException : public PiiExecutionException
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned)
  {
    PII_SERIALIZE_BASE(archive, PiiExecutionException);
    archive & _d()->lstExceptions;
  }
  PII_VIRTUAL_METAOBJECT_FUNCTION;
public:
  typedef QList<QPair<QString,PiiExecutionException*> > ExceptionList;

  PiiCompoundExecutionException(const QString& message = "", const QString& location = "");

  PiiCompoundExecutionException(const PiiCompoundExecutionException& other);

  ~PiiCompoundExecutionException();
  
  void addException(const QString& childName, const PiiExecutionException& exception);

  ExceptionList exceptions() const;

  bool isCompound() const;

protected:
  /// @internal
  void throwThis();

private:
  /// @internal
  class Data : public PiiExecutionException::Data
  {
  public:
    Data(const QString& message, const QString& location);
    Data(const Data& other);
    void addException(const QString& childName, PiiExecutionException* ex);
    ExceptionList lstExceptions;
  };
  PII_D_FUNC;
};

#define PII_SERIALIZABLE_CLASS PiiCompoundExecutionException
#define PII_VIRTUAL_METAOBJECT
#define PII_BUILDING_LIBRARY PII_BUILDING_YDIN

#include "PiiSerializableRegistration.h"

#endif //_PIIEXECUTIONEXCEPTION_H
