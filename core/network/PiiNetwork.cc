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

#include "PiiNetwork.h"

#include <PiiGenericFunction.h>
#include <PiiFileUtil.h>
#include <PiiUtil.h>
#include <PiiIOException.h>
#include <PiiInvalidArgumentException.h>
#include "PiiHttpException.h"
#include "PiiNetworkClient.h"
#include "PiiHttpDevice.h"
#include "PiiHttpProtocol.h"

#include <QUrl>
#include <QFile>

#define PII_THROW_DEV_STATUS PII_THROW(PiiHttpException, pDev->status())

namespace PiiNetwork
{
  const char* pDeviceIsNotConnectedMessage = QT_TR_NOOP("Connection to remote object was broken.");
  const char* pErrorReadingResponseHeader = QT_TR_NOOP("Error reading server's response headers.");
  const char* pServerRepliedWithStatus = QT_TR_NOOP("Server replied with status code %1.");
  const char* pFormContentType = "application/x-www-form-urlencoded";
  const char* pTextArchiveContentType = "application/x-into-txt";
  const char* pBinaryArchiveContentType = "application/x-into-bin";

  static inline QString tr(const char* text)
  {
    return QCoreApplication::translate("PiiNetwork", text);
  }

  PiiGenericFunction* resolveFunction(const QList<QPair<QString,PiiGenericFunction*> >& functions,
                                      const QString& name,
                                      const QVariantList& params,
                                      ResolutionError* errorCode)
  {
    int iBestMatchIndex = -1;
    int iBestMatchCount = -1;
    bool bFunctionNameFound = false;
    for (int i=0; i<functions.size(); ++i)
      {
        // Function name must match
        if (functions[i].first != name)
          continue;

        bFunctionNameFound = true;
        int iMatchCount = functions[i].second->scoreOverload(params);

        // The best overload is the one with the most exact matches on
        // parameter types.
        if (iMatchCount > iBestMatchCount)
          {
            iBestMatchIndex = i;
            iBestMatchCount = iMatchCount;
          }
      }
    if (!bFunctionNameFound)
      {
        if (errorCode != 0) *errorCode = FunctionNameNotFound;
        return 0;
      }
    else if (iBestMatchIndex == -1)
      {
        if (errorCode != 0) *errorCode = OverloadNotFound;
        return 0;
      }
    return functions[iBestMatchIndex].second;
  }

  QString toJson(const QVariantMap& values)
  {
    QString strResult("{");
    for (QVariantMap::const_iterator it = values.begin(); it != values.end(); ++it)
      {
        if (it != values.begin())
          strResult += ",";
        strResult += QString(" \"%0\": %1")
          .arg(it.key())
          .arg(toJson(*it));
      }
    strResult += " }";
    return strResult;
  }

  QString toJson(const QVariantList& values)
  {
    QString strResult("[");
    for (QVariantList::const_iterator it = values.begin(); it != values.end(); ++it)
      {
        if (it != values.begin())
          strResult += ",";
        strResult += " " + toJson(*it);
      }
    strResult += " ]";
    return strResult;
  }

  QString toJson(const QVariant& value)
  {
    switch (int(value.type()))
      {
      case QVariant::Map:
        return toJson(value.toMap());
      case QVariant::List:
        return toJson(value.toList());
      }
    QString strResult = Pii::escape(value);
    if (strResult.isNull())
      return '"' + Pii::escape(value.toString()) + '"';
    return strResult;
  }

  static void readHttpResponse(PiiHttpDevice* dev, const PiiNetworkClient& client)
  {
    if (!dev->readHeader())
      PII_THROW(PiiNetworkException, tr("Error receiving server's reply from %1.").arg(client.serverAddress()));
  }

  PiiHttpDevice* openHttpConnection(PiiNetworkClient& client,
                                    const QString& method,
                                    const QString& uri,
                                    const PiiMimeHeader& requestHeaders)
  {
    QUrl url(uri);
    QString strPath = url.path();
    if (!url.isValid() || strPath.isEmpty())
      PII_THROW(PiiInvalidArgumentException, tr("%1 is not a valid file URI.").arg(uri));

    client.setServerAddress(uri.left(uri.size() - strPath.size()));
    PiiSocketDevice pDev = client.openConnection();
    if (!pDev)
      PII_THROW(PiiNetworkException, tr("Cannot open connection to %1.").arg(client.serverAddress()));

    QString strHost = url.host();
    int iPort = url.port();
    if (iPort != -1)
      strHost += QString(":%1").arg(iPort);

    PiiHttpDevice* pHttpDev = new PiiHttpDevice(pDev, PiiHttpDevice::Client);
    pHttpDev->setRequest(method, strPath);
    pHttpDev->setHeader("Host", strHost);

    QList<QPair<QString,QString> > lstHeaders(requestHeaders.values());
    for (QList<QPair<QString,QString> >::const_iterator it = lstHeaders.begin();
         it != lstHeaders.end(); ++it)
      pHttpDev->setHeader(it->first, it->second);
    return pHttpDev;
  }

  static void throwTooBig()
  {
    PII_THROW(PiiException, tr("The requested file is too big."));
  }

  QString uriToPath(const QString& path)
  {
    QString strPath(path);
    if (strPath.startsWith("file://"))
      strPath = strPath.mid(7);
#ifdef Q_OS_WIN
    // If path begins with a slash and a drive letter, remove the
    // slash.
    return strPath.contains(QRegExp("^/[a-z]:", Qt::CaseInsensitive)) ? strPath.mid(1) : strPath;
#else
    return strPath;
#endif
  }

  static void readLocalFile(const QString& name,
                            QIODevice* device,
                            qint64 maxSize,
                            PiiProgressController* controller)
  {
    QFile file(uriToPath(name));
    if (!file.open(QIODevice::ReadOnly))
      PII_THROW(PiiIOException, tr("Cannot open %1 for reading.").arg(name));
    if (maxSize != 0 && QFileInfo(name).size() > maxSize)
      throwTooBig();
    if (PiiNetwork::passData(&file, device, 0, controller) == -1)
      PII_THROW(PiiIOException, tr("Reading data from %1 failed.").arg(name));
  }

  bool isLocalFile(const QString& uri)
  {
    return uri.startsWith("file://") ||
      !uri.contains(QRegExp("^[a-zA-Z0-9+.-]+://"));
  }

  static void readFile(const QString& uri,
                       const QString& method,
                       const PiiMimeHeader& requestHeaders,
                       QIODevice* device,
                       PiiHttpResponseHeader* header,
                       int maxRedirects,
                       qint64 maxSize,
                       PiiProgressController* controller = 0)
  {
    if (isLocalFile(uri))
      {
        if (device)
          readLocalFile(uri, device, maxSize, controller);
        return;
      }

    PiiNetworkClient client;
    PiiSmartPtr<PiiHttpDevice> pDev(openHttpConnection(client, method, uri, requestHeaders));
    pDev->setController(controller);
    pDev->setMessageSizeLimit(maxSize);

    // Loop redirections to the same server
    forever
      {
        pDev->finish();
        readHttpResponse(pDev, client);

        switch (pDev->status())
          {
          case PiiHttpProtocol::OkStatus:
            if (header) *header = pDev->responseHeader();
            if (maxSize != 0 && pDev->responseHeader().contentLength() > maxSize)
              throwTooBig();
            pDev->readBody(device);
            return;
          case PiiHttpProtocol::NoContentStatus:
          case PiiHttpProtocol::NotModifiedStatus:
            if (header) *header = pDev->responseHeader();
            return;
          case PiiHttpProtocol::MovedPermanentlyStatus:
          case PiiHttpProtocol::TemporaryRedirectStatus:
            {
              QString strNewLocation = pDev->responseHeader().value("Location");
              if (strNewLocation.isEmpty())
                PII_THROW(PiiNetworkException,
                          tr("The requested resource has moved, but server specified no alternative location.")
                          .arg(uri));
              if (maxRedirects == 0)
                PII_THROW(PiiNetworkException, tr("Maximum number of redirections exceeded."));
              // According to rfc2616, location must be absolute. But most
              // (if not all) browsers support relative redirects because
              // they are used by ignorant web developers all over the
              // World.
              if (strNewLocation.startsWith("http://") || strNewLocation.startsWith("https://"))
                {
                  QUrl oldUrl(uri), newUrl(strNewLocation);
                  // If the redirection goes to the same server and the
                  // connection is allowed to be kept alive, try again
                  // through the same socket.
                  if (newUrl.scheme() == oldUrl.scheme() &&
                      newUrl.host() == oldUrl.host() &&
                      newUrl.port() == oldUrl.port() &&
                      pDev->responseHeader().value("Connection").toLower() != "close")
                    pDev->setRequestUri(newUrl.path());
                  else
                    {
                      client.closeConnection();
                      readFile(strNewLocation, method, requestHeaders, device, header, maxRedirects-1, maxSize, controller);
                      return;
                    }
                }
              else if (strNewLocation.startsWith('/')) // absolute path to the same server
                pDev->setRequestUri(strNewLocation);
              else // Relative to the current location... This is already too much.
                PII_THROW(PiiNetworkException, tr("Server provided a relative \"Location\"."));

              --maxRedirects;
              pDev->discardBody();
            } // if redirected
          default:
            PII_THROW_DEV_STATUS;
          }
      } // loop local redirections
  }

  PiiHttpResponseHeader readHeader(const QString& uri,
                                   const PiiMimeHeader& requestHeaders,
                                   int maxRedirects)
  {
    PiiHttpResponseHeader responseHeader;
    readFile(uri, "HEAD", requestHeaders, 0, &responseHeader, maxRedirects, 0);
    return responseHeader;
  }

  PiiHttpResponseHeader readHeader(const QString& uri,
                                   int maxRedirects)
  {
    PiiMimeHeader noRequestHeaders;
    PiiHttpResponseHeader responseHeader;
    readFile(uri, "HEAD", noRequestHeaders, 0, &responseHeader, maxRedirects, 0);
    return responseHeader;
  }

  QByteArray readFile(const QString& uri,
                      qint64 maxSize,
                      PiiHttpResponseHeader* header,
                      int maxRedirects)
  {
    PiiMimeHeader noRequestHeaders;
    QByteArray aBody;
    QBuffer bfr(&aBody);
    bfr.open(QIODevice::WriteOnly);
    readFile(uri, "GET", noRequestHeaders, &bfr, header, maxRedirects, maxSize);
    return aBody;
  }

  QByteArray readFile(const QString& uri,
                      const PiiMimeHeader& requestHeaders,
                      qint64 maxSize,
                      PiiHttpResponseHeader* header,
                      int maxRedirects)
  {
    QByteArray aBody;
    QBuffer bfr(&aBody);
    bfr.open(QIODevice::WriteOnly);
    readFile(uri, "GET", requestHeaders, &bfr, header, maxRedirects, maxSize);
    return aBody;
  }

  void readFile(const QString& uri,
                QIODevice* device,
                qint64 maxSize,
                PiiProgressController* controller,
                const PiiMimeHeader& requestHeaders,
                PiiHttpResponseHeader* responseHeader,
                int maxRedirects)
  {
    readFile(uri, "GET", requestHeaders, device, responseHeader, maxRedirects, maxSize, controller);
  }

  static void writeLocalFile(const QString& fileName, QIODevice* contents, PiiProgressController* controller)
  {
    QFile f(uriToPath(fileName));
    if (!f.open(QIODevice::WriteOnly))
      PII_THROW(PiiNetworkException, tr("%1 is not writable.").arg(fileName));
    if (PiiNetwork::passData(contents, &f, 0, controller) == -1)
      PII_THROW(PiiNetworkException, tr("Writing data to %1 failed.").arg(fileName));
  }

  static void putFile(const QString& uri,
                      QIODevice* contents,
                      const PiiMimeHeader& requestHeaders,
                      PiiHttpResponseHeader* header = 0,
                      PiiProgressController* controller = 0)
  {
    if (isLocalFile(uri))
      {
        writeLocalFile(uri, contents, controller);
        return;
      }

    PiiNetworkClient client;
    PiiSmartPtr<PiiHttpDevice> pDev(openHttpConnection(client, "PUT", uri, requestHeaders));
    pDev->setController(controller);
    if (contents->size())
      pDev->setHeader("Content-Length", contents->size());
    PiiNetwork::passData(contents, pDev, 0, controller);
    pDev->finish();
    readHttpResponse(pDev, client);

    switch (pDev->status())
      {
      case PiiHttpProtocol::OkStatus:
      case PiiHttpProtocol::NoContentStatus:
      case PiiHttpProtocol::CreatedStatus:
        if (header) *header = pDev->responseHeader();
        return;
      default:
        PII_THROW_DEV_STATUS;
      }
  }
  void putFile(const QString& uri,
               const QByteArray& contents,
               const PiiMimeHeader& requestHeaders,
               PiiHttpResponseHeader* header)
  {
    QBuffer bfr;
    bfr.setData(contents);
    bfr.open(QIODevice::ReadOnly);
    putFile(uri, &bfr, requestHeaders, header);
  }

  void putFile(const QString& uri,
               const QByteArray& contents)
  {
    QBuffer bfr;
    bfr.setData(contents);
    bfr.open(QIODevice::ReadOnly);
    putFile(uri, &bfr, PiiHttpResponseHeader());
  }

  void putFile(const QString& uri,
               QIODevice* contents,
               PiiProgressController* controller,
               const PiiMimeHeader& requestHeaders,
               PiiHttpResponseHeader* header)
  {
    putFile(uri, contents, requestHeaders, header, controller);
  }

  static void deleteLocalFile(const QString& path)
  {
    QFileInfo info(uriToPath(path));
    if (!info.exists())
      PII_THROW_HTTP_ERROR(NotFoundStatus);

    if (info.isDir() && !info.isSymLink())
      {
        if (!Pii::deleteDirectory(path))
          PII_THROW_HTTP_ERROR(ForbiddenStatus);
      }
    else if (!QFile(path).remove())
      PII_THROW_HTTP_ERROR(ForbiddenStatus);
  }

  void deleteFile(const QString& uri,
                  const PiiMimeHeader& requestHeaders,
                  PiiHttpResponseHeader* header)
  {
    if (isLocalFile(uri))
      {
        deleteLocalFile(uri);
        return;
      }

    PiiNetworkClient client;
    PiiSmartPtr<PiiHttpDevice> pDev(openHttpConnection(client, "DELETE", uri, requestHeaders));
    pDev->setHeader("Content-Length", 0);
    pDev->finish();
    readHttpResponse(pDev, client);

    switch (pDev->status())
      {
      case PiiHttpProtocol::OkStatus:
      case PiiHttpProtocol::NoContentStatus:
        if (header) *header = pDev->responseHeader();
        return;
      default:
        PII_THROW_DEV_STATUS;
      }
  }

  void deleteFile(const QString& uri)
  {
    deleteFile(uri, PiiMimeHeader());
  }

  static void makeLocalDirectory(const QString& path)
  {
    QDir dir(uriToPath(path));
    QString strName = dir.dirName();
    dir.cdUp();
    if (!dir.exists())
      PII_THROW_HTTP_ERROR(ConflictStatus); // WebDAV spec...
    if (!dir.mkdir(strName))
      PII_THROW_HTTP_ERROR(ForbiddenStatus);
  }

  void makeDirectory(const QString& uri,
                     const PiiMimeHeader& requestHeaders,
                     PiiHttpResponseHeader* header)
  {
    if (isLocalFile(uri))
      {
        makeLocalDirectory(uri);
        return;
      }
    PiiNetworkClient client;
    PiiSmartPtr<PiiHttpDevice> pDev(openHttpConnection(client, "MKCOL", uri, requestHeaders));
    pDev->setHeader("Content-Length", 0);
    pDev->finish();
    readHttpResponse(pDev, client);

    switch (pDev->status())
      {
      case PiiHttpProtocol::OkStatus:
      case PiiHttpProtocol::NoContentStatus:
      case PiiHttpProtocol::CreatedStatus:
        if (header) *header = pDev->responseHeader();
        return;
      default:
        PII_THROW_DEV_STATUS;
      }
  }

  void makeDirectory(const QString& uri)
  {
    makeDirectory(uri, PiiMimeHeader());
  }

  qint64 passData(QIODevice* from,
                  QIODevice* to,
                  qint64 bytes,
                  PiiProgressController* controller)
  {
    char buffer[4096];
    qint64 iTotalBytes = 0;
    if (bytes > 0)
      {
        while (bytes > 0)
          {
            qint64 iBytesRead = from->read(buffer, 4096);
            if (iBytesRead <= 0 ||
                (controller && !controller->canContinue()) ||
                (to && to->write(buffer, iBytesRead) != iBytesRead))
              return -1;
            bytes -= iBytesRead;
            iTotalBytes += iBytesRead;
          }
      }
    else
      {
        forever
          {
            qint64 iBytesRead = from->read(buffer, 4096);
            if (iBytesRead <= 0 ||
                (controller && !controller->canContinue()) ||
                (to && to->write(buffer, iBytesRead) != iBytesRead))
              return -1;
            iTotalBytes += iBytesRead;
            if (iBytesRead != 4096)
              break;
          }
      }
    return iTotalBytes;
  }
}
