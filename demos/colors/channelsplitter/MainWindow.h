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

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <DemoMainWindow.h>
#include "ui_mainwindow.h"

#include <PiiEngine.h>
#include <PiiOperation.h>
#include <PiiProbeInput.h>

class MainWindow : public DemoMainWindow, private Ui::MainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);

protected:
  QStringList requiredPlugins() const { return QStringList() << "piiimage" << "piicolors" << "piibase"; }
  PiiEngine* createEngine();

signals:
  void selectImage(int);

private slots:
  void selectImages();
  void nextButtonClicked();
  void prevButtonClicked();

private:
  void init();

  PiiProbeInput *_pSourceProbeInput, *_pRedProbeInput, *_pGreenProbeInput, *_pBlueProbeInput;
  PiiOperation *_pImageFileReader;
};



#endif //_MAINWINDOW_H
