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

#include "PiiGlobal.h"

#include <QObject>
#include <QVariantList>

/**
 * An object that can be used as a slot for any signal. 
 * PiiUniversalSlot has no static moc-generated slots, but it can
 * create a matching a slot for any signal at run time. When the slot
 * is invoked, the call is passed to the protected [invokeSlot()]
 * function that must be implemented in a subclass.
 *
 * ! Since Qt's meta-object system relies on a static number of
 * signals/slots in a class, classes derived from PiiUniversalSlot
 * cannot have ordinary signals and slots.
 *
 */
class PII_CORE_EXPORT PiiUniversalSlot : public QObject
{
  // Omitting Q_OBJECT on purpose.
public:
  ~PiiUniversalSlot();
  
  /// @internal
  int qt_metacall(QMetaObject::Call call, int id, void** arguments);

  /**
   * Connects the given *signal* in the *sender* object to this
   * object. If the *slot* already exists, the signal will be
   * connected to it. Otherwise, a new slot will be created and
   * assigned a unique id.
   *
   * ~~~
   * class MySlot : public PiiUniversalSlot { ... };
   * QObject obj;
   * MySlot mySlot;
   * mySlot.dynamicConnect(&obj, "destroyed()");
   * ~~~
   *
   * @param sender the sender of the signal
   *
   * @param signal the signature of the signal to connect to. You can
   * use the SIGNAL macro to wrap the signal name, but it is not
   * necessary.
   *
   * @param slot the signature of the dynamic slot to create. If 
   * *slot* is zero, *signal* will be used as the signature of the slot
   * as well. You can use the SLOT macro to wrap the slot name, but it
   * is not necessary.
   *
   * @param type the type of the connection
   *
   * @returns the id of the slot (either new or existing), or -1 on
   * failure.
   */
  int dynamicConnect(QObject* sender, const char* signal, const char* slot = 0,
                     Qt::ConnectionType type = Qt::AutoConnection);

  /**
   * Disconnects a signal from a dynamic slot.
   *
   * @return `true` on success, `false` on failure
   *
   * @see dynamicConnect()
   */
  bool dynamicDisconnect(QObject* sender, const char* signal, const char* slot = 0);

  /**
   * Disconnects all of *sender*'s signals from the slots of this
   * object.
   */
  bool dynamicDisconnect(QObject* sender);

protected:
  PiiUniversalSlot();

  /**
   * Invokes the universal slot.
   *
   * @param id the id of the slot that was invoked
   *
   * @param args typeless pointers to the signal's arguments. args[0]
   * is the return value, and the rest of the values point to function
   * call parameters, whose types match those listed in the signature.
   *
   * @return `true` if the call was successful and `false` otherwise.
   *
   * ~~~
   * bool MySlot::invokeSlot(int id, void** args)
   * {
   *   if (signatureOf(id) == "valueChanged(int)")
   *     {
   *       int iValue = *reinterpret_cast<int*>(args[1]);
   *       // ...
   *     }
   *   return true;
   * }
   * ~~~
   */
  virtual bool invokeSlot(int id, void** args) = 0;

  /**
   * Returns the normalized signature of the slot corresponding to
   * *id*.
   */
  QByteArray signatureOf(int id) const;

  /**
   * Converts the arguments of the slot identified by *id* to a
   * QVariantList.
   *
   * ~~~
   * bool MySlot::invokeSlot(int id, void** args)
   * {
   *   QVariantList lstArgs(argsToList(id, args));
   *   if (lstArgs.size() == 2)
   *     dealWithTwoArgs(); // ...
   *   return true;
   * }
   * ~~~
   */
  QVariantList argsToList(int id, void** args) const;

  /**
   * Returns the number of arguments for the slot identified by *id*.
   */
  int argumentCount(int id) const;

private:
  class Data
  {
  public:
    QList<QByteArray> lstSlots;
    QList<QList<int> > lstTypes;
    QMutex mutex;
  } *d;

  PII_DISABLE_COPY(PiiUniversalSlot);
};
