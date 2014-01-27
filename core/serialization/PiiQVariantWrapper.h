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

#ifndef _PIIQVARIANTWRAPPER_H
#define _PIIQVARIANTWRAPPER_H

#include <QVariant>
#include "PiiSerialization.h"
#include "PiiVirtualMetaObject.h"

/**
 * A serialization wrapper for user types in [QVariant]
 * "QVariants". To enable serialization of a user type as a QVariant,
 * one needs to subclass PiiQVariantWrapper. The subclass must be
 * registered as a serializable object with the standard template
 * serializer naming syntax, e.g. "PiiQVariantWrapper<MyType>". The
 * serializer needs to store and retrieve "MyType" and store it as the
 * value of the internal [_variant] member.
 *
 * ~~~(c++)
 * // In MyClass.h first declare the type as a Qt meta type:
 * extern int iMyClassTypeId;
 * Q_DECLARE_METATYPE(MyClass);
 *
 * // In MyClass.cc register the meta type:
 * int iMyClassTypeId = qRegisterMetaType<MyClass>("MyClass");
 *
 * // Then register the wrapper as a serializable object:
 * #include <PiiQVariantWrapper.h>
 * #define PII_SERIALIZABLE_CLASS PiiQVariantWrapper::Template<MyClass>
 * #define PII_SERIALIZABLE_CLASS_NAME "PiiQVariantWrapper<MyClass>"
 *
 * #include <PiiSerializableRegistration.h>
 * ~~~
 *
 */
class PII_SERIALIZATION_EXPORT PiiQVariantWrapper
{
  friend struct PiiSerialization::Accessor;
  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION = 0;
  template <class Archive> void serialize(Archive&, const unsigned int) {}

public:
  /**
   * Returns the wrapped value as a variant. Subclasses reimplement
   * this function and wrap the actual value into a QVariant.
   */
  virtual void moveTo(QVariant& variant) = 0;

  /**
   * Set the variant to `v`, which stores a user type. Subclasses
   * reimplement this function to store the value inside the variant.
   */
  virtual void setVariant(QVariant& v) = 0;
  void setVariant(const QVariant& v) { setVariant(const_cast<QVariant&>(v)); }

  template <class T> class Template;

  virtual ~PiiQVariantWrapper();
};

/**
 * A template that is instantiated for all user types intended to be
 * serialized as QVariants. For each such user type, an instance of
 * this template must be compiled, and its serializer and factory must
 * be registered. To make `MyType` serializable as a QVariant, do
 * this in a .cc file:
 *
 * ~~~(c++)
 * #include <PiiQVariantWrapper.h>
 * #define PII_SERIALIZABLE_CLASS PiiQVariantWrapper::Template<MyType>
 * #define PII_SERIALIZABLE_CLASS_NAME "PiiQVariantWrapper<MyType>"
 * #include <PiiSerializableRegistration.h>
 * ~~~
 *
 */
template <class T> class PiiQVariantWrapper::Template : public PiiQVariantWrapper
{
  friend struct PiiSerialization::Accessor;
  PII_VIRTUAL_METAOBJECT_FUNCTION
  PII_SEPARATE_SAVE_LOAD_MEMBERS
  template <class Archive> void save(Archive& archive, const unsigned int)
  {
    archive << *_pValue;
  }
  template <class Archive> void load(Archive& archive, const unsigned int)
  {
    archive >> _value;
  }

public:
  Template() : _pValue(0) {}

  void moveTo(QVariant& v)
  {
    v = QVariant::fromValue(_value);
  }

  void setVariant(QVariant& v)
  {
    _pValue = (T*)v.constData();
  }

private:
  T* _pValue;
  T _value;
};

// Tracking is unnecessary because we only use the wrappers as
// temporary objects in storing and restoring user types.
PII_SERIALIZATION_TRACKING(PiiQVariantWrapper, false);
PII_SERIALIZATION_VIRTUAL_METAOBJECT(PiiQVariantWrapper);
PII_SERIALIZATION_ABSTRACT(PiiQVariantWrapper);

#endif //_PIIQVARIANTWRAPPER_H
