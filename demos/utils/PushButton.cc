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

#include "PushButton.h"
#include <QPainter>

PushButton::PushButton(QWidget *parent) :
  QPushButton(parent)
{
  setText("");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  _pixmapLeft = QPixmap(":left");
  _pixmapRight = QPixmap(":right");

  setIconMode(IconLeft);
}

void PushButton::paintEvent(QPaintEvent *event)
{
  QPushButton::paintEvent(event);

  QPainter p(this);
  int h = height();
  int w = width();

  int cap = 5;

  // Draw icon and text
  if (_mode == IconLeft)
    {
      p.drawPixmap(QRect(cap,cap,h-2*cap,h-2*cap), _pixmapLeft, _pixmapLeft.rect());
      p.drawText(QRect(0,0,w,h), Qt::AlignHCenter | Qt::AlignVCenter, _strName);
    }
  else
    {
      p.drawPixmap(QRect(w-h+cap,cap,h-2*cap,h-2*cap), _pixmapRight, _pixmapRight.rect());
      p.drawText(QRect(0,0,w,h), Qt::AlignHCenter | Qt::AlignVCenter, _strName);
    }
  p.end();
}

void PushButton::setIconMode(IconMode mode)
{
  setText("");

  _mode = mode;
  _strName = mode == IconLeft ? tr("Previous image") : tr("Next image");

  // Set minimum width
  QFontMetrics m(font());
  setMinimumWidth(m.width(_strName) + 2*height());

  repaint();
}

