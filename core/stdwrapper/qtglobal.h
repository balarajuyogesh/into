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

#ifndef _QTGLOBAL_H
#define _QTGLOBAL_H

#if defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)
#  define Q_OS_WIN
#elif defined(__APPLE__)
#  define Q_OS_MAC
#elif defined(__linux__)
#  define Q_OS_LINUX
#endif

inline void qt_noop(void) {}

#define Q_DECLARE_FLAGS(A,B) typedef int A
#define Q_FLAGS(A) namespace {}
#define Q_ENUMS(A) namespace {}
#define Q_DECLARE_OPERATORS_FOR_FLAGS(A) namespace {}
#define Q_ASSERT(A) qt_noop()
#define QT_TR_NOOP(A) (A)
#define QT_TRANSLATE_NOOP(A,B) (B)
#define QT_VERSION 0x050000

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef signed char qint8;
typedef unsigned char quint8;
typedef short qint16;
typedef unsigned short quint16;
typedef int qint32;
typedef unsigned int quint32;
#if defined(Q_OS_WIN) && !defined(__GNUC__)
typedef __int64 qint64;
typedef unsigned __int64 quint64;
#else
typedef long long qint64;
typedef unsigned long long quint64;
#endif


template <class T> inline T qMin(T a, T b) { return a <= b ? a : b; }
template <class T> inline T qMax(T a, T b) { return a >= b ? a : b; }
template <class T> inline T qBound(T min, T val, T max)
{
  if (val < min) return min;
  if (val > max) return max;
  return val;
}
#ifdef PII_CXX11
#  include <utility>
#else
// std::swap was in algorithm prior C++11
#  include <algorithm>
#endif
template <class T> inline void qSwap(T& a, T&b) { std::swap(a, b); }

#define Q_UNUSED(X) (void)(X)

#endif //_QTGLOBAL_H
