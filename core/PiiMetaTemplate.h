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

#ifndef _PIIMETATEMPLATE_H
#define _PIIMETATEMPLATE_H

/**
 * Repeats a function call for all arguments in a template argument
 * pack.
 *
 * ~~~(c++)
 * template <class... Args> void func(Args... args)
 * {
 *   PII_FOREACH_TEMPARG(args.member());
 *   PII_FOREACH_TEMPARG(plus(args,2));
 * }
 * ~~~
 */
#define PII_FOREACH_TEMPARG(FUNC) ::Pii::Pass::pass(::Pii::Pass{((FUNC),1)...})

namespace Pii
{
  /**
   * @group pii_template_metaprogramming Template meta-programming
   *
   * Template meta-programs are structures the compiler evaluates.
   * They can be used in creating efficient code, for example control
   * structures that are evaluated at compile time.
   */

  /**
   * A conditional template. If `condition` is true, If::Type is
   * equal to `Then`. The template specialization for condition ==
   * false makes `Type` equal to `Else`.
   *
   * ~~~(c++)
   * template <class T> void myFunc()
   * {
   *   // if T is four bytes, MyType is int, otherwise char
   *   typedef Pii::If<sizeof(T) == 4, int, char>::Type MyType;
   *   MyType myVal(0);
   * }
   * ~~~
   */
  template <bool condition, class Then, class Else>
  struct If
  {
    typedef Then Type;
  };

  template <class Then, class Else>
  struct If<false, Then, Else>
  {
    typedef Else Type;
  };

  /**
   * A conditional template that accepts a type name as the condition
   * parameter. There condition class must declare a static constant
   * called `boolValue`. Otherwise, functions like If.
   *
   * ~~~(c++)
   * template <class T, class U> void myFunc()
   * {
   *   // if T is four bytes and U is two bytes, MyType is int, otherwise long
   *   typedef Pii::IfClass<Pii::And<sizeof(T) == 4,
   *                        sizeof(U) == 2>,
   *                        int, long>::Type MyType;
   *   MyType myVal(0);
   * }
   * ~~~
   */
  template <class Condition, class Then, class Else>
  struct IfClass : public If<Condition::boolValue, Then, Else>
  {};

  /**
   * A structure whose constant `boolValue` member evaluates to
   * `true`.
   */
  struct True
  {
    enum { boolValue = true };
  };

  /**
   * A structure whose constant `boolValue` member evaluates to
   * `false`.
   */
  struct False
  {
    enum { boolValue = false };
  };

  /**
   * Logical not operation. The template parameter must be a boolean
   * value.
   *
   * ~~~(c++)
   * ASSERT(Not<false>::boolValue == true);
   * ~~~
   */
  template <bool b> struct Not
  {
    enum { boolValue = !b };
  };

  /**
   * Logical not operation. The template parameter must be a valid
   * static logical operator.
   *
   * ~~~(c++)
   * ASSERT(NotClass<False>::boolValue == true);
   * ASSERT(NotClass<True>::boolValue == false);
   * ~~~
   */
  template <class T> struct NotClass
  {
    enum { boolValue = !T::boolValue };
  };

  /**
   * A template that performs a logical AND operation on its boolean
   * template parameters. The `boolValue` member evaluates to `true`
   * if all of the template parameters evaluate to true.
   */
  template <bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true>
  struct And : False {};

  template <>
  struct And<true, true, true, true, true, true, true> : True {};

  /**
   * A template that performs a logical OR operation on its boolean
   * template parameters. The `boolValue` member evaluates to `true`
   * if any of the template parameters evaluate to true.
   */
  template <bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false>
  struct Or : True {};

  template <>
  struct Or<false, false, false, false, false, false, false> : False {};

  template <int i> struct IntIdentity
  {
    enum { intValue = i };
  };

  template <int a, int b> struct MinInt : If<(a <= b), IntIdentity<a>, IntIdentity<b> >::Type
  {};

  template <int a, int b> struct MaxInt : If<(a >= b), IntIdentity<a>, IntIdentity<b> >::Type
  {};

#ifdef PII_CXX11
  /*
   * A helper for handling variadic template arguments in function
   * calls.
   */
  struct Pass
  {
    template <class... Args> Pass(Args&&...) {}
    static void pass(const Pass&) {}
  };
#endif

  /// @endgroup
}

#endif //_PIIMETATEMPLATE_H
