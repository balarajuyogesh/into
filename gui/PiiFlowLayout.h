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

#ifndef _PIIFLOWLAYOUT_H
#define _PIIFLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>
#include <PiiGui.h>
#include <QStyle>

class PII_GUI_EXPORT PiiFlowLayout : public QLayout
{
  Q_OBJECT

public:
  PiiFlowLayout(QWidget *parent = 0, int margin = -1, int hSpacing = -1, int vSpacing = -1);
  ~PiiFlowLayout();

  bool move(int from, int to);
  void addItem(QLayoutItem *item);
  int count() const;
  QLayoutItem *itemAt(int index) const;
  QLayoutItem *takeAt(int index);

  Qt::Orientations expandingDirections() const;
  void setGeometry(const QRect &rect);
  QSize sizeHint() const;

  int horizontalSpacing() const;
  int verticalSpacing() const;
  bool hasHeightForWidth() const;
  int heightForWidth(int) const;
  QSize minimumSize() const;

private:
  int doLayout(const QRect &rect, bool testOnly) const;
  int smartSpacing(QStyle::PixelMetric pm) const;

  QList<QLayoutItem*> _lstItems;
  int _iHorizontalSpace;
  int _iVerticalSpace;
};

#endif //_PIIFLOWLAYOUT_H
