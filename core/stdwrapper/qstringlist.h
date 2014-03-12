/* This file is part of Into.
 * Copyright (C) Intopii 2014.
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

#ifndef _QSTRINGLIST_H
#define _QSTRINGLIST_H

#include "qlist.h"
#include "qstring.h"

class PII_CORE_EXPORT QStringList : public QList<QString>
{
public:
  typedef QList<QString> SuperType;

  QStringList() {}
  QStringList(const QStringList& other) : SuperType(other) {}
#ifdef PII_CXX11
  QStringList(QStringList&& other) : SuperType(other) {}
  QStringList& operator= (QStringList&& other)
  {
    return static_cast<QStringList&>(SuperType::operator= (std::move(other)));
  }
#endif
  QStringList& operator= (const QStringList& other)
  {
    return static_cast<QStringList&>(SuperType::operator= (other));
  }

  QStringList& operator<< (const QString& value)
  {
    return static_cast<QStringList&>(SuperType::operator<< (value));
  }
};

#endif //_QSTRINGLIST_H
