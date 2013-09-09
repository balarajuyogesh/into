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

#ifndef _PIIVARIANT_H
#define _PIIVARIANT_H

#include "PiiSharedPtr.h"
#include "PiiTemplateExport.h"
#include "PiiTypeTraits.h"
#include <PiiSerialization.h>
#include <PiiSerializationException.h>
#include <PiiSerializer.h>
#include <PiiNameValuePair.h>
#include <QHash>
#include <QVariant>

class PiiGenericOutputArchive;
class PiiGenericInputArchive;

/**
 * @file
 *
 * Declares the PiiVariant class and provides macros for making custom
 * variant types serializable.
 *
 * @ingroup Core
 */

/**
 * Declares a new custom variant type. Use this macro in a .h file.
 *
 * @relates PiiVariant
 */
#define PII_DECLARE_VARIANT_TYPE(TYPE, ID) \
  namespace Pii { template <> inline unsigned int typeId<TYPE >() { return ID; } }

/**
 * Declares a new custom variant type that will be made available to
 * software that links to this shared library. Use this macro in a .h
 * file.
 *
 * @relates PiiVariant
 */
#define PII_DECLARE_SHARED_VARIANT_TYPE(TYPE, ID, BUILDING_LIB)         \
  PII_DECLARE_EXPORTED_CLASS_TEMPLATE(struct, PiiVariant::VTableImpl<TYPE >, BUILDING_LIB); \
  PII_DECLARE_VARIANT_TYPE(TYPE, ID)

/**
 * Registers a custom type to be used as a PiiVariant. Use this macro
 * in a .cc file.
 *
 * @relates PiiVariant
 */
#define PII_REGISTER_VARIANT_TYPE(TYPE)                                 \
  PII_DEFINE_EXPORTED_CLASS_TEMPLATE(struct, PiiVariant::VTableImpl<TYPE >); \
  template <> PiiVariant::VTableImpl<TYPE >                             \
    PiiVariant::VTableImpl<TYPE >::instance(Pii::typeId<TYPE >())

/**
 * Registers @a ID as an alternative type id for @a TYPE. You must
 * register all custom type ids for non-primitive types before using
 * them. If you don't, the sofware will crash.
 *
 * @code
 * // This is a static initializer; use in the global scope
 * PII_MAP_VARIANT_ID_TO_TYPE(0x55378008, QString);
 *
 * // Somewhere else in your code
 * PiiVariant var(QString("Alternative id"), 0x55378008);
 * @endcode
 *
 * @relates PiiVariant
 */
#define PII_MAP_VARIANT_ID_TO_TYPE(ID,TYPE)     \
  template struct PiiVariant::TypeIdMapper<ID>; \
  template <> PiiVariant::TypeIdMapper<ID> PiiVariant::TypeIdMapper<ID>::instance(Pii::typeId<TYPE>())

/// @cond null
#define PII_MAP_PUT_NTH(N,TYPE) PII_MAP_PUT_SELF(TYPE);


// A type map that maps primitive types to themselves and other types
// to const references.
PII_TYPEMAP(PiiVariantValueMap)
{
  PII_MAP_PUT_DEFAULT(const _T_&);
  PII_FOR_N(PII_MAP_PUT_NTH, 12,
            (char, short, int, qint64,
             unsigned char, unsigned short, unsigned int, quint64,
             float, double, bool, void*));
};
/// @endcond

/**
 * An extensible variant class that can store any data type.
 *
 * A type ID is used in determining the type of the stored object. To
 * check the type of a variant, do the following:
 *
 * @code
 * PiiVariant variant(3);
 * if (variant.type() == PiiVariant::IntType)
 *   int i = variant.valueAs<int>();
 * @endcode
 *
 * Any type can be made compatible with PiiVariant. For this, one
 * needs to first choose a unique variant type ID for the type. The
 * mechanism differs from QVariant's type registration technique 1)
 * for run-time performance reasons and 2) to ensure a static type id
 * across all computing environments. Use the @ref
 * PII_DECLARE_VARIANT_TYPE macro to assign a type ID to a type. The
 * new type must then be registered by the @ref
 * PII_REGISTER_VARIANT_TYPE macro.
 *
 * @code
 * // MyClass.h
 * class MyClass {};
 *
 * // First do whatever it takes to make the class itself
 * // serializable. In the simplest case just this:
 * #define PII_SERIALIZABLE_CLASS MyClass
 * #include <PiiSerializableRegistration.h>
 *
 * PII_DECLARE_VARIANT_TYPE(MyClass, 0x666);
 *
 * // In MyClass.cc
 * #include "MyClass.h"
 * PII_REGISTER_VARIANT_TYPE(MyClass);
 * @endcode
 *
 * @ingroup Core
 */
class PII_CORE_EXPORT PiiVariant
{
  friend struct PiiSerialization::Accessor;
  static const char* pValueStr;

  template <class Archive> void serialize(Archive& archive, const unsigned int /*version*/)
  {
    archive & PII_NVP("id", _uiType);
    if (Archive::InputArchive)
      {
        if (_pVTable != 0) _pVTable->destruct(*this);
        _pVTable = vTableByType(_uiType);
      }
    // All primitive types are serialized directly here
    if (_uiType <= LastPrimitiveType)
      {
        switch (_uiType)
          {
#define PII_VARIANT_SERIALIZE_PRIMITIVE(NAME, PREFIX)                   \
            case NAME ## Type: archive & PII_NVP(pValueStr, _value.PREFIX ## Value); break
            
            // Search in the order of (supposed) prevalence
            PII_VARIANT_SERIALIZE_PRIMITIVE(Int, i);
            PII_VARIANT_SERIALIZE_PRIMITIVE(Double, d);
            PII_VARIANT_SERIALIZE_PRIMITIVE(Bool, b);
            PII_VARIANT_SERIALIZE_PRIMITIVE(Float, f);
            PII_VARIANT_SERIALIZE_PRIMITIVE(Invalid, i);
            PII_VARIANT_SERIALIZE_PRIMITIVE(UnsignedInt, ui);

            PII_VARIANT_SERIALIZE_PRIMITIVE(Char, c);
            PII_VARIANT_SERIALIZE_PRIMITIVE(Short, s);
            PII_VARIANT_SERIALIZE_PRIMITIVE(Int64, l);
      
            PII_VARIANT_SERIALIZE_PRIMITIVE(UnsignedChar, uc);
            PII_VARIANT_SERIALIZE_PRIMITIVE(UnsignedShort, us);
            PII_VARIANT_SERIALIZE_PRIMITIVE(UnsignedInt64, ul);
            
#undef PII_VARIANT_SERIALIZE_PRIMITIVE
          }
      }
    // InvalidType doesn't need serialization. Custom types do. If
    // there is no vtable, we can't do much here.
    else if (_uiType != InvalidType && _pVTable != 0)
      Pii::If<Archive::OutputArchive, Saver, Loader>::Type::template serialize(archive, *this);
  }

  struct Saver
  {
    template <class Archive> static void serialize(Archive& ar, PiiVariant& var)
    {
      var._pVTable->save(ar, var);
    }
  };

  struct Loader
  {
    template <class Archive> static void serialize(Archive& ar, PiiVariant& var)
    {
      var._pVTable->load(ar, var);
    }
  };
    
  
public:
  /**
   * An enumeration for primitive types. The names correspond to
   * primitive C++ types. @p InvalidType (0xffffffff) indicates an
   * unknown type. The type IDs are composed so that their categories
   * can be quickly determined by bit masking. The system is analogous
   * to the netaddress/netmask system with IPv4 addresses.
   *
   * Primitive types cover the IDs 0-31. Thus, they have a
   * "netaddress" of 0 and a netmask of ~0x1f. Testing for
   * "primitiveness" is just @p anding with ~0x1f (0xffffffe0) and
   * comparing for zero.
   *
   * When adding template instances as custom variant types, it is
   * adviced to follow the convention that the lowest bits of the type
   * ID always tell the primitive type, if possible. It is then
   * possible to quickly check for "integerness" or "floatness" of any
   * template type. For example, assume that the "subnet" 1/~0xff (IDs
   * 0x100 - 0x1ff) is reserved for your template type @p MyType<T>. 
   * Then, 0x100 (0x100 + @p CharType) should be @p MyType<@p char>
   * and so on.
   */
  enum PrimitiveType
    {
      CharType = 0x00,
      ShortType,
      IntType,
      Int64Type,
      
      UnsignedCharType = 0x08,
      UnsignedShortType,
      UnsignedIntType,
      UnsignedInt64Type,
      
      FloatType = 0x10,
      DoubleType,
      //LongDoubleType,
      
      BoolType = 0x18,
      VoidPtrType = 0x19,
      LastPrimitiveType = VoidPtrType,

      InvalidType = 0xffffffff
    };

  /**
   * Creates an invalid variant.
   */
  PiiVariant();

  explicit inline PiiVariant(char);
  explicit inline PiiVariant(short);
  explicit inline PiiVariant(int);
  explicit inline PiiVariant(qint64);
  explicit inline PiiVariant(unsigned char);
  explicit inline PiiVariant(unsigned short);
  explicit inline PiiVariant(unsigned int);
  explicit inline PiiVariant(quint64);
  explicit inline PiiVariant(float);
  explicit inline PiiVariant(double);
  explicit inline PiiVariant(bool);
  explicit inline PiiVariant(void*);

  /**
   * Creates a variant that stores @a value. The type @p T must be
   * declared with the @ref PII_DECLARE_VARIANT_TYPE macro and
   * registered with the @ref PII_REGISTER_VARIANT_TYPE macro.
   */
  template <class T> explicit PiiVariant(const T& value);

  /**
   * Creates a variant with a non-default type ID. If you want to give
   * a special meaning to a variant while still storing its actual
   * value as, say, an @p int, you can do this:
   *
   * @code
   * const int MyCustomTypeId = 0x31415927;
   * PiiVariant var(3, MyCustomTypeId);
   * @endcode
   *
   * @note If T is a non-primitive type, you must connect it to your
   * custom @a typeId with @ref PII_MAP_VARIANT_ID_TO_TYPE.
   */
  template <class T> PiiVariant(T value, unsigned int typeId, typename Pii::OnlyPrimitive<T>::Type = 0);
  template <class T> PiiVariant(T value, unsigned int typeId, typename Pii::OnlyNonPrimitive<T,int>::Type = 0);

  /**
   * Creates a copy of @a other.
   */
  PiiVariant(const PiiVariant& other);

  /**
   * Copies the contents of @a other to @p this.
   */
  PiiVariant& operator= (const PiiVariant& other);

  /**
   * Destroys the variant.
   */
  ~PiiVariant();
  
  /**
   * Returns @p true if this variant is a primitive type, @p false
   * otherwise.
   */
  bool isPrimitive() const { return type() <= LastPrimitiveType; }
  /**
   * Returns @p true if @a type represents a primitive type, @p false
   * otherwise.
   */
  static bool isPrimitive(unsigned int type) { return type <= LastPrimitiveType; }
  
  /**
   * Returns @p true if this variant is an integer type (char, short,
   * int, qint64) and @p false otherwise. Note that the function may
   * return true even if the type is not primitive, because it is
   * sometimes useful to be able to check for "integerness" of a
   * template type. To check whether the variant is a "primitive
   * integer", use
   *
   * @code
   * if (variant.isPrimitive() && variant.isInteger())
   *   dealWithAnIntegerType(variant);
   * @endcode
   */
  bool isInteger() const { return (type() & 0x10) == 0; }
  /**
   * Returns @p true if @a type represents an integer type (char,
   * short, int, qint64) and @p false otherwise.
   */
  static bool isInteger(unsigned int type) { return (type & 0x10) == 0; }

  /**
   * Returns @p true if this variant is a floating point type and @p
   * false otherwise. See #isInteger() for more information.
   */
  bool isFloat() const { return (type() & 0x18) == 0x10; }
  /**
   * Returns @p true if @p type represents a floating point type and
   * @p false otherwise. See #isInteger() for more information.
   */
  static bool isFloat(unsigned int type) { return (type & 0x18) == 0x10; }
  
  /**
   * Returns @p true if this variant is an unsigned integer type and
   * @p false otherwise. See isInteger() for more information.
   */
  bool isUnsigned() const { return (type() & 0x18) == 0x8; }
  /**
   * Returns @p true if @a type represents an unsigned integer type
   * and @p false otherwise. See isInteger() for more information.
   */
  static bool isUnsigned(unsigned int type) { return (type & 0x18) == 0x8; }

  /**
   * Returns @p true if the type of this variant is not @p
   * InvalidType, and @p false otherwise.
   */
  bool isValid() const { return type() != InvalidType; }
  /**
   * Returns @p true if @a type is different from @p InvalidType, and
   * @p false otherwise.
   */
  static bool isValid(unsigned int type) { return type != InvalidType; }
      
  /**
   * Get the type of the variant. To compare for a certain type, you
   * may do the following:
   *
   * @code
   * if (variant.type() == PiiVariant::DoubleType)
   *   calculateAccurately(variant.valueAs<double>());
   * else if (variant.type() == MyOwnTypeId)
   *   doSomethingElse(variant.valueAs<MyOwnTypeId>());
   * @endcode
   */
  unsigned int type() const { return _uiType; }

  /**
   * Returns the value of the variant as a @p T.
   *
   * @note This function doesn't check that the stored value actually
   * is of type @p T. The entire responsibility for checking the type
   * is at the caller.
   *
   * @return a @p const reference to the wrapped object, if the type
   * @p T is not one of the primitive types supported by %PiiVariant. 
   * Otherwise, returns the primitive type as a value.
   */
  template <class T> inline typename PII_MAP_TYPE(PiiVariantValueMap, T) valueAs() const
  {
    return *ptrAs<T>();
  }

  template <class T> inline T& valueAs()
  {
    return *ptrAs<T>();
  }

private:
  /* This class plays chicken with binary compatibility. There is no
     d-pointer because PiiVariant is the most common data type used in
     Into, and its allocations and deallocations must be really fast. 
     Therefore, all unnecessary heap allocations must be avoided. The
     idea is to store a copy of the class instance into _buffer if it
     fits there. Otherwise, the data must be placed into heap, and
     _pointer will point to its location.
   */
  template <class T> struct SmallObjectFunctions;
  template <class T> struct LargeObjectFunctions;
  template <class T> struct VTableImpl;
  template <class T> friend struct VTableImpl;
  template <unsigned int typeId> struct TypeIdMapper;
  template <unsigned int typeId> friend struct TypeIdMapper;
  
  /* A changeable virtual function table. Different types have
     different implementations, and we must thus be able to change the
     virtual table. Using a C++ interface for this would cost an
     additional memory indirection.
   */
  struct VTable
  {
    void (*constructCopy)(PiiVariant&, const PiiVariant&);
    void (*destruct)(PiiVariant&);
    void (*copy)(PiiVariant&, const PiiVariant&);
    void (*save)(PiiGenericOutputArchive&, const PiiVariant&);
    void (*load)(PiiGenericInputArchive&, PiiVariant&);
  } *_pVTable;

  unsigned int _uiType;

  union Value
  {
    char cValue;
    short sValue;
    int iValue;
    qint64 lValue;
    unsigned char ucValue;
    unsigned short usValue;
    unsigned int uiValue;
    quint64 ulValue;
    float fValue;
    double dValue;
    bool bValue;
    void* pValue;
  };
  // This union holds either a pointer to a heap-allocated object, a
  // primitive value as such, or a non-pod type in a preallocated
  // buffer.
  union
  {
    void* _pointer;
    Value _value;
    char _buffer[sizeof(Value)];
  };

  // The maximum size for a stored object equals the size of the Value
  // union.
  enum { InternalBufferSize = sizeof(Value) };

  template <class T> inline const T*
    ptrAs(typename Pii::OnlyIf<sizeof(T) <= InternalBufferSize>::Type = 0) const
  {
    return reinterpret_cast<const T*>(_buffer);
  }
  template <class T> inline T*
    ptrAs(typename Pii::OnlyIf<sizeof(T) <= InternalBufferSize>::Type = 0)
  {
    return reinterpret_cast<T*>(_buffer);
  }

  template <class T> inline const T*
    ptrAs(typename Pii::OnlyIf<(sizeof(T) > InternalBufferSize)>::Type = 0) const
  {
    return reinterpret_cast<const T*>(_pointer);
  }

  template <class T> inline T*
    ptrAs(typename Pii::OnlyIf<(sizeof(T) > InternalBufferSize)>::Type = 0)
  {
    return reinterpret_cast<T*>(_pointer);
  }

  static VTable* vTableByType(unsigned int type);
  static QHash<unsigned int, VTable*>* hashVTables();
};


namespace Pii
{
  /**
   * A global function that returns the corresponding variant type ID
   * for any type. A specialization is provided for each primitive
   * type. One should write more specializations for custom types. 
   * There is no default implementation.
   *
   * @internal
   */
  template <class T> unsigned int typeId();
}

/// @cond null

template <class T> PiiVariant::PiiVariant(const T& value) :
  _pVTable(&VTableImpl<T>::instance),
  _uiType(Pii::typeId<T>())
{
  if (sizeof(T) <= InternalBufferSize)
    new ((void*)_buffer) T(value);
  else
    _pointer = new T(value);
}

template <class T> PiiVariant::PiiVariant(T value, unsigned int typeId, typename Pii::OnlyPrimitive<T>::Type) :
  _pVTable(0),
  _uiType(typeId)
{
  *ptrAs<T>() = value;
}

template <class T> PiiVariant::PiiVariant(T value, unsigned int typeId, typename Pii::OnlyNonPrimitive<T,int>::Type) :
  _pVTable(&VTableImpl<T>::instance),
  _uiType(typeId)
{
  if (sizeof(T) <= InternalBufferSize)
    new ((void*)_buffer) T(value);
  else
    _pointer = new T(value);
}

template <class T> struct PiiVariant::SmallObjectFunctions
{
  static void constructCopyImpl(PiiVariant& to, const PiiVariant& from)
  {
    new (to._buffer) T(*from.ptrAs<T>());
  }
  
  static void destructImpl(PiiVariant& var)
  {
    var.ptrAs<T>()->~T();
  }
  
  static void copyImpl(PiiVariant& to, const PiiVariant& from)
  {
    *to.ptrAs<T>() = *from.ptrAs<T>();
  }

  static void saveImpl(PiiGenericOutputArchive& archive, const PiiVariant& var)
  {
    archive << *var.ptrAs<T>();
  }

  static void loadImpl(PiiGenericInputArchive& archive, PiiVariant& var)
  {
    T* obj = new (var._buffer) T;
    archive >> *obj;
  }
};

template <class T> struct PiiVariant::LargeObjectFunctions
{
  static void constructCopyImpl(PiiVariant& to, const PiiVariant& from)
  {
    to._pointer = new T(*from.ptrAs<T>());
  }
  
  static void destructImpl(PiiVariant& var)
  {
    delete var.ptrAs<T>();
  }
  
  static void copyImpl(PiiVariant& to, const PiiVariant& from)
  {
    *to.ptrAs<T>() = *from.ptrAs<T>();
  }

  static void saveImpl(PiiGenericOutputArchive& archive, const PiiVariant& var)
  {
    archive << *var.ptrAs<T>();
  }

  static void loadImpl(PiiGenericInputArchive& archive, PiiVariant& var)
  {
    var._pointer = new T;
    archive >> *reinterpret_cast<T*>(var._pointer);
  }
};

template <class T> struct PiiVariant::VTableImpl :
  PiiVariant::VTable,
  Pii::If<sizeof(T) <= PiiVariant::InternalBufferSize,
                       PiiVariant::SmallObjectFunctions<T>,
                       PiiVariant::LargeObjectFunctions<T> >::Type
{
  typedef typename Pii::If<sizeof(T) <= PiiVariant::InternalBufferSize,
                           PiiVariant::SmallObjectFunctions<T>,
                           PiiVariant::LargeObjectFunctions<T> >::Type ParentType;
  VTableImpl(unsigned int type)
  {
    this->constructCopy = ParentType::constructCopyImpl;
    this->destruct = ParentType::destructImpl;
    this->copy = ParentType::copyImpl;
    this->save = ParentType::saveImpl;
    this->load = ParentType::loadImpl;

    PiiVariant::hashVTables()->insert(type, this);
  }
  // PENDING this may trigger static initialization order fiasco with TypeIdMapper
  static VTableImpl instance;
};

template <unsigned int typeId> struct PiiVariant::TypeIdMapper
{
  TypeIdMapper(unsigned int type)
  {
    PiiVariant::hashVTables()->insert(typeId, PiiVariant::vTableByType(type));
  }
  static TypeIdMapper instance;
};

#define PII_PRIMITIVE_VARIANT_DECL(TYPE, PREFIX, NAME)                  \
  PII_DECLARE_SHARED_VARIANT_TYPE(TYPE, PiiVariant::NAME ## Type, PII_BUILDING_CORE); \
  inline PiiVariant::PiiVariant(TYPE val) : _pVTable(0), _uiType(NAME ## Type) { _value.PREFIX ## Value = val; } \
  template <> inline PII_MAP_TYPE(PiiVariantValueMap, TYPE) PiiVariant::valueAs<TYPE>() const { return _value.PREFIX ## Value; } \
  template <> inline TYPE& PiiVariant::valueAs<TYPE>() { return _value.PREFIX ## Value; } \
  namespace Dummy {}
  

PII_PRIMITIVE_VARIANT_DECL(char, c, Char);
PII_PRIMITIVE_VARIANT_DECL(short, s, Short);
PII_PRIMITIVE_VARIANT_DECL(int, i, Int);
PII_PRIMITIVE_VARIANT_DECL(qint64, l, Int64);
PII_PRIMITIVE_VARIANT_DECL(unsigned char,  uc, UnsignedChar);
PII_PRIMITIVE_VARIANT_DECL(unsigned short, us, UnsignedShort);
PII_PRIMITIVE_VARIANT_DECL(unsigned int, ui, UnsignedInt);
PII_PRIMITIVE_VARIANT_DECL(quint64, ul, UnsignedInt64);
PII_PRIMITIVE_VARIANT_DECL(float, f, Float);
PII_PRIMITIVE_VARIANT_DECL(double, d, Double);
//PII_PRIMITIVE_VARIANT_DECL(long double, ld, LongDouble);
PII_PRIMITIVE_VARIANT_DECL(bool, b, Bool);
PII_PRIMITIVE_VARIANT_DECL(void*, p, VoidPtr);

extern PII_CORE_EXPORT int iPiiVariantTypeId;
Q_DECLARE_METATYPE(PiiVariant);

PII_SERIALIZATION_TRACKING(PiiVariant, false);

extern PII_CORE_EXPORT int iPiiVariantListTypeId;
typedef QList<PiiVariant> PiiVariantList;
Q_DECLARE_METATYPE(PiiVariantList);

#define PII_SERIALIZABLE_CLASS PiiVariant
#define PII_BUILDING_LIBRARY PII_BUILDING_CORE
#define PII_USED_AS_QVARIANT
#include <PiiSerializableRegistration.h>

#define PII_SERIALIZABLE_CLASS PiiVariantList
#define PII_BUILDING_LIBRARY PII_BUILDING_CORE
#define PII_USED_AS_QVARIANT
#include "PiiSerializableRegistration.h"

/// @endcond

namespace Pii
{
  /**
   * A utility function that creates a QVariant holding a
   * PiiVariant that in turn holds the given @p value. A shorthand for
   * QVariant::fromValue(PiiVariant(value)). This function is useful in
   * setting properties whose type is PiiVariant.
   *
   * @code
   * classifier->setProperty("codeBook", Pii::createQVariant(PiiMatrix<int>(2, 2,
   *                                                                        1, 2,
   *                                                                        3, 4)));
   * @endcode
   *
   * @relates PiiVariant
   */
  template <class T> QVariant createQVariant(const T& value)
  {
    return QVariant::fromValue(PiiVariant(value));
  }
}


#endif //_PIIVARIANT_H
