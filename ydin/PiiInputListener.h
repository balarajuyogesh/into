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

#ifndef _PIIINPUTSOCKETLISTENER_H
#define _PIIINPUTSOCKETLISTENER_H

#include "PiiYdin.h"

class PiiAbstractInputSocket;

/**
 * An interface for objects that receive events from input sockets.
 * Instances of this interface are used in communications between
 * connected socket pairs.
 *
 */
class PII_YDIN_EXPORT PiiInputListener
{
public:
  virtual ~PiiInputListener();

  /**
   * Indicates this output socket that `input` is ready to receive
   * new objects.
   */
  virtual void inputReady(PiiAbstractInputSocket* input) = 0;
};


#endif //_PIIINPUTSOCKETLISTENER_H
