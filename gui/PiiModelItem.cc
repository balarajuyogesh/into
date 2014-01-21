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

#include "PiiModelItem.h"

#include <QVariant>

PiiModelItem::Data::Data() :
  _flags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled)
{
}

PiiModelItem::Data::~Data()
{
}

PiiModelItem::PiiModelItem(const QString& text) :
  d(new Data)
{
  setText(text);
}

PiiModelItem::PiiModelItem(Data* data, const QString& text) :
  d(data)
{
  setText(text);
}

PiiModelItem::~PiiModelItem()
{
  delete d;
}

void PiiModelItem::setText(const QString& text)
{
  d->_mapData[Qt::DisplayRole] = text;
}

QString PiiModelItem::text() const
{
  return d->_mapData[Qt::DisplayRole].toString();
}

void PiiModelItem::setIcon(const QIcon& icon)
{
  d->_mapData[Qt::DecorationRole] = icon;
}

QIcon PiiModelItem::icon() const
{
  return d->_mapData[Qt::DecorationRole].value<QIcon>();
}

void PiiModelItem::setFlags(const Qt::ItemFlags& flags)
{
  d->_flags = flags;
}

Qt::ItemFlags PiiModelItem::flags() const
{
  return d->_flags;
}

void PiiModelItem::setData(int role, const QVariant& value)
{
  d->_mapData[role] = value;
}

QVariant PiiModelItem::data(int role) const
{
  return d->_mapData[role];
}

QMap<int,QVariant> PiiModelItem::dataMap() const
{
  return d->_mapData;
}
