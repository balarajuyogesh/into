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

#include "qdatetime.h"
#include <PiiSharedD.h>
#include <ctime>

QDateTime::QDateTime()
{}

QDateTime::QDateTime(const QDateTime& other) :
  _date(other._date),
  _time(other._time)
{}

QDateTime::~QDateTime()
{}

QDateTime& QDateTime::operator= (const QDateTime& other)
{
  _date = other._date;
  _time = other._time;
  return *this;
}

QDateTime QDateTime::currentDateTime()
{
  // TODO this isn't actually correct
  QDateTime result;
  result._date = QDate::currentDate();
  // What if day changes here?
  result._time = QTime::currentTime();
  return result;
}

uint QDateTime::toTime_t() const
{
  return _date._time;
}

QDate QDateTime::date() const { return _date; }
QTime QDateTime::time() const { return _time; }
