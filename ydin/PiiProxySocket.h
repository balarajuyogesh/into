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

#ifndef _PIIPROXYSOCKET_H
#define _PIIPROXYSOCKET_H

#include "PiiYdin.h"

class PiiSocket;
class PiiAbstractInputSocket;
class PiiAbstractOutputSocket;
class PiiProxyInputSocket;
class PiiProxyOutputSocket;
class PiiOperationCompound;

/**
 * A proxy that contains both an input and an output socket and passes
 * incoming objects as such.
 */
class PII_YDIN_EXPORT PiiProxySocket :
  public QObject
{
  Q_OBJECT

public:
  ~PiiProxySocket();

  Q_INVOKABLE PiiAbstractInputSocket* input() const;
  Q_INVOKABLE PiiAbstractOutputSocket* output() const;

  using QObject::parent;

  /**
   * Returns the input side of a socket that is part of a proxy. If
   * *socket* is not in a proxy, returns 0.
   */
  static PiiAbstractInputSocket* input(PiiSocket* socket);
  /**
   * Returns the output side of a socket that is part of a proxy. If
   * *socket* is not in a proxy, returns 0.
   */
  static PiiAbstractOutputSocket* output(PiiSocket* socket);
  /**
   * Returns the parent of *socket*, or 0 if *socket* is not part of a
   * proxy.
   */
  static PiiProxySocket* parent(PiiSocket* socket);
  /**
   * Finds (backwards) the first non-proxy input in a chain of
   * connected sockets. If *output* is not a proxy, returns
   * *output*. Otherwise, goes back through all proxies until finds an
   * output that is not a proxy. If there is no such output, returns
   * 0.
   */
  static PiiAbstractOutputSocket* root(PiiAbstractOutputSocket* output);

  /**
   * Returns the non-proxy inputs to which *input* is connected
   * (recursively). If *input* is not a proxy, returns a list
   * containing just *input*. If *input* is 0, returns an empty list.
   */
  static QList<PiiAbstractInputSocket*> connectedInputs(PiiAbstractInputSocket* input);

  /**
   * Returns `true` if *input* is part of a proxy that is either
   * directly or indirectly connected to the proxy that contains
   * *output* and `false` otherwise.
   */
  static bool isConnected(PiiAbstractInputSocket* input, PiiAbstractOutputSocket* output);

private:
  friend class PiiOperationCompound;
  // Probably an unortohodox solution, but hey, tests are important.
  friend class TestPiiSocket;
  PiiProxySocket(QObject* parent = 0);
  PiiProxySocket(const QString& inputName, const QString& outputName, QObject* parent = 0);
  void reset();
  class Data;
  Data* d;
  PII_DISABLE_COPY(PiiProxySocket);
};

Q_DECLARE_METATYPE(PiiProxySocket*);

#endif //_PIIPROXYSOCKET_H
