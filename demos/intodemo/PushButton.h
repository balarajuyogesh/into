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

#ifndef _PUSHBUTTON_H
#define _PUSHBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <QPaintEvent>

class PushButton : public QPushButton
{
  Q_OBJECT
  
public:
  PushButton(QWidget *parent = 0);
  
  /**
   * Set a pixmap.
   */
  void setPixmap(const QPixmap& pixmap);

  /**
   * Set a name of the button.
   */
  void setName(const QString& name);
  
  /**
   * Set and get a directory name.
   */
  void setDirname(const QString& dirname);
  QString dirname() const;
  
protected:
  void paintEvent(QPaintEvent *event);

private:
  QPixmap _pixmap;
  QString _strName, _strDirname;
};


#endif //_PUSHBUTTON_H
