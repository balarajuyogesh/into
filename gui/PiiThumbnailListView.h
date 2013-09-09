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

#ifndef _PIITHUMBNAILLISTVIEW_H
#define _PIITHUMBNAILLISTVIEW_H

#include <QListView>
#include <QPoint>
#include <QMouseEvent>
#include "PiiImageListModel.h"

class PII_GUI_EXPORT PiiThumbnailListView : public QListView
{
  Q_OBJECT
  
public:
  PiiThumbnailListView(QWidget *parent = 0);
  ~PiiThumbnailListView();
  
  /**
   * Sets the model for the view present.
   */
  void setModel(PiiImageListModel *model);

  /**
   * Return the current selected fileName of the thumbnail. If there
   * is no any item selected, return empty string.
   */
  QString currentThumbnail() const;
  
signals:
  void removeThumbnail(const QModelIndex&);
  void thumbnailActivated(const QString&);
  
protected:
  void mousePressEvent(QMouseEvent *e);

private slots:
  void removeCurrent();
  void itemSelected(const QModelIndex& index);
  
private:
  void showMenu(const QPoint& point);

  class Data
  {
  public:
    PiiImageListModel *pModel;
  } *d;
};

#endif //_PIITHUMBNAILLISTVIEW_H
