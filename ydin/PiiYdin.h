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

#ifndef _PIIYDIN_H
#define _PIIYDIN_H

#include <PiiGlobal.h>
#include <PiiResourceDatabase.h>

#ifdef PII_BUILD_YDIN
#  define PII_YDIN_EXPORT PII_DECL_EXPORT
#  define PII_BUILDING_YDIN 1
#else
#  define PII_YDIN_EXPORT PII_DECL_IMPORT
#  define PII_BUILDING_YDIN 0
#endif

namespace PiiYdin
{ 
  /**
   * Returns a pointer to an application-wide [resource
   * database](PiiResourceDatabase). This database is used to record
   * relationships between components in Into. The structure of the
   * resource database makes it possible to also store
   * application-specific information about plug-ins, operations, and
   * other classes. The following predicates have a special meaning:
   *
   * - `pii:parent` - specifies a parent-child relationship between
   *   two resources. This parent-child relationship has nothing to do
   *   with class hierarchies or QObject's object tree. It exists just
   *   as a general way of grouping things together. A resource may
   *   have many parents. By convention, all plug-ins should specify a
   *   parent-child relationship between the plug-in resource and the
   *   registered operations. That is, the resource database should
   *   contain a statement with "pii:parent" as the predicate for each
   *   registered operation like so:
   *   `(MyOperation,pii:parent,MyPlugin)`.
   *
   * - `pii:class` - specify a superclass for an instantiable
   *   resource. A resource may point to any number of superclass
   *   resources, which may also have superclasses. The class
   *   attribute is used by PiiYdin::createResource() to (recursively)
   *   check that the type of the class being instantiated matches the
   *   type requested. All operations should specify at least
   *   PiiOperation as their superclass like so:
   *   `(MyOperation,pii:class,PiiOperation)`.
   *
   * - `pii:offset` - used in reified statements to specify the byte
   *   offset between the start address of the class instance and that
   *   of the specified superclass. This information is used to
   *   perform cross casts between unrelated superclasses in multiple
   *   inheritance cases. The offset of the first superclass in
   *   inheritance order always has an offset of zero, and can be
   *   omitted. Let us assume `MyOperation` implements `MyInterface`.
   *   This would be stored into the resource database like so:
   *   `(MyOperation,pii:class,MyInterface)`. Consequently, the
   *   resource can be instantiated as a PiiOperation. But a pointer
   *   to a PiiOperation cannot be directly used as a pointer to a
   *   `MyInterface`; an offset must be provided. If the previous
   *   statement got 314 as the id, the offset would be stated as
   *   follows: `([314],pii:offset,16)`, where 16 stands for the
   *   number of bytes a pointer to a PiiOperation (or a QObject) must
   *   be offset to get a pointer to `MyInterface`. See
   *   [PII_REGISTER_CLASS] and [PII_REGISTER_SUPERCLASS].
   *
   * - `pii:display` - marks the object of a statement as a "display"
   *   GUI for the subject.  For example, the statement
   *   `(PiiVisualTrainer,pii:display,PiiVisualTrainerWidget)`
   *   specifies that PiiVisualTrainerWidget can work as a display GUI
   *   for PiiVisualTrainer.
   *
   * - `pii:connector` - used in reified statements to specify a
   *   PiiResourceConnector object that binds two resource instances
   *   together. Into uses connector objects to bind together things
   *   like operations and their user interfaces, classifiers and
   *   distance measures etc. A connector object is attached to a
   *   statement about a relationsip between resources. Let us assume
   *   that the statement in the previous example got a resource id of
   *   [123]. The connector that binds the operation and its GUI
   *   together would then be specified as
   *   `([123],pii:connector,PiiVisualTrainerConnector)`. The object
   *   (PiiVisualTrainerConnector) must be a class that implements the
   *   PiiResourceConnector interface. PiiYdin::connectResources()
   *   uses this information to bind resource instances together. See
   *   [PII_REGISTER_CONNECTION].
   *
   * The resource database is the main source of information related
   * to plug-ins, operations, and their interconnections. A couple of
   * useful searches are shown in the example below:
   *
   * ~~~(c++)
   * PiiResourceDatabase* db = PiiYdin::resourceDatabase();
   * using namespace Pii;
   * // Find all operations
   * QList<QString> lstOperations;
   * lstOperations = db->select(subject,
   *                            attribute("pii:class") == "PiiOperation");
   *
   * // Find all operations in the image plug-in
   * lstOperations = db->select(subject,
   *                            attribute("pii:class") == "PiiOperation") &&
   *                 db->select(subject,
   *                            attribute("pii:parent") == "PiiImagePlugin");
   * ~~~
   *
   * ! To save some bytes of memory, use Pii::classPredicate and
   * Pii::parentPredicate instead of repeating the string literals
   * "pii:class" and "pii:parent".
   */
  PII_YDIN_EXPORT PiiResourceDatabase* resourceDatabase();

  /**
   * A pointer to the string literal "pii:class". Use this instead of
   * repeating the string literal to save memory.
   */
  extern PII_YDIN_EXPORT const char* classPredicate;
  /**
   * A pointer to the string literal "pii:parent". Use this instead of
   * repeating the string literal to save memory.
   */
  extern PII_YDIN_EXPORT const char* parentPredicate;
  /**
   * A pointer to the string literal "pii:connector". Use this instead
   * of repeating the string literal to save memory.
   */
  extern PII_YDIN_EXPORT const char* connectorPredicate;
  /**
   * A pointer to the string literal "pii:offset". Use this instead of
   * repeating the string literal to save memory.
   */
  extern PII_YDIN_EXPORT const char* offsetPredicate;
  /**
   * A pointer to the string literal "pii:metaobject". Use this instead of
   * repeating the string literal to save memory.
   */
  extern PII_YDIN_EXPORT const char* metaObjectPredicate;
}

#endif //_PIIYDIN_H
