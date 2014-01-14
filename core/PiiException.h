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

#ifndef _PIIEXCEPTION_H
#define _PIIEXCEPTION_H

#include "PiiGlobal.h"
#include <QString>

#ifndef _DEBUG
// Release builds save memory by not storing error location.
#  define PII_MAKE_EXCEPTION(EXCEPTION, MESSAGE) EXCEPTION(MESSAGE)
#else
/**
 * Constructs an instance of `EXCEPTION` with the given `MESSAGE`.
 * This macro automatically fills in file and line number information
 * in debug builds. Error location will be omitted in release builds.
 */
#  define PII_MAKE_EXCEPTION(EXCEPTION, MESSAGE) EXCEPTION(MESSAGE, QString(__FILE__ ":%1").arg(__LINE__))
#endif

/**
 * A macro for throwing an exception with error location information. 
 * With this macro, the file name and line number of the current code
 * line are automatically stored as the error location. If you don't
 * need the location information, just throw the exception as in `throw
 * PiiException("Everything just went kablooie.")`. An example:
 *
 * ~~~(c++)
 * PII_THROW(PiiException, tr("The software just failed spectacularly."));
 * ~~~
 *
 * @param EXCEPTION the class name of the exception to be thrown, e.g. 
 * PiiException.
 *
 * @param MESSAGE the error message
 */
#define PII_THROW(EXCEPTION, MESSAGE) throw PII_MAKE_EXCEPTION(EXCEPTION, MESSAGE)

namespace PiiSerialization
{
  struct Accessor;
}

class PiiMetaObject;

/**
 * PiiException is the base class of all exceptions.  Usually, one
 * does not throw an PiiException directly but creates a sub-class
 * whose type identifies the exception more precisely.
 *
 * To support exceptions in remote function calls, all exception
 * classes should be made serializable.
 */
class PII_CORE_EXPORT PiiException
{
#ifndef PII_NO_QT
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned version);
  virtual const PiiMetaObject* piiMetaObject() const;
#endif
public:
  /**
   * Constructs an empty exception.
   */
  PiiException();
  
  /**
   * Constructs a new exception with the given *message*.
   *
   * @param message the error message. The message should be a
   * user-readable explation of the error, and it is typically
   * translatable.
   */
  PiiException(const QString& message);
  /**
   * Constructs a new exception with the given *message* and error
   * location.
   *
   * @param message the error message. The message should be a
   * user-readable explation of the error, and it is typically
   * translatable.
   *
   * @param location the location of the code this error occured at. 
   * The standard, official, God-given format is "%file:line", e.g. 
   * "PiiException.h:30". The reason is that such a string works as a
   * hyperlink to source code when debugging applications with
   * (X)Emacs (which is the standard editor).
   */
  PiiException(const QString& message, const QString& location);

  /**
   * Copy another exception.
   */
  PiiException(const PiiException& other);

  PiiException& operator= (const PiiException& other);

  virtual ~PiiException();

  /**
   * Get the message stored in this exception.
   */
  QString message() const;

  /**
   * Set the message stored in this exception.
   *
   * @param message the new exception message
   */
  void setMessage(const QString& message);

  /**
   * Returns the error location, for example
   * "PiiException.h:106". Note that if you use the [PII_THROW]
   * macro, location will not be included in release builds.
   */
  QString location() const;

  /**
   * Returns *prefix* + [location()] + *suffix*, if location is
   * non-empty. Otherwise returns an empty string.
   */
  QString location(const QString& prefix, const QString& suffix) const;

  /**
   * Set the error location.
   */
  void setLocation(const QString& location);

  /**
   * Returns a textual representation of the exception, including its
   * type, location (if known), and message.
   */
  QString toString() const;

  /**
   * Throws the exception as a value and deletes the this pointer. 
   * This function is used to throw exceptions whose type is not known
   * at compile time.
   *
   * @internal
   */
  void throwIt();

protected:
  /// @hide
  class PII_CORE_EXPORT Data
  {
  public:
    Data();
    Data(const QString& message, const QString& location);
    Data(const Data& other);
    virtual ~Data();
    QString strMessage;
    QString strLocation;
  } *d;
  PiiException(Data* data);
  // Throws *this (as a value).
  virtual void throwThis();
  /// @endhide
};

#ifndef PII_NO_QT
#include <PiiVirtualMetaObject.h>
#include <PiiSerializationUtil.h>
#include <PiiNameValuePair.h>

template <class Archive> void PiiException::serialize(Archive& archive, const unsigned)
{
  archive & PII_NVP("message", d->strMessage);
  archive & PII_NVP("location", d->strLocation);
}

#define PII_SERIALIZABLE_CLASS PiiException
#define PII_VIRTUAL_METAOBJECT
#define PII_BUILDING_LIBRARY PII_BUILDING_CORE

#include "PiiSerializableRegistration.h"
#endif

#endif //_PIIEXCEPTION_H
