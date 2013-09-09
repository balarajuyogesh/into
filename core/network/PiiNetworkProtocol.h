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

#ifndef _PIINETWORKPROTOCOL_H
#define _PIINETWORKPROTOCOL_H

#include "PiiNetwork.h"

class QIODevice;
class PiiProgressController;

/**
 * An interface for all network communication protocols. If the
 * protocol is @e stateless (such as HTTP), all functions must be
 * thread-safe. @e Stateful protocols do not need to provide thread
 * safety in the #communicate() function as PiiNetworkServer clones
 * the class for each worker thread.
 *
 * @ingroup Network
 */
class PII_NETWORK_EXPORT PiiNetworkProtocol
{
public:
  virtual ~PiiNetworkProtocol();
  
  /**
   * The implementation of a network protocol. This function should
   * handle all the communication with a client. It is called by a
   * network server implementation when the I/O channel with a client
   * has been opened, but no data has been either received or sent.
   *
   * Once this function returns, either by finishing the connection or
   * by an interrupt signal, the protocol object must reset to its
   * initial state. The same protocol object may be used to serve many
   * successive clients.
   *
   * @param dev the I/O device used for two-way communication.
   *
   * @param controller a progress controller that either allows or
   * disallows the communication to proceed. The implementation must
   * call the @ref PiiProgressController::canContinue()
   * "canContinue()" function from time to time to check if it is
   * still allowed to run.
   */
  virtual void communicate(QIODevice* dev, PiiProgressController* controller) = 0;

  /**
   * Creates a copy of the protocol. This function is used by
   * PiiNetworkServer to create a copy of a protocol object for each
   * connecting client. If the protocol implementation is re-entrant
   * (i.e. the communicate() function can be called concurrently from
   * many threads and there is no state associated with a client),
   * this function can return @p this. This function needs to be
   * thread-safe.
   *
   * @return an intialized copy of the protocol object. The default
   * implementation returns @p this.
   */
  virtual PiiNetworkProtocol* clone() const;

protected:
  /// @internal
  class Data
  {
  public:
    virtual ~Data();
  } *d;
  /// @internal
  PiiNetworkProtocol(Data* data);
  
  PiiNetworkProtocol();
};

#endif //_PIINETWORKPROTOCOL_H
