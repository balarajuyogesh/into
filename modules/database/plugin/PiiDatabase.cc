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

#include "PiiDatabase.h"
#include <QUrl>
#include <QSqlDatabase>

namespace PiiDatabase
{
  bool createDb(const QString& databaseUri, const QString& connectionName)
  {
    QUrl dbUrl(databaseUri);
    if (!dbUrl.isValid())
      {
        piiWarning("Database URI is incorrectly formatted.");
        return false;
      }

    QString driverName = QString("Q%1").arg(dbUrl.scheme().toUpper());
    if (!QSqlDatabase::isDriverAvailable(driverName))
      {
        piiWarning("There is no database driver available for %s.", qPrintable(dbUrl.scheme()));
        return false;
      }
    // Get rid of "/"
    QString databaseName = dbUrl.path().remove(0,1);

    QSqlDatabase db = QSqlDatabase(QSqlDatabase::addDatabase(driverName, connectionName));

    if (!db.isValid())
      return false;

    db.setHostName(dbUrl.host());
    db.setDatabaseName(databaseName);
    db.setUserName(dbUrl.userName());
    db.setPassword(dbUrl.password());
    if (dbUrl.port() != -1)
      db.setPort(dbUrl.port());

    return true;
  }
}
