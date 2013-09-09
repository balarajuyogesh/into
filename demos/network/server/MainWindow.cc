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
#include <PiiImageViewport.h>

MainWindow::MainWindow(QWidget *parent) :
  DemoMainWindow(parent),
  _pProbeInput(0)
{
  // Initialize engine
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

  _pImageDisplay->imageViewport()->setProperty("fitMode", "FitToView");

  _pStopButton->setIcon(QIcon(QPixmap(":stop")));
  _pStartButton->setIcon(QIcon(QPixmap(":play")));

  // Make ui-connections
  connect(_pProbeInput, SIGNAL(objectReceived(PiiVariant)), _pImageDisplay, SLOT(setImage(PiiVariant)));
  connect(_pStopButton, SIGNAL(clicked()), this, SLOT(stopProcessing()));
  connect(_pStartButton, SIGNAL(clicked()), this, SLOT(startProcessing()));
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine* pEngine = new PiiEngine;
  
  // Create probe input for image display
  _pProbeInput = new PiiProbeInput;

  // Create network input operation
  PiiOperation *pReceiver = pEngine->createOperation("PiiNetworkInputOperation", "receiver");
  pReceiver->setProperty("outputNames", QStringList() << "image");
  pReceiver->setProperty("inputNames", QStringList() << "result");
  pReceiver->setProperty("httpServer", "tcp://0.0.0.0:8080");

  // Create edge detector
  PiiOperation *pDetector = pEngine->createOperation("PiiEdgeDetector");
  
  // Connect operations
  pReceiver->connectOutput("image", pDetector, "image");
  pDetector->connectOutput("edges", pReceiver, "result");

  _pProbeInput->connectOutput(pReceiver->output("image"));

  connect(pEngine, SIGNAL(stateChanged(int)), this, SLOT(updateButtonStates(int)));

  return pEngine;
}


void MainWindow::updateButtonStates(int state)
{
  _pStartButton->setEnabled(state != PiiOperation::Running);
  _pStopButton->setEnabled(state != PiiOperation::Stopped);
}
