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

#ifndef _PIIABSTRACTOUTPUTSOCKET_H
#define _PIIABSTRACTOUTPUTSOCKET_H

#include "PiiSocket.h"
#include <QVector>
#include <QList>
#include <QPair>

class PiiAbstractInputSocket;
class PiiInputController;

/**
 * An abstract superclass for output sockets. Operations transfer
 * objects to other operations through an output socket. The
 * connection between an output socket and an input socket is
 * one-directional: the output socket pushes data forwards, but the
 * input cannot "pull" new objects. The input can, however, tell the
 * output when it is ready to receive new data via the [inputReady()]
 * signal.
 *
 * This class is a representation of a connection point that is able
 * to receive "I'm ready" signals from connected input sockets
 * ([inputReady()]). The actual mechanism of passing data is defined
 * in subclasses (PiiOutputSocket, PiiProxySocket).
 *
 * When an input socket is deleted, its connection to an output socket
 * is automatically destroyed and vice-versa.
 *
 */
class PII_YDIN_EXPORT PiiAbstractOutputSocket :
  public PiiSocket
{
  Q_OBJECT
public:
  ~PiiAbstractOutputSocket();

  /**
   * Connects this socket to the specified input socket. The new input
   * will be appended to the list of connected inputs.
   *
   * @param input the input socket to connect
   */
  Q_INVOKABLE void connectInput(PiiAbstractInputSocket* input);

  /**
   * Updates any cached information related to `input`. This function
   * must be called by a connected input socket whenever its
   * configuration (such as the controller) changes.
   */
  void updateInput(PiiAbstractInputSocket* input);

  /**
   * Disconnects this socket from an input. After disconnect, objects
   * emitted through this socket are no longer delivered to this input
   * socket.
   *
   * @param input the input to disconnect from, or 0 if all
   * connections are to be broken.
   */
  Q_INVOKABLE void disconnectInput(PiiAbstractInputSocket* input);

  Q_INVOKABLE void disconnectInputs() { disconnectInput(0); }

  /**
   * Reconnects all inputs currently connected to this output to
   * `output`, and reconnects this output to `input`.
   */
  Q_INVOKABLE void reconnect(PiiAbstractOutputSocket* output, PiiAbstractInputSocket* input);

  /**
   * Returns the number of connected input sockets. This is equivalent
   * but faster than calling `connectedInputs().size()`.
   */
  Q_INVOKABLE int connectedInputCount() const;

  /**
   * Returns the fully-qualified names (FQN) of all connected
   * inputs. The FQN is formed by concatenating the `objectName`
   * properties of all parent operations of a socket. Therefore, a
   * pointer to the input socket can be obtained by calling
   * `input(FQN)` on the topmost enclosing operation (typically, a
   * PiiEngine).
   *
   * @see PiiSocket::fullName()
   */
  Q_INVOKABLE QStringList connectedInputNames() const;

  /**
   * Returns all inputs this socket is connected to.
   */
  QList<PiiAbstractInputSocket*> connectedInputs() const;

protected:
  /// @hide
  typedef QVector<QPair<PiiAbstractInputSocket*,PiiInputController*> > InputListParent;

  class InputList : public InputListParent
  {
  public:
    InputList() {}
    InputList(const InputList& other) : InputListParent(other) {}

    InputList& operator=(const InputList& other) { InputListParent::operator=(other); return *this; }

    int indexOf(PiiAbstractInputSocket* input) const;
    PiiAbstractInputSocket* takeInputAt(int index);
    PiiAbstractInputSocket* inputAt(int index) const;
    PiiInputController* controllerAt(int index) const;
    void append(PiiAbstractInputSocket* input);
    void updateController(PiiAbstractInputSocket* input);
  };

  class PII_YDIN_EXPORT Data :
    public PiiSocket::Data
  {
  public:
    Data();
    ~Data();

    /*
     * Recursively updates connection statuses of input sockets
     * connected to this output.
     *
     * @param connected a flag that indicates whether the sequence of
     * sockets leading to this socket is connected to an operation or
     * not.
     *
     * @return `true` if the socket is connected, `false` otherwise.
     * If the socket is a proxy, it may be unconnected even after
     * setInputConnected(true) if none of its outgoing connections
     * leads to a connected input.
     */
    virtual bool setOutputConnected(bool connected);

    /*
     * Called by #connectInput() when an input has been connected.
     * The default implementation does nothing.
     */
    virtual void inputConnected(PiiAbstractInputSocket* input);
    /*
     * Called by #updateInput() when an input has been updated
     * (e.g. when the controller of an input changes). The default
     * implementation does nothing.
     */
    virtual void inputUpdated(PiiAbstractInputSocket* input);
    /*
     * Called by #disconnectInput() when an input has been disconnected.
     * The default implementation does nothing.
     */
    virtual void inputDisconnected(PiiAbstractInputSocket* input);

    // All connected input sockets.
    InputList lstInputs;
  };
  PII_D_FUNC;

  PiiAbstractOutputSocket(const QString& name, Data* data);
  /// @endhide
private:
  void disconnectInputAt(int index);
};

Q_DECLARE_METATYPE(PiiAbstractOutputSocket*)

#endif //_PIIABSTRACTOUTPUTSOCKET_H
