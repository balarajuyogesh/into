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
  _pImageCropper(0)
{
  // Initialize engine
  initEngine();

  // Initialize ui
  init();

  // Start processing
  startProcessing();
  emit selectImage(0);
}

void MainWindow::init()
{
  setupUi(this);
  setWindowTitle(demoName());

  _pSourceImageDisplay->imageViewport()->setProperty("fitMode", "FitToView");

  // Make ui-connections
  connect(_pMaxSectorAngle, SIGNAL(valueChanged(int)), this, SLOT(setMaxSectorAngle(int)));
  connect(_pSourceImageDisplay, SIGNAL(areaSelected(const QRect&, int)),
          this, SLOT(setCroppedArea(const QRect&, int)));

  connect(_pSourceProbeInput, SIGNAL(objectReceived(PiiVariant)), _pSourceImageDisplay, SLOT(setImage(PiiVariant)));
  connect(_pResultProbeInput, SIGNAL(objectReceived(PiiVariant)), _pResultImageDisplay, SLOT(setImage(PiiVariant)));
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
  PiiOperation *pImageFileReader = pEngine->createOperation("PiiImageFileReader");
  //pImageFileReader->setProperty("imageType", "GrayScale");
  pImageFileReader->setProperty("fileNames", QString("%1/kuppi.jpg").arg(defaultImageDirPath()));

  // Create image croppper
  _pImageCropper = pEngine->createOperation("PiiImageCropper");
  _pImageCropper->setProperty("area", QRect(0,0,100,100));

  // Create unwarp operation
  _pImageUnwarpOperation = pEngine->createOperation("PiiImageUnwarpOperation");
  //_pImageUnwarpOperation->setProperty("cameraDistance", 1000); //2000mm
  _pImageUnwarpOperation->setProperty("maxSectorAngle", 180);
  //_pImageUnwarpOperation->setProperty("focalLength", 1500);

  // Make operation connections
  connect(this, SIGNAL(selectImage(int)), pTriggerSource, SLOT(trigger(int)));

  pTriggerSource->connectOutput("trigger", pImageFileReader, "trigger");
  pImageFileReader->connectOutput("image", _pImageCropper, "image");
  _pImageCropper->connectOutput("image", _pImageUnwarpOperation, "image");

  _pSourceProbeInput->connectOutput(pImageFileReader->output("image"));
  _pResultProbeInput->connectOutput(_pImageUnwarpOperation->output("image"));

  setCroppedArea(QRect(67, 111, 209, 201), 0);

  return pEngine;
}

void MainWindow::setMaxSectorAngle(int value)
{
  _pImageUnwarpOperation->setProperty("maxSectorAngle", value);
  // Trig the image
  emit selectImage(0);
}

void MainWindow::setCroppedArea(const QRect& area, int /*modifiers*/)
{
  // Fix area x and y coordinates if necessary
  QRect fixedArea = area;
  fixedArea.setX(qMax(fixedArea.x(), 0));
  fixedArea.setY(qMax(fixedArea.y(), 0));

  // Change cropping area
  _pImageCropper->setProperty("area", fixedArea);

  // Trig the image again
  emit selectImage(0);
}
