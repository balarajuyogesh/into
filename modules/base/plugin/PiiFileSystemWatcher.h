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

#ifndef _PIIFILESYSTEMWATCHER_H
#define _PIIFILESYSTEMWATCHER_H

#include <PiiDefaultOperation.h>

#include <QFileSystemWatcher>
#include <QDateTime>

/**
 * An operation for monitoring directories for new files. If the file
 * is added or modified in directory that are being watched, its
 * absolute file name will be emitted.
 *
 * Outputs
 * -------
 *
 * @out filename - the absolute path of a file that was modified or
 * added to a watched directory.
 *
 */
class PiiFileSystemWatcher : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The list of watched directories.
   */
  Q_PROPERTY(QStringList directories READ directories WRITE setDirectories);

  /**
   * Sets the name filters used in watching new files. Each name
   * filter is a wildcard (globbing) filter that understands * and ?
   * wildcards. The operation will only report files whose names match
   * the one of the filters. If the filter list is empty, all changed
   * files will be reported. An example:
   *
   * ~~~(c++)
   * QStringList lstNameFilters = QStringList() << "*.jpg" << "*.png";
   * pFileSystemWatcher->setProperty("nameFilters", lstNameFilters);
   * ~~~
   */
  Q_PROPERTY(QStringList nameFilters READ nameFilters WRITE setNameFilters);

  /**
   * Delay the emission of a file name for this many seconds. If
   * `watchDelay` is non-zero, added and modified files will not be
   * reported until they remain unchanged at least `watchDelay`
   * seconds. This can be used delay the handling of a large file
   * until it is completely stored into the file system. The default
   * value is zero.
   */
  Q_PROPERTY(int watchDelay READ watchDelay WRITE setWatchDelay);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiFileSystemWatcher();

  void setDirectories(const QStringList& directories);
  QStringList directories() const;
  void setNameFilters(const QStringList& nameFilters);
  QStringList nameFilters() const;
  void setWatchDelay(int watchDelay);
  int watchDelay() const;

  void check(bool reset);

protected:
  void process();

private slots:
  /**
   * This slot is called when the directory at a specified path is
   * modified.
   */
  void directoryChanged(const QString& path);
  void emitNotModifiedFileNames();

private:
  void emitAllFileNames();
  int indexOf(const QStringList& paths, const QString& path);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    // List of watched dirs and their last check times
    QStringList lstDirectories;
    QList<QDateTime> lstPreviousCheckTimes;

    QFileSystemWatcher fileSystemWatcher;
    QStringList lstNameFilters;
    int iWatchDelay;

    // List of modified files pending emission
    QStringList lstModifiedFiles;
    // Lists of modified files found during the last check, per folder.
    QList<QStringList> lstLastModifiedFiles;
    QList<QDateTime> lstModifiedTimes;
  };
  PII_D_FUNC;
};


#endif //_PIIFILESYSTEMWATCHER_H
