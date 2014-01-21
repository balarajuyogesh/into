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

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
  DemoMainWindow(parent),
  _pResultProbeInput(0),
  _pFileSystemWatcher(0)
{
  // Intialize engine.
  initEngine();

  // Initialize ui
  init();

  // Start processing
  startProcessing();
}


void MainWindow::init()
{
  setupUi(this);
  setWindowTitle(demoName());

  // Update info text
  QStringList lstFileNames = _pFileSystemWatcher->property("directories").toStringList();
  QString strPath = lstFileNames.size() > 0 ? lstFileNames[0] : "";
  _pInfoLabel->setText(tr("When you copy the image file (jpg or png) to the directory (%1) we will show it for you.").arg(strPath));

  // Make ui-connections
  connect(_pResultProbeInput, SIGNAL(objectReceived(PiiVariant)), _pResultImageDisplay, SLOT(setImage(PiiVariant)));
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine *pEngine = new PiiEngine;

  // Create probe input for result image display
  _pResultProbeInput = new PiiProbeInput;

  // Create file system watcher
  _pFileSystemWatcher = pEngine->createOperation("PiiFileSystemWatcher");
  _pFileSystemWatcher->setProperty("nameFilters", QStringList() << "*.jpg" << "*.png");
  _pFileSystemWatcher->setProperty("directories", QStringList() << QString("%1").arg(applicationDirPath()));
  _pFileSystemWatcher->setProperty("watchDelay", 1);

  // Create image file reader
  PiiOperation *pImageFileReader = pEngine->createOperation("PiiImageFileReader");
  pImageFileReader->setProperty("imageType", "Color");

  // Make operation connections
  _pFileSystemWatcher->connectOutput("filename", pImageFileReader, "filename");
  _pResultProbeInput->connectOutput(pImageFileReader->output("image"));

  return pEngine;
}
