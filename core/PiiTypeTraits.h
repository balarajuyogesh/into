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

#ifndef _PIITYPETRAITS_H
#define _PIITYPETRAITS_H

#include "PiiMetaTemplate.h"

namespace std { template <class T> class complex; }

namespace Pii
{
  /**
   * @group pii_static_type_checking Static type checking
   *
   * This group contains structures that can be used for static type
   * checking in template meta-programming. The structures can be used
   * as checks for conditional structrures such as Pii::If to compile
   * different versions of code for different types.
   *
   * All `IsXXXX` structs implement a similar design pattern:
   *
   * ~~~(c++)
   * ASSERT(Pii::IsPointer<int>::boolValue == false);
   * ASSERT(Pii::IsPointer<int*>::boolValue == true);
   * ASSERT(Pii::IsArray<char[]>::boolValue == true);
   * // etc...
   * ~~~
   *
   * Static type checking is very useful in template meta-programs.
   *
   * ~~~(c++)
   * // Divide by two by shifting an integer one position to the right.
   * struct IntOp { static inline int divide(int value) { return value >> 1; } };
   * // Divide by two by multiplying by 0.5
   * struct FloatOp { static inline float divide(float value) { return value * 0.5; } };
   * // ...
   * template <class T> T calculate(T value)
   * {
   *   // This could be made much simpler with template specialization, but
   *   // it illustrates the idea anyway.
   *   return Pii::IfClass<Pii::IsInteger<T>,
   *                       IntOp, FloatOp>::Type::divide(value);
   * };
   * ~~~
   */

  /**
   * An empty structure.
   */
  struct Empty {};

  /**
   * A structure whose nested type, `Type`, is `T`.
   */
  template <class T> struct Id { typedef T Type; };

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is `void`.
   */
  template <class T> struct IsVoid : False {};
  template <> struct IsVoid<void> : True {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a pointer.
   */
  template <class T> struct IsPointer : False {};
  template <class T> struct IsPointer<T*> : True {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a reference.
   */
  template <class T> struct IsReference : False {};
  template <class T> struct IsReference<T&> : True {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is an array.
   */
  template <class T> struct IsArray : False {};
  template <class T> struct IsArray<T[]> : True {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a signed integer type.
   */
  template <class T> struct IsSigned : False {};
  template <> struct IsSigned<char> : True {};
  template <> struct IsSigned<short> : True {};
  template <> struct IsSigned<int> : True {};
  template <> struct IsSigned<long> : True {};
  template <> struct IsSigned<long long> : True {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is an unsigned integer
   * type.
   */
  template <class T> struct IsUnsigned : False {};
  template <> struct IsUnsigned<unsigned char> : True {};
  template <> struct IsUnsigned<unsigned short> : True {};
  template <> struct IsUnsigned<unsigned int> : True {};
  template <> struct IsUnsigned<unsigned long> : True {};
  template <> struct IsUnsigned<unsigned long long> : True {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is an integer type (either
   * signed or unsigned).
   */
  template <class T> struct IsInteger : Or<IsSigned<T>::boolValue, IsUnsigned<T>::boolValue> {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a floating point number
   * type.
   */
  template <class T> struct IsFloatingPoint : False {};
  template <> struct IsFloatingPoint<float> : True {};
  template <> struct IsFloatingPoint<double> : True {};
  template <> struct IsFloatingPoint<long double> : True {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a numeric primitive
   * (non-bool) type.
   */
  template <class T> struct IsNumeric : Or<IsSigned<T>::boolValue,
                                           IsUnsigned<T>::boolValue,
                                           IsFloatingPoint<T>::boolValue>
  {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a primitive type.
   */
  template <class T> struct IsPrimitive : IsNumeric<T> {};
  template <> struct IsPrimitive<bool> : True {};

  /**
   * A structure that has a nested type `Type` if and only if `T` is
   * a primitive type. This can be exploited as a *substitution
   * failure is not an error* (SFINAE) trick to guide overload
   * resolution.
   *
   * ~~~(c++)
   * // I want this function to be called for primitive types only
   * template <class T> void func(T value, typename Pii::OnlyPrimitive<T>::Type = 0)
   * {}
   *
   * // This will be called for Base and all derived types.
   * void func(const Base& base)
   * {}
   *
   * // Derived inherits Base
   * Derived d;
   * func(d); // calls func(const Base&)
   * ~~~
   *
   * If you left the second dummy parameter out, the template function
   * would be called for types derived from Base, as the template type
   * `T` is always an exact match. Since
   * `Pii::OnlyPrimitive<T>::Type` only exists if `T` is a
   * primitive type, the first function causes a substitution failure
   * for all other types, and will be left out of oveload resolution.
   *
   * The second template parameter can be used to change the type the
   * structure resolves to. By default, Pii::OnlyPrimitive<T>::Type is
   * T for all primitive types, but Pii::OnlyPrimitive<T,int>::Type is
   * an `int`.
   */
  template <class T, class U = T> struct OnlyPrimitive : IfClass<IsPrimitive<T>, Id<U>, Empty>::Type {};

  /**
   * A structure that has a nested type `Type` if and only if `T` is
   * not a primitive type.
   *
   * @see OnlyPrimitive
   */
  template <class T, class U = T> struct OnlyNonPrimitive : IfClass<IsPrimitive<T>, Empty, Id<U> >::Type {};

  /**
   * A structure that has a nested type `Type` if and only if `T` is
   * a floating-point type.
   *
   * @see OnlyPrimitive
   */
  template <class T, class U = T> struct OnlyFloatingPoint : IfClass<IsFloatingPoint<T>, Id<U>, Empty>::Type {};

  /**
   * A structure that has a nested type `Type` if and only if `T` is
   * a numeric primitive type.
   *
   * @see OnlyPrimitive
   */
  template <class T, class U = T> struct OnlyNumeric : IfClass<IsNumeric<T>, Id<U>, Empty>::Type {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a complex number
   * (std::complex).
   */
  template <class T> struct IsComplex : False {};
  template <class T> struct IsComplex<std::complex<T> > : True {};

  /**
   * A structure that has a nested type `Type` if and only if `T` is
   * a numeric primitive type.
   *
   * @see OnlyPrimitive
   */
  template <class T, class U = T> struct OnlyComplex : IfClass<IsComplex<T>, Id<U>, Empty>::Type {};

  template <class T, class U = T> struct OnlyNonComplex : IfClass<IsComplex<T>, Empty, Id<U> >::Type {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a numeric primitive
   * (non-bool) type or a complex number (std::complex).
   */
  template <class T> struct IsNumericOrComplex : Or<IsNumeric<T>::boolValue,
                                                    IsComplex<T>::boolValue>
  {};

  /**
   * A structure that has a nested type `Type` if and only if `T` is
   * a numeric primitive type or a complex number.
   *
   * @see Primitive
   */
  template <class T, class U = T> struct OnlyNumericOrComplex : IfClass<IsNumericOrComplex<T>, Id<U>, Empty>::Type {};

  /**
   * A structure that has a nested `Type` if and only if `rule` is
   * `true`. In other words, OnlyIf<true,T>::Type is T, whereas
   * OnlyIf<false,T>::Type does not exists.
   */
  template <bool rule, class T = int> struct OnlyIf : If<rule, Id<T>, Empty>::Type {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameter `T` is a `const` type.
   */
  template <class T> struct IsConst : False {};
  template <class T> struct IsConst<const T> : True {};
  template <class T> struct IsConst<const T*> : True {};
  template <class T> struct IsConst<const T&> : True {};

  /**
   * A tester struct whose `boolValue` member evaluates statically to
   * `true` if the template parameters `T` and `U` are the same
   * type.
   */
  template <class T, class U> struct IsSame : False {};
  template <class T> struct IsSame<T,T> : True {};

  /**
   * A structure whose `boolValue` member evaluates statically to
   * `true` if (and only if) `Base` is a base class of `Derived`. Note
   * that instances of `Derived` are instances of `Base` also if
   * `Base` and `Derived` are the same type. Therefore, this trait will
   * be `true` if `Base` and `Derived` are the same type.
   */
  template <class Base, class Derived> struct IsBaseOf
  {
    /// @hide
    typedef char (&Yes)[1];
    typedef char (&No)[2];

    template <class T> static Yes isBaseOf(Derived*, T);
    static No isBaseOf(Base*, int);

    struct Converter
    {
      operator Base*() const;
      operator Derived*();
    };

    // New GCC and MSVC versions provide __is_base_of() operator as an
    // extension, but it is unfortunately not available everywhere.
    // Thus, we need to resort to this ugly hack. The explanation is
    // too long to be written here. See
    // http://stackoverflow.com/questions/2910979/how-is-base-of-works
    enum { boolValue = sizeof(isBaseOf(Converter(), int(0))) == sizeof(Yes) ||
           IsSame<Base,Derived>::boolValue };
    /// @endhide
  };

  /**
   * A structure whose member type `Type` is a non-const version of
   * the template parameter `T`.
   */
  template <class T> struct ToNonConst { typedef T Type; };
  template <class T> struct ToNonConst<const T> { typedef T Type; };


  /**
   * A static functor that converts pointers to references and keeps
   * references as such.
   *
   * ~~~(c++)
   * int i = 1, j;
   * // Both of these set j to 1
   * j = ToReference<int>::apply(i);
   * j = ToReference<int*>::apply(&i);
   * ~~~
   *
   * The `Type` member of this class evaluates to the non-pointer
   * type. That is, ToReference<T>::Type is `T`&, and so is
   * ToReference<T*>::Type.
   */
  template <class T> struct ToReference
  {
    typedef T& Type;
    static T& apply(T& value) { return value; }
  };

  template <class T> struct ToReference<T*>
  {
    typedef T& Type;
    static T& apply(T* value) { return *value; }
  };

  /**
   * A static functor that converts references to pointers and keeps
   * pointers as such.
   *
   * @see ToReference
   */
  template <class T> struct ToPointer
  {
    typedef T* Type;
    static T* apply(T& value) { return &value; }
  };

  template <class T> struct ToPointer<T*>
  {
    typedef T* Type;
    static T* apply(T* value) { return value; }
  };

  /**
   * A static functor that converts references to values and keeps
   * other types as such.
   *
   * @see ToReference
   */
  template <class T> struct RefToValue
  {
    typedef T Type;
    static T apply(T& value) { return value; }
  };

  template <class T> struct RefToValue<T&>
  {
    typedef typename ToNonConst<T>::Type Type;
    static Type apply(T& value) { return value; }
  };

  /**
   * A static functor that converts pointers to values and keeps other
   * types as such.
   *
   * @see ToReference
   */
  template <class T> struct PtrToValue
  {
    typedef T Type;
    static T apply(T& value) { return value; }
  };

  template <class T> struct PtrToValue<T*>
  {
    typedef typename ToNonConst<T>::Type Type;
    static Type apply(T* value) { return *value; }
  };

  template <class T> struct ToValue { typedef T Type; };
  template <class T> struct ToValue<T*> { typedef T Type; };
  template <class T> struct ToValue<T&> { typedef T Type; };

  /**
   * Converts a type to a corresponding floating-point type. The
   * `Type` member of this structure is `float` for all primitive types
   * except `double` and `long` `double`, for which no conversion
   * will be made. Composed types such as PiiMatrix should specialize
   * this structure:
   *
   * ~~~(c++)
   * namespace Pii
   * {
   *   template <class T> struct ToFloatingPoint<PiiMatrix<T> >
   *   {
   *     typedef PiiMatrix<typename ToFloatingPoint<T>::Type> Type;
   *     typedef typename ToFloatingPoint<T>::Type PrimitiveType;
   *   };
   * }
   * ~~~
   */
  template <class T> struct ToFloatingPoint
  {
    /**
     * Primitive types are converted to `floats` by default.
     */
    typedef float Type;
    /**
     * Primitive types are primitive, so the `PrimitiveType` equals
     * `Type`.
     */
    typedef float PrimitiveType;
  };

  /**
   * Specialization for `double`.
   */
  template <> struct ToFloatingPoint<double>
  {
    typedef double Type;
    typedef double PrimitiveType;
  };

  /**
   * Specialization for `long` `double`.
   */
  template <> struct ToFloatingPoint<long double>
  {
    typedef long double Type;
    typedef long double PrimitiveType;
  };

  template <class T> struct ToFloatingPoint<std::complex<T> >
  {
    typedef std::complex<typename ToFloatingPoint<T>::Type> Type;
    typedef typename ToFloatingPoint<T>::Type PrimitiveType;
  };

  /**
   * Type for values passed through the va_arg mechanism ("..." in
   * function parameters). When passing through va_args, floats are
   * promoted to doubles, (unsigned) chars/shorts and bools to ints.
   */
  template <class T> struct VaArg { typedef T Type; };
  template <> struct VaArg<float> { typedef double Type; };
  template <> struct VaArg<char> { typedef int Type; };
  template <> struct VaArg<unsigned char> { typedef int Type; };
  template <> struct VaArg<bool> { typedef int Type; };
  template <> struct VaArg<short> { typedef int Type; };
  template <> struct VaArg<unsigned short> { typedef int Type; };

  // PENDING VaArg<enum> -> int
  // Requires an IsEnum trait.

  /**
   * The `Type` member typedef of this structure maps to the
   * `value_type` member type of the type given as the template
   * argument. The structure is specialized for pointers to return the
   * type pointed to.
   *
   * ~~~(c++)
   * using namespace Pii;
   * typedef ValueType<PiiMatrix<double>::column_iterator>::Type DoubleType1;
   * typedef ValueType<PiiMatrix<double>::row_iterator>::Type DoubleType2;
   * ~~~
   */
  template <class T> struct ValueType { typedef typename T::value_type Type; };
  template <class T> struct ValueType<T*> { typedef T Type; };

  /**
   * A trait whose `boolValue` member is `true` if type `T` has
   * less bits than type `U`.
   */
  template <class T, class U> struct LessBits : public If<sizeof(T) < sizeof(U),
                                                          True,
                                                          False>::Type
  {};

  /**
   * A trait whose `boolValue` member is `true` if type `T` has
   * more bits than type `U`.
   */
  template <class T, class U> struct MoreBits : public If<sizeof(T) <= sizeof(U),
                                                          False,
                                                          True>::Type
  {};

  /**
   * A trait whose `Type` is `U` if `sizeof`(T) is less than
   * `sizeof`(U) and `T` otherwise.
   */
  template <class T, class U> struct Larger { typedef typename IfClass<LessBits<T,U>, U, T>::Type Type; };

  /**
   * A trait whose `Type` is `T` if `sizeof`(T) is larger than
   * `sizeof`(U) and `T` otherwise.
   */
  template <class T, class U> struct Smaller { typedef typename IfClass<MoreBits<T,U>, U, T>::Type Type; };

  /**
   * Convert any integer type to its signed counterpart. For example,
   * ToSigned<unsigned int>::Type is `int`.
   */
  template <class T> struct ToSigned { typedef T Type; };
  template <> struct ToSigned<unsigned char> { typedef char Type; };
  template <> struct ToSigned<unsigned short> { typedef short Type; };
  template <> struct ToSigned<unsigned int> { typedef int Type; };
  template <> struct ToSigned<unsigned long> { typedef long Type; };
  template <> struct ToSigned<unsigned long long> { typedef long long Type; };

  /**
   * Convert any integer type to its unsigned counterpart. For
   * example, ToSigned<char>::Type is `unsigned` `char`.
   */
  template <class T> struct ToUnsigned { typedef T Type; };
  template <> struct ToUnsigned<char> { typedef unsigned char Type; };
  template <> struct ToUnsigned<short> { typedef unsigned short Type; };
  template <> struct ToUnsigned<int> { typedef unsigned int Type; };
  template <> struct ToUnsigned<long> { typedef unsigned long Type; };
  template <> struct ToUnsigned<long long> { typedef unsigned long long Type; };

  /**
   * Combine two integer types.
   *
   * @see Combine
   */
  template <class T, class U> struct CombineInts
  {
    // Check if either is signed.
    typedef typename IfClass<Or<IsSigned<T>::boolValue,
                                IsSigned<U>::boolValue>,
                             // At least one signed -> use a signed version
                             // of the type with more bits
                             typename ToSigned<typename Larger<T,U>::Type>::Type,
                             // Both are unsigned -> use larger as such
                             typename Larger<T,U>::Type >::Type Type;
  };

  /**
   * Combine two types, at least one of which is a floating-point number.
   *
   * @see Combine
   */
  template <class T, class U> struct CombineFloats
  {
    // Check if both are floats
    typedef typename IfClass<And<IsFloatingPoint<T>::boolValue,
                                 IsFloatingPoint<U>::boolValue>,
                             // Yes, use the larger
                             typename Larger<T,U>::Type,
                             // No, only one is float -> use it
                             typename IfClass<IsFloatingPoint<T>, T, U>::Type >::Type Type;
  };

  /**
   * A trait that can be used to find a type that best combines the
   * properties of two types. This trait can be used when mixing
   * different types in calculations to ensure the result type will be
   * accurate enough.
   *
   * The logic is as follows:
   *
   * - If both types are floating-point, the result is the type with
   * larger number of bits. For example (`double`, `float`) maps to
   * `double`.
   *
   * - If one of the types is floating-point and the other is integer,
   * the floating-point type will be used. For example (`float`,
   * `int`) maps to `float`.
   *
   * - If both types are integers, and either of them is signed, the
   * result is a signed version of the type with larger number of
   * bits. For example (`unsigned long`, `char`) maps to `long`.
   *
   * - If both types are unsigned integers, the result is the type
   * with larger number of bits. For example (`unsigned int`,
   * `unsigned char`) maps to `unsigned int`.
   *
   * ~~~(c++)
   * template <class T, class U> void calculate(T a, U b)
   * {
   *   typedef typename Pii::Combine<T,U>::Type R;
   *   R result = R(a) * R(b);
   *   std::cout << result << std::endl;
   * }
   *
   * // ...
   *
   * char a = -5;
   * unsigned int b = 2;
   * calculate(a,b);
   * // Outputs -10.
   * // If you directly multiply a*b, you'll get 4294967286
   * // as an unsigned int
   * ~~~
   */
  template <class T, class U> struct Combine
  {
    // Check if both are integer types
    typedef typename IfClass<And<IsInteger<T>::boolValue,
                                 IsInteger<U>::boolValue>,
                             // "Call" CombineInts
                             typename CombineInts<T,U>::Type,
                             // "Call" CombineFloats
                             typename CombineFloats<T,U>::Type >::Type Type;
  };

  /// @endgroup
}

#endif //_PIITYPETRAITS_H
