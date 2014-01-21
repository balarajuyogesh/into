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

#ifndef _PIIMODELITEM_H
#define _PIIMODELITEM_H

#include "PiiGui.h"

#include <QObject>
#include <QMap>
#include <QIcon>

/**
 * A class used by PiiTableModel to store item data for table cells
 * and the header. PiiModelItem uses a map of variants to store data
 * associated with different roles. See [Qt::ItemDataRole] for
 * roles supported by most Qt view classes.
 *
 */
class PII_GUI_EXPORT PiiModelItem : public QObject
{
  Q_OBJECT

  /**
   * The text of the item. Stored as `Qt::DisplayRole` data into the
   * data map.
   */
  Q_PROPERTY(QString text READ text WRITE setText);
  /**
   * The icon of the item. Stored as `Qt::DecorationRole` data into
   * the data map.
   */
  Q_PROPERTY(QIcon icon READ icon WRITE setIcon);
  /**
   * Item flags. The default value is `Qt::ItemIsSelectable |
   * Qt::ItemIsEditable | Qt::ItemIsEnabled`.
   */
  Q_PROPERTY(Qt::ItemFlags flags READ flags WRITE setFlags);

public:
  /**
   * Create a new model item with the given user-visible text.
   */
  PiiModelItem(const QString& text = "");
  /**
   * Destroy the model item.
   */
  ~PiiModelItem();

  /**
   * Set data associated with `role` to `value`.
   *
   * ~~~(c++)
   * // Show "10 %" to the user ...
   * PiiModelItem* pItem = new PiiModelItem("10 %");
   * // ... but store the percentage as a number
   * pItem->setData(Qt::UserRole, 10);
   * ~~~
   */
  void setData(int role, const QVariant& value);
  /**
   * Get data associated with `role`.
   */
  QVariant data(int role) const;

  /**
   * Get the full data map. The data map stores data associated with
   * all roles.
   */
  QMap<int,QVariant> dataMap() const;

  /**
   * A utility function for setting the data associated with the
   * `Qt::DisplayRole`.
   */
  void setText(const QString& text);
  /**
   * A utility function for getting the data associated with the
   * `Qt::DisplayRole`.
   */
  QString text() const;
  /**
   * A utility function for setting the data associated with the
   * `Qt::DecorationRole`.
   */
  void setIcon(const QIcon& icon);
  /**
   * A utility function for getting the data associated with the
   * `Qt::DecorationRole`.
   */
  QIcon icon() const;
  /**
   * Set the item's flags.
   */
  void setFlags(const Qt::ItemFlags& flags);
  /**
   * Get the item's flags.
   */
  Qt::ItemFlags flags() const;

protected:
  /// @internal
  struct Data
  {
    Data();
    virtual ~Data();
    Qt::ItemFlags _flags;
    QMap<int,QVariant> _mapData;
  } *d;

  /// @internal
  PiiModelItem(Data* data, const QString& text = "");
};

#endif //_PIIMODELITEM_H
