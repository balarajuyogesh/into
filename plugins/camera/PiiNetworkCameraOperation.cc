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

#include "PiiNetworkCameraOperation.h"
#include <QUrl>
#include <QFile>
#include <cstdio>
#include <PiiHttpDevice.h>
#include <PiiSocketDevice.h>

PiiNetworkCameraOperation::Data::Data() :
  iPort(0),
  dMaxIoDelay(1.0),
  bStreamMode(false),
  bIgnoreErrors(false)
{
}

PiiNetworkCameraOperation::PiiNetworkCameraOperation() :
  PiiImageReaderOperation(new Data)
{
  PII_D;
  setMaxIoDelay(3.0);
  addSocket(d->pUrlInput = new PiiInputSocket("url"));
  d->pUrlInput->setOptional(true);
}

PiiNetworkCameraOperation::~PiiNetworkCameraOperation()
{
}

void PiiNetworkCameraOperation::setMaxIoDelay(double delay)
{
  PII_D;
  d->dMaxIoDelay = delay;
}

void PiiNetworkCameraOperation::checkUrl(const QUrl& url)
{
  if (url.isEmpty())
    PII_THROW(PiiExecutionException, tr("Image URL is empty."));
  if (!url.isValid())
    PII_THROW(PiiExecutionException, tr("Image URL (%1) is not valid.").arg(url.toString()));
  if (url.scheme().toLower() != "http")
    PII_THROW(PiiExecutionException, tr("Unsupported protocol \"%1\" in image URL.").arg(url.scheme()));
}

void PiiNetworkCameraOperation::check(bool reset)
{
  PII_D;
  PiiImageReaderOperation::check(reset);
  
  QUrl proxy(d->strProxyUrl);
  if (!proxy.isEmpty())
    {
      if (!proxy.isValid())
        PII_THROW(PiiExecutionException, tr("The given proxy URL (%1) is not valid.").arg(d->strProxyUrl));
      if (proxy.scheme().toLower() != "http")
        PII_THROW(PiiExecutionException, tr("Unsupported protocol \"%1\" in proxy URL.").arg(proxy.scheme()));
    }

  if (!d->pUrlInput->isConnected())
    {
      QUrl image(d->strImageUrl);
      checkUrl(image);
      
      d->strHost = image.host();
      d->iPort = image.port(80);
      d->strPath = image.path();
    }

  if (reset)
    d->strPreviousUrl = "";
}

void PiiNetworkCameraOperation::process()
{
  PII_D;
  QString strHost = d->strHost, strPath = d->strPath;
  qint16 iPort = d->iPort;
  QString strUrl = d->strImageUrl;
  
  if (d->pUrlInput->isConnected())
    {
      PiiVariant urlObj = d->pUrlInput->firstObject();
      if (urlObj.type() != PiiYdin::QStringType)
        PII_THROW_UNKNOWN_TYPE(d->pUrlInput);
      strUrl = urlObj.valueAs<QString>();
      QUrl imageUrl(strUrl);
      checkUrl(imageUrl);

      strHost = imageUrl.host();
      iPort = imageUrl.port(80);
      strPath = imageUrl.path();
    }

  // If URL has changed, we need to reconnect
  if (strUrl != d->strPreviousUrl)
    {
      disconnectSocket();
      d->strPreviousUrl = strUrl;
      d->networkClient.setServerAddress(QString("tcp://%1:%2").arg(strHost).arg(iPort));
    }

  PiiSocketDevice socket = d->networkClient.openConnection();
  if (socket == 0)
    {
      if (!d->bIgnoreErrors)
        PII_THROW(PiiExecutionException,
                  tr("Cannot open connection to: %1").arg(d->networkClient.serverAddress()));
      return;
    }
  
  PiiHttpDevice dev(socket, PiiHttpDevice::Client);
  dev.setRequest("GET", strPath);
  dev.setHeader("Host", strHost);
  dev.finish();
  
  // Read response header
  dev.readHeader();
  PiiHttpResponseHeader header = dev.responseHeader();
  QByteArray imageFormat("");
  if (header.statusCode() != 200) // 200 OK
    {
      disconnectSocket();
      if (!d->bIgnoreErrors)
        PII_THROW(PiiExecutionException, tr("HTTP error: %1").arg(header.reasonPhrase()));
      return;
    }
  
  if (header.hasKey("Content-Type") &&
      header.contentType().startsWith("image/") &&
      header.contentType().size() > 6)
    imageFormat = header.contentType().mid(6).toLatin1();
  
  bool bClose = header.value("Connection").toLower() == "close";

  // Decode image
  QImageReader imageReader(&dev, imageFormat);
  QImage image = imageReader.read();
  
  // Reset (and disconnect) socket
  if (bClose) //&& !d->bStreamMode)
    disconnectSocket();

  // Check image for errors
  if (image.isNull() || image.width() == 0 || image.height() == 0)
    {
      piiWarning(tr("Image decoding error: %1").arg(imageReader.errorString()));
      disconnectSocket();
      if (!d->bIgnoreErrors)
        PII_THROW(PiiExecutionException, tr("Could not decode image at %1. Error message: %2")
                  .arg(strUrl).arg(imageReader.errorString()));
    }
  // All is fine
  else
    {
      if (d->imageType == GrayScale)
        emitGrayImage(image);
      else
        emitColorImage(image);
    }
}

void PiiNetworkCameraOperation::disconnectSocket()
{
  PII_D;
  d->networkClient.closeConnection();
}

QString PiiNetworkCameraOperation::imageUrl() const { return _d()->strImageUrl; }
void PiiNetworkCameraOperation::setImageUrl(const QString& url) { _d()->strImageUrl = url; }
QString PiiNetworkCameraOperation::proxyUrl() const { return _d()->strProxyUrl; }
void PiiNetworkCameraOperation::setProxyUrl(const QString& url) { _d()->strProxyUrl = url; }
bool PiiNetworkCameraOperation::streamMode() const { return _d()->bStreamMode; }
void PiiNetworkCameraOperation::setStreamMode(bool streamMode) { _d()->bStreamMode = streamMode; }
bool PiiNetworkCameraOperation::ignoreErrors() const { return _d()->bIgnoreErrors; }
void PiiNetworkCameraOperation::setIgnoreErrors(bool ignoreErrors) { _d()->bIgnoreErrors = ignoreErrors; }
double PiiNetworkCameraOperation::maxIoDelay() const { return _d()->dMaxIoDelay; }
