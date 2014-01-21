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

#ifndef _PIIREMOTEQOBJECT_H
#define _PIIREMOTEQOBJECT_H

#include "PiiRemoteMetaObject.h"
#include <QMetaMethod>

/**
 * A QObject that uses PiiRemoteMetaObject as its meta-object
 * implementation. The signals, slots and properties of this class
 * implemented by a PiiQObjectServer, which makes them completely
 * network-transparent to the programmer. The `Base` template
 * parameter makes it possible to derive the class not only from
 * QObject but any QObject-derived class.
 *
 * ~~~(c++)
 * PiiRemoteQObject<QObject> client("tcp://intopii.com:3142/valuesetter/");
 * connect(pSlider, SIGNAL(valueChanged(int)), &client, SLOT(setValue(int)));
 * connect(&client, SIGNAL(valueChanged(int)), pSlider2, SLOT(setValue(int)));
 * ~~~
 *
 */
template <class Base> class PiiRemoteQObject :
  public Base,
  public PiiRemoteMetaObject
{
public:
  PiiRemoteQObject() : PiiRemoteMetaObject(this)
  {
    new SignalKiller(this);
  }
  PiiRemoteQObject(const QString& serverUri) : PiiRemoteMetaObject(this, serverUri)
  {
    new SignalKiller(this);
  }

  /// @internal
  const QMetaObject* metaObject() const { return PiiRemoteMetaObject::metaObject(); }
  /// @internal
  int qt_metacall(QMetaObject::Call call, int id, void** args) { return PiiRemoteMetaObject::qt_metacall(call, id, args); }

protected:
#if QT_VERSION < 0x050000
  void connectNotify(const char* signal)
  {
    PiiRemoteMetaObject::connectSignal(signal+1); // remove signal identifier byte
  }
  void disconnectNotify(const char* signal)
  {
    PiiRemoteMetaObject::disconnectSignal(signal+1);
  }
#else
  void connectNotify(const QMetaMethod& signal)
  {
    PiiRemoteMetaObject::connectSignal(signal.methodSignature().constData());
  }
  void disconnectNotify(const QMetaMethod& signal)
  {
    PiiRemoteMetaObject::disconnectSignal(signal.methodSignature().constData());
  }
#endif

private:
  class SignalKiller : public QObject
  {
  public:
    SignalKiller(PiiRemoteQObject* parent)
    {
      setParent(parent);
      startTimer(1000);
    }

  protected:
    void timerEvent(QTimerEvent*)
    {
      try { static_cast<PiiRemoteQObject*>(parent())->removeUnconnectedSignals(); }
      catch (...) {}
    }
  };
  friend class SignalKiller;

  void removeUnconnectedSignals();
};


template <class Base> void PiiRemoteQObject<Base>::removeUnconnectedSignals()
{
  // HACK Remove unconnected signals from channel. This must be done
  // here because Qt doesn't always call disconnectNotify() when a
  // connection is broken. See QTBUG-4844.
  QList<Signal>& lstSignals = signalList();
  for (int i=0; i<lstSignals.size(); ++i)
    {
      if (lstSignals[i].iConnectionCount > 0)
        {
          QByteArray aSignature(lstSignals[i].aSignature);
          aSignature.prepend('2'); // 2 at the beginning identifies a signal
          if (this->receivers(aSignature.constData()) == 0)
            {
              lstSignals[i].iConnectionCount = 0;
              this->disconnectFromChannel("signals/" + lstSignals[i].aSignature);
            }
        }
    }
}

#endif //_PIIREMOTEQOBJECT_H
