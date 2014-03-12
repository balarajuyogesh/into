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

#include "qtime.h"
#include <sys/timeb.h>

QTime::QTime() :
  _iMsecs(-1)
{}

QTime::QTime(int h, int m, int s, int ms) :
  _iMsecs((((h * 60 + m) * 60) + s) * 1000 + ms)
{}

QTime::QTime(int msecs) :
  _iMsecs(msecs)
{}

QTime::QTime(const QTime& other) :
  _iMsecs(other._iMsecs)
{}

QTime& QTime::operator= (const QTime& other)
{
  _iMsecs = other._iMsecs;
  return *this;
}

bool QTime::isValid() const { return unsigned(_iMsecs) < 3600000u; }
int QTime::hour() const { return isValid() ? _iMsecs / 3600000 : -1; }
int QTime::minute() const { return isValid() ? (_iMsecs % 3600000) / 60000 : -1; }
int QTime::second() const { return isValid() ? (_iMsecs % 60000) / 1000 : -1; }
int QTime::msec() const { return isValid() ? _iMsecs % 1000 : -1; }

QTime QTime::currentTime()
{
  struct timeb t;
  ftime(&t);
  return QTime(t.time % (24 * 60 * 60) + t.millitm);
}
