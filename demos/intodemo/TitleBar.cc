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

#include <TitleBar.h>

TitleBar::TitleBar(QWidget *parent)
{
  setupUi(this);
  
  // Use the style to set the button pixmaps
  _pCloseButton->setIcon(style()->standardPixmap(QStyle::SP_TitleBarCloseButton));
  _pMinimizeButton->setIcon(style()->standardPixmap(QStyle::SP_TitleBarMinButton));

  _pCloseButton->setForegroundRole(QPalette::NoRole);
  
  _maxPixmap = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);
  _restorePixmap = style()->standardPixmap(QStyle::SP_TitleBarNormalButton);
  
  _pMaximizeButton->setIcon(_maxPixmap);

  _pTitleLabel->setText("Into demos");
  parent->setWindowTitle("Into demos");
        
  _bMaxNormal = false;
  
  connect(_pCloseButton, SIGNAL(clicked()), parent, SLOT(close()));
  connect(_pMinimizeButton, SIGNAL(clicked()), this, SLOT(showSmall()));
  connect(_pMaximizeButton, SIGNAL(clicked()), this, SLOT(showMaxRestore()));
}

void TitleBar::showSmall()
{
  parentWidget()->showMinimized();
}
    
void TitleBar::showMaxRestore()
{
  if (_bMaxNormal)
    {
      parentWidget()->showNormal();
      _bMaxNormal = !_bMaxNormal;
      _pMaximizeButton->setIcon(_maxPixmap);
    }
  else
    {
      parentWidget()->showMaximized();
      _bMaxNormal = !_bMaxNormal;
      _pMaximizeButton->setIcon(_restorePixmap);
    }
}

void TitleBar::mousePressEvent(QMouseEvent *me)
{
  _startPos = me->globalPos();
  _clickPos = mapToParent(me->pos());
}

void TitleBar::mouseMoveEvent(QMouseEvent *me)
{
  if (_bMaxNormal)
    return;
  parentWidget()->move(me->globalPos() - _clickPos);
}

