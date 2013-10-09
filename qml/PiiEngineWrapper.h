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

#ifndef _PIIENGINEWRAPPER_H
#define _PIIENGINEWRAPPER_H

#include <private/qqmlglobal_p.h>
#include <private/qquickvaluetypes_p.h>

#include <PiiEngine.h>

class PiiPluginValueType : public QQmlValueTypeBase<PiiEngine::Plugin>
{
  Q_OBJECT
  
  Q_PROPERTY(QString resourceName READ resourceName);
  Q_PROPERTY(QString libraryName READ libraryName);
  Q_PROPERTY(QString version READ version);

public:
  PiiPluginValueType(QObject* parent = 0) :
    QQmlValueTypeBase<PiiEngine::Plugin>(qMetaTypeId<PiiEngine::Plugin>(), parent)
  {}
  
  virtual QString toString() const { return QString("%0 %1 (%2)").arg(resourceName(), version(), libraryName()); }
  virtual bool isEqual(const QVariant& other) const
  {
    return (other.userType() == qMetaTypeId<PiiEngine::Plugin>()) &&
      (v == other.value<PiiEngine::Plugin>());
  }
  
  QString resourceName() const { return v.resourceName(); }
  QString libraryName() const { return v.libraryName(); }
  QString version() const { return v.version().toString(); }
};

class PiiPluginTypeProvider : public QQmlValueTypeProvider
{
public:
  bool create(int type, QQmlValueType *&v)
  {
    if (type == qMetaTypeId<PiiEngine::Plugin>())
      {
        v = new PiiPluginValueType;
        return true;
      }
    return false;
  }

  static PiiPluginTypeProvider* instance();
};

namespace PiiEngineWrapper
{
  void init(QV8Engine*,v8::Handle<v8::Object>);
}

#endif //_PIIENGINEWRAPPER_H
