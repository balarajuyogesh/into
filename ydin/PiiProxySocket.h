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
#include "PiiSocket.h"
#include "PiiAbstractInputSocket.h"
#include "PiiAbstractOutputSocket.h"
#include "PiiInputController.h"

/// @internal
class PiiProxyInputSocket : public PiiAbstractInputSocket
{
  Q_OBJECT
public:
  PiiInputController* controller() const;
  
private:
  class Data : public PiiAbstractInputSocket::Data
  {
  public:
    Data();
  };
};

/// @internal
class PiiProxyOutputSocket : public PiiAbstractOutputSocket
{
  Q_OBJECT
public:

private:
  class Data : public PiiAbstractOutputSocket::Data
  {
  public:
    Data();
    virtual PiiAbstractOutputSocket* rootOutput() const;
  };
};

/**
 * An input-output socket. This socket implements both
 * PiiAbstractOutputSocket and PiiAbstractInputSocket, and works as a
 * PiiInputController for itself. It merely passes all incoming
 * objects to the output.
 *
 * @ingroup Ydin
 */
class PII_YDIN_EXPORT PiiProxySocket :
  public PiiSocket
{
  Q_OBJECT

public:
  PiiProxySocket();
  ~PiiProxySocket();

  /**
   * Returns @p Proxy.
   */
  Type type() const;
  
  PiiInputController* controller() const;
  PiiAbstractOutputSocket* rootOutput() const;
  bool setInputConnected(bool connected);

  void reset();

  PiiProxySocket* socket();
  PiiAbstractInputSocket* asInput();
  PiiAbstractOutputSocket* asOutput();

private:
  class Data : public PiiSocket::Data
  {
  public:
    Data(PiiProxySocket* owner);

    PiiAbstractOutputSocket* rootOutput() const;
    bool setInputConnected(bool connected);
    void inputConnected(PiiAbstractInputSocket* input);
    void inputDisconnected(PiiAbstractInputSocket* input);
    bool tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ();
    void inputReady(PiiAbstractInputSocket*);

    PiiProxyInputSocket input;
    PiiProxyOutputSocket output;
    
    bool *pbInputCompleted;

    PII_Q_FUNC(PiiProxySocket);
  };
  PII_D_FUNC;
};

Q_DECLARE_METATYPE(PiiProxySocket*);

#endif //_PIIPROXYSOCKET_H
