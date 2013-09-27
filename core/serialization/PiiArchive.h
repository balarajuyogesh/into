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

#ifndef _PIIARCHIVE_H
#define _PIIARCHIVE_H

#include "PiiSerializationGlobal.h"

/// Major archive version number.
#define PII_ARCHIVE_VERSION 1

/**
 * Common base class for all archive types. Stores a version number
 * that affects both input and output archive formats. The version
 * number is composed of a major and a minor version number. The major
 * version reflects format-independent changes in archive
 * implementation. Each subclass has its own (minor) version number
 * that may be affected by the details of its own format. Subclasses
 * must take care of storing and restoring the version numbers.
 *
 */
class PII_SERIALIZATION_EXPORT PiiArchive
{
public:
  /**
   * Initializes major version to [PII_ARCHIVE_VERSION] and minor
   * version to 0.
   */
  PiiArchive() :
    _iMajorVersion(PII_ARCHIVE_VERSION),
    _iMinorVersion(0)
  {}

  /**
   * Get the major version number. The archive version number can be
   * used by serializable objects to change their behavior, but this
   * is seldom necessary. Usually the version number of the type
   * itself is enough.
   */
  int majorVersion() const { return _iMajorVersion; } 
  /// Get the minor version number.
  int minorVersion() const { return _iMinorVersion; }
  
protected:
  /**
   * Set the major version number. This value must be stored and
   * retrieved by archive implementations.
   */
  void setMajorVersion(int majorVersion) { _iMajorVersion = majorVersion; }
  /**
   * Set the minor version number. This value must be stored and
   * retrieved by archive implementations.
   */
  void setMinorVersion(int minorVersion) { _iMinorVersion = minorVersion; }
  
private:
  int _iMajorVersion;
  int _iMinorVersion;
};

#endif //_PIIARCHIVE_H
