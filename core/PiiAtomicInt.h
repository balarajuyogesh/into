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

#ifndef _PIIATOMICINT_H
#define _PIIATOMICINT_H

#include <PiiGlobal.h>

/// @hide

/* A wrapper class for an atomic integer. QAtomicInt interface is not
   used directly by library code because a) it is different in Qt4 and
   Qt5 b) its interface is not convenient.
 */

#ifdef PII_NO_QT
// If Qt is disabled, use C++11 by default
#  ifdef PII_CXX11
#    include <atomic>
typedef std::atomic_int PiiAtomicIntImpl;
#  elif !defined(PII_ATOMIC_INT_IMPL)
// This is a dummy, absolutely non-atomic implementation.
struct PiiAtomicIntImpl
{
  PiiAtomicIntImpl(int i = 0) : i(i) {}
  int operator= (int val) { return i = val; }
  int operator= (const PiiAtomicIntImpl& other) { return i = other.i; }
  int load() const { return i; }
  void store(int val) { i = val; }
  int operator++ () { return ++i; }
  int operator++ (int) { return i++; }
  int operator+= (int val) { return i += val; }
  int operator-- () { return --i; }
  int operator-- (int) { return i--; }
  int operator-= (int val) { return i -= val; }
  bool operator== (const PiiAtomicIntImpl& other) const { return i == other.i; }
  bool operator!= (const PiiAtomicIntImpl& other) const { return i != other.i; }
  int i;
};
#  else
// If C++11 support is not available, you need to have another class
// that implements the same interface as std::atomic_int and let us
// know the type name by setting the PII_ATOMIC_INT_IMPL preprocessor
// variable.
typedef PII_ATOMIC_INT_IMPL PiiAtomicIntImpl;
#  endif

class PII_CORE_EXPORT PiiAtomicInt
{
public:
  PiiAtomicInt(int value = 0) : _value(value) {}

  void ref() { ++_value; }
  int deref() { return --_value; }
  int load() const { return _value.load(); }
  void store(int value) { _value.store(value); }

  int operator++ () { return ++_value; }
  int operator++ (int) { return _value++; }
  int operator-- () { return --_value; }
  int operator-- (int) { return _value--; }
  int operator+= (int value) { return _value += value; }
  int operator-= (int value) { return _value -= value; }

  bool operator== (const PiiAtomicInt& other) const { return _value.load() == other.load(); }
  bool operator!= (const PiiAtomicInt& other) const { return _value.load() != other.load(); }
  bool operator== (int value) const { return _value.load() == value; }
  bool operator!= (int value) const { return _value.load() != value; }
  int operator= (int value) { return _value.operator= (value); }

private:
  PiiAtomicIntImpl _value;
};
#else
#  include <QAtomicInt>

class PII_CORE_EXPORT PiiAtomicInt
{
public:
  PiiAtomicInt(int value = 0) : _value(value) {}

  void ref() { _value.ref(); }
  int deref() { return _value.fetchAndAddOrdered(-1) - 1; }

  int load() const
  {
#if QT_VERSION >= 0x050000
    return _value.load();
#else
    return static_cast<int>(_value);
#endif
  }

  void store(int value)
  {
#if QT_VERSION >= 0x050000
    _value.store(value);
#else
    _value = value;
#endif
  }

  int operator++ () { return _value.fetchAndAddOrdered(1) + 1; }
  int operator++ (int) { return _value.fetchAndAddOrdered(1); }
  int operator-- () { return _value.fetchAndAddOrdered(-1) - 1; }
  int operator-- (int) { return _value.fetchAndAddOrdered(-1); }
  int operator+= (int value) { return _value.fetchAndAddOrdered(value) + value; }
  int operator-= (int value) { return _value.fetchAndAddOrdered(value) - value; }

  bool operator== (const PiiAtomicInt& other) const { return load() == other.load(); }
  bool operator!= (const PiiAtomicInt& other) const { return load() != other.load(); }
  bool operator== (int value) const { return load() == value; }
  bool operator!= (int value) const { return load() != value; }
  int operator= (int value) { store(value); return value; }

private:
  QAtomicInt _value;
};

#endif

/// @endhide

#endif //_PIIATOMICINT_H
