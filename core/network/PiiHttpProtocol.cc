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

#include "PiiHttpProtocol.h"

#include <QIODevice>
#include <QByteArray>
#include <QtAlgorithms>
#include <QMutexLocker>

#include "PiiHttpDevice.h"
#include "PiiHttpException.h"


PiiHttpProtocol::Data::Data() :
  iMaxConnectionTime(0) // No limit by default
{}

PiiHttpProtocol::UriHandler::~UriHandler() {}

PiiHttpProtocol::PiiHttpProtocol() :
  PiiNetworkProtocol(new Data)
{}

PiiHttpProtocol::~PiiHttpProtocol()
{}

void PiiHttpProtocol::registerUriHandler(const QString& uri, UriHandler* handler)
{
  PII_D;
  if (!uri.startsWith('/'))
    return;

  QMutexLocker lock(&d->handlerListLock);
  // Replace old handler if one exists.
  for (int i=0; i<d->lstHandlers.size(); ++i)
    if (d->lstHandlers[i].first == uri)
      {
        d->lstHandlers[i].second = handler;
        return;
      }
  d->lstHandlers << HandlerPair(uri, handler);
}

PiiHttpProtocol::UriHandler* PiiHttpProtocol::unregisterUriHandler(const QString& uri)
{
  PII_D;
  QMutexLocker lock(&d->handlerListLock);
  for (int i=0; i<d->lstHandlers.size(); ++i)
    if (d->lstHandlers[i].first == uri)
      return d->lstHandlers.takeAt(i).second;
  return 0;
}

void PiiHttpProtocol::unregisterUriHandler(UriHandler* handler)
{
  PII_D;
  QMutexLocker lock(&d->handlerListLock);
  for (int i=d->lstHandlers.size(); i--; )
    if (d->lstHandlers[i].second == handler)
      d->lstHandlers.removeAt(i);
}

void PiiHttpProtocol::unregisterAllHandlers(UriHandler* handler)
{
  PII_D;
  QMutexLocker lock(&d->handlerListLock);
  if (handler == 0)
    d->lstHandlers.clear();
  else
    for (int i=d->lstHandlers.size(); i--; )
      if (d->lstHandlers[i].second == handler)
        d->lstHandlers.removeAt(i);
}

void PiiHttpProtocol::communicate(QIODevice* dev, PiiProgressController* controller)
{
  PII_D;
  // This loop handles keep-alive connections.
  while (controller->canContinue())
    {
      PiiHttpDevice httpDevice(dev, PiiHttpDevice::Server);
      httpDevice.setController(controller);
      if (!httpDevice.readHeader())
        return;
      // If the header indicates the message will be too large, cut
      // the request short right here.
      if (httpDevice.messageSizeLimit() > 0 &&
          qMax(qint64(0), httpDevice.bodyLength()) + httpDevice.headerLength() > httpDevice.messageSizeLimit())
        {
          httpDevice.setStatus(RequestEntityTooLargeStatus);
          return;
        }

      // Interrupted
      if (!controller->canContinue())
        {
          httpDevice.setStatus(ServiceUnavailableStatus);
          return;
        }

      HandlerPair pair = findHandler(httpDevice.requestUri());
      if (pair.second != 0)
        {
          TimeLimiter limiter(controller, d->iMaxConnectionTime);
          try
            {
              httpDevice.setController(&limiter);
              piiDebug(httpDevice.requestMethod() + " " + httpDevice.requestPath());
              pair.second->handleRequest(pair.first, &httpDevice, &limiter);
            }
          catch (PiiHttpException& ex)
            {
              httpDevice.setStatus(ex.statusCode());
              httpDevice.print(ex.message());
              piiWarning(ex.location("", ": ") +
                         httpDevice.requestMethod() + " " + httpDevice.requestPath() + " " +
                         QString::number(ex.statusCode()) + " " + ex.message());
            }
          catch (PiiException& ex)
            {
              httpDevice.setStatus(InternalServerErrorStatus);
              httpDevice.print(ex.message());
              piiWarning(ex.location("", ": ") + ex.message());
            }
          httpDevice.setController(0);
        }
      else
        httpDevice.setStatus(NotFoundStatus);

      httpDevice.finish();

      // HTTP/1.1 behavior: we'll only close the connection if the
      // client or the handler specifically asked to do so, or the
      // client just closed the connection.
      if (httpDevice.connectionType() == PiiHttpDevice::CloseConnection ||
          !httpDevice.isWritable())
        return;
    }
}

PiiHttpProtocol::UriHandler* PiiHttpProtocol::uriHandler(const QString& uri, bool exactMatch)
{
  HandlerPair pair = findHandler(uri);
  return exactMatch && pair.first != uri ? 0 : pair.second;
}

PiiHttpProtocol::HandlerPair PiiHttpProtocol::findHandler(const QString& path)
{
  PII_D;
  QMutexLocker lock(&d->handlerListLock);

  int iBestMatchIndex = -1, iBestMatchLength = 0;
  for (int i=0; i<d->lstHandlers.size(); ++i)
    {
      // If the handler's URI matches the beginning of the request,
      // and the match is longer than anything before, this one is the
      // best so far.
      if (path.startsWith(d->lstHandlers[i].first) &&
          d->lstHandlers[i].first.size() > iBestMatchLength)
        {
          iBestMatchIndex = i;
          iBestMatchLength = d->lstHandlers[i].first.size();
        }
    }
  if (iBestMatchIndex != -1)
    return d->lstHandlers[iBestMatchIndex];

  return HandlerPair(path, 0);
}

// Stores HTTP status codes and their descriptions.
struct PiiHttpProtocol::StatusCode
{
  // Numerical status code.
  int code;
  // A human-representable description of the code.
  const char* description;
  bool operator< (const StatusCode& other) const { return code < other.code; }
};

QString PiiHttpProtocol::statusMessage(int code)
{
  static const StatusCode httpStatusCodes[] =
    { {100, "Continue" },
      {101, "Switching Protocols" },
      {102, "Processing" },
      {200, "OK" },
      {201, "Created" },
      {202, "Accepted" },
      {203, "Non-Authoritative Information" },
      {204, "No Content" },
      {205, "Reset Content" },
      {206, "Partial Content" },
      {207, "Multi-Status" },
      {226, "IM Used" },
      {300, "Multiple Choices" },
      {301, "Moved Permanently" },
      {302, "Found" },
      {303, "See Other" },
      {304, "Not Modified" },
      {305, "Use Proxy" },
      {306, "Reserved" },
      {307, "Temporary Redirect" },
      {400, "Bad Request" },
      {401, "Unauthorized" },
      {402, "Payment Required" },
      {403, "Forbidden" },
      {404, "Not Found" },
      {405, "Method Not Allowed" },
      {406, "Not Acceptable" },
      {407, "Proxy Authentication Required" },
      {408, "Request Timeout" },
      {409, "Conflict" },
      {410, "Gone" },
      {411, "Length Required" },
      {412, "Precondition Failed" },
      {413, "Request Entity Too Large" },
      {414, "Request-URI Too Long" },
      {415, "Unsupported Media Type" },
      {416, "Requested Range Not Satisfiable" },
      {417, "Expectation Failed" },
      {422, "Unprocessable Entity" },
      {423, "Locked" },
      {424, "Failed Dependency" },
      {426, "Upgrade Required" },
      {500, "Internal Server Error" },
      {501, "Not Implemented" },
      {502, "Bad Gateway" },
      {503, "Service Unavailable" },
      {504, "Gateway Timeout" },
      {505, "HTTP Version Not Supported" },
      {506, "Variant Also Negotiates" },
      {507, "Insufficient Storage" },
      {510, "Not Extended" }
    };

  int iCodes = sizeof(httpStatusCodes) / sizeof(StatusCode);
  const StatusCode searchCode = { code, 0 };
  const StatusCode* pEnd = httpStatusCodes + iCodes;
  const StatusCode* pMatch = qBinaryFind(httpStatusCodes, pEnd, searchCode);

  if (pMatch != pEnd)
    return pMatch->description;

  return QString();
}


QString PiiHttpProtocol::str11DateFormat("ddd, dd MMM yyyy hh:mm:ss");
QString PiiHttpProtocol::str10DateFormat("dddd, dd-MMM-yy hh:mm:ss");
QString PiiHttpProtocol::strCDateFormat("ddd MMM d hh:mm:ss yyyy");

QDateTime PiiHttpProtocol::stringToTime(const QString& dateTime)
{
  QString strDate(dateTime);
  if (strDate.endsWith(" GMT"))
    strDate.chop(4);
  QDateTime utcTime(QDateTime::fromString(strDate, str11DateFormat));
  if (utcTime.isValid())
    return utcTime;
  utcTime = QDateTime::fromString(strDate, str10DateFormat);
  if (utcTime.isValid())
    return utcTime;
  return QDateTime::fromString(strDate, strCDateFormat);
}

QString PiiHttpProtocol::timeToString(const QDateTime& dateTime)
{
  return dateTime.toString(str11DateFormat) + " GMT";
}

class PiiHttpProtocol::TimeLimiter::Data
{
public:
  Data(PiiProgressController* controller, int maxTime) :
    pController(controller), iMaxTime(maxTime)
  {
    time.start();
  }

  PiiProgressController* pController;
  int iMaxTime;
  QTime time;
};

PiiHttpProtocol::TimeLimiter::TimeLimiter(PiiProgressController* controller, int maxTime) :
  d(new Data(controller, maxTime))
{}
PiiHttpProtocol::TimeLimiter::~TimeLimiter() { delete d; }

bool PiiHttpProtocol::TimeLimiter::canContinue(double progressPercentage) const
{
  return d->pController->canContinue(progressPercentage) &&
    (d->iMaxTime <= 0 || d->time.elapsed() < d->iMaxTime);
}

void PiiHttpProtocol::TimeLimiter::setMaxTime(int maxTime) { d->iMaxTime = maxTime; }
int PiiHttpProtocol::TimeLimiter::maxTime() const { return d->iMaxTime; }
