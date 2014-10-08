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

#include "PiiOperationServer.h"

#include "PiiHttpDevice.h"
#include "PiiHttpException.h"
#include "PiiStreamBuffer.h"

#include <PiiSerializationUtil.h>
#include <PiiGenericTextInputArchive.h>
#include <PiiGenericTextOutputArchive.h>

PiiOperationServer::Data::Data(PiiOperation* operation) :
  PiiQObjectServer::Data(operation,
                         PiiQObjectServer::ExposeSignals |
                         PiiQObjectServer::ExposeSlots |
                         PiiQObjectServer::ExposeProperties |
                         PiiQObjectServer::ExposeDynamicProperties)
{}

PiiOperationServer::Data::~Data()
{
  qDeleteAll(hashConnectedInputs);
}

PiiOperationServer::PiiOperationServer(PiiOperation* operation) :
  PiiQObjectServer(new Data(operation))
{
  addFunction("check", operation, &PiiOperation::check);
  addFunction("start", operation, &PiiOperation::start);
  addFunction("pause", operation, &PiiOperation::pause);
  addFunction("stop", operation, &PiiOperation::stop);
  addFunction("interrupt", operation, &PiiOperation::interrupt);
  addFunction("state", operation, &PiiOperation::state);
  addFunction("wait", operation, &PiiOperation::wait);
  addFunction("startPropertySet", operation, &PiiOperation::startPropertySet);
  addFunction("endPropertySet", operation, &PiiOperation::endPropertySet);
  addFunction("removePropertySet", operation, &PiiOperation::removePropertySet);
  addFunction("reconfigure", operation, &PiiOperation::reconfigure);

  addFunction("connectInput", this, &PiiOperationServer::connectInput);
}

QStringList PiiOperationServer::listRoot() const
{
  QStringList lstFolders = PiiQObjectServer::listRoot();
  lstFolders << "inputs/" << "outputs/";
  return lstFolders;
}

PiiAbstractOutputSocket* PiiOperationServer::findOutput(const QString& name) const
{
  PiiAbstractOutputSocket* pOutput = operation()->output(name);
  if (pOutput == 0)
    PII_THROW_HTTP_ERROR_MSG(BadRequestStatus, tr("There is no output called \"%1\".").arg(name));
  return pOutput;
}

void PiiOperationServer::connectToChannel(Channel* channel, const QString& sourceId)
{
  if (sourceId.startsWith("outputs/"))
    {
      QString strOutputName = sourceId.mid(8);
      // Create a new input socket for each connected output.
      findOutput(strOutputName)->connectInput(static_cast<ChannelImpl*>(channel)->createInput(strOutputName));
    }
  else
    PiiQObjectServer::connectToChannel(channel, sourceId);
}

void PiiOperationServer::disconnectFromChannel(Channel* channel, const QString& sourceId)
{
  if (sourceId.startsWith("outputs/"))
    {
      QString strOutputName = sourceId.mid(8);
      static_cast<ChannelImpl*>(channel)->destroyInput(strOutputName);
    }
  else
    PiiQObjectServer::connectToChannel(channel, sourceId);
}

void PiiOperationServer::sendToInput(const QString& inputName, PiiHttpDevice* dev,
                                     PiiHttpProtocol::TimeLimiter*)
{
  PII_REQUIRE_HTTP_METHOD("POST");
  PiiOutputSocket* pOutput = _d()->hashConnectedInputs[inputName];
  if (pOutput == 0)
    PII_THROW_HTTP_ERROR(NotFoundStatus);
  try
    {
      PiiVariant varObject;
      PiiSerialization::fromByteArray<PiiGenericTextInputArchive>(dev->readBody(), varObject);
      pOutput->emitObject(varObject);
    }
  catch (PiiSerializationException& ex)
    {
      PII_THROW_HTTP_ERROR_MSG(BadRequestStatus, ex.message() + " (" + ex.info() + ")");
    }
}

void PiiOperationServer::connectInput(const QString& inputName)
{
  PII_D;
  PiiAbstractInputSocket* pInput = operation()->input(inputName);
  if (pInput == 0)
    PII_THROW_HTTP_ERROR(NotFoundStatus);
  if (!d->hashConnectedInputs.contains(inputName))
    {
      PiiOutputSocket* pOutput = new PiiOutputSocket("");
      pOutput->connectInput(pInput);
      d->hashConnectedInputs.insert(inputName, pOutput);
    }
}

void PiiOperationServer::handleRequest(const QString& uri, PiiHttpDevice* dev,
                                       PiiHttpProtocol::TimeLimiter* controller)
{
  QString strRequestPath = dev->requestPath(uri);

  if (strRequestPath.startsWith("inputs/"))
    {
      dev->startOutputFiltering(new PiiStreamBuffer);
      if (strRequestPath.size() == 7)
        {
          PII_REQUIRE_HTTP_METHOD("GET");
          dev->print(operation()->inputNames().join("\n"));
        }
      else
        sendToInput(strRequestPath.mid(7), dev, controller); // may throw
    }
  else if (strRequestPath == "outputs/")
    {
      dev->startOutputFiltering(new PiiStreamBuffer);
      dev->print(operation()->outputNames().join("\n"));
    }
  else
    PiiQObjectServer::handleRequest(uri, dev, controller);
}

PiiOperationServer::ChannelImpl::ChannelImpl(const QString& clientId) :
  PiiQObjectServer::ChannelImpl(clientId)
{}

PiiOperationServer::ChannelImpl::~ChannelImpl()
{
  qDeleteAll(_hashInputs);
}

PiiAbstractInputSocket* PiiOperationServer::ChannelImpl::createInput(const QString& outputName)
{
  PiiInputSocket* pInput = new PiiInputSocket(outputName);
  pInput->setController(this);
  _hashInputs.insert(outputName, pInput);
  return pInput;
}

void PiiOperationServer::ChannelImpl::destroyInput(const QString& outputName)
{
  delete _hashInputs.take(outputName);
}

bool PiiOperationServer::ChannelImpl::tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ()
{
  try
    {
      QByteArray aData(PiiSerialization::toByteArray<PiiGenericTextOutputArchive>(object));
      return enqueuePushData("outputs/" + sender->objectName(), aData);
    }
  catch (PiiSerializationException& ex)
    {
      piiCritical(ex.message());
    }
  return false;
}

PiiQObjectServer::ChannelImpl* PiiOperationServer::createChannel(const QString& clientId) const
{
  return new ChannelImpl(clientId);
}
