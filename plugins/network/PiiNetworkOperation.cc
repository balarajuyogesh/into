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

#include "PiiNetworkOperation.h"

#include <PiiMimeHeader.h>
#include <PiiMultipartDecoder.h>
#include <PiiGenericTextInputArchive.h>

#include <QTextCodec>
#if QT_VERSION >= 0x050000
#  include <QUrlQuery>
#else
#  include <QUrl>
#endif

// PENDING body and content type outputs
const char* PiiNetworkOperation::pContentNameHeader = "Content-Name";

PiiNetworkOperation::Data::Data() :
  bIgnoreErrors(false),
  strContentType("text/plain"),
  iResponseTimeout(5000)
{
}

PiiNetworkOperation::PiiNetworkOperation(Data *d) :
  PiiDefaultOperation(d)
{
  setThreadCount(1);
  addSocket(d->pBodyInput = new PiiInputSocket("body"));
  addSocket(d->pTypeInput = new PiiInputSocket("content type"));
  d->pBodyInput->setOptional(true);
  d->pTypeInput->setOptional(true);

  d->iStaticInputCount = d->lstInputs.size();
  d->iStaticOutputCount = d->lstOutputs.size();
}

PiiNetworkOperation::~PiiNetworkOperation()
{
}

void PiiNetworkOperation::setInputNames(const QStringList& inputNames)
{
  PII_D;
  d->lstInputNames = inputNames;
  setNumberedInputs(inputNames.size(), d->iStaticInputCount);
  d->pBodyInput->setOptional(d->lstInputNames.size() > 0);
}

void PiiNetworkOperation::setOutputNames(const QStringList& outputNames)
{
  PII_D;
  d->lstOutputNames = outputNames;
  setNumberedOutputs(outputNames.size());
}

PiiInputSocket* PiiNetworkOperation::input(const QString& name) const
{
  const PII_D;
  PiiInputSocket* result = PiiBasicOperation::input(name);
  if (result == 0)
    {
      int index = d->lstInputNames.indexOf(name);
      if (index != -1)
        return inputAt(index + d->iStaticInputCount);
    }
  return result;
}

PiiOutputSocket* PiiNetworkOperation::output(const QString& name) const
{
  const PII_D;
  PiiOutputSocket* result = PiiBasicOperation::output(name);
  if (result == 0)
    {
      int index = d->lstOutputNames.indexOf(name);
      if (index != -1)
        return outputAt(index + d->iStaticOutputCount);
    }
  return result;
}


bool PiiNetworkOperation::decodeObjects(PiiHttpDevice& h, const PiiMimeHeader& header)
{
  PII_D;
  QString strContentType = header.contentType();
  //qDebug("Decoding %s", qPrintable(strContentType));
  // The server responded with/client sent one serialized object
  if (strContentType == PiiNetwork::pTextArchiveContentType)
    {
      addToOutputMap(header.value(pContentNameHeader), h);
      return true;
    }
  else if (strContentType.startsWith("multipart/"))
    {
      // Decode a multipart message
      PiiMultipartDecoder decoder(&h, header);
      while (decoder.nextMessage())
        {
          // PENDING Content-Disposition: form-data; name="name"
          if (decoder.header().contentType() == PiiNetwork::pTextArchiveContentType)
            addToOutputMap(decoder.header().value(pContentNameHeader), decoder);
          else
            decoder.readAll();
        }
      return true;
    }
  else if (strContentType == "application/x-www-form-urlencoded")
    {
#if QT_VERSION >= 0x050000
      QUrlQuery query(QString(h.readBody()));
      QList<QPair<QString,QString> > lstItems = query.queryItems();
#else
      QByteArray aBody = h.readBody();
      aBody.prepend('?');
      QUrl url(aBody);
      QList<QPair<QString,QString> > lstItems = url.queryItems();
#endif
      for (int i=0; i<lstItems.size(); ++i)
        addToOutputMap(lstItems[i].first, QVariant(lstItems[i].second));
      return true;
    }
  else if (strContentType == "text/plain")
    {
      QString strName = header.value(pContentNameHeader);
      QString strEncoding = header.value("Content-Encoding");
      QTextCodec* pCodec;
      // If encoding is not specified or the codec is not found, use
      // UTF-8 by default.
      if (strEncoding.isEmpty() ||
          (pCodec = QTextCodec::codecForName(strEncoding.toLatin1())) == 0)
        pCodec = QTextCodec::codecForName("UTF-8");
      d->mapOutputValues[strName.isEmpty() ? d->lstOutputNames[0] : strName] =
        PiiVariant(pCodec->toUnicode(h.readBody()));
      return true;
    }
  return false;
}

void PiiNetworkOperation::addToOutputMap(const QVariantMap& variables)
{
  for (QVariantMap::const_iterator i = variables.begin();
       i != variables.end(); ++i)
    addToOutputMap(i.key(), i.value());
}

void PiiNetworkOperation::addToOutputMap(const QString& name, const QVariant& value)
{
  PII_D;
  bool ok;
  // Try to convert to int first
  int iValue = value.toInt(&ok);
  if (ok)
    {
      d->mapOutputValues[name] = PiiVariant(iValue);
      return;
    }
  // No luck -> try double
  double dValue = value.toDouble(&ok);
  if (ok)
    {
      d->mapOutputValues[name] = PiiVariant(dValue);
      return;
    }
  // Damn. Well, it must be a string then.
  d->mapOutputValues[name] = PiiVariant(value.toString());
}
  
void PiiNetworkOperation::addToOutputMap(const QString& name, QIODevice& device)
{
  PII_D;
  PiiGenericTextInputArchive inputArchive(&device);
  PiiVariant obj;
  inputArchive >> obj;
  // If the name of the output is not given, we use the name of the
  // first output.
  d->mapOutputValues[name.isEmpty() ? d->lstOutputNames[0] : name] = obj;
}

void PiiNetworkOperation::emitOutputValues()
{
  PII_D;
  QList<PiiVariant> lstOutputValues;

  // Check that all outputs have been received
  for (int i=0; i<d->lstOutputNames.size(); ++i)
    {
      if (d->mapOutputValues.contains(d->lstOutputNames[i]))
        lstOutputValues << d->mapOutputValues[d->lstOutputNames[i]];
      else
        {
          d->mapOutputValues.clear();
          if (!d->bIgnoreErrors)
            PII_THROW(PiiExecutionException, tr("Objects were not received for all outputs."));
          return;
        }
    }
    
  for (int i=0; i<lstOutputValues.size(); ++i)
    outputAt(i+d->iStaticOutputCount)->emitObject(lstOutputValues[i]);
  
  d->mapOutputValues.clear();
}

void PiiNetworkOperation::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);
  
  d->bBodyConnected = d->pBodyInput->isConnected();
  d->bTypeConnected = d->pTypeInput->isConnected();
  if (!d->bBodyConnected && d->bTypeConnected)
    PII_THROW(PiiExecutionException, tr("The content type input cannot be connected alone."));
  if (d->bBodyConnected && d->lstInputNames.size() > 0)
    PII_THROW(PiiExecutionException, tr("Named inputs cannot be used with the body input."));
}


QStringList PiiNetworkOperation::inputNames() const { return _d()->lstInputNames; }
QStringList PiiNetworkOperation::outputNames() const { return _d()->lstOutputNames; }
void PiiNetworkOperation::setContentType(const QString& contentType) { _d()->strContentType = contentType; }
QString PiiNetworkOperation::contentType() const { return _d()->strContentType; }
void PiiNetworkOperation::setIgnoreErrors(bool ignoreErrors) { _d()->bIgnoreErrors = ignoreErrors; }
bool PiiNetworkOperation::ignoreErrors() const { return _d()->bIgnoreErrors; }
void PiiNetworkOperation::setResponseTimeout(int responseTimeout) { _d()->iResponseTimeout = responseTimeout; }
int PiiNetworkOperation::responseTimeout() const { return _d()->iResponseTimeout; }
