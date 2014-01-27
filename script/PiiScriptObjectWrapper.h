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

#ifndef _PIISCRIPTOBJECTWRAPPER_H
#define _PIISCRIPTOBJECTWRAPPER_H

#include <QObject>

class PiiScriptObjectWrapper : public QObject
{
  Q_OBJECT

  virtual void throwIt() const = 0;
  template <class T> class Template;

public:
  ~PiiScriptObjectWrapper();

  template <class T> T* pointer() const
  {
    // HACK misusing exceptions to perform a dynamic type check
    try { throwIt(); }
    catch (const T* pValue) { return const_cast<T*>(pValue); }
    catch (...) {}
    return 0;
  }

  template <class T> T value() const
  {
    T* ptr = pointer<T>();
    return ptr ? *ptr : T();
  }

  template <class T> static Template<T>* create(const T& wrapped)
  {
    return new Template<T>(wrapped);
  }

protected:
  PiiScriptObjectWrapper();
};

template <class T> class PiiScriptObjectWrapper::Template : public PiiScriptObjectWrapper
{
public:
  Template(const T& wrapped) : wrapped(wrapped) {}
  void throwIt() const { throw &wrapped; }
  T wrapped;
};

#endif //_PIISCRIPTOBJECTWRAPPER_H
