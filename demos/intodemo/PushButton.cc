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
#include <QFontMetrics>

PushButton::PushButton(QWidget *parent) :
  QPushButton("", parent),
  _strName(""),
  _strDirname("")
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  _pixmap = QPixmap(":/icons/folder.png");

  // Initialize height
  setFixedHeight(40);
}

void PushButton::paintEvent(QPaintEvent *event)
{
  QPushButton::paintEvent(event);

  QPainter p(this);
  int h = height();
  int w = width();

  // Draw icon and text
  p.drawPixmap(QRect(10,10,h-20,h-20), _pixmap, _pixmap.rect());
  p.drawText(QRect(h,0,w-h,h), Qt::AlignLeft | Qt::AlignVCenter, _strName);
  p.end();
}

void PushButton::setName(const QString& name)
{
  _strName = name;

  int iHeight = height();

  // Calculate font size
  QFont f = font();
  f.setPixelSize(iHeight/3*2);
  setFont(f);

  repaint();
}

void PushButton::setPixmap(const QPixmap& pixmap)
{
  _pixmap = pixmap;
  repaint();
}

void PushButton::setDirname(const QString& dirname)
{
  _strDirname = dirname;
}

QString PushButton::dirname() const
{
  return _strDirname;
}

