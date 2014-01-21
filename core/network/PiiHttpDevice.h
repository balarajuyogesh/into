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

#ifndef _PIIHTTPDEVICE_H
#define _PIIHTTPDEVICE_H

#include <QIODevice>
#include <QVariantMap>
#include <QStack>
#include <QVariant>

#include <PiiTimer.h>

#include "PiiSocketDevice.h"
#include "PiiStreamFilter.h"
#include "PiiHttpRequestHeader.h"
#include "PiiHttpResponseHeader.h"

class QTextCodec;
class PiiProgressController;

/**
 * An I/O device for HTTP/1.1 communication. This class can be used to
 * send and parse HTTP request and replies. It works as an ordinary
 * I/O device, but ensures that the HTTP protocol format is retained.
 *
 * The class automates the handling of HTTP headers as much as
 * possible. If you request information that is not available without
 * reading the request header, PiiHttpDevice will read the header
 * automatically. It also builds the request or response header, and
 * has reasonable defaults for most header fields. The header will be
 * automatically sent if data is written to the device.
 *
 * PiiHttpDevice supports output filtering. All data written to the
 * client/server can be passed through an unlimited number of filters
 * which may buffer the data. If no filters are installed, data will
 * be written directly to the low-level socket. Headers will be sent
 * just before the first byte of the message body is going to be
 * written to the client. It is not possible to change the headers
 * afterwards.
 *
 * In `Server` mode, PiiHttpDevice is typically used in the
 * `handleRequest`() function of PiiHttpProtocol::UriHandler:
 *
 * ~~~(c++)
 * void MyHandler::handleRequest(const QString& uri,
 *                               PiiHttpDevice* h,
 *                               PiiProgressController* controller)
 * {
 *   if (h->requestMethod() != "GET")
 *     {
 *       h->setStatus(PiiHttpProtocol::MethodNotAllowedStatus);
 *       return; // do not send anything
 *     }
 *   h->startOutputFiltering(new PiiStreamBuffer);
 *   h->setHeader("Content-Type", "text/html");
 *   h->print("<html><body>You hit a handler at " + uri + "</body></html>\n");
 * }
 * ~~~
 *
 * Using a PiiStreamBuffer as an output filter will usually increase
 * performance. The effect is that writes to the device will be stored
 * into memory until the buffer is flushed. PiiHttpProtocol can also
 * utilizes the buffer by automatically setting the Content-Length
 * header.
 *
 * In `Client` mode, the I/O device must be created first.
 * PiiNetworkClient can be used to easily create a suitable I/O
 * device:
 *
 * ~~~(c++)
 * PiiNetworkClient client("tcp://127.0.0.1:80");
 * PiiHttpDevice dev(client.openConnection(), PiiHttpDevice::Client);
 * dev.setRequest("GET", "/");
 * dev.setHeader("Host", "localhost");
 * dev.finish();
 *
 * // Read response header and ignore the response body.
 * dev.readHeader();
 * dev.discardBody();
 * ~~~
 *
 */
class PII_NETWORK_EXPORT PiiHttpDevice : public QIODevice, public PiiStreamFilter
{
  Q_OBJECT

public:
  /**
   * Connection types, specified by the `Connection` request header.
   *
   * - `KeepAliveConnection` - the default for HTTP/1.1. The
   * connection will not be closed after a request.
   *
   * - `CloseConnection` - the connection will be closed after
   * request. This happens when the client sends the "Connection:
   * close" request header.
   */
  enum ConnectionType { KeepAliveConnection, CloseConnection };

  /**
   * Communication modes.
   *
   * - `Client` - the device is used at the client side
   *
   * - `Server` - the device is used at the server side
   */
  enum Mode { Client, Server };

  /**
   * Initialize a HTTP device with the given low-level device.
   *
   * @param device the communication device. Typically, *device* is a
   * PiiWaitingIODevice. The pointer must remain valid during the
   * lifetime of the HTTP device.
   *
   * @param mode communication mode
   */
  PiiHttpDevice(const PiiSocketDevice& device, Mode mode);

  /**
   * Destroy the HTTP device. This will finish all unfinished output
   * filters.
   */
  ~PiiHttpDevice();

  /**
   * Returns the request method. Typically, the answer is either "GET" or
   * "POST", but HTTP extensions may use arbitrary request methods.
   */
  QString requestMethod() const;

  /**
   * Returns the connection type. If either the request or the
   * response header specifies "Connection: close" returns
   * `CloseConnection`. Otherwise returns `KeepAliveConnection`.
   */
  ConnectionType connectionType() const;

  /**
   * Returns the request URI. The returned string contains the full
   * path the client sent with the request, including possible encoded
   * request parameters.
   */
  QString requestUri() const;

  /**
   * Returns the query string part of the request URI, if any. The
   * query string contains URI-encoded query values. The question mark
   * at the beginning of the query string will not be returned.
   *
   * @return the query string. Percent-encoded characters will be
   * automatically converted. May be empty.
   *
   * ~~~(c++)
   * // request URI: /foo/bar/baz?a=2
   * QString str = h->queryString();
   * // str = "a=2"
   * ~~~
   */
  QString queryString() const;

  /**
   * Returns the query string without decoding percent-encoded
   * characters.
   *
   * @see queryString()
   */
  QString rawQueryString() const;

  /**
   * Returns `true` if the request URI contains a query string, and
   * `false` otherwise.
   */
  bool hasQuery() const;

  /**
   * Returns a query value from the request URI. This function returns
   * the variable called *name* in the encoded request string. If
   * there are multiple values associated with the same key, a list of
   * values will be returned. Values are automatically decoded (see
   * [decodeVariant()]).
   *
   * ~~~(c++)
   * // request URI: /path/?var1=a&var2=b&var2=c
   *
   * QString a = h->queryValue("var1").toString();
   * // a = "a"
   * QVariantList bc = h->queryValue("var2").toList();
   * // bc = ("b", "c")
   * ~~~
   */
  QVariant queryValue(const QString& name) const;

  /**
   * Returns all query values as a map of key-value pairs. Parameter
   * values are automatically decoded (see [decodeVariant()]).
   */
  QVariantMap queryValues() const;

  /**
   * Returns the names of all query items as a list. This function
   * returns the "raw" query names in the order they appear in the
   * request URI. The list may contain duplicate entries.
   */
  QStringList queryItems() const;

  /**
   * Adds a query value to the request URI. This function can be used
   * in `Client` mode to automatically encode values in the query.
   *
   * @param name the name of the query value.
   *
   * @param value the value, will be automatically encoded (see
   * [encode()]). Multiple items with the same name may exists. If the
   * value is invalid, it (and the equal sign) will be omitted.
   *
   * ~~~(c++)
   * dev.addQueryValue("v1", 1);
   * dev.addQueryValue("v2", "Hello");
   * dev.addQueryValue("v2", 3.14);
   *
   * // dev.queryString() == "v1=1&v2=%22hello%22&v2=3.14"
   * ~~~
   */
  void addQueryValue(const QString& name, const QVariant& value);

  /**
   * Removes a query value from the request URI.
   */
  void removeQueryValue(const QString& name);

  /**
   * Removes all query values from the request uri.
   */
  void removeQueryValues();

  /**
   * Returns the path part of the request URI.
   *
   * @param basePath strip this path from the beginning of the path.
   * This is useful if you want to find the relative path of a URI
   * handler.
   *
   * ~~~(c++)
   * // request URI: /foo/bar/baz?a=2
   * QString str = h->requestPath("/foo/");
   * // str = "bar/baz"
   * ~~~
   */
  QString requestPath(const QString& basePath = QString()) const;

  /**
   * Same as [requestPath()], but doesn't automatically decode
   * percent-encoded characters in URI.
   */
  QString rawRequestPath(const QString& basePath = QString()) const;

  /**
   * Sets a value in the request header.
   *
   * @param key the name of the header field, such as "Content-Type".
   *
   * @param value the value of the field
   *
   * @param replace indicates whether the header should replace a
   * previous similar header, or add a second header of the same type.
   */
  void setRequestHeader(const QString& name, const QString& value, bool replace);

  /**
   * Returns the full request header.
   */
  PiiHttpRequestHeader requestHeader() const;

  /**
   * Sets a value in the response header.
   *
   * @param key the name of the header field, such as "Location".
   *
   * @param value the value of the field
   *
   * @param replace indicates whether the header should replace a
   * previous similar header, or add a second header of the same type.
   *
   * ! The function will automatically change the status code
   * ([setStatus()]) if needed. For example, setting the `Location`
   * header changes the status code automatically to 302 (redirect),
   * unless the status code is already in the 3xx series.
   */
  void setResponseHeader(const QString& name, const QString& value, bool replace);

  /**
   * Returns the full response header.
   */
  PiiHttpResponseHeader responseHeader() const;

  /**
   * Sets the request method to *method* and request URI to *uri*.
   * The *uri* will not be parsed; you can give anything (including
   * invalid data) as the request uri.
   *
   * If the given *uri* contains query parameters (indicated by a
   * question mark), all query parameters previously added with
   * [addQueryValue()] will be cleared.
   *
   * If *method* is `GET`, Content-Type header will be automatically
   * removed.
   *
   * In `Client` mode, and if the previous request has been finished
   * (see [finish()]), calling this function will call [restart()]
   * first. Note that this resets the device to its initial state,
   * clearing all output buffers.
   */
  void setRequest(const QString& method, const QString& uri);

  void setRequestMethod(const QString& method);

  void setRequestUri(const QString& uri);

  /**
   * Returns a request parameter from an
   * `application`/x-www-form-urlencoded POST body. If the body is not
   * such type, or the value called *name* does not exist, an invalid
   * variant will be returned. Note that this function will only work
   * if you haven't already read the message body yourself.
   *
   * ! If the message body has not been read, this function does
   * so, even though it is `const`.
   *
   * @see [queryValue()]
   */
  QVariant formValue(const QString& name) const;

  /**
   * Returns all form-encoded values in the message body as a map.
   * Parameter values are automatically decoded (see
   * [decodeVariant()]).
   *
   * ! If the message body has not been read, this function does
   * so, even though it is `const`.
   */
  QVariantMap formValues() const;

  /**
   * Returns the names of form fields in the order they appear in the
   * POST message body.
   *
   * ! If the message body has not been read, this function does
   * so, even though it is `const`.
   *
   * @see queryItems()
   */
  QStringList formItems() const;

  /**
   * Returns a request parameter from either the request URI (GET
   * parameters) or the request body (POST parameters). If the
   * variable is found in both, the request URI takes precedence.
   */
  QVariant requestValue(const QString& name) const;

  /**
   * Returns all request parameters (either GET parameters in the URI
   * or form values in the message body) as a map. If the same
   * variable is found in both, the request URI takes precedence.
   * Parameter values are automatically decoded (see
   * [decodeVariant()]).
   */
  QVariantMap requestValues() const;

  /**
   * Returns the client-supplied name of the host, if there is one.
   */
  QString host() const;

  /**
   * Returns the HTTP response status code.
   */
  int status() const;

  /**
   * Set the HTTP status code. The default value is 200 (OK).
   */
  void setStatus(int code);

  /**
   * Puts an output filter on the filter stack. All subsequent writes
   * to the device will go through *filter* first.
   *
   * You may call startOutputFiltering() many times. Just make sure
   * that you call [endOutputFiltering()] the same number of times. If
   * multiple output filters are active, output will be filtered
   * sequentially through each of them.
   *
   * @param filter the output filter. PiiHttpDevice takes the
   * ownership of the pointer.
   *
   * ~~~(c++)
   * // Buffer all data into memory until finish() or endOutputFiltering() is called.
   * dev.startOutputFiltering(new PiiStreamBuffer);
   * ~~~
   */
  void startOutputFiltering(PiiStreamFilter* filter);

  /**
   * Returns the currently active (topmost) output filter.
   *
   * @return the filter that first filters data written to the device,
   * or 0 if no filters have been added.
   */
  PiiStreamFilter* outputFilter() const;

  /**
   * Pops an output filter from the filter stack. This function will
   * collapse the filter stack until it finds *filter* and tell each
   * filter to finish its job. The removed filters will be deleted.
   * The next filter below *filter* on the stack (if any) will be
   * activated.
   *
   * If the last filter is removed, and the filter has buffered data,
   * this function tries to determine the number of bytes to be sent
   * to the client and set the Content-Length header accordingly. If a
   * single byte has been sent previously, it is too late to send the
   * header. If you want to avoid sending the automatic header, call
   * filter->flushFilter() to flush buffered data before
   * endOutputFiltering().
   *
   * @param filter the filter until which all filters will be
   * finished. If *filter* is 0, only the topmost filter will be
   * removed.
   */
  void endOutputFiltering(PiiStreamFilter* filter = 0);

  /**
   * Sets a HTTP request/response header field. If the device is in
   * `Client` mode, this function modifies the request header. In
   * `Server` mode, it modifies the response header.
   *
   * @param name the name of the header field, such as "Location".
   *
   * @param value the value of the field
   *
   * @param replace indicates whether the header should replace a
   * previous similar header, or add a second header of the same type.
   *
   * ~~~(c++)
   * // An example for setHeader() usage on server side.
   *
   * // Set content type
   * h->setHeader("Content-Type", "text/xml");
   *
   * // Set content length
   * h->setHeader("Content-Length", 31416);
   *
   * // Redirect the client to another address
   * h->setHeader("Location", "http://www.intopii.fi/");
   * // Note: it is not a good idea to send data to the client after
   * // redirecting it.
   *
   * // Set authentication style
   * h->setHeader("WWW-Authenticate" "Negotiate");
   * // Add another header with the same name
   * h->setHeader("WWW-Authenticate", "NTLM", false);
   *
   * // Asks the server to close the connection after request.
   * h->setHeader("Connection", "close");
   * ~~~
   *
   * ! Setting the `Content`-Encoding header will automatically
   * change the output text codec. See QTextCodec for supported
   * encodings.
   */
  void setHeader(const QString& name, const QVariant& value, bool replace = true);

  /**
   * Removes the header field(s) with *name*. If the device is in
   * `Client` mode, this function modifies the request header. In
   * `Server` mode, it modifies the response header.
   */
  void removeHeader(const QString& name);

  /**
   * See if request/response header have been successfully read.
   *
   * @return `true`, if request headers have been read, `false`
   * otherwise.
   */
  bool headerRead() const;

  /**
   * Set the maximum number of bytes an HTTP request header is allowed
   * to contain. This value is used to prevent exhaustive memory
   * allocation. The default value is 4096.
   */
  void setHeaderSizeLimit(qint64 headerSizeLimit);
  /**
   * Returns the current header size limit.
   */
  qint64 headerSizeLimit() const;
  /**
   * Set the maximum number of bytes the entire HTTP message
   * (including the header and the body) is allowed to contain. This
   * value can be used to prevent exhaustive memory allocation. The
   * default value is 8M. Once the total size of a HTTP message
   * exceeds this value, reading the device will fail. Setting the
   * size limit to zero disables it.
   */
  void setMessageSizeLimit(qint64 messageSizeLimit);
  /**
   * Returns the current message size limit.
   */
  qint64 messageSizeLimit() const;

  /**
   * Ensures that headers are sent to the other end of the connection,
   * flushes all output filters and flushes the output device. On the
   * client side, this function must be called before reading a
   * response. The final form of the request/response headers is
   * available only after finish() is done. For example,
   * [connectionType()] may be `CloseConnection` even if the client
   * did not request it.
   *
   * Calling finish() multiple times has no effect. If you intend to
   * use the same PiiHttpDevice for successive requests, you need to
   * call [restart()] after handling the request. The [setRequest()] (on
   * the client side) and [readHeader()] (on the server side) functions
   * automatically restart the request.
   */
  void finish();

  qint64 flushFilter();

  /**
   * Restarts the request/reply cycle. Destroys all active output
   * filters and clears stored form/query values.
   *
   * This function needs to be called after each request if the same
   * device is used again. On the client side, [setRequest()]
   * automatically calls this function. On the server side,
   * [readHeader()] does the same.
   */
  void restart();

  /**
   * Returns `true` if the low-level socket device is still
   * connected, `false` otherwise.
   */
  bool isWritable() const;

  /**
   * Returns `true` if the low-level socket device is still
   * connected, `false` otherwise.
   */
  bool isReadable() const;

  /**
   * Prints text to the device. This function is equivalent to
   * QIODevice::write(), but it converts the unicode input text using
   * the encoding style defined with the `Content`-Encoding header.
   * If no encoding has been set, UTF-8 will be used.
   */
  qint64 print(const QString& data);

  /**
   * Encodes *msg* to a byte array using the current encoding. If the
   * `Content`-Encoding header has not been set, UTF-8 will be used.
   */
  QByteArray encode(const QString& msg) const;

  qint64 filterData(const char* data, qint64 maxSize);

  bool isSequential() const;
  qint64 bytesAvailable() const;

  /**
   * Reads the whole message body and returns it as a byte array. Note
   * that the function returns data even if the whole body could not
   * be read.
   *
   * ! If you call this function yourself, a POST encoded message
   * body can no longer be automatically parsed, and functions such as
   * [formItems()] and [formValues()] return empty values.
   */
  QByteArray readBody();

  /**
   * Reads the whole message body to the given I/O device. If the
   * device is zero, the body will be discarded. Returns the number of
   * bytes read on success and -1 on failure.
   *
   * ! If you call this function yourself, a POST encoded message
   * body can no longer be automatically parsed, and functions such as
   * [formItems()] and [formValues()] return empty values.
   */
  qint64 readBody(QIODevice* device);

  /**
   * Discards the whole message body. This is the same as readBody(0).
   */
  void discardBody();

  /**
   * Reads request/response header. This function checks that the
   * header has not been read and calls the protected [decodeHeader()]
   * function if needed.
   */
  bool readHeader();

  /**
   * Sends request/response headers. This function checks that the
   * headers have not been sent and calls the protected
   * [encodeHeader()] function if needed.
   */
  bool sendHeader();

  /**
   * Returns `true` if at least one byte of the message body has been
   * read and `false` otherwise.
   */
  bool isBodyRead() const;

  /**
   * Returns the number of bytes in the message body. This value is
   * known only if the header contains a Content-Length field.
   * Otherwise -1 will be returned.
   */
  qint64 bodyLength() const;
  /**
   * Returns the number of bytes in the parsed request/response
   * header. If the header has not been read, -1 will be returned.
   */
  qint64 headerLength() const;

  /**
   * Decodes *data* and returns its value as a %QVariant. The
   * following conversions are tried, in preference order:
   *
   * - If *data* begins with a magic string that identifies an
   * archive (e.g. "Into Txt" or "Into Bin"), it is assumed to contain
   * a serialized QVariant. The variant will be deserialized.
   *
   * - If *data* can be converted to an `int`, an `int` will be
   * returned.
   *
   * - If *data* can be converted to a `double`, a `double` will
   * be returned.
   *
   * - If *data* is either "true" or "false" (without the quotes),
   * the corresponding boolean value will be returned.
   *
   * - Otherwise, a QString will be returned. If the value is
   * enclosed in double quotes, they will be removed.
   *
   * ! In some occasions, it may be necessary to pass "true",
   * "false", "1.23" or "Into Bin" as strings. To prevent automatic
   * conversion, enclose the value in double quotes.
   *
   * @return the decoded value. If the decoding fails, an invalid
   * variant will be returned.
   */
  QVariant decodeVariant(const QByteArray& data) const;
  QVariant decodeVariant(const QString& data) const;

  /**
   * Encodes a QVariant to a QByteArray. If the variant represents a
   * number, it will be converted to decimal representation. Boolean
   * values will be converted to "true" and "false" (without the
   * double quotes). QStrings will be enclosed in double quotes. All
   * other types will be serialized.
   */
  QByteArray encode(const QVariant& variant, PiiNetwork::EncodingFormat format = PiiNetwork::TextFormat) const;

  /**
   * Set the communication device. Usually, a new PiiHttpDevice is
   * created whenever a new communication device is needed. This
   * function is useful in `Client` mode if you need to reopen a
   * connection to a server but still want to retain old configuration
   * such as request parameters or cookies. Setting *device* to null
   * is not allowed.
   *
   * @exception PiiInvalidArgumentException& if *device* is null.
   */
  void setDevice(const PiiSocketDevice& device);

  /**
   * Returns the communication device. Note that writing to the device
   * directly may break the HTTP protocol. Use the underlying device
   * at your own risk.
   */
  PiiSocketDevice device() const;

  void setController(PiiProgressController* controller);
  PiiProgressController* controller() const;

  void setDataTimeout(int dataTimeout);
  int dataTimeout() const;

protected:
  qint64 readData(char* data, qint64 maxSize);
  qint64 writeData(const char * data, qint64 maxSize);

private:
  void clearBuffer(QIODevice* device);
  void parseQueryValues(const QString& uri);
  void parseVariables(const QByteArray& data, QVariantMap& valueMap, QStringList& names);
  inline void addToMap(QVariantMap& map, const QString& key, const QByteArray& value);
  void addToMap(QVariantMap& map, const QString& key, const QVariant& value);
  template <class Archive> static QVariant decodeVariant(const QByteArray& data);
  template <class Archive> static QByteArray encode(const QVariant& variant);

  inline qint64 writeToSocket(const char * data, qint64 maxSize);
  void checkCodec(const QString& key, const QString& value);

  void destroyOutputFilters();

  void readFormValues();

  void createQueryString();
  bool sendResponseHeader();
  bool decodeResponseHeader();
  bool sendRequestHeader();
  bool decodeRequestHeader();

  /// @internal
  class Data : public PiiStreamFilter::Data
  {
  public:
    Data(PiiHttpDevice* owner, const PiiSocketDevice& device, Mode mode);

    Mode mode;

    QVariantMap mapQueryValues;
    QStringList lstQueryItems;
    QVariantMap mapFormValues;
    QStringList lstFormItems;
    PiiHttpRequestHeader requestHeader;
    PiiHttpResponseHeader responseHeader;

    PiiSocketDevice pSocket;
    PiiProgressController* pController;
    PiiStreamFilter* pActiveOutputFilter;
    QTextCodec* pTextCodec;
    bool bHeaderRead, bHeaderSent;
    qint64 iHeaderSizeLimit;
    qint64 iMessageSizeLimit;
    qint64 iBytesRead, iBytesWritten;
    bool bBodyRead, bFinished;
    qint64 iBodyLength, iHeaderLength;
    int iDataTimeout;
  };
  PII_D_FUNC;

  PII_DISABLE_COPY(PiiHttpDevice);
};

#endif //_PIIHTTPDEVICE_H
