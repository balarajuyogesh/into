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

#include "PiiException.h"
#include "PiiPreprocessor.h"

#ifndef PII_EXCEPTION_CLASS
#  error "Must define PII_EXCEPTION_CLASS"
#endif

#ifndef PII_EXCEPTION_BASE
#  define PII_EXCEPTION_BASE PiiException
#endif

#ifndef PII_EXCEPTION_NAMESPACE
#  define PII_EXCEPTION_NAMESPACE
#  define PII_EXCEPTION_NS_SCOPE
#  define PII_EXCEPTION_NS_START
#  define PII_EXCEPTION_NS_END
#else
#  define PII_EXCEPTION_NS_SCOPE PII_EXCEPTION_NAMESPACE::
#  define PII_EXCEPTION_NS_START namespace PII_EXCEPTION_NAMESPACE {
#  define PII_EXCEPTION_NS_END }
#endif

#ifdef PII_EXCEPTION_BUILDING_LIBRARY
#  define PII_EXCEPTION_EXPORT PII_IF(PII_BUILDING_LIBRARY, PII_DECL_EXPORT, PII_DECL_IMPORT)
#  define PII_BUILDING_LIBRARY PII_EXCEPTION_BUILDING_LIBRARY
#else
#  define PII_EXCEPTION_EXPORT
#endif

PII_EXCEPTION_NS_START

class PII_EXCEPTION_EXPORT PII_EXCEPTION_CLASS : public PII_EXCEPTION_BASE
{
#ifndef PII_NO_QT
  PII_DEFAULT_SERIALIZATION_FUNCTION(PII_EXCEPTION_BASE)
  PII_VIRTUAL_METAOBJECT_FUNCTION;
#endif
public:
  PII_EXCEPTION_CLASS(const QString& message = "", const QString& location = "");
  PII_EXCEPTION_CLASS(const PII_EXCEPTION_CLASS& other);
  ~PII_EXCEPTION_CLASS();
  PII_EXCEPTION_CLASS& operator= (const PII_EXCEPTION_CLASS& other);
protected:
  PII_EXCEPTION_CLASS(Data* d);
  void throwThis();
};

PII_EXCEPTION_NS_END

#ifndef PII_NO_QT
#  define PII_SERIALIZABLE_CLASS PII_EXCEPTION_NS_SCOPE PII_EXCEPTION_CLASS
#  define PII_VIRTUAL_METAOBJECT

#  include "PiiSerializableRegistration.h"
#endif

#ifdef PII_EXCEPTION_DEFINITION

#  ifndef PII_NO_QT
#    include "PiiSerializableExport.h"
PII_SERIALIZABLE_EXPORT(PII_EXCEPTION_NS_SCOPE PII_EXCEPTION_CLASS);
#  endif

PII_EXCEPTION_NS_START

PII_EXCEPTION_CLASS::PII_EXCEPTION_CLASS(const QString& message, const QString& location) :
  PiiException(message, location)
{}

PII_EXCEPTION_CLASS::PII_EXCEPTION_CLASS(const PII_EXCEPTION_CLASS& other) :
  PiiException(other)
{}

PII_EXCEPTION_CLASS::PII_EXCEPTION_CLASS(Data* d) :
  PiiException(d)
{}

PII_EXCEPTION_CLASS::~PII_EXCEPTION_CLASS()
{}

PII_EXCEPTION_CLASS& PII_EXCEPTION_CLASS::operator= (const PII_EXCEPTION_CLASS& other)
{
  return static_cast<PII_EXCEPTION_CLASS&>(PiiException::operator= (other));
}

void PII_EXCEPTION_CLASS::throwThis() { throw *this; }

PII_EXCEPTION_NS_END

#endif // PII_EXCEPTION_DEFINITION

#undef PII_EXCEPTION_CLASS
#undef PII_EXCEPTION_BASE
#undef PII_EXCEPTION_EXPORT
#undef PII_EXCEPTION_DEFINITION
#undef PII_EXCEPTION_BUILDING_LIBRARY
#undef PII_EXCEPTION_NAMESPACE
#undef PII_EXCEPTION_NS_SCOPE
#undef PII_EXCEPTION_NS_START
#undef PII_EXCEPTION_NS_END
