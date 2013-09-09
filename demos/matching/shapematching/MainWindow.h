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

#include <PiiQImage.h>
#include <PiiImageOverlay.h>

#include <QImage>
#include <QList>
#include <QButtonGroup>

class MainWindow : public DemoMainWindow, private Ui::MainWindow
{
  Q_OBJECT
  
public:
  MainWindow(QWidget *parent = 0);

  struct Model
  {
    QString strFileName;
    QColor color;
    QList<PiiMatrix<int> > lstBoundaries;
  };
  
protected:
  QStringList requiredPlugins() const { return QStringList() << "piiimage" << "piibase" << "piimatching"; }
  PiiEngine* createEngine();

  
signals:
  void trigImage(int);
  
private slots:
  void nextButtonClicked();
  void prevButtonClicked();
  void updateModels(const PiiVariant& name, const QVariantList& information);
  void updateImage(const PiiVariant& image, const QVariantList& information);
  
private:
  void init();
  
  QList<Model> _lstModels;
  QList<PiiImageOverlay*> _lstOverlays;
  QList<QColor> _lstColors;
};



#endif //_MAINWINDOW_H
