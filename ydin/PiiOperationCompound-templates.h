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

#ifndef _PIIOPERATIONCOMPOUND_H
# error "Never use <PiiOperationCompound-templates.h> directly; include <PiiOperationCompound.h> instead."
#endif

template <class Archive> void PiiOperationCompound::save(Archive& archive, const unsigned int /*version*/)
{
  PII_D;
  PII_SERIALIZE_BASE(archive, PiiOperation);

  // Save child operations
  PII_SERIALIZE_NAMED(archive, d->lstOperations, "operations");

  // Save all exposed inputs
  int inputCnt = d->lstInputs.size();
  PII_SERIALIZE(archive, inputCnt);

  for (int i=0; i<inputCnt; ++i)
    {
      PiiAbstractInputSocket* pSocket = d->lstInputs[i];
      // For each input, its name is first stored
      QString name = pSocket->objectName();
      PII_SERIALIZE(archive, name);

      // Is it a proxy or not?
      bool proxy = pSocket->isProxy();
      PII_SERIALIZE(archive, proxy);
    }

  // Save all exposed outputs
  int outputCnt = d->lstOutputs.size();
  PII_SERIALIZE(archive, outputCnt);

  for (int i=0; i<outputCnt; ++i)
    {
      PiiAbstractOutputSocket* pSocket = d->lstOutputs[i];
      // For each output, its name is first stored
      QString name = pSocket->objectName();
      PII_SERIALIZE(archive, name);

      // Is it a proxy?
      bool proxy = pSocket->isProxy();
      PII_SERIALIZE(archive, proxy);
    }

  // Store aliased inputs
  for (int i=0; i<inputCnt; ++i)
    {
      PiiAbstractInputSocket* pSocket = d->lstInputs[i];
      if (!pSocket->isProxy())
        {
          EndPointType aliased = locateSocket(pSocket);
          PII_SERIALIZE(archive, aliased);
        }
    }

  // Store aliased outputs
  for (int i=0; i<outputCnt; ++i)
    {
      PiiAbstractOutputSocket* pSocket = d->lstOutputs[i];
      if (!pSocket->isProxy())
        {
          EndPointType aliased = locateSocket(pSocket);
          PII_SERIALIZE(archive, aliased);
        }
    }

  // At this point, every possible connection point has been stored
  // and can be retrieved with a name.

  // Store connections between operations
  for (int i=0; i<d->lstOperations.size(); i++)
    {
      // Find all output sockets
      QList<PiiAbstractOutputSocket*> outputs = d->lstOperations[i]->outputs();

      // Store the number of outputs
      int cnt = outputs.size();
      PII_SERIALIZE(archive, cnt);

      // Store all connections starting from this operation
      for (int j=0; j<outputs.size(); j++)
        {
          // Store the name of the start point
          QString name = outputs[j]->objectName();
          PII_SERIALIZE(archive, name);

          // Store all end points
          EndPointListType inputs = buildEndPointList(outputs[j]);
          PII_SERIALIZE(archive, inputs);
        }
    }

  // Store connections from proxy inputs
  for (int i=0; i<inputCnt; ++i)
    {
      PiiAbstractInputSocket* pSocket = d->lstInputs[i];
      if (pSocket->isProxy())
        {
          EndPointListType inputs(buildEndPointList(PiiProxySocket::output(pSocket)));
          PII_SERIALIZE(archive, inputs);
        }
    }

  // Store connections from proxy outputs. It is in principle possible
  // (albeit stupid) to connect an output proxy inside the compound.
  for (int i=0; i<outputCnt; ++i)
    {
      PiiAbstractOutputSocket* pSocket = d->lstOutputs[i];
      if (pSocket->isProxy())
        {
          EndPointListType inputs(buildEndPointList(pSocket));
          PII_SERIALIZE(archive, inputs);
        }
    }

  // Save properties if this is the most derived class
  if (PiiOperationCompound::metaObject() == metaObject())
    PiiSerialization::saveProperties(archive, *this);
}

template <class Archive> void PiiOperationCompound::load(Archive& archive, const unsigned int version)
{
  PII_D;
  PII_SERIALIZE_BASE(archive, PiiOperation);

  clear();

  // Restore operations
  QList<PiiOperation*> operations;
  PII_SERIALIZE_NAMED(archive, operations, "operations");
  for (int i=0; i<operations.size(); i++)
    addOperation(operations[i]);

  // Load all exposed input sockets
  int inputCnt;
  PII_SERIALIZE(archive, inputCnt);

  for (int i=0; i<inputCnt; ++i)
    {
      QString name;
      PII_SERIALIZE(archive, name);
      bool proxy;
      PII_SERIALIZE(archive, proxy);

      if (proxy)
        createInputProxy(name);
      else
        d->lstInputs << 0;
    }

  // Load all exposed output sockets
  int outputCnt;
  PII_SERIALIZE(archive, outputCnt);

  for (int i=0; i<outputCnt; ++i)
    {
      QString name;
      PII_SERIALIZE(archive, name);
      bool proxy;
      PII_SERIALIZE(archive, proxy);

      if (proxy)
        createOutputProxy(name);
      else
        d->lstOutputs << 0;
    }

  // Load aliased inputs
  for (int i=0; i<inputCnt; ++i)
    {
      PiiAbstractInputSocket* pSocket = d->lstInputs[i];
      // Proxies have the socket at this point
      if (pSocket == 0)
        {
          EndPointType aliased;
          PII_SERIALIZE(archive, aliased);
          d->lstInputs[i] = aliased.first->input(aliased.second);
        }
    }

  // Load aliased outputs
  for (int i=0; i<outputCnt; ++i)
    {
      PiiAbstractOutputSocket* pSocket = d->lstOutputs[i];
      if (pSocket == 0)
        {
          EndPointType aliased;
          PII_SERIALIZE(archive, aliased);
          d->lstOutputs[i] = aliased.first->output(aliased.second);
        }
    }

  // Restore connections for each operation
  for (int i=0; i<d->lstOperations.size(); i++)
    {
      // Read the number of stored outputs
      int cnt;
      PII_SERIALIZE(archive, cnt);

      // Read connection specs
      while (cnt-- > 0)
        {
          // Read starting point name
          QString name;
          PII_SERIALIZE(archive, name);

          // Read end points
          EndPointListType inputs;
          PII_SERIALIZE(archive, inputs);

          connectAll(d->lstOperations[i]->output(name), inputs);
        }
    }

  // Load connections from proxy inputs
  for (int i=0; i<inputCnt; ++i)
    {
      PiiAbstractInputSocket* pSocket = d->lstInputs[i];
      if (pSocket->isProxy())
        {
          EndPointListType inputs;
          PII_SERIALIZE(archive, inputs);
          connectAll(PiiProxySocket::output(pSocket), inputs);
        }
    }

  // Load connections from proxy outputs
  for (int i=0; i<outputCnt; ++i)
    {
      PiiAbstractOutputSocket* pSocket = d->lstOutputs[i];
      if (pSocket->isProxy())
        {
          EndPointListType inputs;
          PII_SERIALIZE(archive, inputs);
          connectAll(pSocket, inputs);
        }
    }

  // Load properties if this is not the most derived class
  if (PiiOperationCompound::metaObject() == metaObject() && version > 0)
    PiiSerialization::loadProperties(archive, *this);
}
