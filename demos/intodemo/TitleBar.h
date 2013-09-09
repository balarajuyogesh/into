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

#ifndef _TITLEBAR_H
#define _TITLEBAR_H

#include <QApplication>
#include <QtGui>
#include <QLayout>
#include <QStyle>
#include <ContentWidget.h>
#include <ui_titlebar.h>

class TitleBar : public QWidget, private Ui_TitleBar
{
  Q_OBJECT
  
public:
  TitleBar(QWidget *parent);

public slots:
  void showSmall();
  void showMaxRestore();
  
protected:
  void mousePressEvent(QMouseEvent *me);
  void mouseMoveEvent(QMouseEvent *me);
  
private:
  QPixmap _restorePixmap, _maxPixmap;
  bool _bMaxNormal;
  QPoint _startPos;
  QPoint _clickPos;
};

#endif //_TITLEBAR_H
