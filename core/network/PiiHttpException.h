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

#ifndef _PIIHTTPEXCEPTION_H
#define _PIIHTTPEXCEPTION_H

#include "PiiNetworkException.h"

/**
 * Thrown when a HTTP request handler cannot complete a request.
 *
 */
class PII_NETWORK_EXPORT PiiHttpException : public PiiNetworkException
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned)
  {
    PII_SERIALIZE_BASE(archive, PiiNetworkException);
    archive & PII_NVP("code", _d()->iStatusCode);
  }
  PII_VIRTUAL_METAOBJECT_FUNCTION;
public:
  /**
   * Constructs a new PiiHttpException with the given *statusCode*
   * and *location*.
   */
  PiiHttpException(int statusCode = 200, const QString& location = "");

  /**
   * Constructs a new PiiHttpException with the given *statusCode*
   * and *location*.
   */
  PiiHttpException(int statusCode, const QString& message, const QString& location);
  PiiHttpException(const PiiHttpException& other);
  ~PiiHttpException();
  PiiHttpException& operator= (const PiiHttpException& other);

  /**
   * Returns the status code.
   */
  int statusCode() const;
protected:
  /// @internal
  void throwThis();
  
private:
  /// @internal
  class Data : public PiiException::Data
  {
  public:
    Data(int statusCode, const QString& location);
    Data(int statusCode, const QString& message, const QString& location);
    int iStatusCode;
  };
  PII_D_FUNC;
};

#define PII_SERIALIZABLE_CLASS PiiHttpException
#define PII_VIRTUAL_METAOBJECT
#define PII_BUILDING_LIBRARY PII_BUILDING_NETWORK

#include "PiiSerializableRegistration.h"

#endif //_PIIHTTPEXCEPTION_H
