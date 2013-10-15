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

#ifndef _PIISOCKETDEVICE_H
#define _PIISOCKETDEVICE_H

#include <QThread>
#include <QMutex>
#include "PiiNetwork.h"
#include <PiiTimer.h>
#include <PiiDelay.h>

class PiiProgressController;
class QIODevice;
class QAbstractSocket;
class QLocalSocket;

/**
 * A wrapper class that works as a generic socket device. The purpose
 * of this class is to work around Qt's lack of a common socket-type
 * superclass for QLocalSocket and QAbstractSocket. These cannot be
 * used interchangeably because their common superclass, QIODevice,
 * provides no general way of checking if the device is
 * readable/writable. For example, TCP sockets are "open" (isOpen()
 * returns `true`) even after the connection has been broken. This
 * class provides the [isReadable()] and [isWritable()] functions that
 * can be used to check if the socket is really available for reading
 * and writing. This class also provides functionality for reading and
 * writing data to/from devices that may not have everything available
 * at once.
 *
 * PiiSocketDevice is special in that it can be used in most places as
 * if it was a QIODevice pointer. Once initialized with a QIODevice
 * pointer, the class masquerades as one itself.
 *
 */
class PII_NETWORK_EXPORT PiiSocketDevice
{
public:
  /**
   * Constructs a null socket, which behaves like a null pointer. Any
   * access to the socket will cause a segfault.
   */
  PiiSocketDevice();
  /**
   * Constructs a PiiSocketDevice that wraps the given *device*. The
   * type of the device will be automatically determined. All Qt
   * socket types are recognized.
   */
  PiiSocketDevice(QIODevice* device);
  /**
   * Constructs a PiiSocketDevice that wraps the given *socket*.
   */
  PiiSocketDevice(QAbstractSocket* socket);
  /**
   * Constructs a PiiSocketDevice that wraps the given *socket*.
   */
  PiiSocketDevice(QLocalSocket* socket);
  /**
   * Constructs a copy of *other*.
   */
  PiiSocketDevice(const PiiSocketDevice& other);
  ~PiiSocketDevice();

  PiiSocketDevice& operator= (const PiiSocketDevice& other);
  PiiSocketDevice& operator= (QIODevice* device);

  /**
   * Returns `true` if it is possible that the socket still has data
   * and `false` otherwise. This function handles special cases such
   * as a disconnected socket with unread data in its receive buffer.
   */
  bool isReadable() const;
  
  /**
   * Returns `true` if the socket can be written to and `false`
   * otherwise.
   */
  bool isWritable() const;

  /**
   * Reads at most *maxSize* bytes of data from the socket and places
   * it to *data*. If not enough data is immediately available, waits
   * at most *waitTime* milliseconds to get more. If *controller* is
   * given, it can be used to terminate a long wait.
   */
  qint64 readWaited(char* data, qint64 maxSize, int waitTime = 5000, PiiProgressController* controller = 0);
  /**
   * Writes at most *maxSize* bytes of data from *data* to the
   * socket. If all data cannot be immediately written, waits at most
   * *waitTime* milliseconds. If *controller* is given, it can be
   * used to terminate a long wait.
   */
  qint64 writeWaited(const char* data, qint64 maxSize, int waitTime = 5000, PiiProgressController* controller = 0);

  bool waitForConnected(int waitTime);
  
  /**
   * Disconnects the socket if it is connected to a server.
   */
  void disconnect();

  bool waitForDisconnected(int waitTime);

  QIODevice* device() const;
  operator QIODevice* () const;
  QIODevice* operator-> () const;

private:
  enum Type { IODevice, AbstractSocket, LocalSocket };
  class Data : public PiiSharedD<Data>
  {
  public:
    Data();
    Data(QIODevice*);
    Data(QIODevice*, Type);
    Data(const Data& other);
    QIODevice* pDevice;
    Type type;
  } *d;
  
  inline bool waitForDataAvailable(int waitTime, PiiProgressController* controller);
  inline bool waitForDataWritten(int waitTime, PiiProgressController* controller);
  static Type typeOf(QIODevice* device);
};

#endif //_PIISOCKETDEVICE_H
