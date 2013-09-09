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

#include "PiiTableModelControlWidget.h"

#include <QPushButton>
#include <QCoreApplication>
#include "PiiTableModel.h"

QString PiiTableModelControlWidget::Data::tr(const char* text)
{
  return QCoreApplication::translate("PiiTableModelControlWidget::Data", text);
}

PiiTableModelControlWidget::Data::Data(PiiTableModelControlWidget* parent) :
  pbtnAdd(new QPushButton(tr("Add"))),
  pbtnDelete(new QPushButton(tr("Delete"))),
  pbtnMoveUp(new QPushButton(tr("Move up"))),
  pbtnMoveDown(new QPushButton(tr("Move down")))
{
  QBoxLayout* pLayout = new QBoxLayout(QBoxLayout::TopToBottom, parent);
  pLayout->addWidget(pbtnAdd);
  pLayout->addWidget(pbtnDelete);
  pLayout->addWidget(pbtnMoveUp);
  pLayout->addWidget(pbtnMoveDown);
  pLayout->setContentsMargins(0,0,0,0);
}

PiiTableModelControlWidget::PiiTableModelControlWidget(QWidget* parent) :
  QWidget(parent),
  d(new Data(this))
{
}

PiiTableModelControlWidget::~PiiTableModelControlWidget()
{
  delete d;
}

void PiiTableModelControlWidget::setDirection(QBoxLayout::Direction direction)
{
  static_cast<QBoxLayout*>(layout())->setDirection(direction);
}

QBoxLayout::Direction PiiTableModelControlWidget::direction() const
{
  return static_cast<QBoxLayout*>(layout())->direction();
}

void PiiTableModelControlWidget::setMoveButtonsVisible(bool moveButtonsVisible)
{
  d->pbtnMoveUp->setVisible(moveButtonsVisible);
  d->pbtnMoveDown->setVisible(moveButtonsVisible);
}

bool PiiTableModelControlWidget::moveButtonsVisible() const
{
  return d->pbtnMoveUp->isHidden();
}

void PiiTableModelControlWidget::connect(PiiTableModel* model)
{
  QObject::connect(d->pbtnAdd, SIGNAL(clicked()), model, SLOT(addRow()));
  QObject::connect(d->pbtnDelete, SIGNAL(clicked()), model, SLOT(deleteSelectedRows()));
  QObject::connect(d->pbtnMoveUp, SIGNAL(clicked()), model, SLOT(moveSelectedRowsUp()));
  QObject::connect(d->pbtnMoveDown, SIGNAL(clicked()), model, SLOT(moveSelectedRowsDown()));

  QObject::connect(model, SIGNAL(deleteEnabled(bool)), d->pbtnDelete, SLOT(setEnabled(bool)));
  QObject::connect(model, SIGNAL(moveUpEnabled(bool)), d->pbtnMoveUp, SLOT(setEnabled(bool)));
  QObject::connect(model, SIGNAL(moveDownEnabled(bool)), d->pbtnMoveDown, SLOT(setEnabled(bool)));

  d->pbtnMoveUp->setEnabled(model->canMoveUp());
  d->pbtnMoveDown->setEnabled(model->canMoveDown());
  d->pbtnDelete->setEnabled(model->canDelete());
}
