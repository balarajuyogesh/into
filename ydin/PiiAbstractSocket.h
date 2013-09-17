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

#ifndef _PIIABSTRACTSOCKET_H
#define _PIIABSTRACTSOCKET_H

#include "PiiYdin.h"

class PiiSocket;
class PiiInputSocket;
class PiiOutputSocket;
class PiiProxySocket;

/**
 * A superclass for socket interfaces. Due to the fact that QObject
 * doesn't support virtual inheritance, all concrete implementations
 * of this interface also derive from PiiSocket. To avoid
 * dynamic_casts at run-time, this interfaces provides a virtual
 * function, [socket()], for performing a cross-cast.
 *
 */
class PII_YDIN_EXPORT PiiAbstractSocket
{
public:
  virtual ~PiiAbstractSocket();

  /**
   * Returns a pointer to this socket as a PiiSocket. This is the
   * preferred way of performing a cross-cast, even if `dynamic_cast`
   * would work in most cases.
   */
  virtual PiiSocket* socket() = 0;

  /**
   * Returns a pointer to this socket as a `const` PiiSocket. This
   * function calls the non-const version.
   */
  inline const PiiSocket* socket() const
  {
    return const_cast<PiiAbstractSocket*>(this)->socket();
  }

protected:
  PiiAbstractSocket();
  
  // No d-pointer here to avoid dynamic_cast at run time.
};

Q_DECLARE_METATYPE(PiiAbstractSocket*);

#endif //_PIIABSTRACTSOCKET_H
