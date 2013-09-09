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
  _pSourceProbeInput(0),
  _pResultProbeInput(0),
  _pMatrixFlipper(0)
{
  // Initialize engine
  initEngine();

  // Initialize ui
  init();

  // Start processing
  startProcessing();
  emit selectImage(1);
}

void MainWindow::init()
{
  setupUi(this);
  setWindowTitle(demoName());

  // Make ui-connections
  connect(_pFlipModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setFlipMode(int)));

  connect(_pSourceProbeInput, SIGNAL(objectReceived(PiiVariant)), _pSourceImageDisplay, SLOT(setImage(PiiVariant)));
  connect(_pResultProbeInput, SIGNAL(objectReceived(PiiVariant)), _pResultImageDisplay, SLOT(setImage(PiiVariant)));
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine* pEngine = new PiiEngine;

  // Create trigger source
  PiiOperation *pTriggerSource = pEngine->createOperation("PiiTriggerSource");

  // Create probe input for source and result image display
  _pSourceProbeInput = new PiiProbeInput;
  _pResultProbeInput = new PiiProbeInput;
  
  // Create image source
  PiiOperation *pImageFileReader = pEngine->createOperation("PiiImageFileReader");
  pImageFileReader->setProperty("fileNames", QString("%1/olga.jpg").arg(defaultImageDirPath()));

  // Create matrix flipper
  _pMatrixFlipper = pEngine->createOperation("PiiMatrixFlipper");
  _pMatrixFlipper->setProperty("flipMode", "FlipHorizontally");
  
  // Make operation connections
  pTriggerSource->connectOutput("trigger", pImageFileReader, "trigger");
  pImageFileReader->connectOutput("image", _pMatrixFlipper, "input");
  
  _pSourceProbeInput->connectOutput(pImageFileReader->output("image"));
  _pResultProbeInput->connectOutput(_pMatrixFlipper->output("output"));
  
  connect(this, SIGNAL(selectImage(int)), pTriggerSource, SLOT(trigger(int)));

  return pEngine;
}

void MainWindow::setFlipMode(int mode)
{
  // Change camera distance
  _pMatrixFlipper->setProperty("flipMode",mode);

  // Trig the image
  emit selectImage(0);
}

