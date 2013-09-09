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

#include "PiiImageListModelItem.h"
#include <QFileInfo>

PiiImageListModelItem::Data* PiiImageListModelItem::Data::sharedNull()
{
  static PiiImageListModelItem::Data nullData;
  return &nullData;
}

PiiImageListModelItem::Data::Data(const QString& fileName) :
  strFileName(fileName)
{
  QFileInfo info(fileName);
  strTooltipText = info.filePath();
  strDisplayName = info.fileName();
}

PiiImageListModelItem::PiiImageListModelItem() :
  d(Data::sharedNull())
{
  d->reserve();
}

PiiImageListModelItem::PiiImageListModelItem(const PiiImageListModelItem& other) :
  d(other.d)
{
  d->reserve();
}

PiiImageListModelItem::PiiImageListModelItem(const QString& fileName) :
  d(new Data(fileName))
{
  setIcon(QIcon(QPixmap(":/icons/defaultIcon.png").scaled(70,90,Qt::KeepAspectRatio)));
}

PiiImageListModelItem::~PiiImageListModelItem()
{
  d->release();
}

PiiImageListModelItem& PiiImageListModelItem::operator= (const PiiImageListModelItem& other)
{
  other.d->assignTo(d);
  return *this;
}


QVariantMap PiiImageListModelItem::dataMap() const
{
  QVariantMap data;
  data["fileName"] = fileName();
  data["displayName"] = displayName();
  data["tooltipText"] = tooltipText();

  return data;
}

QString PiiImageListModelItem::fileName() const
{
  return d->strFileName;
}

QString PiiImageListModelItem::displayName() const { return d->strDisplayName; }
void PiiImageListModelItem::setDisplayName(const QString& displayName)
{
  d = d->detach();
  d->strDisplayName = displayName;
}

QString PiiImageListModelItem::tooltipText() const { return d->strTooltipText; }
void PiiImageListModelItem::setTooltipText(const QString& text)
{
  d = d->detach();
  d->strTooltipText = text;
}

QIcon PiiImageListModelItem::icon() const { return d->icon; }
void PiiImageListModelItem::setIcon(const QIcon& icon)
{
  d = d->detach();
  d->icon = icon;
}

QBrush PiiImageListModelItem::background() const { return d->backgroundBrush; }
void PiiImageListModelItem::setBackground(const QBrush& background)
{
  d = d->detach();
  d->backgroundBrush = background;
}

void PiiImageListModelItem::setData(int role, const QVariant& data)
{
  int iIndex = indexOf(role);
  if (iIndex == -1)
    {
      if (data.isValid())
        d->lstData << qMakePair(role,data);
    }
  else if (data.isValid())
    d->lstData[iIndex].second = data;
  else
    d->lstData.removeAt(iIndex);
}

QVariant PiiImageListModelItem::data(int role) const
{
  for (int i=0; i<d->lstData.size(); ++i)
    if (d->lstData[i].first == role)
      return d->lstData[i].second;
  return QVariant();
}

int PiiImageListModelItem::indexOf(int role) const
{
  for (int i=0; i<d->lstData.size(); ++i)
    if (d->lstData[i].first == role)
      return i;
  return -1;
}
