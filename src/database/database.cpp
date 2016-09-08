/***************************************************************************************************
**
** $QTCARTO_BEGIN_LICENSE:GPL3$
**
** Copyright (C) 2016 Fabrice Salvaire
** Contact: http://www.fabrice-salvaire.fr
**
** This file is part of the QtCarto library.
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
** $QTCARTO_END_LICENSE$
**
***************************************************************************************************/

/**************************************************************************************************/

#include "database.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcDatabase::QcDatabase()
  : m_database()
{}

QcDatabase::~QcDatabase()
{
  if (m_database.isValid())
    m_database.close();
}

bool
QcDatabase::execute_query(const QString & sql_query)
{
  QSqlQuery query = new_query();
  if (!query.exec(sql_query)) {
    qWarning() << query.lastError().text();
    return false;
  } else
    return true;
}

bool
QcDatabase::execute_queries(const QStringList & sql_queries, bool commit_request)
{
  // Fixme: ok ?
  if (commit_request)
    transaction();

  QSqlQuery query = new_query();
  bool ok = true;
  for (const auto & sql_query : sql_queries)
    if (!query.exec(sql_query)) {
      ok = false;
      qWarning() << query.lastError().text();
    }

  if (commit_request)
    commit();

  return ok;
}

QString
QcDatabase::format_kwarg(const KeyValuePair & kwargs, const QString & separator)
{
  QString kwarg_string;
  int i = 0;
  // for (const auto & pair: kwargs) {
  for (const auto & field: kwargs.keys()) {
    if (i)
      kwarg_string += separator;
    kwarg_string += field + '=';
    QVariant variant = kwargs[field];
    bool is_string = variant.canConvert<QString>();
    if (is_string)
      kwarg_string += '"';
    kwarg_string += variant.toString();
    if (is_string)
      kwarg_string += '"';
    i++;
  }
  return kwarg_string;
}

QString
QcDatabase::format_simple_where(const KeyValuePair & kwargs)
{
  return format_kwarg(kwargs, QStringLiteral(" AND "));
}

QSqlQuery
QcDatabase::prepare_query(const QString & sql_query)
{
  transaction();
  QSqlQuery query = new_query();
  query.prepare(sql_query);
  return query;
}

QSqlQuery
QcDatabase::insert(const QString & table, const KeyValuePair & kwargs, bool commit_request)
{
  QSqlQuery query = new_query();
  QStringList fields = kwargs.keys();
  QString sql_query =
    QStringLiteral("INSERT INTO ") + table +
    QStringLiteral(" (") + fields.join(',') + QStringLiteral(") VALUES (");
  for (int i = 0; i < fields.size(); i++) {
    if (i > 0)
      sql_query += ',';
    sql_query += '?';
  }
  sql_query += ')';
  qInfo() << sql_query << kwargs;
  query.prepare(sql_query);

  for (const auto & value : kwargs.values())
    query.addBindValue(value);

  if (!query.exec())
    qWarning() << query.lastError().text();

  if (commit_request)
    commit();

  return query;
}

QSqlQuery
QcDatabase::select(const QString & table, const QStringList & fields, const QString & where)
{
  QSqlQuery query = new_query();
  QString sql_query = QStringLiteral("SELECT ") + fields.join(',') + QStringLiteral(" FROM ") + table;
  if (!where.isEmpty())
    sql_query += QStringLiteral(" WHERE ") + where;
  qInfo() << sql_query << fields;

  if (!query.exec(sql_query))
    qWarning() << query.lastError().text();

  return query;
}

QSqlRecord
QcDatabase::select_one(const QString & table, const QStringList & fields, const QString & where)
{
  QSqlQuery query = select(table, fields, where);
  QSqlRecord record;
  if (query.next()) {
    record = query.record();
    if (query.next())
      qWarning() << "More than one rows returned";
  } else
    qWarning() << "Any row";

  return record; // test with isEmpty()
}


QSqlQuery
QcDatabase::update(const QString & table, const KeyValuePair & kwargs, const QString & where)
{
  QSqlQuery query = new_query();
  QString sql_query = QStringLiteral("UPDATE ") + table + QStringLiteral(" SET ") + format_kwarg(kwargs);
  if (!where.isEmpty())
    sql_query += QStringLiteral(" WHERE ") + where;
  qInfo() << sql_query << kwargs;

  if (!query.exec(sql_query))
    qWarning() << query.lastError().text();

  return query;
}

QSqlQuery
QcDatabase::delete_row(const QString & table, const QString & where)
{
  QSqlQuery query = new_query();
  QString sql_query = QStringLiteral("DELETE FROM ") + table;
  if (!where.isEmpty())
    sql_query += QStringLiteral(" WHERE ") + where;
  qInfo() << sql_query;

  if (!query.exec(sql_query))
    qWarning() << query.lastError().text();

  return query;
}

/**************************************************************************************************/

QcNetworkDatabase::QcNetworkDatabase()
{}

QcNetworkDatabase::~QcNetworkDatabase()
{}

void
QcNetworkDatabase::open(const QcDatabaseConnectionData & connection_data)
{
  m_database = QSqlDatabase::addDatabase(driver_name());
  m_database.setHostName(connection_data.host);
  m_database.setPort(connection_data.port);
  m_database.setDatabaseName(connection_data.database);
  m_database.setUserName(connection_data.user);
  m_database.setPassword(connection_data.password);
  if (!m_database.open())
    qWarning() << m_database.lastError().text();

  QStringList tables = m_database.tables();
  // qInfo() << "Tables" << tables;
  if (tables.empty())
    create_tables();
}

bool
QcNetworkDatabase::create_extension(const QString & extension)
{
  // Fixme: PostresSQL
  QString sql_query = "CREATE EXTENSION IF NOT EXISTS " + extension + " WITH SCHEMA public;";
  return execute_query(sql_query);
}

/**************************************************************************************************/

QcSqliteDatabase::QcSqliteDatabase()
{}

QcSqliteDatabase::~QcSqliteDatabase()
{}

bool
QcSqliteDatabase::open(QString sqlite_path)
{
  bool created = !QFile(sqlite_path).exists();

  // Set the connection name to sqlite_path
  m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), sqlite_path);
  m_database.setDatabaseName(sqlite_path);
  if (!m_database.open())
    qWarning() << m_database.lastError().text();

  if (created)
    create_tables();

  return created;
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
