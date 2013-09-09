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

#include "PiiThumbnailListView.h"
#include <QMenu>

#include <QtDebug>

PiiThumbnailListView::PiiThumbnailListView(QWidget *parent) :
  QListView(parent),
  d(new Data)
{
  setGridSize(QSize(100,120));
  setIconSize(QSize(70,90));

  //setWrapping(true);
  setResizeMode(QListView::Adjust);
  setViewMode(QListView::IconMode);
  setFlow(QListView::TopToBottom);

  connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemSelected(const QModelIndex&)));
  connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemSelected(const QModelIndex&)));
}

PiiThumbnailListView::~PiiThumbnailListView()
{
  delete d;
}

void PiiThumbnailListView::setModel(PiiImageListModel *model)
{
  d->pModel = model;
  QListView::setModel(model);
}

void PiiThumbnailListView::mousePressEvent(QMouseEvent *e)
{
  QListView::mousePressEvent(e);
  if ( e->button() == Qt::RightButton )
    showMenu(e->globalPos());
}

void PiiThumbnailListView::showMenu(const QPoint& point)
{
  QModelIndex ci = currentIndex();
  if (ci.isValid())
    {
      
      QMenu *menu = new QMenu;
      menu->addAction(tr("Remove this thumbnail"), this, SLOT(removeCurrent()));
      menu->addSeparator();
      menu->addAction(tr("Clear"), d->pModel, SLOT(clear()));
      menu->exec(point);
      delete menu;
    }
}

void PiiThumbnailListView::removeCurrent()
{
  QModelIndex ci = currentIndex();
  if (ci.isValid())
    {
      d->pModel->removeRow(ci.row());
      int rowCount = d->pModel->rowCount(QModelIndex());
      QModelIndex ind = ci.row() < rowCount ? ci : d->pModel->index(rowCount-1,0);
      setCurrentIndex(ind);
      itemSelected(ind);
    }
}

void PiiThumbnailListView::itemSelected(const QModelIndex& index)
{
  if (index.isValid())
    {
      QMap<int, QVariant> itemData = d->pModel->itemData(index);
      emit thumbnailActivated(itemData[Qt::UserRole].toMap()["fileName"].toString());
    }
  else
    emit thumbnailActivated("");
}

QString PiiThumbnailListView::currentThumbnail() const
{
  QModelIndex ci = currentIndex();
  if (ci.isValid())
    {
      QMap<int, QVariant> itemData = d->pModel->itemData(ci);
      return itemData[Qt::UserRole].toMap()["fileName"].toString();
    }
  else
    return "";
}

