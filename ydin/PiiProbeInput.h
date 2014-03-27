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

#ifndef _PIIPROBEINPUT_H
#define _PIIPROBEINPUT_H

#include "PiiInputSocket.h"
#include "PiiInputController.h"

/**
 * An input socket that works without a controlling operation. The
 * socket works like a measurement probe that emits the
 * [objectReceived()] signal whenever a new object is received. It
 * also saves the last received object.
 */
class PII_YDIN_EXPORT PiiProbeInput :
  public PiiAbstractInputSocket
{
  Q_OBJECT

  /**
   * The last received object. If no object has been received,
   * contains an invalid variant.
   */
  Q_PROPERTY(PiiVariant savedObject READ savedObject WRITE setSavedObject);

  /**
   * Toggles filtering of control objects. The default value is
   * `false`, which means every incoming object will be
   * emitted. Turning this value to `true` blocks the saving and
   * emitting of control objects.
   */
  Q_PROPERTY(bool discardControlObjects READ discardControlObjects WRITE setDiscardControlObjects);

  /**
   * The minimum time between successive objectReceived() signals in
   * milliseconds. Setting this value to a positive value limits the
   * rate of signals emitted. Note that the emission interval affects
   * only outgoing signals; the last incoming object will be saved
   * anyway. Setting *signalInterval* to -1 disables the signal
   * altogether. In this mode, the objects are just saved. The default
   * value is zero, which means every incoming object will be
   * signalled.
   */
  Q_PROPERTY(int signalInterval READ signalInterval WRITE setSignalInterval);

public:
  /**
   * Constructs a new probe input and sets its `objectName` property
   * to *name*.
   */
  PiiProbeInput(const QString& name = "probe");

  /**
   * Constructs a new probe input and connects it to *output*.
   * Connects the [objectReceived()] signal to *slot* in *receiver*.
   *
   * ! If it is important that the objects are received in the
   * order they were emitted, use Qt::DirectConnection as the
   * connection type. The objects may be emitted from different
   * threads, and Qt's event loop doesn't guarantee chronological
   * ordering in such a case.
   *
   * ~~~(c++)
   * PiiOperation* reader = engine.createOperation("PiiImageFileReader");
   * PiiImageDisplay* display = new PiiImageDisplay;
   * PiiProbeInput* probe = new PiiProbeInput(reader->output("image"),
   *                                          display, SLOT(setImage(PiiVariant)));
   * ~~~
   */
  PiiProbeInput(PiiAbstractOutputSocket* output, const QObject* receiver,
                const char* slot, Qt::ConnectionType = Qt::AutoConnection);

  ~PiiProbeInput();

  PiiVariant savedObject() const;
  void setSavedObject(const PiiVariant& obj);
  /**
   * Returns true if an object has been saved into this socket.
   */
  Q_INVOKABLE bool hasSavedObject() const;

  void setDiscardControlObjects(bool discardControlObjects);
  bool discardControlObjects() const;

  void setSignalInterval(int signalInterval);
  int signalInterval() const;

  PiiInputController* controller() const;

signals:
  /**
   * Emitted whenever an object is received in this input socket. Note
   * that all objects, including control objects, will be emitted by
   * default. The slot receiving the objects can filter the objects
   * based on their type. Additionally, the [discardControlObjects]
   * property can be used to filter out control objects.
   *
   * @see PiiYdin::isControlType()
   */
  void objectReceived(const PiiVariant& obj, PiiProbeInput* sender);

protected:
  void timerEvent(QTimerEvent*);

private:
  class Data;
  PII_UNSAFE_D_FUNC;
};

Q_DECLARE_METATYPE(PiiProbeInput*);

#endif //_PIIPROBEINPUT_H
