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

#ifndef _FRAME_H
#define _FRAME_H

#include <QApplication>
#include <QtGui>
#include <QLayout>
#include <QStyle>
#include <TitleBar.h>
#include <ContentWidget.h>
#include <ui_frame.h>

class Frame : public QFrame, private Ui_Frame
{
public:

  Frame();

  // Allows you to access the content area of the frame
  // where widgets and layouts can be added
  QWidget *contentWidget() const;
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void leaveEvent(QEvent *e);

private:
  QPoint _oldPos;
  bool _bMouseDown;
  bool _bLeft, _bRight, _bBottom;
};


#endif //_FRAME_H
