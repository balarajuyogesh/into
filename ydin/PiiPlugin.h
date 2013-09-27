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

#ifndef _PIIPLUGIN_H
#define _PIIPLUGIN_H

#include <PiiSerializationUtil.h>
#include "PiiPreprocessor.h"
//#include "PiiYdin.h"
#include "PiiClassInfoRegisterer.h"
#include <PiiSerializableExport.h>
#include <PiiVirtualMetaObject.h>
#include "PiiYdinResources.h"
#include <QtGlobal>


#define PII_PLUGIN_RETURN_SPEC PII_DECL_EXPORT const char*

/// @internal
typedef const char* (*pii_plugin_function)();

/**
 * The name of the plug-in name getter C function within a shared
 * library.
 */
#define PII_PLUGIN_NAME_FUNCTION pii_get_plugin_name
/**
 * The name of the plug-in name getter C function as a character
 * string.
 */
#define PII_PLUGIN_NAME_FUNCTION_STR PII_STRINGIZE(PII_PLUGIN_NAME_FUNCTION)

/**
 * The name of the plug-in version getter C function within a shared
 * library.
 */
#define PII_PLUGIN_VERSION_FUNCTION pii_get_plugin_version
/**
 * The name of the plug-in version getter C function as a character
 * string.
 */
#define PII_PLUGIN_VERSION_FUNCTION_STR PII_STRINGIZE(PII_PLUGIN_VERSION_FUNCTION)

/**
 * A macro for implementing a plug-in. A plug-in is implemented once
 * (and only once) in a shared library. This is how:
 *
 * @param PLUGIN_NAME a unique identifier (resource name) for the
 * plug-in.
 *
 * ~~~(c++)
 * PII_IMPLEMENT_PLUGIN(PiiImagePlugin);
 * ~~~
 */
#define PII_IMPLEMENT_PLUGIN(PLUGIN_NAME) \
  extern "C" PII_PLUGIN_RETURN_SPEC PII_PLUGIN_NAME_FUNCTION() { return PII_STRINGIZE(PLUGIN_NAME); } \
  extern "C" PII_PLUGIN_RETURN_SPEC PII_PLUGIN_VERSION_FUNCTION() { return INTO_VERSION_STR; } \
  static const char* pluginName() { static const char* pName = PII_STRINGIZE(PLUGIN_NAME); return pName; } \
  static PiiClassInfoRegisterer PII_JOIN(classInfoOf,PLUGIN_NAME)("plugins", PII_STRINGIZE(PLUGIN_NAME))

/**
 * Register an operation to Ydin's global resource database. This
 * macro registers the given `CLASS_NAME` as an instantiable resource
 * to the object registry, and places it as a child to the current
 * plug-in. The plug-in must have been implemented in the same
 * translation unit (see [PII_IMPLEMENT_PLUGIN]).
 *
 * **Note:** Registered operations must be serializable and they must
 * declare a virtual piiMetaObject() function. If you don't want your
 * class to be serializable, you can add statements about it in the
 * [statements](PII_BEGIN_STATEMENTS) section of a plug-in.
 *
 * @param CLASS_NAME the name of the class to register.
 *
 * ~~~(c++)
 * PII_IMPLEMENT_PLUGIN(PiiImagePlugin);
 *
 * PII_REGISTER_OPERATION(PiiImageFileReader);
 * ~~~
 */
#define PII_REGISTER_OPERATION(CLASS_NAME)                              \
  PII_DEFINE_VIRTUAL_METAOBJECT_FUNCTION(CLASS_NAME);                   \
  PII_SERIALIZATION_NAME(CLASS_NAME);                                   \
  PII_SERIALIZABLE_EXPORT(CLASS_NAME);                                  \
  static PiiClassInfoRegisterer                                         \
    PII_JOIN(classInfoOf,CLASS_NAME)(pluginName(),                      \
                                     PII_STRINGIZE(CLASS_NAME),         \
                                     PiiYdin::operationResourceName(static_cast<CLASS_NAME*>(0)))
/**
 * Same as [PII_REGISTER_OPERATION], but for operations that
 * implement a special `Void` constructor for deserialization
 * purposes. See [PII_SERIALIZATION_CONSTRUCTOR].
 */
#define PII_REGISTER_OPERATION_VOIDCTOR(CLASS_NAME)                     \
  PII_SERIALIZATION_NAME(CLASS_NAME);                                   \
  PII_DEFINE_VIRTUAL_METAOBJECT_FUNCTION(CLASS_NAME);                   \
  PII_SERIALIZATION_CONSTRUCTOR(CLASS_NAME, Void, ());                  \
  PII_SERIALIZATION_NORMAL_CONSTRUCTOR(CLASS_NAME, 0, ());              \
  PII_INSTANTIATE_ARCHIVE_FACTORY(CLASS_NAME, PiiGenericInputArchive);  \
  PII_SERIALIZABLE_EXPORT(CLASS_NAME);                                  \
  static PiiClassInfoRegisterer                                         \
    PII_JOIN(classInfoOf,CLASS_NAME)(pluginName(),                      \
                                     PII_STRINGIZE(CLASS_NAME),         \
                                     PiiYdin::operationResourceName(static_cast<CLASS_NAME*>(0)))

/**
 * Use this macro to register operation compounds. An alias for
 * [PII_REGISTER_OPERATION_VOIDCTOR].
 */
#define PII_REGISTER_COMPOUND(CLASS_NAME) PII_REGISTER_OPERATION_VOIDCTOR(CLASS_NAME)

/**
 * Register an instance of an operation template. The standard
 * programming pattern for registering template classes to the
 * resource database is to create an inner template class named
 * "Template". Properties are stored/retrieved with pure virtual
 * functions. This macro exports an implementation of such a template.
 * Do this:
 *
 * ~~~(c++)
 * class MyClass : public QObject
 * {
 *   Q_OBJECT
 *   Q_PROPERTY(int value READ value WRITE setValue);
 * public:
 *   virtual int value() const = 0;
 *   virtual void setValue(int value) = 0;
 *   template <class T> class Template;
 * };
 *
 * template <class T> class MyClass::Template
 * {
 *   PII_PROPERTY_SERIALIZATION_FUNCTION(QObject)
 * public:
 *   int value() const; //implement this somewhere
 *   void setValue(int value); //same here
 * private:
 *   int _iValue;
 * };
 *
 * PII_IMPLEMENT_PLUGIN(MyPlugin);
 *
 * PII_REGISTER_OPERATION_TEMPLATE(MyClass, float);
 * // This will register MyClass::Template<float> as "MyClass<float>" to the database.
 * ~~~
 *
 * This macro registers *CLASS_NAME::Template*<PRIMITIVE> as an
 * instantiable resource to the object registry, and uses
 * CLASS_NAME<PRIMITIVE> as the resource name. The resource will be
 * placed as a child to the current plug-in. The plug-in must have
 * been implemented in the same translation unit (see 
 * [PII_IMPLEMENT_PLUGIN]).
 */
#define PII_REGISTER_OPERATION_TEMPLATE(CLASS_NAME, PRIMITIVE) \
  PII_DEFINE_SPECIALIZED_VIRTUAL_METAOBJECT_FUNCTION(CLASS_NAME, PRIMITIVE); \
  PII_SERIALIZATION_NAME_CUSTOM(CLASS_NAME::Template<PRIMITIVE>, PII_STRINGIZE(CLASS_NAME<PRIMITIVE>)); \
  PII_SERIALIZABLE_EXPORT(CLASS_NAME::Template<PRIMITIVE>); \
  static PiiClassInfoRegisterer PII_JOIN3(classInfoOf,CLASS_NAME,PRIMITIVE)(pluginName(), \
                                                                            PII_STRINGIZE(CLASS_NAME<PRIMITIVE>), \
                                                                            PiiYdin::operationResourceName(static_cast<CLASS_NAME*>(0)))

/// @internal
#define PII_POINTER_DIFF(CLASS, SUPERCLASS) reinterpret_cast<ptrdiff_t>(static_cast<SUPERCLASS*>(reinterpret_cast<CLASS*>(1)))-1

/**
 * Register *CLASS* as an instantiable object to the resource
 * database. This macro does the following:
 *
 * - Register the name of the type as a string literal to the
 * serialization library.
 *
 * - Create a factory object for the type.
 *
 * - Add two statements to the resource database: one that specifies
 * a parent-child relationship (pii:parent) between the current
 * plug-in and the class, and another that labels the class as an
 * instantiable object (pii:class) of type *SUPERCLASS*.
 *
 * - Add a pii:offset statement if *SUPERCLASS* is not the first
 * superclass of *CLASS*.
 *
 * If *CLASS* is not a descendant of *SUPERCLASS*, the registration
 * will fail at compile time. If you use a custom interface as the 
 * *SUPERCLASS*, make sure it has the class name trait defined.
 *
 * ~~~(c++)
 * class MyInterface {};
 * class MyClass : public MyInterface {};
 *
 * PII_IMPLEMENT_PLUGIN(MyPlugin);
 *
 * // MyClass is instantiable as MyInterface
 * PII_REGISTER_CLASS(MyClass, MyInterface);
 * ~~~
 */
#define PII_REGISTER_CLASS(CLASS, SUPERCLASS) \
  PII_SERIALIZATION_NAME(CLASS); \
  PII_INSTANTIATE_FACTORY(CLASS); \
  static PiiClassInfoRegisterer PII_JOIN(classInfoOf,CLASS)(pluginName(), \
                                                            PII_STRINGIZE(CLASS), \
                                                            PiiYdin::resourceName<SUPERCLASS >(), \
                                                            PII_POINTER_DIFF(CLASS, SUPERCLASS))
/**
 * Same as [PII_REGISTER_CLASS], but also registers serializers for
 * `CLASS`, which must be serializable.
 */
#define PII_REGISTER_SERIALIZABLE_CLASS(CLASS, SUPERCLASS) \
  PII_SERIALIZATION_NAME(CLASS); \
  PII_SERIALIZABLE_EXPORT(CLASS); \
  static PiiClassInfoRegisterer PII_JOIN(classInfoOf,CLASS)(pluginName(), \
                                                            PII_STRINGIZE(CLASS), \
                                                            PiiYdin::resourceName<SUPERCLASS >(), \
                                                            PII_POINTER_DIFF(CLASS, SUPERCLASS))

/**
 * Same as [PII_REGISTER_CLASS] but for class templates. This macro
 * just registers *CLASS*<TYPE> instead of *CLASS*.
 */
#define PII_REGISTER_CLASS_TEMPLATE(CLASS, TYPE, SUPERCLASS) \
  PII_SERIALIZATION_NAME_CUSTOM(CLASS<TYPE >, CLASS<TYPE>); \
  PII_INSTANTIATE_FACTORY(CLASS<TYPE >); \
  static PiiClassInfoRegisterer PII_JOIN3(classInfoOf,CLASS,TYPE)(pluginName(), \
                                                                  PII_STRINGIZE(CLASS<TYPE>), \
                                                                  PiiYdin::resourceName<SUPERCLASS >(), \
                                                                  PII_POINTER_DIFF(CLASS, SUPERCLASS))


/// @internal
#define PII_BEGIN_STATEMENTS_IMPL(STATEMENTS_CLASS) \
class PII_DECL_EXPORT STATEMENTS_CLASS \
{ \
public: \
  STATEMENTS_CLASS()                            \
  { \
    _lstIds = PiiYdin::resourceDatabase()->addStatements(statements()); \
  } \
  ~STATEMENTS_CLASS() \
  { \
    PiiYdin::resourceDatabase()->removeStatements(_lstIds); \
  } \
  static QList<PiiResourceStatement> statements(); \
private: \
  QList<int> _lstIds; \
  static STATEMENTS_CLASS instance; \
}; \
STATEMENTS_CLASS STATEMENTS_CLASS::instance; \
\
QList<PiiResourceStatement> STATEMENTS_CLASS::statements() \
{ \
  static const QList<PiiResourceStatement> lstStatements = QList<PiiResourceStatement>()

/**
 * A macro that starts a statement registration section. If you want
 * to add arbitrary statements to Ydin's global resource database, use
 * the `PII_BEGIN_STATEMENTS` and [PII_END_STATEMENTS] macros as
 * delimiters. Any number of statements can be registered in between. 
 * Note that this macro can appear only once per plugin to avoid name
 * clashes. It is however allowed to have many registration sections
 * if each of them has a globally unique name.
 *
 * ~~~(c++)
 * PII_BEGIN_STATEMENTS(PiiImagePlugin)
 *   PII_REGISTER_LITERAL_STATEMENT(PiiImageViewer, "my:opinion", "crap")
 *   PII_REGISTER_RESOURCE_STATEMENT(PiiImageViewer, "my:recursion", PiiImageViewer)
 *   // If PiiImageDisplay is used as "pii:display" for a
 *   // MyImageViewer, MyImageDisplayConnector can connect the class
 *   // instances.
 *   PII_REGISTER_CONNECTION(MyImageViewer, "pii:display", PiiImageDisplay, MyImageDisplayConnector)
 * PII_END_STATEMENTS
 * ~~~
 */
#define PII_BEGIN_STATEMENTS(PLUGIN_NAME) PII_BEGIN_STATEMENTS_IMPL(PLUGIN_NAME ## Statements)

/**
 * Insert the statement "the PREDICATE of SUBJECT is OBJECT" to the
 * resource database. This macro assumes that `OBJECT` is a string
 * literal.
 *
 * @see PII_BEGIN_STATEMENTS
 */
#define PII_REGISTER_LITERAL_STATEMENT(SUBJECT, PREDICATE, OBJECT) \
  << PiiResourceStatement(PII_STRINGIZE(SUBJECT), PREDICATE, OBJECT, PiiResourceStatement::LiteralType)

/**
 * Insert the statement "the PREDICATE of SUBJECT is OBJECT" to the
 * resource database. This macro assumes that `OBJECT` is a resource
 * identifier such as a class name (not quoted).
 *
 * @see PII_BEGIN_STATEMENTS
 */
#define PII_REGISTER_RESOURCE_STATEMENT(SUBJECT, PREDICATE, OBJECT) \
  << PiiResourceStatement(PII_STRINGIZE(SUBJECT), PREDICATE, PII_STRINGIZE(OBJECT), PiiResourceStatement::ResourceType)

/**
 * Register a named connection between two related resources. Both 
 * *SUBJECT* and *OBJECT* must be resource identifiers. The 
 * *CONNECTOR* is also a resource that configures the two related
 * resources so that they can work together. Either party of the
 * relationship can also work as the connector, which makes it
 * unnecessary to instantiate a separate connector object. The special
 * resources `pii:subject` and `pii:object` are reserved for this
 * purpose. *ROLE* is a string literal that names the connection
 * between two resources. This macro must be used between 
 * [PII_BEGIN_STATEMENTS] and [PII_END_STATEMENTS].
 *
 * ~~~(c++)
 * PII_BEGIN_STATEMENTS(MyPlugin)
 *   // MyConfiguratorWidget can work as a configurator UI for MyOperation.
 *   // When connected together in this role, MyConfiguratorWidget
 *   // takes care of setting up the connection (it is the object of
 *   // the statement).
 *   PII_REGISTER_CONNECTION(MyOperation, "pii:configurator", MyConfiguratorWidget, pii:object)
 * PII_END_STATEMENTS
 * ~~~
 *
 * @see PII_BEGIN_STATEMENTS
 * @see PiiResourceConnector
 */
#define PII_REGISTER_CONNECTION(SUBJECT, ROLE, OBJECT, CONNECTOR) \
  PII_REGISTER_RESOURCE_STATEMENT(SUBJECT, ROLE, OBJECT) \
  << PiiResourceStatement("#", PiiYdin::connectorPredicate, PII_STRINGIZE(CONNECTOR), PiiResourceStatement::ResourceType)

/**
 * Register a superclass. This macro is used to make Ydin aware of
 * multiple inheritance as the usual [PII_REGISTER_CLASS] macro
 * only takes one superclass as an argument.
 *
 * ~~~(c++)
 * class MyClass : public QObject, public MyInterface {};
 *
 * // MyClass is instantiable as a QObject
 * PII_REGISTER_CLASS(MyClass, QObject);
 *
 * PII_BEGIN_STATEMENTS(MyPlugin)
 *   // It also inherits MyInterface
 *   PII_REGISTER_SUPERCLASS(MyClass, MyInterface)
 * PII_END_STATEMENTS
 * ~~~
 *
 * @see resource_cast()
 */
#define PII_REGISTER_SUPERCLASS(CLASS, SUPERCLASS) \
  PII_REGISTER_RESOURCE_STATEMENT(CLASS, PiiYdin::classPredicate, SUPERCLASS) \
  << PiiResourceStatement("#", PiiYdin::offsetPredicate, \
                          QString::number(PII_POINTER_DIFF(CLASS, SUPERCLASS)), \
                          PiiResourceStatement::LiteralType)

/**
 * End a statement registration section. See [PII_BEGIN_STATEMENTS].
 */
#define PII_END_STATEMENTS ; return lstStatements; }

namespace PiiYdin
{
  inline const char* operationResourceName(const PiiOperation*)
  {
    return PiiYdin::resourceName<PiiOperation>();
  }

  inline const char* operationResourceName(const PiiOperationCompound*)
  {
    return PiiYdin::resourceName<PiiOperationCompound>();
  }

  inline const char* operationResourceName(const PiiEngine*)
  {
    return PiiYdin::resourceName<PiiEngine>();
  }
}

#endif //_PIIPLUGIN_H
