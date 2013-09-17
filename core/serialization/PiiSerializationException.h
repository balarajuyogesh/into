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

#ifndef _PIISERIALIZATIONEXCEPTION_H
#define _PIISERIALIZATIONEXCEPTION_H

#include <PiiException.h>
#include "PiiSerializationGlobal.h"

/**
 * @file
 *
 * Macros and declarations for handling serialization errors.
 *
 */

/**
 * Throw a PiiSerializationException with the given error code. This
 * macro is useful mostly in archive implementations.
 *
 * ~~~
 * PII_SERIALIZATION_ERROR(StreamError);
 * ~~~
 */
#define PII_SERIALIZATION_ERROR(code) throw PiiSerializationException(PiiSerializationException::code, QString(__FILE__ ":%1").arg(__LINE__))

/**
 * Throw a PiiSerializationException with the given error code and
 * extra information. This macro is useful mostly in archive
 * implementations.
 *
 * ~~~
 * PII_SERIALIZATION_ERROR_INFO(SerializerNotFound, className);
 * ~~~
 */
#define PII_SERIALIZATION_ERROR_INFO(code, info) throw PiiSerializationException(PiiSerializationException::code, info, QString(__FILE__ ":%1").arg(__LINE__))

/**
 * Throw a PiiSerializationException with a custom error message.
 */
#define PII_SERIALIZATION_CUSTOM_ERROR(message) throw PiiSerializationException(message, QString(__FILE__ ":%1").arg(__LINE__))

/**
 * Thrown when an error occurs in (de)serializing data.
 *
 */
class PII_SERIALIZATION_EXPORT PiiSerializationException : public PiiException
{
public:
  /**
   * Error codes for serialization failures.
   *
   * - `Unknown` An unknow error.
   *
   * - `InvalidDataFormat` The data read from an archive is
   * corrupted. This error happens only when the archive notices it is
   * reading something it did not except.
   *
   * - `UnregisteredClass` An input archive does not find a factory
   * for a class name read from an archive.
   *
   * - `SerializerNotFound` An output archive cannot find a
   * serializer for the object to be serialized.
   *
   * - `ClassVersionMismatch` The class version number read from an
   * archive is greater than the current class version number.
   *
   * - `StreamError` The underlying input stream cannot be accessed.
   *
   * - `StreamNotOpen` The underlying input stream is not open.
   *
   * - `UnrecognizedArchiveFormat` The magic key in the beginning of
   * an archive does not match the key of the reading archive.
   *
   * - `ArchiveVersionMismatch` The input archive version is greater
   * than the current version of the archive implementation.
   */
  enum Code
    {
      Unknown = 0,
      InvalidDataFormat,
      UnregisteredClass,
      SerializerNotFound,
      ClassVersionMismatch,
      StreamError,
      StreamNotOpen,
      UnrecognizedArchiveFormat,
      ArchiveVersionMismatch
    };
  
  /**
   * Construct a new PiiSerializationException with the given error
   * code. The exception message will be automatically generated with
   * messageForCode().
   */
  PiiSerializationException(Code code, const QString& location = "");

  /**
   * Construct an `Unknown` exception with the given error message.
   */
  PiiSerializationException(const QString& message, const QString& location = "");

  /**
   * Construct a new PiiSerializationException with the given error
   * code. The exception message will be automatically generated with
   * messageForCode(). The `info` parameter gives extra
   * information such as a class name with `UnregisteredClass`.
   */
  PiiSerializationException(Code code, const QString& info, const QString& location);

  /// Creates a copy of @a other.
  PiiSerializationException(const PiiSerializationException& other);

  PiiSerializationException& operator= (const PiiSerializationException& other);

  ~PiiSerializationException();
  
  /**
   * Convert a numerical error code into human-readable text.
   */
  static const char* messageForCode(Code code);

  /**
   * Returns "message (info)", if info is non-empty. Otherwise returns
   * "message".
   */
  QString message() const;
  
  /**
   * Get the error code.
   */
  Code code() const;

  /**
   * Get additional information. This can be used for any purpose.
   * Currently, `SerializerNotFound` and `UnregisteredClass` store
   * the class name as additional information.
   */
  QString info() const;

protected:
  /// @internal
  void throwThis();

private:
  /// @internal
  class Data : public PiiException::Data
  {
  public:
    Data(Code c, const QString& location);
    Data(const QString& message, const QString& location);
    Data(Code c, const QString& info, const QString& location);
    Code code;
    QString strInfo;
  };
  PII_D_FUNC;
};

#endif //_PIISERIALIZATIONEXCEPTION_H
