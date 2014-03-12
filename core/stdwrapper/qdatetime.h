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

#ifndef _QDATETIME_H
#define _QDATETIME_H

#include <PiiGlobal.h>

#include "qtime.h"
#include "qdate.h"

class PII_CORE_EXPORT QDateTime
{
public:
  QDateTime();
  QDateTime(const QDateTime& other);
  ~QDateTime();

  QDateTime& operator= (const QDateTime& other);

  uint toTime_t() const;
  QDate date() const;
  QTime time() const;

  static QDateTime currentDateTime();

private:
  QDate _date;
  QTime _time;
};

#endif //_QDATETIME_H
