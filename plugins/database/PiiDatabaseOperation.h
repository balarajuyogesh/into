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

#ifndef _PIIDATABASEOPERATION_H
#define _PIIDATABASEOPERATION_H

#include <PiiDefaultOperation.h>
#include "PiiDatabaseGlobal.h"

class QSqlDatabase;
class QSqlQuery;
class QSqlDriver;

/**
 * An abstract superclass for operations that read/write databases. 
 * This class provides functionality for maintaining the database
 * connection.
 *
 * ! Due to limitations of the low-level SQL drivers, database
 * queries must be always made from the thread that initiated the
 * driver. Therefore, PiiDatabaseOperation disallows changing its
 * [processing mode](PiiDefaultOperation::processingMode).
 *
 */
class PII_DATABASE_EXPORT PiiDatabaseOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Database URI specified the details necessary for connecting to a
   * database. The format is
   * scheme://[user:password@]host[:port][/database]. Bracketed parts
   * are optional. "scheme" tells the database type. Valid values are:
   *
   * - `db2` - IBM DB2, v7.1 and higher
   * - `ibase` - Borland InterBase
   * - `mysql` - MySQL
   * - `oci` - Oracle
   * - `odbc` - ODBC (includes Microsoft SQL Server)
   * - `psql` - PostgreSQL 7.3 and above
   * - `sqlite` - SQLite version 3 or above
   * - `sqlite2` - SQLite version 2
   * - `tds` - Sybase Adaptive Server
   * - `qt` - use a connection created by
   * QSqlDatabase::addDatabase(). If the database name is empty, the
   * default database connection will be used.
   * - `null` - no output (default value)
   *
   * Some examples:
   *
   * - `mysql://user:password@10.10.10.2/dumpdb`
   * - `null://`
   * - `qt://main`
   *
   * @see [databaseName]
   */
  Q_PROPERTY(QString databaseUri READ databaseUri WRITE setDatabaseUri);

  /**
   * The name of the database. Database naming depends on the database
   * engine. For Oracle, use the TNS service name. For ODBC, use
   * either a DSN, a DSN file name (must end with .dsn), or a
   * connection string.
   *
   * ~~~(c++)
   * PiiOperation* pDb = engine.createOperation("PiiDatabaseWriter");
   *
   * // Open a Microsoft Access database
   * pDb->setProperty("databaseUri", "odbc://");
   * pDb->setProperty("databaseName",
   *                  "Driver={Microsoft Access Driver (*.mdb)};"
   *                  "Dbq=accessfile.mdb;"
   *                  "Uid=Admin;Pwd=;");.
   *
   * // Append to or read from a CSV file
   * pDb->setProperty("databaseUri", "csv://");
   * pDb->setProperty("databaseName", "outputfile.csv");
   * ~~~
   *
   * The database name can also be omitted. In such a case you can set
   * it directly with `databaseUri`.
   */
  Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName);

  /**
   * By default, all errors cause a run-time exception and stop the
   * operation. Setting this flag to `true` suppresses the exceptions
   * and allows the operation to continue running. A log message will
   * be generated instead.
   */
  Q_PROPERTY(bool ignoreErrors READ ignoreErrors WRITE setIgnoreErrors);

  /**
   * The number of times to retry a failed query before giving up. If
   * [ignoreErrors] is `false`, the operation will stop after this
   * many unsuccessful retry attempts. Default is 0. Only
   * connection-related errors (not SQL errors) may cause a retrial.
   */
  Q_PROPERTY(int retryCount READ retryCount WRITE setRetryCount);

  /**
   * The number of milliseconds to wait after a failed query. Default
   * is 50.
   */
  Q_PROPERTY(int retryDelay READ retryDelay WRITE setRetryDelay);

public:
  PiiDatabaseOperation();
  ~PiiDatabaseOperation();

protected:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    
    QString strConnectionId;
    QString strDatabaseUri, strDatabaseName;

    QSqlDatabase* pDb;

    int iRetryDelay;
    int iRetryCount;
    bool bConnected, bIgnoreErrors;
  };
  PII_D_FUNC;

  /// @internal
  PiiDatabaseOperation(Data* d);

  QString databaseUri() const;
  void setDatabaseUri(const QString& databaseUri);

  QString databaseName() const;
  void setDatabaseName(const QString& databaseName);

  bool ignoreErrors() const;
  void setIgnoreErrors(bool ignoreErrors);

  int retryDelay() const;
  void setRetryDelay(int retryDelay);

  int retryCount() const;
  void setRetryCount(int retryCount);

  /**
   * Opens a database connection to the given [databaseUri]. This
   * function must be called from the process() function to ensure the
   * database is always accessed from the same thread. This is a
   * limitation of the low-level SQL drivers.
   *
   * @return `true` if the connection was successfully opened, 
   * `false` otherwise.
   *
   * @exception PiiExecutionException& if the connection cannot be
   * opened and [ignoreErrors] is `false`.
   */
  bool openConnection();
  
  /**
   * Closes the database connection.
   */
  void closeConnection();

  /**
   * Returns a pointer to the internal database connection, or a null
   * pointer if no connection is open.
   */
  QSqlDatabase* db();

  /**
   * Shorthand for db()->driver().
   */
  QSqlDriver* driver();

  /**
   * Returns `true` if the database connection is open and `false`
   * otherwise.
   */
  bool isConnected() const;

  /**
   * Returns a pointer to a newly allocated QSqlDatabase object. 
   * Subclasses may override this function to provide new connection
   * schemes and to perform additional initialization.
   */
  virtual QSqlDatabase* createDatabase(const QString& driver,
                                       const QString& user,
                                       const QString& password,
                                       const QString& host,
                                       int port,
                                       const QString& database);

  /**
   * Throws a PiiExecutionException with the given *message*, if
   * [ignoreErrors] is `false`. Otherwise, writes *message* to log.
   */
  void error(const QString& message);

  /**
   * Executes *query*. If the query fails with a connection-related
   * error, retries [retryCount] times.
   */
  bool exec(QSqlQuery& query);
  /**
   * Checks an executed *query* for errors. Returns `true` if the
   * query was successfully executed, `false` otherwise. Throws a
   * PiiExecutionException if the query failed and [ignoreErrors] is 
   * `false`.
   */
  bool checkQuery(QSqlQuery& query);
  /**
   * Closes the database connection if *state* is `Stopped`.
   */
  void aboutToChangeState(State state);

private:
  int defaultPort(const QString& driver);
  void init();
  static int _iConnectionIndex;
};


#endif //_PIIDATABASEOPERATION_H

