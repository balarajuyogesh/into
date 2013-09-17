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

#ifndef _PIINETWORKINPUTOPERATION_H
#define _PIINETWORKINPUTOPERATION_H

#include <QMutex>
#include <QStringList>
#include <PiiHttpProtocol.h>
#include <PiiWaitCondition.h>

#include "PiiNetworkOperation.h"

class PiiHttpServer;

/**
 * An operation that receives objects through a network connection
 * with the HTTP protol. @p PiiNetworkInputOperation registers itself
 * as a handler into a PiiHttpServer's protocol. It accepts many
 * different input formats and is able to automatically decode data in
 * various formats.
 *
 * Inputs
 * ------
 *
 * @in status - an optional input for HTTP status code (int). The
 * default status code is 200.
 *
 * ~~~
 * // This code implements a complete HTTP/1.1 server that calculates
 * // the sum of two numerical query arguments. The server supports
 * // both GET and POST queries.
 * // Start the server and browse to http://localhost:8080/sum?arg1=2&arg2=4.
 * // The server will respond with "6".
 *
 * #include <QCoreApplication>
 * #include <PiiHttpServer.h>
 * #include <PiiEngine.h>
 *
 * int main(int argc, char* argv[])
 * {
 *   QCoreApplication app(argc, argv);
 *
 *   PiiEngine engine;
 *   try
 *     {
 *       PiiEngine::loadPlugin("piinetwork");
 *       PiiEngine::loadPlugin("piibase");
 *
 *       // Create a configuration
 *       PiiOperation* net = engine.createOperation("PiiNetworkInputOperation");
 *       net->setProperty("outputNames", QStringList() << "arg1" << "arg2");
 *       net->setProperty("httpServer", "myServer");
 *       net->setObjectName("sum");
 *       PiiOperation* sum = engine.createOperation("PiiArithmeticOperation");
 *       net->connectOutput("arg1", sum, "input0");
 *       net->connectOutput("arg2", sum, "input1");
 *       sum->connectOutput("output", net, "body");
 *
 *       // Start the HTTP server
 *       PiiHttpServer* pServer = PiiHttpServer::addServer("myServer", "tcp://0.0.0.0:8080");
 *       if (!pServer->start())
 *         return 1;
 *
 *       // Start the execution engine
 *       engine.execute();
 *     }
 *   catch (PiiException& ex)
 *     {
 *       qDebug(ex.message().toUtf8().constData());
 *       return 1;
 *     }
 *   // Run eternally
 *   return app.exec();
 * }
 * ~~~
 *
 * Input decoding
 * --------------
 *
 * The operation does its best in guessing the type of the input data,
 * if the type is not explicitly specified. The operation supports
 * HTML form submissions (GET and POST with
 * application/x-www-form-urlencoded and multipart/form-data
 * encodings). If the string representing the value of a submitted
 * form field can be converted to an `int`, it will be converted. 
 * `double` will be tried next, and if that is not successful, the value
 * will be used as a string.
 *
 */
class PiiNetworkInputOperation : public PiiNetworkOperation,
                                 public PiiHttpProtocol::UriHandler
{
  Q_OBJECT
  Q_INTERFACES(PiiHttpProtocol::UriHandler)

  /**
   * The name of the HTTP server you want to handle incoming requests
   * to this operation. The server must be previously created with
   * PiiHttpServer::addServer(). If you want to create a server for
   * this operation only, use a URI here. See
   * PiiHttpServer::addServer() for URI syntax.
   *
   * ~~~
   * PiiOperation* pReceiver = engine.createOperation("PiiNetworkInputOperation");
   * pReceiver->setProperty("httpServer", "tcp://0.0.0.0:8080/");
   * ~~~
   */
  Q_PROPERTY(QString httpServer READ httpServer WRITE setHttpServer);

  /**
   * The URI of the operation within the server. Note that each
   * operation must have a unique URI in the context of a HTTP server. 
   * If `uri` is empty, @p /objectName will be used as the URI.
   */
  Q_PROPERTY(QString uri READ uri WRITE setUri);

  /**
   * The message sent to the HTTP client if the operation is
   * interrupted while processing a request. The default is "The
   * operation was interrupted."
   */
  Q_PROPERTY(QString interruptedResponse READ interruptedResponse WRITE setInterruptedResponse);

  /**
   * The message sent to the HTTP client if the operation fails to
   * receive a response object within time limits. The default is
   * "Timed out while waiting for response."
   *
   * @see PiiNetworkOperation::responseTimeout
   */
  Q_PROPERTY(QString timeoutResponse READ timeoutResponse WRITE setTimeoutResponse);

  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  PiiNetworkInputOperation();
  ~PiiNetworkInputOperation();

  void stop();
  void pause();

  void check(bool reset);

  void handleRequest(const QString& uri, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller);

  void setHttpServer(const QString& httpServer);
  QString httpServer() const;
  void setUri(const QString& uri);
  QString uri() const;
  void setInterruptedResponse(const QString& interruptedResponse);
  QString interruptedResponse() const;
  void setTimeoutResponse(const QString& timeoutResponse);
  QString timeoutResponse() const;

protected:
  void process();
  
private:
  void replyToClient(PiiHttpDevice* h);
  void destroyServer();
  
  /// @internal
  class Data : public PiiNetworkOperation::Data
  {
  public:
    Data();
    
    PiiHttpServer* pServer;
    bool bOwnServer;
    bool bNeedToWaitResponse;
    PiiWaitCondition responseCondition;
    QMutex requestLock;
    
    PiiInputSocket* pStatusInput;
    bool bStatusConnected;
    int iStatusCode;
    
    QList<PiiVariant> lstResponseValues;
    
    QString strCurrentContentType;
    QString strResponseData;
    
    QString strHttpServer;
    QString strUri;
    QString strInterruptedResponse;
    QString strTimeoutResponse;
  };
  PII_D_FUNC;
};


#endif //_PIINETWORKINPUTOPERATION_H
