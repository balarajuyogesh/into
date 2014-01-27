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
#include <QPen>
#include <QColor>

MainWindow::MainWindow(QWidget *parent) :
  DemoMainWindow(parent),
  _pResultProbeInput(0),
  _pImageFileReader(0),
  _pThresholder(0)
{
  // Initialize engine
  initEngine();

  // Initialize ui
  init();

  startProcessing();
  // Show the first selected image
  emit selectImage(1);
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

  connect(_pResultProbeInput, SIGNAL(objectReceived(PiiVariant)), _pResultImageDisplay, SLOT(setImage(PiiVariant)));

  _pThresholdSlider->setValue(_pThresholder->property("threshold").toInt());
  connect(_pThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(changeThreshold(int)));
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine *pEngine = new PiiEngine;

  // Create trigger sources
  PiiOperation *pReadImageTriggerSource = pEngine->createOperation("PiiTriggerSource", "readImageTriggerSource");
  connect(this, SIGNAL(selectImage(int)), pReadImageTriggerSource, SLOT(trigger(int)));

  PiiOperation *pUpdateImageTriggerSource = pEngine->createOperation("PiiTriggerSource", "updateImageTriggerSource");
  connect(this, SIGNAL(updateImage(int)), pUpdateImageTriggerSource, SLOT(trigger(int)));

  PiiOperation *pSwitch = pEngine->createOperation("PiiSwitch", "switch");
  pSwitch->setProperty("operationMode", "AsynchronousMode");
  pSwitch->setProperty("passThrough", true);

  _pImageFileReader = pEngine->createOperation("PiiImageFileReader");
  _pImageFileReader->setProperty("fileNamePattern", QString("%1/*.jpg").arg(defaultImageDirPath()));
  PiiOperation *pConversionSwitch = pEngine->createOperation("PiiImageConversionSwitch");
  PiiOperation *pAnnotator = pEngine->createOperation("PiiImageAnnotator");
  QColor clr(Qt::red); // MSVC fails without this unnecessary temporary
  QPen boundaryPen(clr);
  pAnnotator->setProperty("pen", boundaryPen);
  PiiOperation *pBoundaryFinder = pEngine->createOperation("PiiBoundaryFinderOperation");
  _pThresholder = pEngine->createOperation("PiiThresholdingOperation");
  _pThresholder->setProperty("inverse", true);
  _pThresholder->setProperty("thresholdType", "RelativeToMeanAdaptiveThreshold");
  // Create probe input for result image display
  _pResultProbeInput = new PiiProbeInput;

  // Connect operations
  pReadImageTriggerSource->connectOutput("trigger", _pImageFileReader, "trigger");
  _pImageFileReader->connectOutput("image", pSwitch, "input");
  pSwitch->connectOutput("output", pConversionSwitch, "image");
  pUpdateImageTriggerSource->connectOutput("trigger", pSwitch, "trigger");
  pConversionSwitch->connectOutput("gray image", _pThresholder, "image");
  pConversionSwitch->connectOutput("color image", pAnnotator, "image");
  _pThresholder->connectOutput("image", pBoundaryFinder, "image");
  pBoundaryFinder->connectOutput("boundaries", pAnnotator, "annotation");

  // Connect probe input to the annotated image output
  _pResultProbeInput->connectOutput(pAnnotator->output("image"));

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

void MainWindow::changeThreshold(int threshold)
{
  showSliderValue(_pThresholdSlider, QString::number(threshold));

  // Change threshold
  _pThresholder->setProperty("absoluteThreshold", threshold);

  // Select the same image again
  emit updateImage(0);
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
