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

#ifndef _PIIRESOURCESTATEMENT_H
#define _PIIRESOURCESTATEMENT_H

#include "PiiGlobal.h"
#include "PiiSharedD.h"
#include <QString>

/**
 * A class used by PiiResourceDatabase to store statements about
 * resources. Each statement is composed of a subject, a predicate and
 * an object, which are stored as strings. The subject always refers
 * to a resource about which the statement is being made. The
 * predicate describes which trait or aspect of the subject is being
 * described, and object specifies the value of the aspect. The value
 * may be either a string literal or a reference to another resource.
 */
class PII_CORE_EXPORT PiiResourceStatement
{
public:
  /**
   * Possible types of the object of the statement.
   *
   * - Invalid - the statement is invalid.
   *
   * - Literal - the object is a string literal.
   *
   * - Resource - the object is a resource id and references another
   * resource.
   *
   * @li Pointer - the object is a pointer to any user-defined
   * object.
   */
  enum Type { Invalid, Literal, Resource, Pointer };

  /**
   * Creates an invalid statement.
   */
  PiiResourceStatement();

  ~PiiResourceStatement();
  
  /**
   * Creates a copy of another statement.
   */
  PiiResourceStatement(const PiiResourceStatement& other);

  PiiResourceStatement& operator= (const PiiResourceStatement& other);
    
  /**
   * Returns `true` if this is a valid statement, and `false`
   * otherwise. A statement is valid if and only if both subject and
   * object are non-empty, and the type of the object is not
   * `Invalid`. The predicate may be an empty string.
   */
  bool isValid() const;

  /**
   * Returns the subject of the statement. The subject is always a
   * resource identifier.
   */
  QString subject() const;
  /**
   * Returns the predicate of the statement. The predicate is always a
   * string literal.
   */
  QString predicate() const;
  /**
   * Returns the object of the statement. The object is either a
   * string literal, a resource identifier or a pointer, depending on
   * [type()]. If the object is a pointer, this function returns null
   * string.
   */
  QString object() const;
  /**
   * Returns the object of the statement as a pointer. If the type of
   * the statement is not `Pointer`, returns 0.
   */
  void* objectPtr() const;
  /**
   * Returns the type of the statement. The type specifies how the
   * object should be interpreted.
   */
  Type type() const;
  /**
   * Returns the id number of the statement. PiiResourceDatabase
   * automatically assigns id numbers to inserted statements.
   */
  int id() const;

private:
  class Data;
  Data* d;
  PII_SHARED_D_FUNC_DECL;
  friend class PiiResourceDatabase;
  PiiResourceStatement(const char* subject,
                       const char* predicate,
                       const char* object,
                       Type type);

  PiiResourceStatement(const char* subject,
                       const char* predicate,
                       void* object);

  PiiResourceStatement(const QString& subject,
                       const QString& predicate,
                       const QString& object,
                       Type type);

  PiiResourceStatement(const QString& subject,
                       const QString& predicate,
                       void* object);

  PiiResourceStatement(int subject,
                       const QString& predicate,
                       const QString& object,
                       Type type);

  void setId(int id);
};

#endif //_PIIRESOURCESTATEMENT_H
