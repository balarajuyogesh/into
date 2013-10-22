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

#ifndef _PIIINPUTCONTROLLER_H
#define _PIIINPUTCONTROLLER_H

#include "PiiVariant.h"
#include "PiiExecutionException.h"

class PiiAbstractInputSocket;

/**
 * An interface for classes that receive objects from input sockets. 
 * Input controllers can implement any imaginable object passing
 * mechanisms. One must make sure that operations connected to each
 * other are driven by compatible input controllers. For example, any
 * operation connected to an operation derived from
 * PiiDefaultOperation must be able to handle synchronization tags and
 * stop tags.
 *
 */
class PII_YDIN_EXPORT PiiInputController
{
public:
  virtual ~PiiInputController();

  /**
   * Activate any functionality that should be performed when a new
   * object arrives into an input. This method is called by an input
   * socket whenever a new object is received on it.
   *
   * @param sender a pointer to the socket that received the object
   *
   * @param object the object that was received
   *
   * @return `true` if the object was accepted, `false` otherwise.
   * Typically, an object is accepted if there is no blocking object
   * in `sender` by calling [PiiInputSocket::receive()]. Note that
   * `true` should be returned whenever the object is accepted for
   * processing, even if the processing itself will fail.
   *
   * Note that the function may not throw exceptions. If your
   * implementation passes the incoming objects right away, you must
   * catch the exceptions thrown by 
   * [PiiOutputSocket::emitObject()], for example.
   */
  virtual bool tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw () = 0;
};

#endif //_PIIINPUTCONTROLLER_H
