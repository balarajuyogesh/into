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
  _pImageFileReader(0),
  _pEdgeDetector(0)
{
  // Initialize engine
  initEngine();
  
  // Initialize ui
  init();
}

void MainWindow::init()
{
  setupUi(this);
  setWindowTitle(demoName());

  _pNextImageButton->setIconMode(PushButton::IconRight);

  // Make ui-connections
  connect(_pPreviousImageButton, SIGNAL(clicked()), this, SLOT(prevButtonClicked()));
  connect(_pNextImageButton, SIGNAL(clicked()), this, SLOT(nextButtonClicked()));
  connect(_pActionSelectImages, SIGNAL(triggered()), this, SLOT(selectImages()));

  connect(_pSourceProbeInput, SIGNAL(objectReceived(PiiVariant)), _pSourceImageDisplay, SLOT(setImage(PiiVariant)));
  connect(_pResultProbeInput, SIGNAL(objectReceived(PiiVariant)), _pResultImageDisplay, SLOT(setImage(PiiVariant)));

  // Make detectors and connect them
  QStringList lstDetectorNames = QStringList() << "Sobel" << "Roberts" << "Prewitt" << "Canny";
  QStringList lstDetectors = QStringList() << "SobelDetector" << "RobertsDetector" << "PrewittDetector" << "CannyDetector";
  
  for (int i=0; i<lstDetectorNames.size(); i++)
    _pEdgeDetectorCombo->addItem(lstDetectorNames[i], lstDetectors[i]);

  connect(_pEdgeDetectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(detectorChanged(int)));

  // Set AutoScale to the result image display
  _pResultImageDisplay->setProperty("displayType", "AutoScale");
  
  // Select default detector
  selectDetector("SobelDetector");
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine *pEngine = new PiiEngine;

  // Create trigger source
  PiiOperation *pTriggerSource = pEngine->createOperation("PiiTriggerSource");

  // Create probe input for source and result image display
  _pSourceProbeInput = new PiiProbeInput;
  _pResultProbeInput = new PiiProbeInput;
  
  // Create image source
  _pImageFileReader = pEngine->createOperation("PiiImageFileReader");
  _pImageFileReader->setProperty("imageType", "GrayScale");
  _pImageFileReader->setProperty("fileNamePattern", QString("%1/*.jpg").arg(defaultImageDirPath()));

  // Create edge detector operation
  _pEdgeDetector = pEngine->createOperation("PiiEdgeDetector");
  
  // Make operation connections
  connect(this, SIGNAL(selectImage(int)), pTriggerSource, SLOT(trigger(int)));

  pTriggerSource->connectOutput("trigger", _pImageFileReader, "trigger");
  _pImageFileReader->connectOutput("image", _pEdgeDetector, "image");
  
  _pSourceProbeInput->connectOutput(_pImageFileReader->output("image"));
  _pResultProbeInput->connectOutput(_pEdgeDetector->output("edges"));

  return pEngine;
}

void MainWindow::detectorChanged(int index)
{
  selectDetector(_pEdgeDetectorCombo->itemData(index, Qt::UserRole).toString());
}

void MainWindow::prevButtonClicked()
{
  emit selectImage(-1);
}

void MainWindow::nextButtonClicked()
{
  emit selectImage(1);
}

void MainWindow::selectDetector(const QString& detectorName)
{
  // Pause engine
  pauseProcessing();

  // Change detector name
  _pEdgeDetector->setProperty("detector", detectorName);
  
  // Restart engine
  startProcessing();

  // Trig the same image again
  emit selectImage(0);
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
