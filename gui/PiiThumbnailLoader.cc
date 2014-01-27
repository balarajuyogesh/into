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

#include "PiiThumbnailLoader.h"
#include <PiiQImage.h>

PiiThumbnailLoader::PiiThumbnailLoader(QObject *parent) : QThread(parent), _bRunning(false)
{
}

void PiiThumbnailLoader::run()
{
  while (_bRunning)
    {
      _loadingMutex.lock();
      if (_lstFileNames.isEmpty())
        {
          _bRunning = false;
          _loadingMutex.unlock();
          break;
        }
      QString strFileName = _lstFileNames.takeFirst();
      _loadingMutex.unlock();

      QImage image(strFileName);
      if (image.format() == QImage::Format_ARGB32)
        Pii::setQImageFormat(&image, QImage::Format_RGB32);
      emit thumbnailReady(strFileName, image.scaled(70,90,Qt::KeepAspectRatio));

      _loadingMutex.lock();
      if (_lstFileNames.isEmpty())
        _bRunning = false;
      _loadingMutex.unlock();
    }
}

void PiiThumbnailLoader::setFileNames(const QStringList& fileNames)
{
  _loadingMutex.lock();
  _lstFileNames = fileNames;
  _loadingMutex.unlock();

  if (!_bRunning)
    startLoading();
}

void PiiThumbnailLoader::addFileName(const QString& fileName)
{
  _loadingMutex.lock();
  _lstFileNames << fileName;
  _loadingMutex.unlock();

  if (!_bRunning)
    startLoading();
}

void PiiThumbnailLoader::startLoading()
{
  _bRunning = true;
  start();
}

void PiiThumbnailLoader::stopLoading()
{
  _bRunning = false;
}

