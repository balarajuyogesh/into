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

#ifndef _PIIDATABASE_H
#define _PIIDATABASE_H

#include "PiiDatabaseGlobal.h"
#include <QString>

namespace PiiDatabase
{
  /**
   * Create a database connection to `databaseUri`.
   *
   * @param databaseUri the URI of the database. The general syntax is
   * `driver:`//user:password\@host:port/database. E.g. 
   * `psql:`//me:secret\@localhost/mybase
   *
   * @param connectionName the name of the connection. The database
   * can be later retrieved with QSqlDatabase::database() with the
   * given name. If this parameter is omitted, the new database
   * connection becomes the default connection.
   *
   * ~~~
   * PiiDatabase::createDb("mysql://localhost/test", "myConnection");
   * QSqlDatabase db = QSqlDatabase::database("myConnection");
   * if (db.isValid())
   *   doSomething();
   * ~~~
   */
  bool PII_DATABASE_EXPORT createDb(const QString& databaseUri, const QString& connectionName = "");
}

#endif //_PIIDATABASE_H
