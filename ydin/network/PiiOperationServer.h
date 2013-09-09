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

#ifndef _PIIOPERATIONSERVER_H
#define _PIIOPERATIONSERVER_H

#include <PiiQObjectServer.h>
#include <PiiYdin.h>
#include <PiiOperation.h>
#include <PiiOutputSocket.h>

#include <QHash>

PII_MAP_METATYPE(PiiOperation::State, int);

/**
 * PiiOperationServer provides a network-transparent interface to
 * PiiOperation with the aid of PiiQObjectServer. It adds the outputs
 * of an operation as pushable sources to the server and maps its
 * inputs to the URI space as well.
 *
 * PiiOperationServer adds "/inputs/" and "/outputs/" to the root of
 * the server. A request to these URIs returns a list of input and
 * output names, respectively.
 *
 * @ingroup Ydin
 */
class PII_YDIN_EXPORT PiiOperationServer : public PiiQObjectServer
{
public:
  PiiOperationServer(PiiOperation* operation);
  
  void handleRequest(const QString& uri, PiiHttpDevice* dev,
                     PiiHttpProtocol::TimeLimiter* controller);
protected:
  QStringList listRoot() const;

  void connectToChannel(Channel* channel, const QString& sourceId);
  void disconnectFromChannel(Channel* channel, const QString& sourceId);

  /// @internal
  ChannelImpl* createChannel() const;

private:
  class Data : public PiiQObjectServer::Data
  {
  public:
    Data(PiiOperation* operation);
    ~Data();

    QHash<QString,PiiOutputSocket*> hashConnectedInputs;
  };
  PII_D_FUNC;

  class ChannelImpl :
    public PiiQObjectServer::ChannelImpl,
    public PiiInputController
  {
  public:
    ~ChannelImpl();
    
    PiiAbstractInputSocket* createInput(const QString& outputName);
    void destroyInput(const QString& outputName);
    
    bool tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ();

  private:
    QHash<QString,PiiAbstractInputSocket*> _hashInputs;
  };

  inline PiiOperation* operation() const { return static_cast<PiiOperation*>(_d()->pObject); }
  PiiAbstractOutputSocket* findOutput(const QString& name) const;
  void connectInput(const QString& inputName);
  void sendToInput(const QString& inputName, PiiHttpDevice* dev,
                   PiiHttpProtocol::TimeLimiter* controller);
};

#endif //_PIIOPERATIONSERVER_H
