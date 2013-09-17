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

#ifndef _PIISERIALIZABLEEXPORT_H
#define _PIISERIALIZABLEEXPORT_H

#include "PiiSerializationFactory.h"
#include "PiiSerializer.h"
#include "PiiGenericInputArchive.h"
#include "PiiGenericOutputArchive.h"

/**
 * @file
 *
 * Macros for exporting serializable classes from dlls.
 *
 */

/**
 * Make a class known to the serialization system. This macro must be
 * included in library class definitions (in a .cc file) that may be
 * serialized via a base class pointer. The macro instantiates a
 * serializer for type *CLASS_NAME* to PiiGenericInputArchive and
 * PiiGenericOutputArchive. It also instantiates a factory object for
 * the class.
 */
#define PII_SERIALIZABLE_EXPORT(CLASS_NAME) \
  PII_INSTANTIATE_SERIALIZER(PiiGenericInputArchive, CLASS_NAME); \
  PII_INSTANTIATE_SERIALIZER(PiiGenericOutputArchive, CLASS_NAME); \
  PII_INSTANTIATE_FACTORY(CLASS_NAME)


#endif //_PIISERIALIZABLEEXPORT_H
