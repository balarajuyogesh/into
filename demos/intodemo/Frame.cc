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

#include <Frame.h>

Frame::Frame()
{
  setupUi(this);
  
  _bMouseDown = false;
  setFrameShape(Panel);
        
  // Make this a borderless window which can't
  // be resized or moved via the window system
  setWindowFlags(Qt::FramelessWindowHint);
  setMouseTracking(true);

  connect(_pContent, SIGNAL(quit()), this, SLOT(close()));
}
    
// Allows you to access the content area of the frame
// where widgets and layouts can be added
QWidget* Frame::contentWidget() const
{
  return _pContent;
}
    
void Frame::mousePressEvent(QMouseEvent *e)
{
  _oldPos = e->pos();
  _bMouseDown = e->button() == Qt::LeftButton;
}
    
void Frame::mouseMoveEvent(QMouseEvent *e)
{
  int x = e->x();
  int y = e->y();
        
  if (_bMouseDown)
    {
      int dx = x - _oldPos.x();
      int dy = y - _oldPos.y();
      
      QRect g = geometry();
      
      if (_bLeft)
        g.setLeft(g.left() + dx);
      if (_bRight)
        g.setRight(g.right() + dx);
      if (_bBottom)
        g.setBottom(g.bottom() + dy);
      
      setGeometry(g);
      
      _oldPos = QPoint(!_bLeft ? e->x() : _oldPos.x(), e->y());
    }
  else
    {
      QRect r = rect();
      _bLeft = qAbs(x - r.left()) <= 5;
      _bRight = qAbs(x - r.right()) <= 5;
      _bBottom = qAbs(y - r.bottom()) <= 5;
      bool hor = _bLeft | _bRight;
      
      if (hor && _bBottom)
        {
          if (_bLeft)
            setCursor(Qt::SizeBDiagCursor);
          else 
            setCursor(Qt::SizeFDiagCursor);
        }
      else if (hor)
        {
          setCursor(Qt::SizeHorCursor);
        }
      else if (_bBottom)
        {
          setCursor(Qt::SizeVerCursor);
        }
      else
        {
          setCursor(Qt::ArrowCursor);
        }
    }
}

void Frame::leaveEvent(QEvent *e)
{
  setCursor(Qt::ArrowCursor);
}

void Frame::mouseReleaseEvent(QMouseEvent *e)
{
  _bMouseDown = false;
}


