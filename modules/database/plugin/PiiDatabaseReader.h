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

#ifndef _PIIDATABASEREADER_H
#define _PIIDATABASEREADER_H

#include "PiiDatabaseOperation.h"
#include <QSqlDatabase>

class QFile;
class QSqlQuery;

/**
 * PiiDatabaseReader description
 *
 * Inputs
 * ------
 *
 * @in name - Description
 *
 * Outputs
 * -------
 *
 * @out outputX - X ranges from 0 to the number of column names - 1.
 * The outputs can also be retrieved with the column name using the
 * [output()] function. The type of data emitted through the output
 * depends on the type of the database column. With CSV input, the
 * type is always QString unless explicitly changed with the
 * [defaultValues] property.
 *
 */
class PiiDatabaseReader : public PiiDatabaseOperation
{
  Q_OBJECT

  /**
   * The name of the table data will be read from. The table
   * description must match the column names set with [columnNames].
   */
  Q_PROPERTY(QString tableName READ tableName WRITE setTableName);

  /**
   * Database column names. When this property is set, an equivalent
   * number of output sockets will be created. The values read from
   * the columns of the database table will be sent to the
   * corresponding output sockets.
   *
   * To create two outputs, do this:
   *
   * ~~~(c++)
   * reader->setProperty("columnNames", QStringList() << "filename" << "objects");
   * ~~~
   */
  Q_PROPERTY(QStringList columnNames READ columnNames WRITE setColumnNames);

  /**
   * A map that speficies default values for some columns. These
   * columns are allowed to have NULL values in the database. For CSV
   * input, the `defaultValues` map will also be used to determine
   * the type of the data field.
   *
   * ~~~(c++)
   * QVariantMap values;
   * values["filename"] = "N/A";
   * values["objects"] = 0;
   * reader->setProperty("defaultValues", values);
   * ~~~
   */
  Q_PROPERTY(QVariantMap defaultValues READ defaultValues WRITE setDefaultValues);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiDatabaseReader();

protected:
  void process();
  void aboutToChangeState(State state);

  QSqlDatabase* createDatabase(const QString& driver,
                               const QString& user,
                               const QString& password,
                               const QString& host,
                               int port,
                               const QString& database);

  PiiOutputSocket* output(const QString& name) const;

  QString tableName() const;
  void setTableName(const QString& tableName);

  QStringList columnNames() const;
  void setColumnNames(const QStringList& columnNames);

  QVariantMap defaultValues() const;
  void setDefaultValues(const QVariantMap& defaultValues);

private:
  /// @internal
  class Data : public PiiDatabaseOperation::Data
  {
  public:
    Data();
    ~Data();

    QStringList lstColumnNames;
    QVariantMap mapDefaultValues;
    QString strTableName;
    QSqlQuery *pQuery;
    QFile *pFile;
    QVector<PiiVariant> vecDefaultValues;
  };
  PII_D_FUNC;

  void initializeDefaults();
  void createQuery();
};

#endif //_PIIDATABASEREADER_H
