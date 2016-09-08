// -*- mode: c++ -*-

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

#ifndef __DATABASE_H__
#define __DATABASE_H__

/**************************************************************************************************/

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QVariant>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcDatabase
{
public:
  typedef QHash<QString, QVariant> KeyValuePair;

public:
  QcDatabase();
  virtual ~QcDatabase();

  virtual void create_tables() = 0;

  inline QSqlDatabase & database () { return m_database; }

  inline bool transaction() { return m_database.transaction(); }
  inline bool commit() { return m_database.commit(); }
  inline QSqlQuery new_query() const { return QSqlQuery(m_database); }

  bool execute_query(const QString & sql_query);
  bool execute_queries(const QStringList & sql_queries, bool commit = true);

  QString format_kwarg(const KeyValuePair & kwargs, const QString & sperator = QStringLiteral(","));
  QString format_simple_where(const KeyValuePair & kwargs);

  QSqlQuery select(const QString & table, const QStringList & fields, const QString & where = QStringLiteral(""));
  QSqlRecord select_one(const QString & table, const QStringList & fields, const QString & where = QStringLiteral(""));
  QSqlQuery prepare_query(const QString & sql_query);
  QSqlQuery insert(const QString & table, const KeyValuePair & kwargs, bool commit = false);
  QSqlQuery update(const QString & table, const KeyValuePair & kwargs, const QString & where = QStringLiteral(""));
  QSqlQuery delete_row(const QString & table, const QString & where);

protected:
  QSqlDatabase m_database;
};

/**************************************************************************************************/

struct QcDatabaseConnectionData
{
  QString host;
  int port;
  QString database;
  QString user;
  QString password;
};

/**************************************************************************************************/

class QcNetworkDatabase : public QcDatabase
{
public:
  QcNetworkDatabase();
  virtual ~QcNetworkDatabase();

  void open(const QcDatabaseConnectionData & connection_data);

  bool create_extension(const QString & extension);

  virtual QString driver_name() const = 0;
};

/**************************************************************************************************/

class QcSqliteDatabase : public QcDatabase
{
public:
  QcSqliteDatabase();
  virtual ~QcSqliteDatabase();

  bool open(QString sqlite_path);
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __DATABASE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
