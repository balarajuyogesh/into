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

#ifndef Q_MOC_RUN // As of Qt 5, moc fails on this file

#ifdef PII_SERIALIZABLE_QOBJECT
#  undef PII_SERIALIZABLE_CLASS
#  define PII_SERIALIZABLE_CLASS PII_SERIALIZABLE_QOBJECT
#  define PII_SERIALIZABLE_DYNAMIC
#endif

#ifndef PII_SERIALIZABLE_CLASS
#  error "PII_SERIALIZABLE_CLASS must be defined before including PiiSerializableRegistration.h"
#endif

#ifdef PII_CREATE_QVARIANT_WRAPPER
#  include <PiiQVariantWrapper.h>
#  ifdef PII_TEMPLATE_TYPEDEF
#    define PII_SERIALIZABLE_CLASS_ PiiQVariantWrapper::Template<PII_TEMPLATE_TYPEDEF>
#    define PII_SERIALIZABLE_CLASS_NAME_ "PiiQVariantWrapper<" PII_STRINGIZE(PII_TEMPLATE_TYPEDEF) ">"
#  else
#    define PII_SERIALIZABLE_CLASS_ PiiQVariantWrapper::Template<PII_SERIALIZABLE_CLASS >
#    define PII_SERIALIZABLE_CLASS_NAME_ "PiiQVariantWrapper<" PII_STRINGIZE(PII_SERIALIZABLE_CLASS) ">"
#  endif
#else
// Replace template class name with a typedef
#  ifdef PII_TEMPLATE_TYPEDEF
typedef PII_SERIALIZABLE_CLASS_ PII_TEMPLATE_TYPEDEF;
#    define PII_SERIALIZABLE_CLASS_ PII_TEMPLATE_TYPEDEF
#  else
#    define PII_SERIALIZABLE_CLASS_ PII_SERIALIZABLE_CLASS
#  endif
#  ifndef PII_SERIALIZABLE_CLASS_NAME
#    define PII_SERIALIZABLE_CLASS_NAME_ PII_STRINGIZE(PII_SERIALIZABLE_CLASS_)
#  else
#    define PII_SERIALIZABLE_CLASS_NAME_ PII_SERIALIZABLE_CLASS_NAME
#  endif
#endif


#if defined(PII_BUILDING_LIBRARY) && !defined(PII_SERIALIZABLE_IS_SHARED)
#  define PII_SERIALIZABLE_IS_SHARED
#endif


PII_SERIALIZATION_NAME_CUSTOM(PII_SERIALIZABLE_CLASS_, PII_SERIALIZABLE_CLASS_NAME_);

// Store version trait
#ifdef PII_SERIALIZABLE_CLASS_VERSION
PII_SERIALIZATION_VERSION(PII_SERIALIZABLE_CLASS_, PII_SERIALIZABLE_CLASS_VERSION);
#endif

// Store tracking trait
#ifdef PII_NO_TRACKING
PII_SERIALIZATION_TRACKING(PII_SERIALIZABLE_CLASS_, false);
#endif

#ifdef PII_SERIALIZABLE_IS_ABSTRACT
PII_SERIALIZATION_ABSTRACT(PII_SERIALIZABLE_CLASS_);
#  if !defined(PII_VIRTUAL_METAOBJECT) && !defined(PII_SERIALIZABLE_DYNAMIC)
#    define PII_VIRTUAL_METAOBJECT
#  endif
#endif

// Virtual metaobject implementation requested (otherwise, use
// default)
#ifdef PII_VIRTUAL_METAOBJECT
PII_SERIALIZATION_VIRTUAL_METAOBJECT(PII_SERIALIZABLE_CLASS_);
#elif defined(PII_SERIALIZABLE_DYNAMIC)
PII_SERIALIZATION_DYNAMIC(PII_SERIALIZABLE_CLASS_)
#endif

// The factory and serializer of serializable objects in library
// cannot be instantiated here because that would cause multiple
// definitions.
#ifndef PII_SERIALIZABLE_IS_SHARED
#  if !defined(PII_ARCHIVE_TYPE) || PII_ARCHIVE_TYPE == 0
#    include "PiiGenericInputArchive.h"
#    include "PiiGenericOutputArchive.h"
#    define PII_INPUT_ARCHIVE_NAME PiiGenericInputArchive
#    define PII_OUTPUT_ARCHIVE_NAME PiiGenericOutputArchive
#  elif PII_ARCHIVE_TYPE == 1
#    include "PiiTextInputArchive.h"
#    include "PiiTextOutputArchive.h"
#    define PII_INPUT_ARCHIVE_NAME PiiTextInputArchive
#    define PII_OUTPUT_ARCHIVE_NAME PiiTextOutputArchive
#  else
#    include "PiiBinaryInputArchive.h"
#    include "PiiBinaryOutputArchive.h"
#    define PII_INPUT_ARCHIVE_NAME PiiBinaryInputArchive
#    define PII_OUTPUT_ARCHIVE_NAME PiiBinaryOutputArchive
#  endif
#  include "PiiSerializationUtil.h"
PII_INSTANTIATE_SERIALIZER(PII_INPUT_ARCHIVE_NAME, PII_SERIALIZABLE_CLASS_);
PII_INSTANTIATE_SERIALIZER(PII_OUTPUT_ARCHIVE_NAME, PII_SERIALIZABLE_CLASS_);
PII_INSTANTIATE_FACTORY(PII_SERIALIZABLE_CLASS_);
#  ifdef PII_CUSTOM_FACTORY
PII_INSTANTIATE_ARCHIVE_FACTORY(PII_SERIALIZABLE_CLASS_, PII_INPUT_ARCHIVE_NAME);
#  endif
#else
#  ifndef PII_BUILDING_LIBRARY
#    error "PII_BUILDING_LIBRARY must be defined if PII_SERIALIZABLE_IS_SHARED is defined."
#  endif
PII_DECLARE_FACTORY(PII_SERIALIZABLE_CLASS_, PII_BUILDING_LIBRARY);
#endif // ifndef PII_SERIALIZABLE_SHARED

#undef PII_SERIALIZABLE_CLASS_
#undef PII_SERIALIZABLE_CLASS_NAME_
#undef PII_ARCHIVE_TYPE
#undef PII_INPUT_ARCHIVE_NAME
#undef PII_OUTPUT_ARCHIVE_NAME

#endif
