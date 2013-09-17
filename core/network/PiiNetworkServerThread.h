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

#ifndef _PIINETWORKSERVERTHREAD_H
#define _PIINETWORKSERVERTHREAD_H

#include <QThread>
#include <PiiWaitCondition.h>

#include <PiiProgressController.h>
#include "PiiNetworkProtocol.h"
#include "PiiGenericSocketDescriptor.h"

/**
 * A worker thread that handles an incoming network connection. The
 * thread sleeps if there are no clients to be handled and will be
 * finally killed by PiiNetworkServer. An idle thread can be assigned
 * to a new client once the old one has been handled.
 *
 */
class PII_NETWORK_EXPORT PiiNetworkServerThread : public QThread, public PiiProgressController
{
  Q_OBJECT

public:
  /**
   * An intereface for thread controllers. A controller listens to
   * thread status changes and creates sockets on request.
   */
  class Controller
  {
  public:
    virtual ~Controller() {}
    
    /**
     * Called by the thread when the protocol has done its work. 
     */
    virtual void threadAvailable(PiiNetworkServerThread* thread) = 0;

    /**
     * Called by the thread when it is about to finish. The thread
     * finishes if it has not been assigned new clients for a while,
     * or [stop()] has been called. Use the [setTimeOut()] function to
     * change the maximum wait time.
     */
    virtual void threadFinished(PiiNetworkServerThread* thread) = 0;

    /**
     * Create a new socket device for communicating through the given
     * socket. It is advisable to use a PiiWaitingIoDevice to ensure
     * that the protocol doesn't need to cope with slow connections
     * and buffering. @p PiiNetworkServerThread calls this function
     * to create a socket device and passes the returned pointer to
     * the protocol implementation. Once the protocol is done, the
     * device will be closed and deleted.
     */
    virtual QIODevice* createSocket(PiiGenericSocketDescriptor socketDescriptor) = 0;
  };

  
  /**
   * Create a new worker thread that uses `protocol` to communicate
   * with clients. If `protocol` is stateful (@ref
   * PiiNetworkProtocol::hasState() return `true`), the thread
   * assumes the ownership of the pointer. Otherwise, the pointer must
   * remain valid during the lifetime of the thread.
   */
  PiiNetworkServerThread(PiiNetworkProtocol* protocol);

  /**
   * Calls [stop()] and waits for the thread to exit, if it is still
   * running. Then deletes the protocol if it is stateful.
   */
  ~PiiNetworkServerThread();

  /**
   * Set the controller. The controller must be set before
   * [startRequest()] is called.
   */
  void setController(Controller* controller);

  /**
   * Get the controller.
   */
  Controller* controller() const;  
  
  /**
   * Start handling of an incoming network request. This function
   * stores the socket descriptor and either starts or wakes up the
   * thread.
   *
   * @param socketDescriptor the socket descriptor of a connected
   * socket.
   */
  void startRequest(PiiGenericSocketDescriptor socketDescriptor);

  /**
   * Sends a stop signal to the thread. The thread will later exit
   * asynchronously.
   *
   * @param mode `InterruptClients`, the protocol will be interrupted
   * even if it is not done with the client yet. If `WaitClients`,
   * the thread will wait until the client finishes cleanly.
   */
  void stop(PiiNetwork::StopMode mode = PiiNetwork::WaitClients);

  /**
   * Returns `false` if the thread has been stopped with the 
   * `interrupt` flag set to `true`. Otherwise returns `true`.
   */
  bool canContinue(double percentage) const;

  /**
   * Set the maximum number of milliseconds the thread will wait for a
   * new client before terminating itself.
   */
  void setMaxIdleTime(int maxIdleTime);
  /**
   * Get the maximum wait time (milliseconds).
   */
  int maxIdleTime() const;

protected:
  void run();

private:
  /// @internal
  class Data
  {
  public:
    Data(PiiNetworkProtocol* protocol);
    PiiNetworkProtocol* pProtocol;
    bool bOwnProtocol;
    Controller* pController;
    PiiGenericSocketDescriptor socketDescriptor;
    PiiWaitCondition requestCondition;
    volatile bool bRunning, bInterrupted;
    int iMaxIdleTime;
  } *d;
};

#endif //_PIINETWORKSERVERTHREAD_H
