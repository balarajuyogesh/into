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

#ifndef _PIIVARIANTWRAPPER_H
#define _PIIVARIANTWRAPPER_H

#include <private/qqmlglobal_p.h>
#include <private/qquickvaluetypes_p.h>

#include <PiiYdinTypes.h>

class PiiVariantValueType : public QQmlValueTypeBase<PiiVariant>
{
  Q_OBJECT

public:
  PiiVariantValueType(QObject* parent = 0) :
    QQmlValueTypeBase<PiiVariant>(qMetaTypeId<PiiVariant>(), parent)
  {}

  virtual QString toString() const { return PiiYdin::convertToQString(v); }
  virtual bool isEqual(const QVariant& /*other*/) const
  {
    // TODO: implement this
    return false;
  }

public slots:
  int toInt() const
  {
    return PiiYdin::convertPrimitiveTo<int>(v);
  }

  double toDouble() const
  {
    return PiiYdin::convertPrimitiveTo<double>(v);
  }

  bool toBool() const
  {
    return PiiYdin::convertPrimitiveTo<bool>(v);
  }
};

#endif //_PIIVARIANTWRAPPER_H
