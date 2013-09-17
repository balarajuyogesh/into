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

#ifndef _PIICLASSIFICATIONEXCEPTION_H
#define _PIICLASSIFICATIONEXCEPTION_H

#include <PiiException.h>
#include <PiiClassificationGlobal.h>

/**
 * PiiClassificationException is thrown when errors occur in
 * classification.
 *
 */
class PII_CLASSIFICATION_EXPORT PiiClassificationException : public PiiException
{
public:
  /**
   * Codes for different exception types.
   *
   * - `Unknown` - unknown error
   *
   * - `LearningInterrupted` - learning was interrupted by user.
   */
  enum Code
    {
      Unknown,
      LearningInterrupted
    };
  
  /**
   * Construct a new PiiClassificationException with the given error
   * code. The exception message will be automatically generated with
   * messageForCode().
   */
  PiiClassificationException(Code code, const QString& location = "");

  /**
   * Construct a `Unknown` exception with the given error message.
   */
  PiiClassificationException(const QString& message, const QString& location = "");
  
  /**
   * Get the cause of the exception.
   */
  Code code() const;

  /**
   * Convert a numerical error code into human-readable text.
   */
  static const char* messageForCode(Code code);
  
private:
  class Data : public PiiException::Data
  {
  public:
    Data(Code code, const QString& location);
    Data(const QString& message, const QString& location);
    Code code;
  };
  PII_D_FUNC;
};


#endif //_PIICLASSIFICATIONEXCEPTION_H
