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

#ifndef _PIINETWORKSERVER_H
#define _PIINETWORKSERVER_H

#include "PiiNetwork.h"
#include <QString>
#include <QQueue>
#include <QMutex>

#include "PiiNetworkServerThread.h"

/**
 * An implementation of a threaded network server. This class provides
 * a framework for server processes that handle incoming connections
 * by creating a worker thread for each. PiiNetworkServer is designed
 * to work with any native socket type. Subclasses provide
 * implementations for TCP servers and local socket servers.
 *
 * PiiNetworkServer is intended to be used with an event loop. The
 * loop is needed in delivering incoming connections to worker
 * threads. If you create a server in a thread other than the main
 * thread, you must start the server in the same thread and run an
 * event loop there. It is not possible to move servers from a thread
 * to another due to limitations of the Qt threading system.
 *
 * @see PiiTcpServer
 * @see PiiLocalServer
 *
 */
class PII_NETWORK_EXPORT PiiNetworkServer :
  public QObject,
  private PiiNetworkServerThread::Controller
{
  Q_OBJECT

  /**
   * Minimum number of worker threads always available. Keeping a
   * reasonable number of idle threads available increases the
   * server's performance under load. The default value is 0.
   */
  Q_PROPERTY(int minWorkers READ minWorkers WRITE setMinWorkers);

  /**
   * The maximum number of worker threads ever running concurrently. 
   * If the number of concurrent connections exceeds this value,
   * subsequent connection attempts will be stored as pending
   * connections. The upper bound for this value is 1000. The default
   * value is 10.
   *
   * @see maxPendingConnections
   */
  Q_PROPERTY(int maxWorkers READ maxWorkers WRITE setMaxWorkers);

  /**
   * The time (in milliseconds) a worker thread is allowed to be idle
   * before dying. If a worker thread has not been activated within
   * this time, and more than [minWorkers] worker threads are alive,
   * the thread will be destroyed. The default value is 10000.
   */
  Q_PROPERTY(int workerMaxIdleTime READ workerMaxIdleTime WRITE setWorkerMaxIdleTime);

  /**
   * The number of pending connection attempts to keep. If the server
   * cannot create new worker threads due to [maxWorkers] limitation,
   * it will place the connection into a list of pending connections.
   * Keep this value low to avoid client timeouts without any
   * response. The default value is 0.
   */
  Q_PROPERTY(int maxPendingConnections READ maxPendingConnections WRITE setMaxPendingConnections);

  /**
   * The address the server binds to. The format of the address
   * depends on the server type. PiiTcpServer uses a combination of IP
   * address and port number, PiiLocalServer users the name of a
   * socket or a pipe.
   */
  Q_PROPERTY(QString serverAddress READ serverAddress WRITE setServerAddress);
 
  /**
   * The message sent to the client when the server cannot handle more
   * connections. The default value is "Server busy\\n".
   */
  Q_PROPERTY(QString busyMessage READ busyMessage WRITE setBusyMessage);

public:
  /**
   * Interrupts all open connections and destroys the server.
   */
  ~PiiNetworkServer();
  
  /**
   * Starts the server. This function initializes the server's thread
   * pool and calls [startListening()] to allow actual server implementations
   * to start listening to a socket.
   *
   * Note that this function will not block. If the server starts
   * successfully, it will continue running in the backround.
   */
  bool start();

  /**
   * Sends a stop signal to the server. This function stops all worker
   * threads and waits until they are done. It then calls
   * [stopListening()].
   *
   * @param mode InterruptClients: the worker threads will be
   * interrupted even if they are not done with their clients yet. 
   * WaitClients: the threads will wait until all clients finish
   * cleanly.
   *
   * @return `true` if the server was successfully stopped, `false`
   * if it wasn't running.
   *
   * @see QTcpServer::close()
   */
  bool stop(PiiNetwork::StopMode mode = PiiNetwork::WaitClients);

  bool isRunning() const;

  void setMinWorkers(int minWorkers);
  int minWorkers() const;
  void setMaxWorkers(int maxWorkers);
  int maxWorkers() const;
  void setWorkerMaxIdleTime(int workerMaxIdleTime);
  int workerMaxIdleTime() const;
  void setMaxPendingConnections(int maxPendingConnections);
  int maxPendingConnections() const;
  void setBusyMessage(const QString& busyMessage);
  QString busyMessage() const;

  /**
   * Get the communication protocol.
   */
  PiiNetworkProtocol* protocol() const;

  /**
   * Set the server's bind address.
   *
   * @param serverAddress the address in an implementation-dependent
   * format.
   *
   * @return `true` if the address was successfully set, `false`
   * otherwise.
   */
  virtual bool setServerAddress(const QString& serverAddress) = 0;
  virtual QString serverAddress() const = 0;

protected:
  /// @internal
  enum State { Stopped, Stopping, Running };
  /// @internal
  class Data
  {
  public:
    Data(PiiNetworkProtocol* protocol);
    virtual ~Data();
    
    int iMinWorkers;
    int iMaxWorkers;
    int iWorkerMaxIdleTime;
    int iMaxPendingConnections;
    QByteArray aBusyMessage;
    
    QMutex threadListLock;
    QList<PiiNetworkServerThread*> lstFreeThreads, lstAllThreads, lstFinishedThreads;
    QQueue<PiiGenericSocketDescriptor> lstPendingConnections;
    PiiNetworkProtocol* pProtocol;
    
    State state;
    QString strServerAddress;
  } *d;

  /// @internal
  PiiNetworkServer(Data* d);

  /// @internal
  void timerEvent(QTimerEvent*);
  
  /**
   * Initializes the server with a communication protocol.
   *
   * @param protocol The protocol used for communication. If the
   * protocol is stateless, it will be shared among all worker
   * threads. A stateful protocol will be cloned for each worker. The
   * protocol must be non-zero and it must remain valid during the
   * lifetime of the server.
   */
  PiiNetworkServer(PiiNetworkProtocol* protocol);

  /**
   * This function is called by [start()] after the thread pool has
   * been initialized. A typical overridden implementation makes the
   * low-level server implementation to start listening to a socket.
   *
   * @return `true` if the server was successfully started, `false`
   * otherwise.
   */
  virtual bool startListening() = 0;
  
  /**
   * This function is called by [stop()] after the thread pool has been
   * destroyed. Typically, a listening socket is be closed. The server
   * implementation must not accept new connections after this
   * function has been called unless [startListening()] is called
   * again.
   */
  virtual void stopListening() = 0;
  
  /**
   * Handles an incoming connection. This function tries the following,
   * in the order of precedence:
   *
   * -# Pick an idle worker thread to handle the connection.
   *
   * -# Create a new thread with [createWorker()].
   *
   * -# Put the connection to the list of pending connections.
   *
   * -# Call [serverBusy()].
   *
   * The function is called by subclasses that handle the conversion
   * from their native socket descriptor format to
   * PiiGenericSocketDescriptor.
   */
  void incomingConnection(PiiGenericSocketDescriptor socketDescriptor);

  /**
   * Called by the server when [maxWorkers] threads are running, and
   * [maxPendingConnections] connection attempts have been already
   * queued when a new connection attempt comes in. Subclasses
   * implement this function to provide a protocol-specific response.
   *
   * The default implementation sends a user-specified message (UTF-8
   * encoded) to the device returned by `createSocket`().
   *
   * @see [setBusyMessage()]
   */
  virtual void serverBusy(PiiGenericSocketDescriptor socketDescriptor);
  
  /**
   * Create a new worker thread. Subclasses may override this function
   * to set thread priorities etc. The default implementation creates
   * a new PiiNetworkServerThread.
   *
   * @param protocol the protocol used for communication. If the
   * protocol is stateless, PiiNetworkServer retains its ownership. If
   * it is stateful, the passed pointer is a clone that must be
   * deleted by whoever will finally take the pointer.  Usually, the
   * pointer is just passed to the constructor of
   * PiiNetworkServerThread, which automatically takes care of
   * deleting the pointer.
   */
  virtual PiiNetworkServerThread* createWorker(PiiNetworkProtocol* protocol);

private:
  friend class PiiNetworkServerThread;
  void threadAvailable(PiiNetworkServerThread* thread);
  void threadFinished(PiiNetworkServerThread* tread);  

  void deleteFinishedThreads();
  void waitAll(const QList<PiiNetworkServerThread*>& threads);
  
  PII_DISABLE_COPY(PiiNetworkServer);
};

#endif //_PIINETWORKSERVER_H
