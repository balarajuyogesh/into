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
#include <PiiMatrix.h>
#include <PiiVariant.h>
#include <PiiYdinTypes.h>
#include <PiiMath.h>


MainWindow::MainWindow(QWidget *parent) :
  DemoMainWindow(parent)
{
  // Initialize ui
  init();
}

void MainWindow::updateValues()
{
  PiiMatrix<unsigned char> fractalMatrix;

  _fractalGenerator.setMinimum(_pMinimum->value());
  _fractalGenerator.setMaximum(_pMaximum->value());
  _fractalGenerator.setRoughnessScale(_pRoughnessScale->value());
  
  fractalMatrix = _fractalGenerator.generateSquareFractal<unsigned char>(Pii::pow(2,_pFractalSize->currentIndex()+1),
                                                                         _pLeftTop->value(),
                                                                         _pRightTop->value(),
                                                                         _pLeftBottom->value(),
                                                                         _pRightBottom->value(),
                                                                         _pRoughness->value());

  emit imageChanged(PiiVariant(fractalMatrix));
}

void MainWindow::init()
{
  setupUi(this);
  setWindowTitle(demoName());

  // Make ui-connections
  connect(_pFractalSize, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValues()));

  connect(_pMinimum, SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
  connect(_pMaximum, SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
  connect(_pRoughnessScale, SIGNAL(valueChanged(double)), this, SLOT(updateValues()));
  connect(_pRoughness, SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
  
  connect(_pLeftTop, SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
  connect(_pRightTop, SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
  connect(_pLeftBottom, SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
  connect(_pRightBottom, SIGNAL(valueChanged(int)), this, SLOT(updateValues()));

  connect(this, SIGNAL(imageChanged(PiiVariant)), _pImageDisplay, SLOT(setImage(PiiVariant)));
  
  // Init button states
  updateValues();
}
