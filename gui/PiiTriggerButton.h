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

#ifndef _PIITRIGGERBUTTON_H
#define _PIITRIGGERBUTTON_H

#include <QWidget>
#include "PiiGui.h"

class QToolButton;

/**
 * A pair of arrow buttons. This widget can be used to trigger, for
 * example, PiiImageFileReader and PiiVideoReader.
 *
 */
class PII_GUI_EXPORT PiiTriggerButton : public QWidget
{
  Q_OBJECT

  /**
   * Determines the buttons to display. The default `Left` |
   * `Right`, which means that both arrow buttons will be shown.
   */
  Q_PROPERTY(Buttons buttons READ buttons WRITE setButtons);
  Q_FLAGS(Buttons);

public:
  /**
   * Buttons to show.
   *
   * - `Left` - show left arrow button
   * - `Right` - show right arrow button
   */
  enum Button { Left = 1, Right = 2 };
  Q_DECLARE_FLAGS(Buttons, Button);

  PiiTriggerButton(Buttons buttons = QFlags<Button>(Left) | Right);
  ~PiiTriggerButton();

  void setButtons(Buttons buttons);
  Buttons buttons() const;

signals:
  /**
   * Emitted when either of the buttons is pressed. The *direction*
   * parameter tells which one: -1 means back and 1 means forward.
   */
  void triggered(int direction);

private:
  class Data
  {
  public:
    Data(Buttons buttons);
    Buttons buttons;
    QToolButton *pLeft, *pRight;
  } *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PiiTriggerButton::Buttons);

#endif //_PIITRIGGERBUTTON_H
