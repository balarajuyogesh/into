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

#ifndef _PIIDATABASEWRITER_H
#define _PIIDATABASEWRITER_H

#include "PiiDatabaseOperation.h"
#include <QSqlDatabase>

class QFile;
class QSqlQuery;

/**
 * An operation that writes rows into SQL databases and flat files.
 * The operation has a user-configurable number of inputs that accept
 * primitive values and strings. Whenever all inputs have an incoming
 * object, a new row is created into a database table. The incoming
 * values are written to the table based on the names of the inputs as
 * specified by the [columnNames] property.
 *
 * This operation adds "csv" as a supported connection scheme. See
 * PiiDatabaseOperation::databaseName for examples.
 *
 * Inputs
 * ------
 *
 * @in inputX - input sockets. X is a zero-based index. Inputs can
 * also be accessed with the names given by the [columnNames] property.
 *
 */
class PiiDatabaseWriter : public PiiDatabaseOperation
{
  Q_OBJECT

  /**
   * The name of the table data will be written into. The table
   * description must match the column names set with [columnNames].
   */
  Q_PROPERTY(QString tableName READ tableName WRITE setTableName);

  /**
   * Database column names. When this property is set, an equivalent
   * number of input sockets will be created. The values read from
   * these input sockets are stored into the database table to the
   * named column.
   *
   * To create two inputs, do this:
   *
   * ~~~(c++)
   * writer->setProperty("columnNames", QStringList() << "filename" << "objects");
   * ~~~
   */
  Q_PROPERTY(QStringList columnNames READ columnNames WRITE setColumnNames);

  /**
   * A map that speficies default values for some columns. The
   * corresponding inputs will be flagged optional. If they are left
   * unconnected, the default values will always be substituted.
   *
   * ~~~(c++)
   * QVariantMap values;
   * values["filename"] = "N/A";
   * values["objects"] = 0;
   * writer->setProperty("defaultValues", values);
   * ~~~
   */
  Q_PROPERTY(QVariantMap defaultValues READ defaultValues WRITE setDefaultValues);

  /**
   * Turns database writing on and off.
   */
  Q_PROPERTY(bool writeEnabled READ writeEnabled WRITE setWriteEnabled);

  /**
   * The number of decimal digits written to CSV files. The other
   * database back-ends don't treat decimal numbers as text, and are
   * not affected by rounding.
   */
  Q_PROPERTY(int decimalsShown READ decimalsShown WRITE setDecimalsShown);

  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  PiiDatabaseWriter();
  ~PiiDatabaseWriter();

  PiiInputSocket* input(const QString& name) const;

  void check(bool reset);

protected:
  void process();
  void aboutToChangeState(State state);

  QSqlDatabase* createDatabase(const QString& driver,
                               const QString& user,
                               const QString& password,
                               const QString& host,
                               int port,
                               const QString& database);

  QStringList columnNames() const;
  void setColumnNames(const QStringList& columnNames);

  QVariantMap defaultValues() const;
  void setDefaultValues(const QVariantMap& defaultValues);

  bool writeEnabled() const;
  void setWriteEnabled(bool writeEnabled);

  void setDecimalsShown(int decimalsShown);
  int decimalsShown() const;

  QString tableName() const;
  void setTableName(const QString& tableName);

private:
  void initializeDefaults();

  /// @internal
  class Data : public PiiDatabaseOperation::Data
  {
  public:
    Data();
    ~Data();

    QStringList lstColumnNames;
    QVariantMap mapDefaultValues;
    QVector<QVariant> vecDefaultValues;
    QString strTableName;
    bool bWriteEnabled;
    int iDecimalsShown;
    QSqlQuery* pQuery;
    QFile *pFile;
  };
  PII_D_FUNC;

  void createQuery();
};


#endif //_PIIDATABASEWRITER_H
