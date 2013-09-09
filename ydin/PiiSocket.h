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

class PiiAbstractInputSocket;
class PiiAbstractOutputSocket;

/**
 * Socket is a connection point between operations. Each socket works
 * either as an input, an output, or a proxy. Sockets within an
 * operation are named uniquely so that inputs and outputs can be
 * retrieved with identifier strings. Examples of such strings include
 * "features", "class label", and "image". Usually, @p objectName is
 * used to store the name of the socket. The name may, however, be
 * different in different contexts. @ref PiiOperationCompound
 * "Operation compounds" may expose their internal sockets with
 * different names. To get the name of a socket in the context of an
 * operation, use the @ref PiiOperation::socketName() function.
 *
 * Due to the fact that QObject doesn't support virtual inheritance,
 * all concrete implementations of this interface also derive from
 * PiiAbstractSocket.
 *
 * @ingroup Ydin
 */
class PII_YDIN_EXPORT PiiSocket : public QObject
{
  Q_OBJECT
  Q_ENUMS(Type);
  
public:
  /**
   * Socket types.
   */
  enum Type { Input, Output, Proxy };
  
  ~PiiSocket();

  /**
   * Get the type of the socket. This function can be used for fast
   * run-time type checking.
   */
  Q_INVOKABLE virtual Type type() const = 0;

  /**
   * Returns a pointer to this socket as PiiAbstractInputSocket, if this
   * socket is either an @p Input or a @p Proxy. Otherwise returns
   * zero.
   */
  virtual PiiAbstractInputSocket* asInput() = 0;

  /**
   * Returns a pointer to this socket as PiiAbstractOutputSocket, if this
   * socket is either an @p Output or a @p Proxy. Otherwise returns
   * zero.
   */
  virtual PiiAbstractOutputSocket* asOutput() = 0;

protected:
  PiiSocket();
};

Q_DECLARE_METATYPE(PiiSocket*);

#endif //_PIISOCKET_H
