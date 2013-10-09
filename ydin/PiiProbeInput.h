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
 * #objectReceived() signal whenever a new object is received. It also
 * saves the last received object.
 *
 * @ingroup Ydin
 */
class PII_YDIN_EXPORT PiiProbeInput :
  public PiiSocket,
  public PiiAbstractInputSocket,
  public PiiInputController
{
  Q_OBJECT

  /**
   * The last received object. If no object has been received,
   * contains an invalid variant.
   */
  Q_PROPERTY(PiiVariant savedObject READ savedObject WRITE setSavedObject);
  Q_INTERFACES(PiiAbstractInputSocket)
public:
  /**
   * Constructs a new probe input and sets its @p objectName property
   * to @a name.
   */
  PiiProbeInput(const QString& name = "probe");

  /**
   * Constructs a new probe input and connects it to @a output. 
   * Connects the #objectReceived() signal to @a slot in @a receiver.
   *
   * @note If it is important that the objects are received in the
   * order they were emitted, use Qt::DirectConnection as the
   * connection type. The objects may be emitted from different
   * threads, and Qt's event loop doesn't guarantee chronological
   * ordering in such a case.
   *
   * @code
   * PiiOperation* reader = engine.createOperation("PiiImageFileReader");
   * PiiImageDisplay* display = new PiiImageDisplay;
   * PiiProbeInput* probe = new PiiProbeInput(reader->output("image"),
   *                                          display, SLOT(setImage(PiiVariant)));
   * @endcode
   */
  PiiProbeInput(PiiAbstractOutputSocket* output, const QObject* receiver,
                const char* slot, Qt::ConnectionType = Qt::AutoConnection);

  /**
   * Returns @p Input.
   */
  PiiSocket::Type type() const;
  
  /**
   * Emits #objectReceived() and saves the received object.
   */
  bool tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ();

  PiiVariant savedObject() const;
  void setSavedObject(const PiiVariant& obj);

  /**
   * Returns true if an object has been saved into this socket.
   */
  Q_INVOKABLE bool hasSavedObject() const;

  PiiInputController* controller() const;

  PiiProbeInput* socket();
  PiiAbstractInputSocket* asInput();
  PiiAbstractOutputSocket* asOutput();

signals:
  /**
   * Emitted whenever an object is received in this input socket. Note
   * that all objects, including control objects, will be emitted. The
   * slot receiving the objects can filter the objects based on their
   * type.
   *
   * @see PiiYdin::isControlType()
   */
  void objectReceived(const PiiVariant& obj);

private:
  class Data : public PiiAbstractInputSocket::Data
  {
  public:
    PiiVariant varSavedObject;
  };
  PII_UNSAFE_D_FUNC;
};

Q_DECLARE_METATYPE(PiiProbeInput*);

#endif //_PIIPROBEINPUT_H
