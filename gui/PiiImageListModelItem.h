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

#ifndef _PIIIMAGELISTMODELITEM_H
#define _PIIIMAGELISTMODELITEM_H

#include <QIcon>
#include <QBrush>
#include <QVariantMap>
#include "PiiGui.h"
#include <PiiSharedD.h>

class PII_GUI_EXPORT PiiImageListModelItem
{
public:
  PiiImageListModelItem();
  PiiImageListModelItem(const QString& fileName);
  PiiImageListModelItem(const PiiImageListModelItem& other);
  ~PiiImageListModelItem();

  PiiImageListModelItem& operator= (const PiiImageListModelItem& other);

  QVariantMap dataMap() const;
  QString fileName() const;
  QString displayName() const;
  QString tooltipText() const;
  QIcon icon() const;
  QBrush background() const;

  void setDisplayName(const QString& displayName);
  void setTooltipText(const QString& text);
  void setIcon(const QIcon& icon);
  void setBackground(const QBrush& background);

  QVariant data(int role) const;
  void setData(int role, const QVariant& data);

private:
  class Data : public PiiSharedD<Data>
  {
  public:
    Data() {}
    Data(const QString& fileName);

    QString strDisplayName, strTooltipText, strFileName;
    QIcon icon;
    QBrush backgroundBrush;
    QList<QPair<int,QVariant> > lstData;

    static Data* sharedNull();
  } *d;

  int indexOf(int role) const;
};

#endif //_PIIIMAGELISTMODELITEM_H
