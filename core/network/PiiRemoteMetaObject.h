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

#ifndef _PIIREMOTEMETAOBJECT_H
#define _PIIREMOTEMETAOBJECT_H

#include "PiiRemoteObject.h"

/**
 * A client-side interface for PiiQObjectServer. PiiRemoteMetaObject
 * inspects the signals, slots and properties provided by the a
 * PiiQObjectServer and reflects them as dynamic signals, slots and
 * properties on the client side. Use the PiiRemoteQObject class to
 * connect the remote meta-object implementation to a client-side
 * QObject instance.
 *
 */
class PII_NETWORK_EXPORT PiiRemoteMetaObject : public PiiRemoteObject
{
public:
  /**
   * Creates a new PiiRemoteMetaObject that implements QObject's
   * interface for the given *object*. The remote object becomes
   * functional only after [setServerUri()] has been called.
   */
  PiiRemoteMetaObject(QObject* object);

  /**
   * Creates a new PiiRemoteMetaObject and sets the server URI at
   * the same time. See [setServerUri()].
   *
   * @exception PiiNetworkException& if the server cannot be connected
   * @exception PiiInvalidArgumentException& if *serverUri* is
   * incorrecly formatted
   */
  PiiRemoteMetaObject(QObject* object, const QString& serverUri);

  ~PiiRemoteMetaObject();

  /// @hide
  void connectSignal(const char* signal);
  void disconnectSignal(const char* signal);

  const QMetaObject* metaObject() const;
  int qt_metacall(QMetaObject::Call call, int id, void** arguments);

protected:
  struct Function
  {
    Function(const char* sig, int r, const QString& n, const QList<int>& t) :
      aSignature(sig), returnType(r), strName(n), lstParamTypes(t)
    {}
    QByteArray aSignature;
    int returnType;
    QString strName;
    QList<int> lstParamTypes;
  };

  struct Signal : Function
  {
    Signal(const char* sig, int r, const QString& n, const QList<int>& t) :
      Function(sig,r,n,t),
      iConnectionCount(0)
    {}
    int iConnectionCount;
  };

  struct Property
  {
    Property(int t, const QString& n) : type(t), strName(n) {}
    int type;
    QString strName;
  };

  QList<Signal>& signalList();

  class Data;
  PII_UNSAFE_D_FUNC;
  /// @endhide

  void decodePushedData(const QString& sourceId, const QByteArray& data);
  void serverUriChanged(const QString& serverUri);

private:
  inline static QString tr(const char* s) { return QCoreApplication::translate("PiiRemoteMetaObject", s); }

  int metaCall(QMetaObject::Call call, int id, void** arguments);
  void createMetaObject();
  void collectProperties();
  void collectFunctions(bool listSignals);
  void emitSignal(int id, const QByteArray& data);
  void buildArrayData();
};


#endif //_PIIREMOTEMETAOBJECT_H
