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

#include "PiiFlowLayout.h"
#include <QWidget>

PiiFlowLayout::PiiFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
  : QLayout(parent), _iHorizontalSpace(hSpacing), _iVerticalSpace(vSpacing)
{
  setContentsMargins(margin, margin, margin, margin);
}

PiiFlowLayout::~PiiFlowLayout()
{
}

bool PiiFlowLayout::move(int from, int to)
{
  if (from >= 0 && from < _lstItems.size() &&
      to >= 0 && to < _lstItems.size())
    {
      _lstItems.insert(to, _lstItems.takeAt(from));
      invalidate();
      return true;
    }
  
  return false;
}

void PiiFlowLayout::addItem(QLayoutItem *item)
{
  _lstItems.append(item);
}

int PiiFlowLayout::horizontalSpacing() const
{
  if (_iHorizontalSpace >= 0)
    return _iHorizontalSpace;
  else
    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int PiiFlowLayout::verticalSpacing() const
{
  if (_iVerticalSpace >= 0)
    return _iVerticalSpace;
  else
    return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

int PiiFlowLayout::count() const
{
  return _lstItems.size();
}

QLayoutItem *PiiFlowLayout::itemAt(int index) const
{
  return _lstItems.value(index);
}

QLayoutItem *PiiFlowLayout::takeAt(int index)
{
  if (index >= 0 && index < _lstItems.size())
    return _lstItems.takeAt(index);
  else
    return 0;
}

Qt::Orientations PiiFlowLayout::expandingDirections() const
{
  return 0;
}

bool PiiFlowLayout::hasHeightForWidth() const
{
  return true;
}

int PiiFlowLayout::heightForWidth(int width) const
{
  int height = doLayout(QRect(0, 0, width, 0), true);
  return height;
}

void PiiFlowLayout::setGeometry(const QRect &rect)
{
  QLayout::setGeometry(rect);
  doLayout(rect, false);
}

QSize PiiFlowLayout::sizeHint() const
{
  return minimumSize();
}

QSize PiiFlowLayout::minimumSize() const
{
  QSize size;
  QLayoutItem *item;
  foreach (item, _lstItems)
    size = size.expandedTo(item->minimumSize());

  size += QSize(2*margin(), 2*margin());
  return size;
}

int PiiFlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
  int left, top, right, bottom;
  getContentsMargins(&left, &top, &right, &bottom);
  QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
  int x = effectiveRect.x();
  int y = effectiveRect.y();
  int lineHeight = 0;

  QLayoutItem *item;
  foreach (item, _lstItems)
    {
      QWidget *wid = item->widget();
      int spaceX = horizontalSpacing();
      if (spaceX == -1)
        spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton,
                                             QSizePolicy::PushButton,
                                             Qt::Horizontal);
      int spaceY = verticalSpacing();
      if (spaceY == -1)
        spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton,
                                             QSizePolicy::PushButton,
                                             Qt::Vertical);
      int nextX = x + item->sizeHint().width() + spaceX;
      if (nextX - spaceX > effectiveRect.right() && lineHeight > 0)
        {
          x = effectiveRect.x();
          y = y + lineHeight + spaceY;
          nextX = x + item->sizeHint().width() + spaceX;
          lineHeight = 0;
        }

      if (!testOnly)
        item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
      
      x = nextX;
      lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
  return y + lineHeight - rect.y() + bottom;
}

int PiiFlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
  QObject *parent = this->parent();
  if (!parent)
    return -1;
  else if (parent->isWidgetType())
    {
      QWidget *pw = static_cast<QWidget *>(parent);
      return pw->style()->pixelMetric(pm, 0, pw);
    }
  else
    return static_cast<QLayout *>(parent)->spacing();
}
