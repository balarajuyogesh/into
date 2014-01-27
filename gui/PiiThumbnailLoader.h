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

#ifndef _PIITHUMBNAILLOADER_H
#define _PIITHUMBNAILLOADER_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QStringList>

#include "PiiGui.h"

class PII_GUI_EXPORT PiiThumbnailLoader : public QThread
{
  Q_OBJECT

public:
  PiiThumbnailLoader(QObject *parent = 0);

  void run();

  /**
   * Start the loading thread.
   */
  void startLoading();

  /**
   * Stop the loading thread.
   */
  void stopLoading();

  /**
   * Get the list of the file names which are waiting list.
   */
  QStringList fileNames() const;

  /**
   * Set the given file names for the loading. If the thread is not
   * running, we will start it automatically.
   */
  void setFileNames(const QStringList& fileNames);

  /**
   * Add the given file name to the loading list. If the thread is not
   * running, we will start it automatically.
   */
  void addFileName(const QString& fileName);

signals:
  /**
   * This signal was emitted just after we have created the thumbnail
   * by the file name.
   */
  void thumbnailReady(const QString& fileName, const QImage& image);

private:
  bool _bRunning;
  QMutex _loadingMutex;
  QStringList _lstFileNames;
};

#endif //_PIITHUMBNAILLOADER_H
