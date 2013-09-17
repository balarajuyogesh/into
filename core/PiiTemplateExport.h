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

#ifndef _PIITEMPLATEEXPORT_H
#define _PIITEMPLATEEXPORT_H

/**
 * @file
 *
 * Contains macros for exporting explicit template instances from
 * shared libraries. Different compilers need different (non-standard)
 * tricks to export template instantiations. The standard-compliant 
 * `export` keyword, which was intended approximately for the purpose
 * falls miles short of its target and is actually not supported by
 * most of the compilers.
 *
 * - GCC, MinGW: The `DECLARE` macros declare an `extern` explicit
 * instance of a template as per C++0x. The `DEFINE` macros create an
 * explicit instance.
 *
 * - MSVC: The `DECLARE` macros expand to nothing if building a
 * library. If the library is not being build, the `DECLARE` macros
 * declare an explicit instance with `__declspec`(dllimport) storage
 * class. The `DEFINE` macros create an explicit instance with 
 * `__declspec`(dllexport) storage class. This trick is needed because
 * there is no way to just declare an exported explicit instance in
 * MSVC. Even if the storage class is `dllexport`, the compiler
 * instantiates the template. In some cases all definitions for the
 * required template instance may not be available yet, and the
 * template must be instantiated later.
 *
 * ~~~
 * // In MyStruct.h
 * #include <PiiTemplateExport.h>
 *
 * #ifdef BUILD_MYLIB
 * #  define BUILDING_MYLIB 1
 * #else
 * #  define BUILDING_MYLIB 0
 * #endif
 *
 * template <class T> struct MyStruct
 * {
 * };
 *
 * template <class T> T identityFunc(T value) { return value; }
 *
 * // Declare an exported explicit instance of the class
 * PII_DECLARE_EXPORTED_CLASS_TEMPLATE(struct, MyStruct<double>, BUILDING_MYLIB);
 * // Declare an exported explicit instance of a template function.
 * PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE(void, identityFunc<int>, (int), BUILDING_MYLIB);
 * ~~~
 *
 * ~~~
 * // In MyStruct.cc, create explicit instances:
 * #include "MyStruct.h"
 * PII_DEFINE_EXPORTED_CLASS_TEMPLATE(struct, MyStruct<double>);
 * PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE(void, identityFunc<int>, (int));
 * ~~~
 *
 * @see PiiGlobal.h
 */

#include "PiiPreprocessor.h"

#if defined (__GNUC__)
#  define PII_DECLARE_EXPORTED_CLASS_TEMPLATE(KEYWORD, TYPENAME, BUILDING_LIB) \
  extern template KEYWORD PII_IF(BUILDING_LIB, PII_DECL_EXPORT, PII_DECL_IMPORT) TYPENAME
#  define PII_DEFINE_EXPORTED_CLASS_TEMPLATE(KEYWORD, TYPENAME) template KEYWORD TYPENAME

#  define PII_DECLARE_EXPORTED_CLASS_TEMPLATE2(KEYWORD, TYPENAME1, TYPENAME2, BUILDING_LIB) \
  extern template KEYWORD PII_IF(BUILDING_LIB, PII_DECL_EXPORT, PII_DECL_IMPORT) TYPENAME1, TYPENAME2
#  define PII_DEFINE_EXPORTED_CLASS_TEMPLATE2(KEYWORD, TYPENAME1, TYPENAME2) template KEYWORD TYPENAME1,TYPENAME2

#  define PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE(RETURN, NAME, PARAMS, BUILDING_LIB) extern template RETURN NAME PARAMS
#  define PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE(RETURN, NAME, PARAMS) template RETURN NAME PARAMS

#  define PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE2(RETURN, NAME1, NAME2, PARAMS, BUILDING_LIB) \
  extern template PII_IF(BUILDING_LIB, PII_DECL_EXPORT, PII_DECL_IMPORT) RETURN NAME1, NAME2 PARAMS
#  define PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE2(RETURN, NAME1, NAME2, PARAMS) template RETURN NAME1,NAME2 PARAMS

#elif defined (_WIN32)
#  define PII_DECLARE_EXPORTED_CLASS_TEMPLATE(KEYWORD, TYPENAME, BUILDING_LIB) \
  PII_IF(BUILDING_LIB, namespace Dummy {}, template KEYWORD PII_DECL_IMPORT TYPENAME)
#  define PII_DEFINE_EXPORTED_CLASS_TEMPLATE(KEYWORD, TYPENAME) template KEYWORD PII_DECL_EXPORT TYPENAME

#  define PII_DECLARE_EXPORTED_CLASS_TEMPLATE2(KEYWORD, TYPENAME1, TYPENAME2, BUILDING_LIB) \
  PII_IF(BUILDING_LIB, namespace Dummy {}, template KEYWORD PII_DECL_IMPORT TYPENAME1 PII_COMMA TYPENAME2)
#  define PII_DEFINE_EXPORTED_CLASS_TEMPLATE2(KEYWORD, TYPENAME1, TYPENAME2) \
  template KEYWORD PII_DECL_EXPORT TYPENAME1, TYPENAME2

#  define PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE(RETURN, NAME, PARAMS, BUILDING_LIB) \
  PII_IF(BUILDING_LIB, namespace Dummy {}, template PII_DECL_IMPORT RETURN NAME PARAMS)
#  define PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE(RETURN, NAME, PARAMS) template PII_DECL_EXPORT RETURN NAME PARAMS

#  define PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE2(RETURN, NAME1, NAME2, PARAMS, BUILDING_LIB) \
  PII_IF(BUILDING_LIB, namespace Dummy {}, template PII_DECL_IMPORT RETURN NAME1 PII_COMMA NAME2 PARAMS)
#  define PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE2(RETURN, NAME1, NAME2, PARAMS) \
  template PII_DECL_EXPORT RETURN NAME1,NAME2 PARAMS

#else
#  define PII_DECLARE_EXPORTED_CLASS_TEMPLATE(KEYWORD, TYPENAME, BUILDING_LIB) extern template KEYWORD TYPENAME
#  define PII_DEFINE_EXPORTED_CLASS_TEMPLATE(KEYWORD, TYPENAME) template KEYWORD TYPENAME
#  define PII_DECLARE_EXPORTED_CLASS_TEMPLATE2(KEYWORD, TYPENAME1, TYPENAME2, BUILDING_LIB) extern template KEYWORD TYPENAME1, TYPENAME2
#  define PII_DEFINE_EXPORTED_CLASS_TEMPLATE2(KEYWORD, TYPENAME1, TYPENAME2) template KEYWORD TYPENAME1, TYPENAME2
#  define PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE(RETURN, NAME, PARAMS, BUILDING_LIB) extern template RETURN NAME PARAMS
#  define PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE(RETURN, NAME, PARAMS) template RETURN NAME PARAMS
#  define PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE2(RETURN, NAME, PARAMS, BUILDING_LIB) extern template RETURN NAME1,NAME2 PARAMS
#  define PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE2(RETURN, NAME, PARAMS) template RETURN NAME1,NAME2 PARAMS
#endif

#endif //_PIITEMPLATEEXPORT_H
