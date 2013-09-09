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
  _pSourceProbeInput(0),
  _pApproximationProbeInput(0),
  _pHorizontalProbeInput(0),
  _pVerticalProbeInput(0),
  _pDiagonalProbeInput(0),
  _pImageFileReader(0)
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

  _pNextImageButton->setIconMode(PushButton::IconRight);

  _pSourceImageDisplay->imageViewport()->setProperty("fitMode", "FitToView");
  _pApproximationDisplay->imageViewport()->setProperty("fitMode", "FitToView");
  _pHorizontalDisplay->imageViewport()->setProperty("fitMode", "FitToView");
  _pVerticalDisplay->imageViewport()->setProperty("fitMode", "FitToView");
  _pDiagonalDisplay->imageViewport()->setProperty("fitMode", "FitToView");

  // Make ui-connections
  connect(_pPreviousImageButton, SIGNAL(clicked()), this, SLOT(prevButtonClicked()));
  connect(_pNextImageButton, SIGNAL(clicked()), this, SLOT(nextButtonClicked()));
  connect(_pActionSelectImages, SIGNAL(triggered()), this, SLOT(selectImages()));

  connect(_pSourceProbeInput, SIGNAL(objectReceived(PiiVariant)), _pSourceImageDisplay, SLOT(setImage(PiiVariant)));
  connect(_pApproximationProbeInput, SIGNAL(objectReceived(PiiVariant)),_pApproximationDisplay, SLOT(setImage(PiiVariant)));
  connect(_pHorizontalProbeInput, SIGNAL(objectReceived(PiiVariant)), _pHorizontalDisplay, SLOT(setImage(PiiVariant)));
  connect(_pVerticalProbeInput, SIGNAL(objectReceived(PiiVariant)), _pVerticalDisplay, SLOT(setImage(PiiVariant)));
  connect(_pDiagonalProbeInput, SIGNAL(objectReceived(PiiVariant)), _pDiagonalDisplay, SLOT(setImage(PiiVariant)));

  _pApproximationDisplay->setProperty("displayType", "AutoScale");
  _pHorizontalDisplay->setProperty("displayType", "AutoScale");
  _pVerticalDisplay->setProperty("displayType", "AutoScale");
  _pDiagonalDisplay->setProperty("displayType", "AutoScale");
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine *pEngine = new PiiEngine;

  // Create trigger source
  PiiOperation *pTriggerSource = pEngine->createOperation("PiiTriggerSource");

  // Create probe inputs for source and result image display
  _pSourceProbeInput = new PiiProbeInput;
  _pApproximationProbeInput = new PiiProbeInput;
  _pHorizontalProbeInput = new PiiProbeInput;
  _pVerticalProbeInput = new PiiProbeInput;
  _pDiagonalProbeInput = new PiiProbeInput;
  
  // Create image source
  _pImageFileReader = pEngine->createOperation("PiiImageFileReader");
  _pImageFileReader->setProperty("imageType", "GrayScale");
  _pImageFileReader->setProperty("fileNamePattern", QString("%1/*.jpg").arg(defaultImageDirPath()));

  // Create wavelet operation
  PiiOperation *pWaveletTransform = pEngine->createOperation("PiiDwtOperation");
  
  // Make operation connections
  connect(this, SIGNAL(selectImage(int)), pTriggerSource, SLOT(trigger(int)));

  pTriggerSource->connectOutput("trigger", _pImageFileReader, "trigger");
  _pImageFileReader->connectOutput("image", pWaveletTransform, "input");
  
  _pSourceProbeInput->connectOutput(_pImageFileReader->output("image"));
  _pApproximationProbeInput->connectOutput(pWaveletTransform->output("approximation"));
  _pHorizontalProbeInput->connectOutput(pWaveletTransform->output("horizontal"));
  _pVerticalProbeInput->connectOutput(pWaveletTransform->output("vertical"));
  _pDiagonalProbeInput->connectOutput(pWaveletTransform->output("diagonal"));

  return pEngine;
}

void MainWindow::prevButtonClicked()
{
  emit selectImage(-1);
}

void MainWindow::nextButtonClicked()
{
  emit selectImage(1);
}

void MainWindow::selectImages()
{
  QStringList lstFileNames = getImageFiles();

  if (lstFileNames.size() > 0)
    {
      // Pause processing when setting new images
      pauseProcessing();

      // Set new images
      _pImageFileReader->setProperty("fileNames", lstFileNames);

      // Restart engine
      startProcessing();

      // Trig the image
      emit selectImage(1);
    }
}
