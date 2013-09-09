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

#ifndef _PIIIMAGELISTMODEL_H
#define _PIIIMAGELISTMODEL_H

#include "PiiGui.h"

#include <QAbstractListModel>
#include <PiiImageListModelItem.h>
#include <PiiThumbnailLoader.h>

class PII_GUI_EXPORT PiiImageListModel : public QAbstractListModel
{
  Q_OBJECT
  
public:
  PiiImageListModel(QObject *parent = 0);
  ~PiiImageListModel();
  
  /**
   * Returns the number of rows under the given parent.
   */
  int rowCount(const QModelIndex& parent = QModelIndex()) const;

  /**
   * Returns the data stored under the given @a role for the item
   * referred to by @a index.
   */
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

  /**
   * Sets the @a data stored under the given @a role for the item
   * referred to by @p index.
   */
  void setData(const QModelIndex& index, int role, const QVariant& data);

  /// @overload
  void setData(const QString& fileName, int role, const QVariant& data);
  
  /**
   * Returns a map with values for all predefined roles in the model
   * for the item at the given index. There is also role Qt::UserRole
   * where is the dataMap from the PiiImageListModelItem.
   */
  QMap<int, QVariant> itemData(const QModelIndex& index) const;

  /**
   * Inserts count rows starting with the given row. Returns true if
   * the rows were succesfully inserted, other return false. If row is
   * 0, the rows are prepended. If row is rowCount(), the rows are
   * appended.
   */
  bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
  
  /**
   * Removes count rows starting with the given row. Returns true if
   * the rows were succesfully removed, other return false.
   */
  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

  /**
   * Set a list of the file names. All old items will be removed.
   */
  void setFileNames(const QStringList& fileNames);

  /**
   * Returns all file names as a list.
   */
  QStringList fileNames() const;

  /**
   * Returns the data associated with @a role for each item in the
   * model.
   */
  QVariantList dataList(int role) const;

  /**
   * Set a list of the items. All old items will be removed.
   */
  void setItemList(const QList<PiiImageListModelItem>& itemList);

  /**
   * Append one item to the list with given fileName.
   */
  void appendItem(const QString& fileName);

  using QAbstractListModel::index;
  /**
   * Returns a model index associated with @a fileName. If no such
   * file name exists, returns an invalid model index.
   */
  QModelIndex index(const QString& fileName) const;

public slots:
  /**
   * Update an icon to the item by the given fileName.
   */
  void updateThumbnail(const QString& fileName, const QImage& image);

  /**
   * Clear all items from the list.
   */
  void clear();
  
private:
  class Data
  {
  public:
    QList<PiiImageListModelItem> lstItems;
    PiiThumbnailLoader thumbnailLoader;
  } *d;
};

#endif //_PIIIMAGELISTMODEL_H
