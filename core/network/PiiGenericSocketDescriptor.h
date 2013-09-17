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

#ifndef _PIIGENERICSOCKETDESCRIPTOR_H
#define _PIIGENERICSOCKETDESCRIPTOR_H

#include "PiiNetwork.h"
#include <cstring>

/**
 * A generic socket descriptor that can be used with many native
 * socket descriptor types. It supports at least network sockets
 * (UDP/TCP) and local sockets (unix domain sockets/pipes). More
 * socket descriptors can be added by using the `pCustomDescriptor`
 * member.
 *
 */
struct PII_NETWORK_EXPORT PiiGenericSocketDescriptor
{
  /**
   * Create an invalid socket descriptor.
   */
  PiiGenericSocketDescriptor() : customDescriptor(0) {}
  
  /**
   * Create a new %SocketDescriptor with the given native
   * socketDescriptor. This constructor is suitable for use with
   * network sockets.
   *
   * @see PiiTcpServer
   */
  PiiGenericSocketDescriptor(PiiNetwork::SocketDescriptorType socketDescriptor) :
    networkSocketDescriptor(socketDescriptor) {}
  /**
   * Create a new %SocketDescriptor with the given native
   * socketDescriptor. This constructor is suitable for use with
   * local sockets.
   *
   * @see PiiLocalServer
   */
  PiiGenericSocketDescriptor(quintptr socketDescriptor) : localSocketDescriptor(socketDescriptor) {}
  /**
   * Create a new %SocketDescriptor with the given native
   * socketDescriptor. This constructor is suitable for use with any
   * native socket descriptor.
   */
  PiiGenericSocketDescriptor(void* socketDescriptor) : customDescriptor(socketDescriptor) {}
    
  union
  {
    PiiNetwork::SocketDescriptorType networkSocketDescriptor;
    quintptr localSocketDescriptor;
    void* customDescriptor;
  };

  inline bool operator== (PiiGenericSocketDescriptor other) { return memcmp(this, &other, sizeof(PiiGenericSocketDescriptor)) == 0; }
};


#endif //_PIIGENERICSOCKETDESCRIPTOR_H
