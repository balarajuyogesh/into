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

#ifndef _PIIFILESYSTEMURIHANDLER_H
#define _PIIFILESYSTEMURIHANDLER_H

#include "PiiHttpProtocol.h"
#include "PiiMimeTypeMap.h"

#include <QObject>
#include <QFileInfoList>

/**
 * A URI handler for PiiHttpProtocol that maps request URIs to files.
 * This handler can be used with PiiHttpProtocol to serve static
 * files.
 *
 * ~~~(c++)
 * PiiFileSystemUriHandler* pFileServer = new PiiFileSystemUriHandler("/var/www/");
 * _pFileServer->setIndexFile("index.html");
 * _pHttpServer = PiiHttpServer::addServer("KideServer", address);
 * if (_pHttpServer == 0)
 *   piiWarning(QString("Could not bind server to %1.").arg(address));
 * else
 *   {
 *     _pHttpServer->protocol()->registerUriHandler("/", _pFileServer);
 *     _pHttpServer->start();
 *   }
 * ~~~
 *
 * PiiFileSystemUriHandler supports GET, HEAD, PUT, DELETE and MKCOL
 * methods. Methods allowed for clients are defined by the
 * [allowedMethods] property.
 *
 * Files uploaded with the PUT method will be initially placed to a
 * temporary directory, whose location is controlled by an environment
 * variable (TMPDIR on Unix, TEMP on Windows). To ensure atomicity of
 * file uploads, this directory should be on the same mounted file
 * system as the target directory.
 *
 */
class PII_NETWORK_EXPORT PiiFileSystemUriHandler :
  public QObject,
  public PiiHttpProtocol::UriHandler
{
  /**
   * The root directory of the file system from which files are
   * served. When a client requests "/", this directory will be
   * accessed. The default is an empty string, which means the
   * server's working directory. The path must end with a slash (/).
   */
  Q_PROPERTY(QString fileSystemRoot READ fileSystemRoot WRITE setFileSystemRoot);

  /**
   * The name of a directory index file. If `indexFile` is set to a
   * non-empty string, automatic directory lists will be disabled and
   * the contents of the specified file will be sent to the client
   * instead. If the requested folder doesn't contain an index file,
   * the request will fail with error code 403 (Forbidden).
   */
  Q_PROPERTY(QString indexFile READ indexFile WRITE setIndexFile);

  /**
   * The default format for automatically generated directory lists,
   * if [indexFile] is not set. The format can be changed by the client
   * by adding "?format=xxx" to the GET request. Supported formats are
   * "text", "html", and "json".
   */
  Q_PROPERTY(DirectoryListFormat defaultDirectoryListFormat READ defaultDirectoryListFormat
             WRITE setDefaultDirectoryListFormat);
  Q_ENUMS(DirectoryListFormat);

  /**
   * If this flag is `false` (the default), hidden files will not be
   * shown in directory listings and clients will not be able to
   * access them. Setting the property to `true` reveals hidden
   * files.
   */
  Q_PROPERTY(bool showHiddenFiles READ showHiddenFiles WRITE setShowHiddenFiles);

  /**
   * If this flag is `true` (the default), symbolic links in the
   * directory structure are followed. Otherwise not.
   */
  Q_PROPERTY(bool followSymLinks READ followSymLinks WRITE setFollowSymLinks);

  /**
   * Controls advisory file locking on Linux. If this flag is `true`,
   * a shared lock will be acquired for a file being read. The default
   * is `false`.
   */
  Q_PROPERTY(bool lockFiles READ lockFiles WRITE setLockFiles);

  /**
   * A list of allowed HTTP methods. By default, only GET and HEAD are
   * allowed. PiiFileSystemUriHandler also supports PUT, MKCOL and
   * DELETE requests.
   *
   * ~~~(c++)
   * handler->setProperty("allowedMethods",
   *                      QStringList() << "GET" << "HEAD" << "PUT");
   * ~~~
   */
  Q_PROPERTY(QStringList allowedMethods READ allowedMethods WRITE setAllowedMethods);

public:
  /**
   * Supported automatic directory list formats.
   *
   * - `TextFormat` - plain text.
   *
   * - `HtmlFormat` - HTML4 with links to files.
   *
   * - `JsonFormat` - JSON object for easy integration with JavaScript.
   */
  enum DirectoryListFormat
    {
      TextFormat,
      HtmlFormat,
      JsonFormat
    };
  PiiFileSystemUriHandler(const QString& fileSystemRoot = "");
  ~PiiFileSystemUriHandler();

  void handleRequest(const QString& uri, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller);

  void setFollowSymLinks(bool followSymLinks);
  bool followSymLinks() const;
  void setMimeTypeMap(PiiMimeTypeMap* map);
  PiiMimeTypeMap* mimeTypeMap() const;
  void setIndexFile(const QString& indexFile);
  QString indexFile() const;
  void setFileSystemRoot(const QString& fileSystemRoot);
  QString fileSystemRoot() const;
  void setDefaultDirectoryListFormat(DirectoryListFormat defaultDirectoryListFormat);
  DirectoryListFormat defaultDirectoryListFormat() const;
  void setShowHiddenFiles(bool showHiddenFiles);
  bool showHiddenFiles() const;
  void setLockFiles(bool lockFiles);
  bool lockFiles() const;
  void setAllowedMethods(const QStringList& allowedMethods);
  QStringList allowedMethods() const;

private:
  class Data
  {
  public:
    Data(const QString&);

    void ensureTrainingSlash();

    QString strFileSystemRoot;
    bool bFollowSymLinks;
    bool bShowHiddenFiles;
    PiiMimeTypeMap defaultMimeTypeMap;
    PiiMimeTypeMap* pMimeTypeMap;
    QString strIndexFile;
    DirectoryListFormat defaultDirectoryListFormat;
    bool bLockFiles;
    QStringList lstAllowedMethods;
  } *d;

  void getFile(const QString& fileName, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller);
  void putFile(const QString& fileName, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller);
  void deleteFile(const QString& fileName, PiiHttpDevice* dev);
  void getDirectory(const QString& path, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller);
  void listDirectoryAsText(PiiHttpDevice* dev, const QFileInfoList& files);
  void listDirectoryAsHtml(PiiHttpDevice* dev, const QFileInfoList& files);
  void listDirectoryAsJson(PiiHttpDevice* dev, const QFileInfoList& files);
  void deleteDirectory(const QString& dirName, PiiHttpDevice* dev);
  void createDirectory(const QString& dirName, PiiHttpDevice* dev);
  void createPath(const QString& path, PiiHttpDevice* dev);

  PII_DISABLE_COPY(PiiFileSystemUriHandler);
};

#endif //_PIIFILESYSTEMURIHANDLER_H
