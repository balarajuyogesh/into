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

#include "PiiSocketWrapper.h"

#include "PiiScript.h"

#include <PiiOperation.h>
#include <PiiInputSocket.h>
#include <PiiOutputSocket.h>
#include <PiiProxySocket.h>
#include <PiiProbeInput.h>

#define PII_SOCKET_CONVERSION_FUNCTIONS_IMPL(CLASS, DIRECTION)          \
  static QScriptValue convert ## CLASS ## ToScriptValue(QScriptEngine* engine, CLASS* const & socket) \
  {                                                                     \
    return engine->newQObject(socket,                                   \
                              QScriptEngine::AutoOwnership,             \
                              PiiScript::defaultWrapOptions);           \
  }                                                                     \
  static void convertScriptValueTo ## CLASS(const QScriptValue& obj, CLASS*& socket) \
  {                                                                     \
    PiiSocket* pSocket = qobject_cast<PiiSocket*>(obj.toQObject());     \
    if (pSocket != 0 && pSocket->is ## DIRECTION())                     \
      socket = static_cast<CLASS*>(pSocket);                            \
    else                                                                \
      socket = 0;                                                       \
  }

// Creates conversion functions to/from socket pointers of type CLASS
#define PII_SOCKET_CONVERSION_FUNCTIONS(DIRECTION) PII_SOCKET_CONVERSION_FUNCTIONS_IMPL(PiiAbstract ## DIRECTION ## Socket, DIRECTION)

namespace PiiSocketWrapper
{
  PII_STATIC_TR_FUNC(PiiSocket);

  PII_SOCKET_CONVERSION_FUNCTIONS(Input)
  PII_SOCKET_CONVERSION_FUNCTIONS(Output)
  PII_QOBJECT_CONVERSION_FUNCTIONS(PiiProxySocket)
  PII_QOBJECT_CONVERSION_FUNCTIONS(PiiProbeInput)

  PII_SEQUENCE_CONVERSION_FUNCTIONS(PiiInputSocketList)
  PII_SEQUENCE_CONVERSION_FUNCTIONS(PiiOutputSocketList)

  // Checks that "this" is an output socket and the first argument an
  // input socket. Places the socket pointers to the output arguments.
  static QScriptValue convertSockets(QScriptContext* context,
                                     PiiAbstractOutputSocket*& output,
                                     PiiAbstractInputSocket*& input,
                                     const char* function)
  {
    PII_CHECK_THIS_TYPE(PiiSocket, convertSockets);
    if (!pThis->isOutput())
      return context->throwError(tr(PiiScript::pInstanceOfXRequired).arg(function).arg("PiiOutputSocket"));
    PiiSocket* pSocket = qscriptvalue_cast<PiiSocket*>(context->argument(0));
    if (pSocket == 0 || !pSocket->isInput())
      return context->throwError(tr(PiiScript::pArgumentNMustBeX).arg(function).arg(0).arg("PiiInputSocket"));
    output = static_cast<PiiAbstractOutputSocket*>(pThis);
    input = static_cast<PiiAbstractInputSocket*>(pSocket);
    return QScriptValue();
  }
  
  static QScriptValue connectInput(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_ONE_ARGUMENT(connectInput);
    PiiAbstractOutputSocket* pOutput = 0;
    PiiAbstractInputSocket* pInput = 0;
    QScriptValue result = convertSockets(context, pOutput, pInput, "connectInput");
    if (result.isError())
      return result;
    pOutput->connectInput(pInput);
    return engine->undefinedValue();
  }

  static QScriptValue disconnectInput(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_ONE_ARGUMENT(disconnectInput);
    PiiAbstractOutputSocket* pOutput = 0;
    PiiAbstractInputSocket* pInput = 0;
    QScriptValue result = convertSockets(context, pOutput, pInput, "disconnectInput");
    if (result.isError())
      return result;
    pOutput->disconnectInput(pInput);
    return engine->undefinedValue();
  }

  static QScriptValue connectedInputs(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_THIS_TYPE(PiiSocket, type);
    PII_CHECK_NO_ARGUMENTS(connectedInputs);
    if (!pThis->isOutput())
      return context->throwError(tr(PiiScript::pInstanceOfXRequired).arg("connectedInputs").arg("PiiOutputSocket"));
    return qScriptValueFromSequence(engine, static_cast<PiiAbstractOutputSocket*>(pThis)->connectedInputs());
  }

  static QScriptValue connectedOutput(QScriptContext* context, QScriptEngine* engine)
  {
    PII_CHECK_THIS_TYPE(PiiSocket, type);
    PII_CHECK_NO_ARGUMENTS(connectedOutput);
    if (!pThis->isInput())
      return context->throwError(tr(PiiScript::pInstanceOfXRequired).arg("connectedOutput").arg("PiiInputSocket"));
    return qScriptValueFromValue(engine, static_cast<PiiAbstractInputSocket*>(pThis)->connectedOutput());
  }
}

void initPiiSocket(QScriptEngine* engine)
{
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiSocketWrapper, PiiAbstractInputSocket);
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiSocketWrapper, PiiAbstractOutputSocket);
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiSocketWrapper, PiiProxySocket);
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiSocketWrapper, PiiProbeInput);

  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiSocketWrapper, PiiInputSocketList);
  PII_REGISTER_SCRIPT_TYPE_CONVERSION(PiiSocketWrapper, PiiOutputSocketList);
  
  QScriptValue socketObj = engine->newQMetaObject(&PiiSocket::staticMetaObject);
  engine->globalObject().setProperty("PiiSocket", socketObj);

  QScriptValue outputProto = engine->newObject();
  outputProto.setProperty("connectInput", engine->newFunction(PiiSocketWrapper::connectInput));
  outputProto.setProperty("disconnectInput", engine->newFunction(PiiSocketWrapper::disconnectInput));
  outputProto.setProperty("connectedInputs", engine->newFunction(PiiSocketWrapper::connectedInputs));
  engine->setDefaultPrototype(qMetaTypeId<PiiOutputSocket*>(), outputProto);

  QScriptValue inputProto = engine->newObject();
  inputProto.setProperty("connectedOutput", engine->newFunction(PiiSocketWrapper::connectedOutput));
  engine->setDefaultPrototype(qMetaTypeId<PiiInputSocket*>(), inputProto);

  QScriptValue probeConstructor = PiiScript::createQObjectConstructor(engine, "PiiProbeInput", "PiiSocket");
  PiiScript::configure(probeConstructor, engine->newQMetaObject(&PiiProbeInput::staticMetaObject));
  engine->globalObject().setProperty("PiiProbeInput", probeConstructor);
  engine->setDefaultPrototype(qMetaTypeId<PiiProbeInput*>(), probeConstructor.property("prototype"));
}
