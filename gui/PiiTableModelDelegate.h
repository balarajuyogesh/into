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

#ifndef _PIITABLEMODELDELEGATE_H
#define _PIITABLEMODELDELEGATE_H

#include <QItemDelegate>

#include "PiiTableModel.h"

/**
 * An implementation of the QItemDelegate interface that just passes
 * calls to a PiiTableModel.
 *
 * @internal
 *
 */
class PiiTableModelDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  /**
   * Create a new PiiTableModelDelegate and attach to the given
   * model.
   */
  PiiTableModelDelegate(PiiTableModel *parent);

  QWidget *createEditor(QWidget *parentWidget,
                        const QStyleOptionViewItem& option,
                        const QModelIndex &index) const;

  void setEditorData(QWidget *editor,
                     const QModelIndex &index) const;

  void setModelData(QWidget *editor,
                    QAbstractItemModel *model,
                    const QModelIndex &index) const;

private:
  PiiTableModel* tableModel() const { return static_cast<PiiTableModel*>(parent()); }
};

#endif //_PIITABLEMODELDELEGATE_H
