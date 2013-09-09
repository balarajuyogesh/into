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

/**
 * @file
 *
 * Serializable objects are most conveniently registered by including
 * this file. The most straightforward way of making your class
 * serializable is as follows:
 *
 * @code
 * class MyClass
 * {
 *   friend class Piiserialization::Accessor;
 *   template <class Archive> void serialize(Archive& ar, const unsigned int version)
 *   {
 *      ar & i;
 *   }
 *   int i;
 * };
 *
 * #define PII_SERIALIZABLE_CLASS MyClass
 * #include <PiiSerializableRegistration.h>
 * @endcode
 *
 * The file recognizes the following macros (everything but @p
 * PII_SERIALIZABLE_CLASS are optional):
 *
 * @lip PII_SERIALIZABLE_CLASS - the name of the serializable type.
 *
 * @lip PII_SERIALIZABLE_QOBJECT - the name of the serializable type. 
 * Same as PII_SERIALIZABLE_CLASS, but marks the class dynamic at the
 * same time. The difference to PII_VIRTUAL_METAOBJECT is that
 * QObjects have a default metaobject implementation based on
 * QMetaObject that is often good enough and no PiiMetaObject is
 * needed.
 *
 * @lip PII_SERIALIZABLE_CLASS_NAME - a stringified version of the
 * name. This is needed if you want a custom name for your type.
 *
 * @lip PII_TEMPLATE_TYPEDEF - a custom "class name" for a template
 * class. Use this if your class is a template instance with more than
 * one template parameter. It is a necessary work-around for the
 * preprocessor.
 *
 * @lip PII_SERIALIZABLE_CLASS_VERSION - the version number of your
 * class. The default is 0.
 *
 * @lip PII_NO_TRACKING - disable object tracking for this type. If
 * object tracking is disabled, references to the same memory location
 * may be serialized many times.
 *
 * @lip PII_VIRTUAL_METAOBJECT - tell the serialization system that
 * your class implements a virtual piiMetaObject() function. This is
 * necessary if the class is going to be serialized via a base class
 * pointer/reference. Actually, it is not strictly necessary to repeat
 * this macro for each derived class if the application never sees
 * them. But make sure you use it for the base class. Furthermore, you
 * need to place @ref PII_VIRTUAL_METAOBJECT_FUNCTION into the class
 * declaration of the base type and all derived types.
 *
 * @lip PII_SERIALIZABLE_IS_ABSTRACT - disables the creation of a
 * default factory object. Since abstract classes cannot be
 * instantiated, defining this value makes the library not try "new"
 * for the type. It also sets the corresponding type trait. This macro
 * implies PII_VIRTUAL_METAOBJECT, unless PII_SERIALIZABLE_QOBJECT is
 * explicitly specified.
 *
 * @lip PII_SERIALIZABLE_IS_SHARED - the serializable object is shared
 * by many translation units. This macro effectively suppresses the
 * creation of factory and serializer objects for the type. Just the
 * necessary declarations will be present. This is needed to avoid
 * multiple definitions. One must use the @ref PII_SERIALIZABLE_EXPORT
 * macro in a .cc file to create the necessary definitions. If your
 * serializable type is used only within one translation unit, you
 * don't need to define this value.
 *
 * @lip PII_USED_AS_QVARIANT - the object is going to be used as a
 * QVariant. This effectively registers a PiiQVariantWrapper to the
 * serializable object as well as the object itself.
 *
 * @lip PII_BUILDING_LIBRARY - 1 if you are building a shared library
 * from which you want to export a serializable type, 0 otherwise. 
 * This is needed to properly handle <tt>__declspec(dllexport)</tt>
 * and <tt>__declspec(dllimport)</tt>. This macro implies @p
 * PII_SERIALIZABLE_SHARED.
 *
 * @lip PII_ARCHIVE_TYPE - the archive type the object's serializer is
 * registered to. The default is 0, which registers the serializers to
 * generic archive types (PiiGenericInputArchive,
 * PiiGenericOutputArchive). This makes it possible to use the
 * serializable class with any archive implementation, but imposes a
 * performance penalty. Use 1 for text archives and 2 for binary
 * archives. Note that this macro has no effect if @p
 * PII_SERIALIZABLE_SHARED is defined, because it suppresses
 * serializer registrations.
 *
 * @lip PII_CUSTOM_FACTORY - if this macro is defined, a factory
 * object will be registered not only to the default serialization
 * factory but also to the archive-specific factory determined by @p
 * PII_ARCHIVE_TYPE.
 *
 * The following example shows how to register an abstract class that
 * will be linked to an application from a dynamic library.
 *
 * @code
 * // In PiiOperation.h
 * #define PII_SERIALIZABLE_CLASS PiiOperation
 * #define PII_SERIALIZABLE_IS_ABSTRACT
 * #define PII_VIRTUAL_METAOBJECT // Actually implied by PII_SERIALIZABLE_IS_ABSTRACT
 * #define PII_BUILDING_LIBRARY PII_BUILDING_YDIN
 *
 * #include <PiiSerializableRegistration.h>
 *
 * // In PiiOperation.cc
 * PII_DEFINE_VIRTUAL_METAOBJECT_FUNCTION(PiiOperation);
 * PII_SERIALIZABLE_EXPORT(PiiOperation);
 * @endcode
 *
 * @b NOTE! It is @b not possible to register class templates. 
 * Template instantiations are OK, but generic templates cannot be
 * registered without instantiating them. Furthermore, template
 * instantiations with more than one template parameter cannot be
 * registered directly because the preprocessor doesn't tolerate
 * commas in macro parameters. Use the @p PII_TEMPLATE_TYPEDEF macro,
 * preferably with @p PII_SERIALIZABLE_CLASS_NAME in these cases.
 *
 * An example of registering a template instance:
 *
 * @code
 * template <class T1, class T2> class MyClass
 * {
 *   friend class Piiserialization::Accessor;
 *   template <class Archive> void serialize(Archive& ar, const unsigned int version)
 *   {
 *      ar & t1;
 *      ar & t2;
 *   }
 *   T1 t1;
 *   T2 t2;
 * };
 *
 * // Explicit instantiation
 * template MyClass<int,int>;
 *
 * #define PII_SERIALIZABLE_CLASS MyClass<int,int>
 * #define PII_TEMPLATE_TYPEDEF MyClass_int_int
 * #define PII_SERIALIZABLE_CLASS_NAME "MyClass<int,int>"
 * #include <PiiSerializableRegistration.h>
 * @endcode
 *
 * @ingroup Serialization
 */

#include "PiiSerializationTraits.h"
#include "PiiSerializationFactory.h"
#include "PiiVirtualMetaObject.h"

#include "PiiRegistrationImpl.h"

#ifdef PII_USED_AS_QVARIANT
#  undef PII_SERIALIZABLE_CLASS_VERSION
#  undef PII_VIRTUAL_METAOBJECT
#  undef PII_NO_TRACKING
#  undef PII_SERIALIZABLE_IS_ABSTRACT
#  undef PII_SERIALIZABLE_QOBJECT
#  undef PII_SERIALIZABLE_DYNAMIC
#  define PII_CREATE_QVARIANT_WRAPPER
#  include "PiiRegistrationImpl.h"
#  undef PII_CREATE_QVARIANT_WRAPPER
#  undef PII_USED_AS_QVARIANT
#endif


#undef PII_SERIALIZABLE_CLASS
#undef PII_TEMPLATE_TYPEDEF
#undef PII_SERIALIZABLE_CLASS_NAME
#undef PII_SERIALIZABLE_CLASS_VERSION
#undef PII_VIRTUAL_METAOBJECT
#undef PII_NO_TRACKING
#undef PII_SERIALIZABLE_IS_SHARED
#undef PII_BUILDING_LIBRARY
#undef PII_SERIALIZABLE_IS_ABSTRACT
#undef PII_SERIALIZABLE_QOBJECT
#undef PII_SERIALIZABLE_DYNAMIC
