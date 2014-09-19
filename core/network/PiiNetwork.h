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

#ifndef _PIINETWORK_H
#define _PIINETWORK_H

#include <PiiPreprocessor.h>
#include <QVariantList>
#include <QString>
#include "PiiNetworkGlobal.h"
#include "PiiMimeHeader.h"

class PiiHttpResponseHeader;
class PiiProgressController;
class PiiHttpDevice;
class PiiNetworkClient;

/// @hide
#define PII_THROW_IF_NOT_CONNECTED if (!pDev->isReadable()) PII_THROW(PiiNetworkException, tr(PiiNetwork::pDeviceIsNotConnectedMessage))
#define PII_CHECK_SERVER_RESPONSE                                       \
  do {                                                                  \
    PII_THROW_IF_NOT_CONNECTED;                                         \
    if (!pDev->readHeader())                                            \
      PII_THROW(PiiNetworkException, tr(PiiNetwork::pErrorReadingResponseHeader)); \
    if (pDev->status() != PiiHttpProtocol::OkStatus)                    \
      PII_THROW(PiiNetworkException, tr(PiiNetwork::pServerRepliedWithStatus).arg(pDev->status())); \
  } while(false)

#define PII_THROW_HTTP_ERROR(CODE) PII_THROW(PiiHttpException, PiiHttpProtocol::CODE)
#define PII_THROW_HTTP_ERROR_MSG(CODE, MSG) throw PiiHttpException(PiiHttpProtocol::CODE, MSG, QString(__FILE__ ":%1").arg(__LINE__))
#define PII_REQUIRE_HTTP_METHOD(METHOD) do { if (dev->requestMethod() != METHOD) PII_THROW_HTTP_ERROR(MethodNotAllowedStatus); } while (false)

#define PII_REMOTE_CALL_TPL_PARAM(N,PARAM) class PARAM
#define PII_REMOTE_CALL_FUNC_PARAM(N,PARAM) , PARAM _##PARAM
#define PII_REMOTE_CALL_LIST_PARAM(N,PARAM) << QVariant::fromValue(typename Pii::ToPlainMetaType<PARAM >::Type(_##PARAM))

#define PII_CREATE_REMOTE_CALL_IMPL(NAME, N, PARAMS)                    \
  template <class R, PII_FOR_N_SEP(PII_REMOTE_CALL_TPL_PARAM, PII_COMMA_SEP, N, PARAMS) > \
  R NAME(const QString& function                                        \
         PII_FOR_N(PII_REMOTE_CALL_FUNC_PARAM, N, PARAMS))              \
  {                                                                     \
    QVariantList lstParams;                                             \
    lstParams PII_FOR_N(PII_REMOTE_CALL_LIST_PARAM, N, PARAMS);         \
    return PiiNetwork::returnValue<R>(NAME##List(function, lstParams)); \
  }
#define PII_CREATE_REMOTE_CALL(N, PARAMS) PII_CREATE_REMOTE_CALL_IMPL(call, N, PARAMS)
#define PII_CREATE_REMOTE_CALLBACK(N, PARAMS) PII_CREATE_REMOTE_CALL_IMPL(callBack, N, PARAMS)
/// @endhide
class PiiGenericFunction;

/**
 * Definitions and functions used commonly by networking classes.
 *
 */
namespace PiiNetwork
{
  /// @internal
  template <class T> inline T returnValue(const QVariant& ret) { return ret.value<T>(); }
  /// @internal
  template <> inline void returnValue<void>(const QVariant&) {}

  /**
   * Encoding formats for data passed over network.
   *
   * - `TextFormat` - data is encoded as UTF-8 text. See
   * PiiTextOutputArchive and PiiTextInputArchive.
   *
   * - `BinaryFormat` - data is encoded in a raw binary format. See
   * PiiBinaryOutputArchive and PiiBinaryInputArchive.
   */
  enum EncodingFormat { TextFormat, BinaryFormat };

  enum StopMode { InterruptClients, WaitClients };
  enum { RemoteExceptionStatus = 666 };

#if QT_VERSION < 0x050000
  typedef int SocketDescriptorType;
#else
  typedef qintptr SocketDescriptorType;
#endif

  PII_NETWORK_EXPORT QString toJson(const QVariantMap& values);
  PII_NETWORK_EXPORT QString toJson(const QVariantList& values);
  PII_NETWORK_EXPORT QString toJson(const QVariant& values);

  /**
   * Returns the contents of the file at the given *uri*.
   *
   * @param uri the location of a file. A valid uri consists of a
   * scheme, host name, and part. It may optionally contain a port
   * number. This function uses PiiNetworkClient and thus supports all
   * schemes supported by it. Additionally, `file:`// is recognized
   * as a scheme that refers to local files.
   *
   * @param maxSize the maximum number of bytes to read. If the file
   * is bigger, an exception will be thrown. Zero means no limit, but
   * it is a good idea to always specify a non-zero maximum size to
   * avoid running out of memory.
   *
   * @param responseHeader if this parameter is non-zero, the server's
   * response headers will be returned in it.
   *
   * @param maxRedirects the maximum number of redirections to accept.
   * -1 means any number.
   *
   * @exception PiiInvalidArgumentException& if *uri* is incorrectly
   * formatted.
   *
   * @exception PiiNetworkException& if the file cannot be retrieved.
   *
   * @exception PiiIOException& if *uri* refers to a local file that
   * cannot be read.
   *
   * @exception PiiException& if the file is too big
   */
  PII_NETWORK_EXPORT QByteArray readFile(const QString& uri,
                                         qint64 maxSize = 0,
                                         PiiHttpResponseHeader* responseHeader = 0,
                                         int maxRedirects = 1);

  /**
   * This version allows one to add custom values to the HTTP request
   * header.
   *
   * ~~~(c++)
   * PiiHttpResponseHeader header;
   * // Read a remote file and store response header.
   * QByteArray aReply = PiiNetwork::readFile("http://intopii.com/into/", &header);
   *
   * // Reread the file, but only if it has been modified since the last request.
   * PiiMimeHeader requestHeader;
   * requestHeader.setValue("If-Modified-Since", header.value("Last-Modified"));
   * aReply = PiiNetwork::readFile("http://intopii.com/into/", requestHeader);
   * ~~~
   */
  PII_NETWORK_EXPORT QByteArray readFile(const QString& uri,
                                         const PiiMimeHeader& requestHeader,
                                         qint64 maxSize = 0,
                                         PiiHttpResponseHeader* responseHeader = 0,
                                         int maxRedirects = 1);

  /**
   * Reads the file at *uri* and writes it directly to *device*.
   */
  PII_NETWORK_EXPORT void readFile(const QString& uri,
                                   QIODevice* device,
                                   qint64 maxSize = 0,
                                   PiiProgressController* controller = 0,
                                   const PiiMimeHeader& requestHeaders = PiiMimeHeader(),
                                   PiiHttpResponseHeader* responseHeader = 0,
                                   int maxRedirects = 1);

  /**
   * Makes a HTTP HEAD request to *uri* and returns the result.
   *
   * @exception PiiInvalidArgumentException& if *uri* is incorrectly
   * formatted.
   *
   * @exception PiiNetworkException& if the header cannot be retrieved.
   *
   * @see readFile()
   */
  PII_NETWORK_EXPORT PiiHttpResponseHeader readHeader(const QString& uri,
                                                      int maxRedirects = 1);
  /**
   * This version makes it possible to provide custom header fields to
   * the request.
   */
  PII_NETWORK_EXPORT PiiHttpResponseHeader readHeader(const QString& uri,
                                                      const PiiMimeHeader& requestHeader,
                                                      int maxRedirects = 1);

  /**
   * Uses the HTTP PUT method to write a file to the given *uri*.
   */
  PII_NETWORK_EXPORT void putFile(const QString& uri,
                                  const QByteArray& contents,
                                  const PiiMimeHeader& requestHeaders,
                                  PiiHttpResponseHeader* header = 0);
  PII_NETWORK_EXPORT void putFile(const QString& uri,
                                  const QByteArray& contents);
  PII_NETWORK_EXPORT void putFile(const QString& uri,
                                  QIODevice* contents,
                                  PiiProgressController* controller,
                                  const PiiMimeHeader& requestHeaders,
                                  PiiHttpResponseHeader* header);

  PII_NETWORK_EXPORT void deleteFile(const QString& uri,
                                     const PiiMimeHeader& requestHeaders,
                                     PiiHttpResponseHeader* header = 0);
  PII_NETWORK_EXPORT void deleteFile(const QString& uri);

  PII_NETWORK_EXPORT void makeDirectory(const QString& uri,
                                        const PiiMimeHeader& requestHeaders,
                                        PiiHttpResponseHeader* header = 0);
  PII_NETWORK_EXPORT void makeDirectory(const QString& uri);

  /// @hide
  extern PII_NETWORK_EXPORT const char* pFormContentType;
  extern PII_NETWORK_EXPORT const char* pDeviceIsNotConnectedMessage;
  extern PII_NETWORK_EXPORT const char* pErrorReadingResponseHeader;
  extern PII_NETWORK_EXPORT const char* pServerRepliedWithStatus;
  extern PII_NETWORK_EXPORT const char* pTextArchiveContentType;
  extern PII_NETWORK_EXPORT const char* pBinaryArchiveContentType;

  enum ResolutionError { NoResolutionError, FunctionNameNotFound, OverloadNotFound };
  PiiGenericFunction* resolveFunction(const QList<QPair<QString,PiiGenericFunction*> >& functions,
                                      const QString& name,
                                      const QVariantList& params,
                                      ResolutionError* errorCode = 0);

  PII_NETWORK_EXPORT qint64 passData(QIODevice* from, QIODevice* to, qint64 bytes = 0, PiiProgressController* controller = 0);
  PII_NETWORK_EXPORT PiiHttpDevice* openHttpConnection(PiiNetworkClient& client,
                                                       const QString& method,
                                                       const QString& uri,
                                                       const PiiMimeHeader& requestHeaders = PiiMimeHeader());
  /// @endhide
}

#endif //_PIINETWORK_H
