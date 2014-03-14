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

#ifndef _PIISOCKET_H
#define _PIISOCKET_H

#include "PiiYdin.h"
#include <QObject>

class PiiProxySocket;
class PiiOperation;

/**
 * Socket is a connection point between operations. Each socket works
 * either as an input (receiving data) or as an output (sending data).
 *
 * In an operation, each input socket and each output socket has a
 * unique name. It is however possible to have an input and an output
 * with the same name. The name of a socket is stored as its
 * `objectName` property.
 */
class PII_YDIN_EXPORT PiiSocket : public QObject
{
  Q_OBJECT

public:
  ~PiiSocket();

  /**
   * Returns `true` if this socket either an input or an output that
   * is part of a proxy and `false` otherwise. Either isInput() or
   * isOutput() (but not both) will also return `true`.
   */
  Q_INVOKABLE bool isProxy() const;
  /**
   * Returns `true` if this socket is either "normal" or a proxied
   * input and `false` otherwise.
   */
  Q_INVOKABLE bool isInput() const;
  /**
   * Returns `true` if this socket is either "normal" or a proxied
   * output and `false` otherwise.
   */
  Q_INVOKABLE bool isOutput() const;

  /**
   * Returns the fully-qualified name (FQN) of the socket. The FQN is
   * formed by concatenating the `objectName` properties of all parent
   * operations of the socket except the topmost one, using dots (.)
   * as separators. The FQN can be passed to the input() or output()
   * function of the topmost enclosing operation (usually a PiiEngine)
   * to get a pointer to this socket.
   */
  Q_INVOKABLE QString fullName() const;

  /**
   * Returns a pointer to the operation that owns this socket, or 0 if
   * there is no such operation.
   */
  Q_INVOKABLE PiiOperation* parentOperation() const;

protected:
  /// @hide
  enum Type { Input, Output, ProxyInput, ProxyOutput };

  class PII_YDIN_EXPORT Data
  {
  public:
    Data(Type t);
    virtual ~Data();
    Type type;
  } *d;

  PiiSocket(Data*);
  PII_DISABLE_COPY(PiiSocket);
  /// @endhide
};

Q_DECLARE_METATYPE(PiiSocket*);

#endif //_PIISOCKET_H
