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

#include "offline_cache_database.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

const QString COLUMN = "column";
const QString LEVEL = "level";
const QString MAP_ID = "map_id";
const QString MAP_LEVEL = "map_level";
const QString MAP_LEVEL_ID = "map_level_id";
const QString NAME = "name";
const QString OFFLINE_COUNT = "offline_count";
const QString PROVIDER = "provider";
const QString PROVIDER_ID = "provider_id";
const QString ROW = "row";
const QString TILE = "tile";

/**************************************************************************************************/

QcOfflineCacheDatabase::QcOfflineCacheDatabase(QString sqlite_path)
{
  bool create = !QFile(sqlite_path).exists();

  m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), sqlite_path);
  m_database.setDatabaseName(sqlite_path);
  if (!m_database.open())
    qWarning() << m_database.lastError().text();

  if (create)
    create_tables();
  else
    init_cache();
}

QcOfflineCacheDatabase::~QcOfflineCacheDatabase()
{
  m_database.close();
}

QSqlQuery
QcOfflineCacheDatabase::new_query() const
{
  return QSqlQuery(m_database);
}

bool
QcOfflineCacheDatabase::commit()
{
  return m_database.commit();
}

QSqlQuery
QcOfflineCacheDatabase::insert(const QString & table, const KeyValuePair & kwargs, bool commit_request)
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
QcOfflineCacheDatabase::select(const QString & table, const QStringList & fields, const QString & where)
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
QcOfflineCacheDatabase::select_one(const QString & table, const QStringList & fields, const QString & where)
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
QcOfflineCacheDatabase::delete_row(const QString & table, const QString & where)
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

QString
QcOfflineCacheDatabase::format_kwarg(const KeyValuePair & kwargs, const QString & separator)
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
QcOfflineCacheDatabase::format_simple_where(const KeyValuePair & kwargs)
{
  return format_kwarg(kwargs, QStringLiteral(" AND "));
}

QSqlQuery
QcOfflineCacheDatabase::update(const QString & table, const KeyValuePair & kwargs, const QString & where)
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

void
QcOfflineCacheDatabase::create_tables()
{
  // # https://www.sqlite.org/autoinc.html
  // # Fixme: how to handle auto-increment overflow ?

  QList<QString> schemas;

  const QString metadata_schema =
    "CREATE TABLE metadata ("
    "version INTEGER"
    ")";
  schemas << metadata_schema;

  const QString provider_schema =
    "CREATE TABLE provider ("
    "provider_id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT NOT NULL"
    ")";
  schemas << provider_schema;

  const QString map_level_schema =
    "CREATE TABLE map_level ("
    "map_level_id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "provider_id INTEGER, "
    "map_id INTEGER, "
    "level INTEGER, "
    "FOREIGN KEY(provider_id) REFERENCES provider_id(provider_id)"
    ")";
  schemas << map_level_schema;

  //  PRIMARY KEY (map_level_id, row, column),
  const QString tile_schema =
    "CREATE TABLE tile ("
    "map_level_id INTEGER, "
    "row INTEGER, "
    "column INTEGER, "
    "offline_count INTEGER, "
    "FOREIGN KEY(map_level_id) REFERENCES map_level(map_level_id)"
    ")";
  schemas << tile_schema;

  QSqlQuery query = new_query();
  for (const auto & sql_query : schemas)
    if (!query.exec(sql_query))
      qWarning() << query.lastError().text();

  init_version();
  commit();
}

void
QcOfflineCacheDatabase::init_version()
{
  KeyValuePair kwargs;
  kwargs.insert(QStringLiteral("version"), 1);
  insert(QStringLiteral("metadata"), kwargs);
}

void
QcOfflineCacheDatabase::init_cache()
{
  load_providers();
  load_map_levels();
}

void
QcOfflineCacheDatabase::load_providers()
{
  QStringList fields; // QStringList(PROVIDER_ID, NAME)
  fields << PROVIDER_ID << NAME;
  QSqlQuery provider_query = select(PROVIDER, fields);
  while (provider_query.next()) {
    QSqlRecord record = provider_query.record();
    int i = 0;
    int provider_id = record.value(i++).toInt();
    QString name = record.value(i++).toString();
    qInfo() << "Provider" << provider_id << name;
    m_providers.insert(name, provider_id);
  }
}

void
QcOfflineCacheDatabase::load_map_levels()
{
  QStringList fields; // QStringList()
  fields << MAP_LEVEL_ID << PROVIDER_ID << MAP_ID << LEVEL;
  QSqlQuery provider_query = select(MAP_LEVEL, fields);
  while (provider_query.next()) {
    QSqlRecord record = provider_query.record();
    int i = 0;
    int map_level_id = record.value(i++).toInt();
    int provider_id = record.value(i++).toInt();
    int map_id = record.value(i++).toInt();
    int level = record.value(i++).toInt();
    unsigned int map_level_hash = hash_tile_spec(provider_id, map_id, level);
    qInfo() << "Map level" << map_level_id << provider_id << map_id << level;
    m_map_levels.insert(map_level_hash, map_level_id);
  }
}

int
QcOfflineCacheDatabase::get_provider_id(const QString & provider)
{
  if (m_providers.contains(provider))
    return m_providers[provider];
  else {
    KeyValuePair kwargs;
    kwargs.insert(NAME, provider);
    insert(PROVIDER, kwargs);
    QString where = format_simple_where(kwargs);
    QSqlRecord record = select_one(PROVIDER, QStringList(PROVIDER_ID), where);
    int provider_id = record.value(0).toInt();
    m_providers.insert(provider, provider_id);
    return provider_id;
  }
}

unsigned int
QcOfflineCacheDatabase::hash_tile_spec(int provider_id, int map_id, int level)
{
  // 31 is coded on 5-bit
  // unsigned int result = (qHash(tile_spec.plugin()) * 13) % 31;
  unsigned int result = (provider_id * 13) % 31;
  result += ((map_id * 17) % 31) << 5;
  result += ((level * 19) % 31) << 10;
  return result;
}

int
QcOfflineCacheDatabase::get_map_level_id(const QcTileSpec & tile_spec)
{
  int provider_id = get_provider_id(tile_spec.plugin()); // Fixme: provider
  unsigned int map_level_hash = hash_tile_spec(provider_id, tile_spec.map_id(), tile_spec.level());
  if (m_map_levels.contains(map_level_hash))
    return m_map_levels[map_level_hash];
  else {
    KeyValuePair kwargs;
    kwargs.insert(PROVIDER_ID, provider_id);
    kwargs.insert(MAP_ID, tile_spec.map_id());
    kwargs.insert(LEVEL, tile_spec.level());
    insert(MAP_LEVEL, kwargs);
    QString where = format_simple_where(kwargs);
    QSqlRecord record = select_one(MAP_LEVEL, QStringList(MAP_LEVEL_ID), where);
    int map_level_id = record.value(0).toInt();
    m_map_levels.insert(map_level_hash, map_level_id);
    return map_level_id;
  }
}

QString
QcOfflineCacheDatabase::tile_where_clause(const QcTileSpec & tile_spec)
{
  int map_level_id = get_map_level_id(tile_spec);
  KeyValuePair kwargs;
  kwargs.insert(MAP_LEVEL_ID, map_level_id);
  kwargs.insert(ROW, tile_spec.x());
  kwargs.insert(COLUMN, tile_spec.y());
  return format_simple_where(kwargs);
}

int
QcOfflineCacheDatabase::has_tile(const QcTileSpec & tile_spec)
{
  QSqlRecord record = select_one(TILE, QStringList(OFFLINE_COUNT), tile_where_clause(tile_spec));
  if (record.isEmpty())
    return 0;
  else
    return record.value(0).toInt();
}

void
QcOfflineCacheDatabase::insert_tile(const QcTileSpec & tile_spec)
{
  QString where = tile_where_clause(tile_spec);
  QSqlRecord record = select_one(TILE, QStringList(OFFLINE_COUNT), where);
  if (record.isEmpty()) {
    int map_level_id = get_map_level_id(tile_spec);
    KeyValuePair kwargs;
    kwargs.insert(MAP_LEVEL_ID, map_level_id);
    kwargs.insert(ROW, tile_spec.x());
    kwargs.insert(COLUMN, tile_spec.y());
    kwargs.insert(OFFLINE_COUNT, 1);
    insert(TILE, kwargs);
  } else {
    int offline_count = record.value(0).toInt();
    KeyValuePair kwargs;
    kwargs.insert(OFFLINE_COUNT, offline_count +1);
    update(TILE, kwargs, where);
  }
}

/*
void
QcOfflineCacheDatabase::update_tile_offline_count(const QcTileSpec & tile_spec, int offline_count)
{
  KeyValuePair kwargs;
  kwargs.insert(OFFLINE_COUNT, offline_count);
  update(TILE, kwargs, tile_where_clause(tile_spec));
}
*/

void
QcOfflineCacheDatabase::delete_tile(const QcTileSpec & tile_spec)
{
  QString where = tile_where_clause(tile_spec);
  QSqlRecord record = select_one(TILE, QStringList(OFFLINE_COUNT), where);
  if (!record.isEmpty()) {
    int offline_count = record.value(0).toInt();
    if (offline_count > 1) {
      KeyValuePair kwargs;
      kwargs.insert(OFFLINE_COUNT, offline_count -1);
      update(TILE, kwargs, where);
    }
    else
      delete_row(TILE, where);
  }
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
