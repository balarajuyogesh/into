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

#include <QApplication>
#include <Frame.h>
#include <QRect>
#include <QDesktopWidget>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  Frame box;

  //init view
  QRect screenRect = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
  QRect windowRect(0, 0, 800, 600);
  if (screenRect.width() < 800)
    windowRect.setWidth(screenRect.width());
  if (screenRect.height() < 600)
    windowRect.setHeight(screenRect.height());
  windowRect.moveCenter(screenRect.center());
  box.setGeometry(windowRect);
  box.setMinimumSize(400, 300);
  box.setWindowTitle("Into demos");

  box.show();

  return app.exec();
}

