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
#include <PiiEngine.h>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  try
    {
      PiiEngine::loadPlugins(QStringList() << "piiimage" << "piimatching" << "piiflowcontrol");
    }
  catch (PiiLoadException& ex)
    {
      qCritical("%s", qPrintable(ex.message()));
      return 1;
    }

  // Create and execute main window
  MainWindow mainWindow;
  mainWindow.resize(1100,800);
  mainWindow.show();

  return app.exec();
}
