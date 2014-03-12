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

#ifndef _QTIME_H
#define _QTIME_H

#include <PiiGlobal.h>

class PII_CORE_EXPORT QTime
{
public:
  QTime();
  QTime(int h, int m, int s = 0, int ms = 0);
  QTime(const QTime& other);
  QTime& operator= (const QTime& other);

  bool isValid() const;

  int hour() const;
  int minute() const;
  int second() const;
  int msec() const;

  static QTime currentTime();

private:
  explicit QTime(int msecs);
  int _iMsecs;
};

#endif //_QTIME_H
