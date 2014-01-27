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

#include "PiiFileSystemUriHandler.h"

#include "PiiHttpDevice.h"
#include "PiiHttpException.h"
#include "PiiStreamBuffer.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>

#include <PiiFileUtil.h>

#ifdef Q_OS_LINUX
#  include <cstdio> // rename
#  include <sys/file.h> // flock
#  include <errno.h>
#endif

PiiFileSystemUriHandler::Data::Data(const QString& fileSystemRoot) :
  strFileSystemRoot(fileSystemRoot),
  bFollowSymLinks(true),
  bShowHiddenFiles(false),
  pMimeTypeMap(&defaultMimeTypeMap),
  defaultDirectoryListFormat(HtmlFormat),
  bLockFiles(false)
{
  ensureTrainingSlash();
  lstAllowedMethods << "GET" << "HEAD";
}

void PiiFileSystemUriHandler::Data::ensureTrainingSlash()
{
  if (!strFileSystemRoot.isEmpty() &&
      strFileSystemRoot[strFileSystemRoot.size()-1] != '/')
    strFileSystemRoot += '/';
}

PiiFileSystemUriHandler::PiiFileSystemUriHandler(const QString& fileSystemRoot) :
  d(new Data(fileSystemRoot))
{}

PiiFileSystemUriHandler::~PiiFileSystemUriHandler()
{
  delete d;
}

void PiiFileSystemUriHandler::getFile(const QString& fileName,
                                      PiiHttpDevice* dev,
                                      PiiHttpProtocol::TimeLimiter* controller)
{
  if (dev->requestMethod() not_member_of<QString> ("GET", "HEAD"))
    PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);
  QFileInfo info(fileName);
  if ((!d->bFollowSymLinks && info.isSymLink()) ||
      (!d->bShowHiddenFiles && info.isHidden()))
    PII_THROW_HTTP_ERROR(NotFoundStatus);

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    PII_THROW_HTTP_ERROR(NotFoundStatus);

  QDateTime modTime(info.lastModified().toUTC());

  // If the client gave us an If-Modified-Since header, we may not
  // need to send the file at all.
  QString strReqTime = dev->requestHeader().value("If-Modified-Since");
  if (!strReqTime.isEmpty())
    {
      QDateTime reqTime(PiiHttpProtocol::stringToTime(strReqTime));
      if (reqTime >= modTime)
        {
          dev->setHeader("Date", PiiHttpProtocol::timeToString(QDateTime::currentDateTime()));
          PII_THROW_HTTP_ERROR(NotModifiedStatus);
        }
    }

  qint64 iSize = info.size();
  dev->setHeader("Last-Modified", PiiHttpProtocol::timeToString(modTime));
  dev->setHeader("Content-Length", iSize);
  dev->setHeader("Content-Type", d->pMimeTypeMap->typeForExtension(info.suffix()));
  // PENDING Range
  if (dev->requestMethod() == "HEAD")
    return;

#ifdef Q_OS_LINUX
  bool bLock = d->bLockFiles;
  if (bLock && flock(file.handle(), LOCK_SH) == -1)
    piiWarning(tr("Cannot obtain a shared lock for %1.").arg(file.fileName()));
#endif
  // Read file contents and pass to the socket.
  PiiNetwork::passData(&file, dev, iSize, controller);
#ifdef Q_OS_LINUX
  if (bLock && flock(file.handle(), LOCK_UN) == -1)
    piiWarning(tr("Cannot unlock %1.").arg(file.fileName()));
#endif
}

void PiiFileSystemUriHandler::putFile(const QString& fileName,
                                      PiiHttpDevice* dev,
                                      PiiHttpProtocol::TimeLimiter* controller)
{
  QFileInfo info(fileName);
  bool bExisted = info.exists();

  // If hidden files are not shown to clients, they cannot be created,
  // either.
  if (!d->bShowHiddenFiles && info.isHidden())
    {
      piiWarning(tr("%1 would be a hidden file. Refusing to create.").arg(fileName));
      PII_THROW_HTTP_ERROR(ForbiddenStatus);
    }

  QTemporaryFile tmpFile(QDir::tempPath() + "/uploadtmp");
  if (!tmpFile.open())
    {
      piiWarning(tr("Cannot open %1 for writing.").arg(tmpFile.fileName()));
      PII_THROW_HTTP_ERROR(InternalServerErrorStatus);
    }
  if (PiiNetwork::passData(dev, &tmpFile, dev->requestHeader().contentLength(), controller) == -1)
    {
      piiWarning(tr("Uploading to %1 failed.").arg(tmpFile.fileName()));
      PII_THROW_HTTP_ERROR(InternalServerErrorStatus);
    }
#ifdef Q_OS_LINUX
  // Atomic rename/replace
  if (!rename(qPrintable(tmpFile.fileName()),
              qPrintable(fileName)))
    {
      dev->setStatus(bExisted ? PiiHttpProtocol::NoContentStatus : PiiHttpProtocol::CreatedStatus);
      return;
    }
  else if (errno != EXDEV) // rename() cannot cross file systems, but we have a back-up plan.
    {
      piiWarning(tr("Atomically renaming %1 to %2 failed.").arg(tmpFile.fileName()).arg(fileName));
      if (errno member_of (EPERM, EACCES, EROFS))
        PII_THROW_HTTP_ERROR(ForbiddenStatus);
      else
        PII_THROW_HTTP_ERROR(InternalServerErrorStatus);
    }
  else
#endif
    {
      // PENDING This is far from atomic. We just hope no one is reading the file at the same time.
      QFile targetFile(fileName);
      if (targetFile.exists())
        {
          if (!targetFile.remove())
            {
              piiWarning(tr("Cannot remove %1.").arg(fileName));
              PII_THROW_HTTP_ERROR(ForbiddenStatus);
            }
        }
      if (!tmpFile.copy(fileName))
        {
          piiWarning(tr("Copying %1 to %2 failed.").arg(tmpFile.fileName()).arg(fileName));
          if (tmpFile.error() == QFile::PermissionsError)
            PII_THROW_HTTP_ERROR(ForbiddenStatus);
          else
            PII_THROW_HTTP_ERROR(InternalServerErrorStatus);
        }
      dev->setStatus(bExisted ? PiiHttpProtocol::NoContentStatus : PiiHttpProtocol::CreatedStatus);
      return;
    }
}

void PiiFileSystemUriHandler::deleteFile(const QString& fileName,
                                         PiiHttpDevice* dev)
{
  QFile file(fileName);
  if (!file.remove())
    {
      piiWarning(tr("Deleting %1.").arg(fileName));
      if (file.error() == QFile::PermissionsError)
        PII_THROW_HTTP_ERROR(ForbiddenStatus);
      else
        PII_THROW_HTTP_ERROR(InternalServerErrorStatus);
    }
  dev->setStatus(PiiHttpProtocol::NoContentStatus);
}

void PiiFileSystemUriHandler::createDirectory(const QString& dirName,
                                              PiiHttpDevice* dev)
{
  if (dev->requestHeader().contentLength() != 0)
    PII_THROW_HTTP_ERROR(UnsupportedMediaTypeStatus);
  QDir dir(dirName);
  if (dir.exists())
    {
      piiWarning(tr("Trying to recreate directory %1.").arg(dirName));
      PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);
    }
  QString strName = dir.dirName();
  dir.cdUp();
  if (!QFileInfo(dir.path()).isWritable())
    {
      piiWarning(tr("Cannot create directory %1. Parent directory not writable.").arg(dirName));
      PII_THROW_HTTP_ERROR(ForbiddenStatus);
    }
  if (!dir.mkdir(strName))
    {
      piiWarning(tr("Unknown error in creating directory %1.").arg(dirName));
      PII_THROW_HTTP_ERROR(InternalServerErrorStatus);
    }
  dev->setStatus(PiiHttpProtocol::CreatedStatus);
}

void PiiFileSystemUriHandler::createPath(const QString& path,
                                         PiiHttpDevice* dev)
{
  if (dev->requestHeader().contentLength() != 0)
    PII_THROW_HTTP_ERROR(UnsupportedMediaTypeStatus);

  QFileInfo info(path);
  if (info.isDir())
    dev->setStatus(PiiHttpProtocol::NoContentStatus);
  else if (info.exists())
    PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);
  else if (!QDir().mkpath(path))
    dev->setStatus(PiiHttpProtocol::CreatedStatus);
  else
    dev->setStatus(PiiHttpProtocol::NoContentStatus);
}


void PiiFileSystemUriHandler::deleteDirectory(const QString& dirName,
                                              PiiHttpDevice* dev)
{
  if (!Pii::deleteDirectory(dirName))
    PII_THROW_HTTP_ERROR(ForbiddenStatus);
  dev->setStatus(PiiHttpProtocol::NoContentStatus);
}

void PiiFileSystemUriHandler::listDirectoryAsText(PiiHttpDevice* dev, const QFileInfoList& files)
{
  for (QFileInfoList::const_iterator i=files.begin(); i!=files.end(); ++i)
    {
      dev->print(i->fileName());
      if (i->isDir())
        dev->print("/");
      dev->print("\n");
    }
  dev->setHeader("Content-Type", "text/plain");
}

void PiiFileSystemUriHandler::listDirectoryAsHtml(PiiHttpDevice* dev, const QFileInfoList& files)
{
  dev->print("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
             "<html>\n"
             "<head><title>" + dev->requestPath() + "</title></head>\n"
             "<body>\n"
             "<ul>\n");

  for (QFileInfoList::const_iterator it = files.begin(); it != files.end(); ++it)
    {
      if (it->isDir())
        dev->print(QString("  <li class=\"dir\"><a href=\"%1/\">%1/</a></li>\n").arg(it->fileName()));
      else
        dev->print(QString("  <li class=\"file\"><a href=\"%1\">%1</a></li>\n").arg(it->fileName()));
    }

  dev->print("</ul>\n"
             "</body>\n"
             "</html>\n");

  dev->setHeader("Content-Type", "text/html");
}

void PiiFileSystemUriHandler::listDirectoryAsJson(PiiHttpDevice* dev, const QFileInfoList& files)
{
  QStringList lstFields = dev->queryValue("fields").toString().split(',', QString::SkipEmptyParts);
  enum { Name = 1, Size = 2, LastModified = 4 };
  int iFields = lstFields.isEmpty() ? Name : 0;
  for (QStringList::const_iterator it = lstFields.begin(); it != lstFields.end(); ++it)
    {
      if (*it == "name") iFields |= Name;
      else if (*it == "size") iFields |= Size;
      else if (*it == "modified") iFields |= LastModified;
    }

  dev->print("[");
  for (QFileInfoList::const_iterator it = files.begin(); it != files.end(); ++it)
    {
      QVariantMap mapValues;
      if (iFields & Name)
        mapValues["name"] = it->fileName();
      if (iFields & Size)
        mapValues["size"] = it->size();
      if (iFields & LastModified)
        mapValues["modified"] = it->lastModified();

      if (it != files.begin()) dev->print(",\n ");
      dev->print(PiiNetwork::toJson(mapValues));
    }
  dev->print("\n]");
  dev->setHeader("Content-Type", "application/javascript");
}

void PiiFileSystemUriHandler::getDirectory(const QString& path, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller)
{
  if (!d->strIndexFile.isEmpty())
    {
      QString strFile = path + d->strIndexFile;
      // If index file is set and it doesn't exist, fail.
      if (!QFileInfo(strFile).exists())
        PII_THROW_HTTP_ERROR(ForbiddenStatus);
      getFile(strFile, dev, controller);
      return;
    }

  if (dev->requestMethod() not_member_of<QString> ("GET", "HEAD"))
    PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);

  QDir::Filters filters = QDir::AllEntries | QDir::Readable | QDir::NoDotAndDotDot;
  if (!d->bFollowSymLinks)
    filters |= QDir::NoSymLinks;
  if (d->bShowHiddenFiles)
    filters |= QDir::Hidden;
  QFileInfoList lstFiles = QDir(path).entryInfoList(filters, QDir::Name | QDir::DirsFirst | QDir::LocaleAware);

  PiiStreamBuffer* pBuffer = new PiiStreamBuffer;
  dev->startOutputFiltering(pBuffer);

  static const QStringList lstFormats = QStringList() << "text" << "html" << "json";
  int iFormat = lstFormats.indexOf(dev->queryValue("format").toString());
  if (iFormat == -1)
    iFormat = 0;

  switch (iFormat)
    {
    case TextFormat:
      listDirectoryAsText(dev, lstFiles);
      break;
    case HtmlFormat:
      listDirectoryAsHtml(dev, lstFiles);
      break;
    case JsonFormat:
      listDirectoryAsJson(dev, lstFiles);
    }

  // Discard message body, but retain the Content-Length header.
  if (dev->requestMethod() == "HEAD")
    {
      dev->setHeader("Content-Length", pBuffer->size());
      dev->setStatus(PiiHttpProtocol::NoContentStatus);
      pBuffer->clear();
    }
}

void PiiFileSystemUriHandler::handleRequest(const QString& uri, PiiHttpDevice* dev, PiiHttpProtocol::TimeLimiter* controller)
{
  QString strMethod = dev->requestMethod();
  if (!d->lstAllowedMethods.contains(strMethod))
    PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);

  // The request path relative to my root URI (no leading slash).
  QString strFileName(dev->requestPath(uri));

  if (strFileName.isEmpty())
    {
      getDirectory(d->strFileSystemRoot, dev, controller);
      return;
    }

  QStringList lstPaths = strFileName.split('/', QString::SkipEmptyParts);
  strFileName = d->strFileSystemRoot;

  for (QStringList::iterator i=lstPaths.begin(); i!=lstPaths.end(); )
    {
      if (*i == "..")
        {
          if (i == lstPaths.begin())
            // Disallow escaping from the file system root.
            PII_THROW_HTTP_ERROR(NotFoundStatus);
          i = lstPaths.erase(i-1, i+1);
        }
      else
        ++i;
    }

  // Stretching the standard here... If the request has a
  // Create-Parents header, we'll create intermediate collections even
  // though the WebDAV standard forbids doing so.
  if (dev->requestHeader().hasKey("Create-Parents") &&
      dev->requestMethod() == "MKCOL")
    {
      createPath(d->strFileSystemRoot + lstPaths.join("/"), dev);
      return;
    }

  QStringList::const_iterator i = lstPaths.constBegin();
  while (i != lstPaths.constEnd())
    {
      strFileName += '/';
      strFileName += *i;
      QFileInfo info(strFileName);
      // Symlinks are followed only if explicitly allowed.
      if (!d->bFollowSymLinks && info.isSymLink())
        PII_THROW_HTTP_ERROR(NotFoundStatus);

      if (i != lstPaths.constEnd()-1)
        {
          // All except the last path component must be directories.
          if (!info.isReadable() || !info.isDir())
            {
              if (strMethod == "MKCOL") // WebDAV spec: missing intermediate collections are "conflict"
                PII_THROW_HTTP_ERROR(ConflictStatus);
              else
                PII_THROW_HTTP_ERROR(NotFoundStatus);
            }
        }
      else // Last path component
        {
          if (info.isFile()) // existing file
            {
              if (strMethod member_of<QString> ("GET", "HEAD"))
                getFile(strFileName, dev, controller);
              else if (strMethod == "PUT")
                putFile(strFileName, dev, controller);
              else if (strMethod == "DELETE")
                deleteFile(strFileName, dev);
              else
                PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);
            }
          else if (info.isDir()) // existing directory
            {
              if (strMethod member_of<QString> ("GET", "HEAD"))
                getDirectory(strFileName + '/', dev, controller);
              else if (strMethod == "MKCOL")
                createDirectory(strFileName, dev);
              else if (strMethod == "DELETE")
                deleteDirectory(strFileName, dev);
              else
                PII_THROW_HTTP_ERROR(MethodNotAllowedStatus);
            }
          else if (!info.exists()) // non-existing file/directory
            {
              if (strMethod == "PUT")
                putFile(strFileName, dev, controller);
              else if (strMethod == "MKCOL")
                createDirectory(strFileName, dev);
              else
                PII_THROW_HTTP_ERROR(NotFoundStatus);
            }
          return;
        }
      ++i;
    }
}

void PiiFileSystemUriHandler::setFollowSymLinks(bool followSymLinks) { d->bFollowSymLinks = followSymLinks; }
bool PiiFileSystemUriHandler::followSymLinks() const { return d->bFollowSymLinks; }

void PiiFileSystemUriHandler::setMimeTypeMap(PiiMimeTypeMap* map)
{
  if (map != 0)
    d->pMimeTypeMap = map;
  else
    d->pMimeTypeMap = &d->defaultMimeTypeMap;
}

PiiMimeTypeMap* PiiFileSystemUriHandler::mimeTypeMap() const { return d->pMimeTypeMap; }

void PiiFileSystemUriHandler::setIndexFile(const QString& indexFile) { d->strIndexFile = indexFile; }
QString PiiFileSystemUriHandler::indexFile() const { return d->strIndexFile; }

void PiiFileSystemUriHandler::setFileSystemRoot(const QString& fileSystemRoot)
{
  d->strFileSystemRoot = fileSystemRoot;
  d->ensureTrainingSlash();
}
QString PiiFileSystemUriHandler::fileSystemRoot() const { return d->strFileSystemRoot; }

void PiiFileSystemUriHandler::setDefaultDirectoryListFormat(DirectoryListFormat defaultDirectoryListFormat)
{ d->defaultDirectoryListFormat = defaultDirectoryListFormat; }
PiiFileSystemUriHandler::DirectoryListFormat PiiFileSystemUriHandler::defaultDirectoryListFormat() const
{ return d->defaultDirectoryListFormat; }

void PiiFileSystemUriHandler::setShowHiddenFiles(bool showHiddenFiles) { d->bShowHiddenFiles = showHiddenFiles; }
bool PiiFileSystemUriHandler::showHiddenFiles() const { return d->bShowHiddenFiles; }

void PiiFileSystemUriHandler::setLockFiles(bool lockFiles) { d->bLockFiles = lockFiles; }
bool PiiFileSystemUriHandler::lockFiles() const { return d->bLockFiles; }

void PiiFileSystemUriHandler::setAllowedMethods(const QStringList& allowedMethods) { d->lstAllowedMethods = allowedMethods; }
QStringList PiiFileSystemUriHandler::allowedMethods() const { return d->lstAllowedMethods; }
