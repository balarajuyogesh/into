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

#ifndef _PIITABLEMODEL_H
#define _PIITABLEMODEL_H

#include "PiiGui.h"

#include <QAbstractTableModel>
#include <QAbstractItemView>
#include <QList>

class PiiTableModelDelegate;
class PiiModelItem;

/**
 * A hybrid of a table model and an item "delegate". This model can be
 * used with Qt's view classes to create editable lists and tables.
 * PiiTableModel is useful with data such as database query results
 * where all items in a table column contain similar data. The same
 * editor is used for each row, but columns may have different
 * editors.
 *
 * This class uses PiiModelItem to store item and header data. Each
 * item in the model has two associated values with different data
 * roles:
 *
 * - `Qt::DisplayRole` - the text displayed in the table cell when
 * the editor is not active.
 *
 * - `ColumnEditorValueRole` - the actual value of the cell, which
 * may be different from the text. For example, the value of an
 * element in a combo box column is the index of the selected item,
 * but the cell displays the text associated with the index. See
 * [EditorType] for data types associated with editors.
 *
 */
class PII_GUI_EXPORT PiiTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  /**
   * Custom data roles used by PiiTableModel to store information
   * about column editors and their values.
   *
   * - `ColumnDefaultValueRole` - the default value for a cell in a
   * column. Any variant. Used only in header items.
   *
   * - `ColumnEditorTypeRole` - the type of the column editor. An
   * integer. See [EditorType] for valid values. Used only in header
   * items.
   *
   * - `ColumnEditorPropertiesRole` - a QVariantMap that can be used
   * to customize the column editor's properties. Used only in header
   * items.
   *
   * - `ColumnEditorValueRole` - the current value of the item, in
   * the editor's internal representation. Any variant type. Used only
   * in table data items.
   */
  enum ColumnEditorDataRole
    {
      ColumnDefaultValueRole = Qt::UserRole + 1,
      ColumnEditorTypeRole,
      ColumnEditorPropertiesRole,
      ColumnEditorValueRole
    };

  /**
   * Supported editor types.
   *
   * - `LineEditor` - a QLineEdit. Suitable for text input with or
   * without validation. The value and text are the same. To configure
   * a validator for the editor, use the `validator` property
   * (QVariantMap).
   *
   * - `IntegerSpinBoxEditor` - a QSpinBox. Suitable for integer
   * input. The value is an integer. If the spin box has the `prefix`
   * a and `suffix` properties set, the text will be different from
   * the value.
   *
   * - `DoubleSpinBoxEditor` - a QDoubleSpinBox. Suitable for
   * double-precision value input. The value is a double. If the spin
   * box has the `prefix` a and `suffix` properties set, the text
   * will be different from the value.
   *
   * - `ComboBoxEditor` - a QComboBox. Suitable for multiple choices
   * type input. The value is an integer that stores the index of the
   * selected choice. The text is the corresponding element in the
   * combo box. To set the selectable items, use the `items` property
   * (StringList).
   *
   * @see [setColumnEditorType()]
   * @see [setColumnEditorProperties()]
   */
  enum EditorType
    {
      LineEditor,
      IntegerSpinBoxEditor,
      DoubleSpinBoxEditor,
      ComboBoxEditor
    };

  /**
   * A flag for [setValue()] that can be used to enable/disable
   * automatic setting of the displayed text of the element.
   */
  enum ValueChangeBehavior
    {
      ChangeTextAutomatically,
      DoNotChangeText
    };

  /**
   * Creates a new PiiTableModel and binds it to the `parent` view.
   * Since PiiTableModel is not a pure model, but works also as the
   * "delegate", it cannot be used with many views simultaneously.
   */
  PiiTableModel(QAbstractItemView *parent);

  /**
   * Destroys the model.
   */
  ~PiiTableModel();

  /**
   * Clears all data, excluding column header definitions.
   */
  void clear();

  /**
   * Returns the item in the given cell. If there is no such cell, 0
   * will be returned.
   */
  PiiModelItem* itemAt(int row, int column = 0) const;
  /**
   * Returns the data associated with `role` in the given cell. If
   * there is no such cell or no such data, an invalid variant will be
   * returned.
   */
  QVariant data(int row, int column, int role = Qt::DisplayRole) const;
  /**
   * Sets the data associated with `role` in the given cell.
   */
  void setData(int row, int column, const QVariant& value, int role);
  /**
   * Returns the displayed text in the given cell.
   */
  inline QString text(int row, int column) const { return data(row, column).toString(); }
  /**
   * Sets the displayed text in the given cell.
   */
  inline void setText(int row, int column, const QString& text) { setData(row, column, text, Qt::DisplayRole); }
  /**
   * Returns the data associated with `ColumnEditorValueRole` in the
   * given cell.
   */
  inline QVariant value(int row, int column) const { return data(row, column, ColumnEditorValueRole); }

  /**
   * Returns all data associated with `ColumnEditorValueRole` in the
   * given `column` as a variant list.
   */
  QVariantList columnValues(int column) const;
  /**
   * Sets all data associated with `ColumnEditorValueRole` in the
   * given `column` as a variant list. The number of rows in the
   * model will be adjusted to match the length of `values`.
   */
  void setColumnValues(int column, const QVariantList& values);

  /**
   * Returns all data associated with `Qt::DisplayRole` in the given
   * `column` as a string list.
   */
  QStringList columnTexts(int column) const;
  /**
   * Returns all data associated with `Qt::DisplayRole` in the given
   * `column` as a string list. The number of rows in the model will
   * be adjusted to match the length of `texts`.
   */
  void setColumnTexts(int column, const QStringList& texts);

  /**
   * Sets the value of the element at the given coordinates. The value
   * may be different from the displayed text. If `behavior` is
   * `ChangeTextAutomatically`, the text will be automatically set based
   * on column configuration. For example, setting the value of a
   * combo box column automatically changes the text based on the
   * combo's item list. To disable the default behavior, set
   * `behavior` `DoNotChangeText`.
   */
  void setValue(int row, int column, const QVariant& value, ValueChangeBehavior behavior = ChangeTextAutomatically);
  /**
   * Converts `value` into user-displayable text. The default
   * implementation returns different text depending on the column
   * editor.
   *
   * - LineEditor - the value will be converted to a QString
   *
   * - {Integer,Double}SpinBoxEditor - the value will be converted
   * to a QString and wrapped into the editor's `prefix` and
   * `suffix`, if set. The `specialValueText` property of the spin box
   * will be taken into account.
   *
   * - ComboBoxEditor - the value is converted to an int, and the
   * corresponding text in the combo's item list will be returned.
   */
  virtual QString textForValue(int column, const QVariant& value) const;

  /**
   * Inserts the given items as a new row to the model.
   *
   * @param items a list of items. The length of this list must equal
   * the number of columns in the model. The model takes the ownership
   * of the pointers.
   *
   * @param row insert the row after this index. -1 means last.
   */
  void insertRow(const QList<PiiModelItem*>& items, int row);
  void insertRow(const QList<PiiModelItem*>& items);

  /**
   * Removes `row` and returns its items as a list. The model no
   * longer owns the pointers, and they must be deleted by the caller.
   */
  QList<PiiModelItem*> takeRow(int row);

  /**
   * Returns the number of rows in the model.
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  /**
   * Returns the number of columns in the model.
   */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  bool setHeaderData(int section, Qt::Orientation, const QVariant& value, int role);

  /**
   * Sets the default `value` for cells in `column`.
   */
  inline void setDefaultValue(int column, const QVariant& value)
  {
    setHeaderData(column, Qt::Horizontal, value, ColumnDefaultValueRole);
  }
  /**
   * Returns the default value for cells in `column`.
   */
  inline QVariant defaultValue(int column) const
  {
    return headerData(column, Qt::Horizontal, ColumnDefaultValueRole);
  }
  /**
   * Sets the editor used to edit items in the specified column.
   */
  inline void setColumnEditorType(int column, EditorType type)
  {
    setHeaderData(column, Qt::Horizontal, static_cast<int>(type), ColumnEditorTypeRole);
  }
  /**
   * Returns the editor used to edit items in the specified column.
   */
  inline EditorType columnEditorType(int column) const
  {
    return static_cast<EditorType>(headerData(column, Qt::Horizontal, ColumnEditorTypeRole).toInt());
  }
  /**
   * Configures the column editor in the specified column. Except for
   * a few special properties recognized by PiiTableModel, the given
   * properties will be directly transferred to the editor widget.
   *
   * ~~~(c++)
   * pModel->setHeaderTitles(QStringList() << "Combo" << "IntSpin"
   *                                       << "DoubleSpin" << "Text");
   * // Combo box for column 0
   * pModel->setColumnEditorType(0, PiiTableModel::ComboBoxEditor);
   * QVariantMap map;
   * map["items"] = QStringList() << tr("Jaa") << tr("Ei");
   * // Create icons automatically from the given colors
   * map["colors"] = QVariantList() << QColor(Qt::green) << QColor(Qt::red);
   * // Use custom icons
   * map["icons"] = QVariantList() << QIcon(":yes.svg") << QIcon(":no.svg");
   * pModel->setColumnEditorProperties(0, map);
   *
   * pModel->setColumnEditorType(1, PiiTableModel::IntegerSpinBoxEditor);
   * map.clear();
   * map["minimum"] = 2;
   * map["singleStep"] = 2;
   * map["maximum"] = 24;
   * pModel->setColumnEditorProperties(1, map);
   *
   * pModel->setColumnEditorType(2, PiiTableModel::DoubleSpinBoxEditor);
   * map.clear();
   * map["minimum"] = 1.0;
   * map["maximum"] = 100.0;
   * pModel->setColumnEditorProperties(2, map);
   *
   * pModel->setColumnEditorType(3, PiiTableModel::LineEditor);
   * map.clear();
   * QVariantMap mapValidator;
   * mapValidator["regexp"] = "[1-9][0-9]{3}-[0-9]{2}-[0-9]{2}";
   * map["validator"] = mapValidator;
   * pModel->setColumnEditorProperties(3, map);
   * ~~~
   *
   * ! `LineEditor` supports regexp and numeric validators. If
   * the editor properties contains a QVariantMap named "validator",
   * its properties will be used to configure a validator. The
   * validator's type is auto-detected based on the properties. If
   * "regexp" is present, a QRegExpValidator will be used. If "top"
   * and "bottom" are present, a QIntValidator or QDoubleValidator
   * will be used, based on the type (int or double) of the limits.
   */
  inline void setColumnEditorProperties(int column, const QVariantMap& properties)
  {
    setHeaderData(column, Qt::Horizontal, properties, ColumnEditorPropertiesRole);
  }
  /**
   * Returns the properties of the editor at `column`. This function
   * returns the properties set with [setColumnEditorProperties()]
   * only. No other properties will be returned.
   */
  inline QVariantMap columnEditorProperties(int column) const
  {
    return headerData(column, Qt::Horizontal, ColumnEditorPropertiesRole).toMap();
  }

  /**
   * Sets the column header titles with one function call.
   *
   * @param headerTitles the titles. The length of this list must
   * equal the number of columns.
   */
  void setHeaderTitles(const QStringList& headerTitles);

  /**
   * Returns data associated with any role at the given model index.
   * See [PiiModelItem::_d()].
   */
  QMap<int, QVariant> itemData(const QModelIndex &index) const;

  void beginInsertRows(int row, int count);
  void endInsertRows();

  bool insertRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());

  bool insertColumns(int column, int count = 1, const QModelIndex &parent = QModelIndex());
  bool removeColumns(int column, int count = 1, const QModelIndex &parent = QModelIndex());

  Qt::ItemFlags flags(const QModelIndex &index) const;

  /**
   * Returns the indices of all rows in which at least one cell is
   * selected. The list is sorted in ascending order.
   */
  QList<int> selectedRows() const;

  /**
   * Enables/disables deletion of the last row in the model. If this
   * flag is set to `false`, [canDelete()] will return `false` if
   * there is only one row left.
   */
  void setCanDeleteLast(bool canDeleteLast);
  /**
   * Returns `true` if the last row can be deleted, `false`
   * otherwise.
   */
  bool canDeleteLast() const;

  /**
   * Returns `true` if the selected rows can be moved up, `false`
   * otherwise. Moving rows up is possible if a) at least one row is
   * selected and b) the first row is not selected.
   */
  bool canMoveUp() const;
  /**
   * Returns `true` if the selected rows can be moved down, `false`
   * otherwise. Moving rows down is possible if a) at least one row is
   * selected and b) the last row is not selected.
   */
  bool canMoveDown() const;
  /**
   * Returns `true` if the selected rows can be deleted, `false`
   * otherwise. Deleting rows down is possible if a) at least one row
   * is selected and b) either the [canDeleteLast()] flag is `true` or
   * there are more than one row left.
   */
  bool canDelete() const;

signals:
  /**
   * Signals that selection has changed on the attached view.
   */
  void selectionChanged();
  /**
   * Signals that the current item has changed on the attached view.
   */
  void currentItemChanged();
  /**
   * Emitted whenever the selection has changed. If the `enabled`
   * flag is `true`, the selected rows can be moved up.
   */
  void moveUpEnabled(bool enabled);
  /**
   * Emitted whenever the selection has changed. If the `enabled`
   * flag is `true`, the selected rows can be moved down.
   */
  void moveDownEnabled(bool enabled);
  /**
   * Emitted whenever the selection has changed. If the `enabled`
   * flag is `true`, there are selected rows.
   */
  void deleteEnabled(bool enabled);

public slots:
  /**
   * Inserts a new row at the end of the table and fills it with
   * default values.
   */
  void addRow();
  /**
   * Deletes selected rows.
   */
  void deleteSelectedRows();
  /**
   * Moves selected rows one step up. Does nothing if there are now
   * selected rows or the first row is selected.
   */
  void moveSelectedRowsUp();
  /**
   * Moves selected rows down step up. Does nothing if there are now
   * selected rows or the last row is selected.
   */
  void moveSelectedRowsDown();

private slots:
  void enableControls();

protected:
  /// @internal
  struct Data
  {
    Data(PiiTableModel* model);
    ~Data();

    int rows() const { return lstItems.size()-1; }
    int columns() const { return lstItems[0].size(); }

    PiiTableModelDelegate* pDelegate;
    QList<QList<PiiModelItem*> > lstItems;
    bool bCanDeleteLast;
    int iLastInsertRow;
    int iInsertCount;
  } *d;

  /**
   * Creates a new item at the given coordinates. The default
   * implementation sets the item's value (`ColumnEditorValueRole`)
   * to the default value of the column, if given.
   */
  virtual PiiModelItem* createItem(int row, int column);
  /**
   * Creates an editor for editing the item at the given coordinates.
   * The default implementation creates one of the supported editor
   * types, based on header data. If you override this function, you
   * probably need to override [setEditorData()], [setModelData()], and
   * [textForValue()] as well.
   */
  virtual QWidget* createEditor(QWidget* parent, int row, int column) const;
  /**
   * Sets up the contents of `editor` based on the contents of the
   * cell at the given position.
   */
  virtual void setEditorData(QWidget* editor, int row, int column) const;
  /**
   * Modifies the cell at the given position based on the contents of
   * `editor`.
   */
  virtual void setModelData(QWidget* editor, int row, int column);

  /// @internal
  PiiTableModel(Data* data, QAbstractItemView *parent);

private:
  friend class PiiTableModelDelegate;

  QList<PiiModelItem*> createRow(int row = -1);
  inline void selectRows(const QList<int>& rows);
  void selectRow(int row);
  inline QItemSelectionModel* selectionModel() const { return static_cast<QAbstractItemView*>(QObject::parent())->selectionModel(); }
  void initialize(QAbstractItemView* parent);
};


#endif //_PIITABLEMODEL_H
