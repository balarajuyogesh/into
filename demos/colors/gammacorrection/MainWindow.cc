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
  _pResultProbeInput(0),
  _pImageFileReader(0)
{
  // Initialize engine
  initEngine();

  // Initialize ui
  init();

  // Start processing
  startProcessing();
  emit selectImage(1);
  emit updateImage(0);
}

void MainWindow::init()
{
  setupUi(this);
  setWindowTitle(demoName());

  _pNextImageButton->setIconMode(PushButton::IconRight);

  _pSourceImageDisplay->imageViewport()->setProperty("fitMode", "FitToView");
  _pResultImageDisplay->imageViewport()->setProperty("fitMode", "FitToView");
  
  // Make ui-connections
  connect(_pPreviousImageButton, SIGNAL(clicked()), this, SLOT(prevButtonClicked()));
  connect(_pNextImageButton, SIGNAL(clicked()), this, SLOT(nextButtonClicked()));
  connect(_pActionSelectImages, SIGNAL(triggered()), this, SLOT(selectImages()));
  connect(_pGammaSlider, SIGNAL(valueChanged(int)), this, SLOT(setGamma(int)));

  connect(_pSourceProbeInput, SIGNAL(objectReceived(PiiVariant)), _pSourceImageDisplay, SLOT(setImage(PiiVariant)));
  connect(_pResultProbeInput, SIGNAL(objectReceived(PiiVariant)), _pResultImageDisplay, SLOT(setImage(PiiVariant)));

  setGamma(_pGammaSlider->value());
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine* pEngine = new PiiEngine;

  // Create trigger source
  PiiOperation *pImageTrigger = pEngine->createOperation("PiiTriggerSource", "imageTrigger");

  // Create probe input for source and result image display
  _pSourceProbeInput = new PiiProbeInput;
  _pResultProbeInput = new PiiProbeInput;
  
  // Create image source
  _pImageFileReader = pEngine->createOperation("PiiImageFileReader");
  _pImageFileReader->setProperty("imageType", "Color");
  _pImageFileReader->setProperty("fileNamePattern", QString("%1/*.jpg").arg(defaultImageDirPath()));

  // Create trigger source and switch
  PiiOperation *pSwitchTrigger = pEngine->createOperation("PiiTriggerSource", "switchTrigger");
  _pSwitch = pEngine->createOperation("PiiSwitch", "switch");
  _pSwitch->setProperty("operationMode", "AsynchronousMode");
  _pSwitch->setProperty("dynamicInputCount", 1);
  _pSwitch->setProperty("passThrough", true);
  
  // Create color converter
  _pColorConverter = pEngine->createOperation("PiiColorConverter");
  _pColorConverter->setProperty("colorConversion", "GammaCorrection");
  
  // Make operation connections
  connect(this, SIGNAL(selectImage(int)), pImageTrigger, SLOT(trigger(int)));
  connect(this, SIGNAL(updateImage(int)), pSwitchTrigger, SLOT(trigger(int)));
  
  pImageTrigger->connectOutput("trigger", _pImageFileReader, "trigger");
  pSwitchTrigger->connectOutput("trigger", _pSwitch, "trigger");
  _pImageFileReader->connectOutput("image", _pSwitch, "input0");
  _pSwitch->connectOutput("output0", _pColorConverter, "image");
  
  _pSourceProbeInput->connectOutput(_pImageFileReader->output("image"));
  _pResultProbeInput->connectOutput(_pColorConverter->output("image"));

  return pEngine;
}

void MainWindow::setGamma(int value)
{
  static const double dMinGamma = 0.2, dMaxGamma = 5.0;

  double dValue = dMinGamma +
    double(value - _pGammaSlider->minimum()) / (_pGammaSlider->maximum() - _pGammaSlider->minimum()) *
    (dMaxGamma - dMinGamma);
  
  // Change gamma
  _pColorConverter->setProperty("gamma",dValue);

  showSliderValue(_pGammaSlider, QString::number(dValue,'f',2));

  // Trigger the same image
  _pSwitch->setProperty("passThrough", false);
  emit updateImage(0);
}

void MainWindow::prevButtonClicked()
{
  _pSwitch->setProperty("passThrough", true);
  emit selectImage(-1);
}

void MainWindow::nextButtonClicked()
{
  _pSwitch->setProperty("passThrough", true);
  emit selectImage(1);
}

void MainWindow::selectImages()
{
  QStringList lstFileNames = getImageFiles();

  if (lstFileNames.size() > 0)
    {
      // Pause processing when setting new images
      stopProcessing();

      // Set new images
      _pImageFileReader->setProperty("fileNames", lstFileNames);

      // Restart engine
      startProcessing();

      // Trig the image
      emit selectImage(1);
    }
}
