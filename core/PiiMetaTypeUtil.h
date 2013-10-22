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

#ifndef _PIIMETATYPEUTIL_H
#define _PIIMETATYPEUTIL_H

#include "PiiGlobal.h"
#include <QVariantList>
#include <QByteArray>

namespace Pii
{
  /**
   * Converts comma-separated type names to a list of type IDs. If any
   * of the types is invalid, or if *types* is empty, returns an
   * empty list.
   *
   * ~~~(c++)
   * QList<int> lstTypes = Pii::parseTypes("QString,int");
   * // lstTypes = (QMetaType::String, QMetaType::Int)
   * ~~~
   */
  PII_CORE_EXPORT QList<int> parseTypes(const QByteArray& types);

  /**
   * Returns a numerical score for the goodness of a parameter set
   * given a list of expected types. The returned value is the number
   * of elements in *params* whose type exactly matches the
   * corresponding type in *types*. If the length of *params* is
   * different from that of *types*, or any of the parameters cannot
   * be converted to the expected type, returns -1.
   *
   * ~~~(c++)
   * QList<int> lstTypes;
   * lstTypes << QMetaType::Int << QMetaType::String;
   * QVariantList lstParams;
   * lstParams << QString("abc") << QString("def");
   * int iScore = Pii:scoreOverload(lstParams, lstTypes);
   * // iScore == 1 because QVariant allows conversions from QString to int
   * ~~~
   */
  PII_CORE_EXPORT int scoreOverload(const QVariantList& params, const QList<int>& types);

  /**
   * Copies the object pointed to by *source* to the memory location
   * at *target*. Both *source* and *target* must point to an
   * object whose type matches *type*.
   *
   * @param type the type ID of the objects. See QMetaType::Type.
   *
   * @param source the source object
   *
   * @param target the target object
   *
   * ~~~(c++)
   * int iSource = 1, iTarget = 0;
   * Pii::copyMetaType(QVariant::Int, &iSource, &iTarget);
   * // iTarget == 1
   * ~~~
   */
  PII_CORE_EXPORT void copyMetaType(int type, const void* source, void* target);

  /// @internal
  PII_CORE_EXPORT bool copyMetaType(const QVariant& source, int expectedType, void** args);
  /// @internal
  PII_CORE_EXPORT QVariant argsToVariant(void** args, int type);

  /**
   * Converts *args* to to a QVariantList, assuming that each
   * argument is a valid pointer of the type specified by *types*.
   *
   * ~~~(c++)
   * int i;
   * double d;
   * void* args[2] = { &i, &d };
   * QList<int> lstTypes;
   * lstTypes << QMetaType::Int << QMetaType::Double;
   * QVariantList lstArgs(PiiUniversalSlot::argsToList(lstTypes, args);
   * ~~~
   */
  PII_CORE_EXPORT QVariantList argsToList(const QList<int>& types, void** args);
}

#endif //_PIIMETATYPEUTIL_H
