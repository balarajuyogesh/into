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

#include "ComboBox.h"
#include <QLineEdit>

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
}

void ComboBox::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Return)
    {
      QString text = lineEdit()->text();
      if (findText(text) < 0)
        insertItem(0, text);
      setCurrentIndex(0);
    }

  QComboBox::keyPressEvent(event);
}

