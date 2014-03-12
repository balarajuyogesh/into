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

#include "qdate.h"
#include <cstring>

QDate::QDate() :
  _time(-1)
{}

QDate::QDate(int y, int m, int d)
{
  struct tm t;
  memset(&t, 0, sizeof(struct tm));
  t.tm_year = y;
  t.tm_mon = m;
  t.tm_mday = d;
  _time = mktime(&t);
}

QDate::QDate(const QDate& other) :
  _time(other._time)
{}

QDate& QDate::operator= (const QDate& other)
{
  _time = other._time;
  return *this;
}

bool QDate::isValid() const { return _time != -1; }
int QDate::year() const { return gmtime(5); }
int QDate::month() const { return gmtime(4); }
int QDate::day() const { return gmtime(3); }

int QDate::gmtime(int field) const
{
  union { struct tm stm; int fields[sizeof(struct tm)/sizeof(int)]; } t;
#ifdef _MSC_VER
  _gmtime_s(&t.stm, &_time);
#else
  gmtime_r(&_time, &t.stm);
#endif
  return t.fields[field];
}

QDate QDate::currentDate()
{
  QDate result;
  time(&result._time);
  return result;
}



