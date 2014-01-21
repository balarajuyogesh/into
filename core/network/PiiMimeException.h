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

#ifndef _PIIMIMEEXCEPTION_H
#define _PIIMIMEEXCEPTION_H

#include <PiiException.h>
#include "PiiNetwork.h"

#define PII_THROW_MIME(CODE) PII_THROW(PiiMimeException, PiiMimeException::CODE)

/**
 * Used to indicate error conditions in MIME headers.
 *
 */
class PII_NETWORK_EXPORT PiiMimeException : public PiiException
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned)
  {
    PII_SERIALIZE_BASE(archive, PiiException);
    archive & PII_NVP("code", PII_ENUM(_d()->code));
  }
  PII_VIRTUAL_METAOBJECT_FUNCTION;
public:
  /**
   * Codes for different exception types.
   *
   * - `HeaderTooLarge` - header size exceeds limit
   *
   * - `InvalidFormat` - the MIME message has invalid format.
   */
  enum Code { HeaderTooLarge, InvalidFormat };

  /**
   * Constructs a new PiiMimeException with the given code, error
   * message and location.
   */
  PiiMimeException(Code code = HeaderTooLarge, const QString& location = "");
  PiiMimeException(const PiiMimeException& other);
  ~PiiMimeException();
  PiiMimeException& operator= (const PiiMimeException& other);

  /**
   * Get the cause of the exception.
   */
  Code code() const;

  /**
   * Returns a textual representation of the error associated with
   * *code*.
   */
  static const char* messageForCode(Code code);

protected:
  /// @internal
  void throwThis();

private:
  class Data : public PiiException::Data
  {
  public:
    Data(Code code, const QString& location);
    Data(const QString& message, const QString& location);
    Data(const Data& other);
    Code code;
  };
  PII_D_FUNC;
};

#define PII_SERIALIZABLE_CLASS PiiMimeException
#define PII_VIRTUAL_METAOBJECT
#define PII_BUILDING_LIBRARY PII_BUILDING_NETWORK

#include "PiiSerializableRegistration.h"

#endif //_PIIMIMEEXCEPTION_H
