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
  _pProbeInput(0)
{
  // Initialize engine
  initEngine();

  // Initialize ui
  init();
}

void MainWindow::updateButtonStates(int state)
{
  _pStopButton->setEnabled(state == PiiOperation::Running);
  _pStartButton->setEnabled(state != PiiOperation::Running);
}

void MainWindow::init()
{
  setupUi(this);
  setWindowTitle(demoName());

  // Make ui-connections
  connect(_pImageUrlCombo, SIGNAL(activated(const QString&)), this, SLOT(urlActivated(const QString&)));
  connect(_pStartButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
  connect(_pStopButton, SIGNAL(clicked()), this, SLOT(stopProcessing()));

  _pImageDisplay->setProperty("displayType", "AutoScale");
  connect(_pProbeInput, SIGNAL(objectReceived(PiiVariant)), _pImageDisplay, SLOT(setImage(PiiVariant)));
  connect(_pFpsProbeInput, SIGNAL(objectReceived(PiiVariant)), this, SLOT(setFps(PiiVariant)));
  
  // Init button states
  updateButtonStates(engine()->state());
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine* pEngine = new PiiEngine;
  connect(pEngine, SIGNAL(stateChanged(int)), this, SLOT(updateButtonStates(int)));
  
  // Create probe input for image display
  _pProbeInput = new PiiProbeInput;
  _pFpsProbeInput = new PiiProbeInput;
  
  // Create network camera operation
  _pNetworkCameraOperation = pEngine->createOperation("PiiNetworkCameraOperation");
  _pNetworkCameraOperation->setProperty("imageType", "Color");
  _pNetworkCameraOperation->setProperty("ignoreErrors", true);

  PiiOperation *pBackgroundExtractor = pEngine->createOperation("PiiBackgroundExtractor");
  
  // Create frequency counter
  PiiOperation *pFpsCounter = pEngine->createOperation("PiiFrequencyCounter");
  pFpsCounter->setProperty("measurementFrequency", 1);
  pFpsCounter->setProperty("maxFrequency", 0);
  
  // Connect operations
  _pNetworkCameraOperation->connectOutput("image", pFpsCounter, "input0");

  pFpsCounter->connectOutput("output0", pBackgroundExtractor, "image");
  _pProbeInput->connectOutput(pBackgroundExtractor->output("image"));
  
  //_pProbeInput->connectOutput(pFpsCounter->output("output0"));
  _pFpsProbeInput->connectOutput(pFpsCounter->output("frequency"));
  
  return pEngine;
}

void MainWindow::setFps(PiiVariant fpsObject)
{
    _pFpsLabel->setText(tr("Fps: %1").arg(fpsObject.valueAs<int>()));
}

void MainWindow::startButtonClicked()
{
  // Update image url
  _pNetworkCameraOperation->setProperty("imageUrl", _pImageUrlCombo->currentText());

  // Start processing
  startProcessing();
}

void MainWindow::urlActivated(const QString& imageUrl)
{
  // Pause processing
  pauseProcessing();

  // Set image url
  _pNetworkCameraOperation->setProperty("imageUrl", imageUrl);
  
  // Restart processing
  startProcessing();
}
