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

#ifndef _PIIMIMEHEADER_H
#define _PIIMIMEHEADER_H

#include "PiiNetworkGlobal.h"
#include <QByteArray>
#include <QCoreApplication>

/**
 * A class that stores header information for MIME messages as defined
 * in RFC 1341, 2045 and 2387. PiiMimeHeader eases parsing of such
 * headers by providing some useful functions for querying common
 * header fields.
 *
 * @note An important thing to note about MIME headers is that the
 * field names (keys) are case-insensitive. That is, "Content-Type"
 * and "CONTENT-TYPE" are equivalent. If you first set a header in
 * capital letters, and later replace its value with setValue() using
 * a lowercase key, the capitalized key will remain.
 *
 * @ingroup Network
 */
class PII_NETWORK_EXPORT PiiMimeHeader
{
public:
  /**
   * Creates an empty header.
   */
  PiiMimeHeader();
  PiiMimeHeader(const PiiMimeHeader& other);
  /**
   * Parses the given header data and adds found fields to the header
   * list. If the data is not correctly formatted, no fields will be
   * added.
   *
   * @param headerData the header as raw ASCII data.
   */
  PiiMimeHeader(const QByteArray& headerData);

  ~PiiMimeHeader();

  /**
   * Returns @p true if no header fields have been added.
   */
  bool isEmpty() const;
  
  /**
   * Returns @p true if there is at least one valid header field.
   */
  bool isValid() const;

  /**
   * Sets the value of the entry with the given @a key to @a value. If
   * no entry with the given key exists, a new entry will be created. 
   * If an entry with the key already exists, the first of them will
   * be replaced with the given value.
   *
   * @note Keys are case-insensitive.
   */
  void setValue(const QString& key, const QString& value);
  void addValue(const QString& key, const QString& value);
  /**
   * Decodes the given raw header data and adds all key-value pairs in
   * it. Returns @p true if @a headerData was correctly formatted, and
   * @p false otherwise. Note that if a decoding error occurs, all
   * values found so far will be retained.
   */
  bool addValues(const QByteArray& headerData);
  bool addValues(const char* data, int length);

  QString value(const QString& key) const;
  /**
   * Removes the first field with the given @p key from the header.
   */
  void removeValue(const QString& key);
  /**
   * Removes all header fields with the given key.
   */
  void removeAllValues(const QString& key);
  /**
   * Removes all header fields.
   */
  void clear();
  /**
   * Returns all header fields as an ordered list of key-value pairs.
   */
  QList<QPair<QString,QString> > values() const;

  /**
   * Returns @p true if there is a value associated with @a key, and
   * @p false otherwise.
   */
  bool hasKey(const QString& key) const;
  
  /**
   * Returns @p true if the header describes a multipart message, and
   * @p false otherwise. The @p Content-Type header field of a
   * multipart message begins with @p multipart/. The body of a
   * multipart message consists of many entities.
   */
  bool isMultipart() const;

  /**
   * Sets the preamble.
   */
  void setPreamble(const QByteArray& preamble);
  
  /**
   * Returns the "preamble" of a multipart message, if there is one. 
   * The preamble can be used to transfer additional information that
   * is not part of the entity itself.
   */
  QByteArray preamble() const;

  /**
   * Returns @p true if the header describes a file uploaded from a
   * HTML form, and @p false otherwise. Browsers use @p
   * multipart/form-data encoding to send files from HTML forms. If
   * the @p Content-Disposition header contains a file name, the
   * entity can be treated as an uploaded file. Use the #fileName()
   * function to get the name of the file.
   */
  bool isUploadedFile() const;

  /**
   * Returns the name of the HTML form control that sent the following
   * entity. If the header does not contain such information, an empty
   * string will be returned.
   *
   * @code
   * // Content-Disposition: form-data; name=control
   * QString name = header.controlName(); // returns "control"
   * @endcode
   */
  QString controlName() const;

  /**
   * Returns the name of an uploaded file, if the entity is a file
   * uploaded from an HTML form. If there is no file name, an empty
   * string will be returned.
   */
  QString fileName() const;

  /**
   * Returns the boundary string that separates entities in this
   * multipart message. If this header does not represent a multipart
   * message, an empty string will be returned.
   */
  QByteArray boundary() const;

  /**
   * Returns the value of the Content-Type header field.
   */
  QString contentType() const;

  /**
   * Sets the Content-Length header field to the given value.
   */
  void setContentLength(uint length);
  
  /**
   * Returns the value of the Content-Length header as an unsigned
   * integer. If there is no Content-Length header, returns 0.
   */
  uint contentLength() const;

  bool hasContentLength() const;
  
  /**
   * Returns the value of the @p Content-Disposition header, without
   * parameters.
   *
   * @code
   * // Content-Disposition: form-data; name=control
   * QString disposition = header.contentDisposition(); // returns "form-data"
   * @endcode
   */
  QString contentDisposition() const;

  PiiMimeHeader& operator= (const PiiMimeHeader& other);

  /**
   * Encodes the header into a QByteArray, one key-value pair (key:
   * value) on a line. CRLF (\r\n) will be used as a line separator. 
   * The last line of the header will always be empty.
   *
   * @note If any of the values contains invalid characters (such as a
   * linefeed), the resulting chunk of bytes cannot be parsed as a
   * MIME header. PiiMimeHeader doesn't encode the values.
   */
  QByteArray toByteArray() const;

  /**
   * Reads a MIME header from @a device. This functions reads lines
   * from @a device until an empty line is found.
   *
   * @param device the input device
   *
   * @param maxLength the maximum number of bytes to read
   *
   * @param bytesRead a return-value parameter that stores the number
   * of bytes actually read from @a device. Note that this value may
   * be different from the size of the returned array.
   *
   * @return header data as a byte array, excluding the empty line at
   * the end.
   */
  static QByteArray readHeaderData(QIODevice* device, qint64 maxLength, qint64* bytesRead = 0);
  
protected:  
  /// @internal
  class Data : public PiiSharedD<Data>
  {
  public:
    Data();
    Data(const Data& other);
    virtual ~Data();
    virtual Data* clone() const;

    bool bValid;
    QList<QPair<QString,QString> > lstHeaders;
    QByteArray aPreamble;
  } *d;
  PII_SHARED_D_FUNC;
  /// @internal
  PiiMimeHeader(Data* d);

private:
  int indexOf(const QString& key) const;
  bool addLine(const char* ptr, int length);
  QString dispositionParam(const QString& param) const;

  static inline QString tr(const char* s) { return QCoreApplication::translate("PiiMimeHeader", s); }
};

#endif //_PIIMIMEHEADER_H
