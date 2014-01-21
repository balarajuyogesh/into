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

#ifndef _PIITABLEMODELCONTROLWIDGET_H
#define _PIITABLEMODELCONTROLWIDGET_H

#include "PiiGui.h"
#include <QWidget>
#include <QBoxLayout>

class PiiTableModel;
class QPushButton;

/**
 * A widget with Add, Delete, Move up, and Move down buttons for
 * controlling a PiiTableModel.
 *
 *
 */
class PII_GUI_EXPORT PiiTableModelControlWidget : public QWidget
{
  Q_OBJECT

  /**
   * Layout direction. By default, control buttons are laid out
   * vertically, from top to bottom.
   */
  Q_PROPERTY(QBoxLayout::Direction direction READ direction WRITE setDirection);

  /**
   * Show/hide move buttons. The buttons are show by default. If you
   * do not want the user to be able to move items in a PiiTableModel,
   * set this flag to `false`.
   */
  Q_PROPERTY(bool moveButtonsVisible READ moveButtonsVisible WRITE setMoveButtonsVisible);

public:
  /**
   * Create a new PiiTableModelControlWidget with the given parent
   * widget.
   */
  PiiTableModelControlWidget(QWidget* parent = 0);
  /**
   * Destroy the widget.
   */
  ~PiiTableModelControlWidget();

  /**
   * Connect the control widget to the given `model`. The Delete and
   * Move buttons of a connected widget will be automatically enabled
   * and disabled. Pushing the buttons will modify the model.
   */
  void connect(PiiTableModel* model);

  void setDirection(QBoxLayout::Direction direction);
  QBoxLayout::Direction direction() const;

  void setMoveButtonsVisible(bool moveButtonsVisible);
  bool moveButtonsVisible() const;

private:
  struct Data
  {
    Data(PiiTableModelControlWidget* parent);
    static QString tr(const char* text);

    QPushButton* pbtnAdd;
    QPushButton* pbtnDelete;
    QPushButton* pbtnMoveUp;
    QPushButton* pbtnMoveDown;
  } *d;
};

#endif //_PIITABLEMODELCONTROLWIDGET_H
