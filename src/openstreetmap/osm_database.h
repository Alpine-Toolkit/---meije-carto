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

#ifndef __OSM_DATABASE_H__
#define __OSM_DATABASE_H__

/**************************************************************************************************/

#include "database/database.h"
#include "openstreetmap/osm_pbf.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE
/**************************************************************************************************/

class QcOsmDatabase : public QcNetworkDatabase
{
public:
  QcOsmDatabase(const QcDatabaseConnectionData & connection_data);
  ~QcOsmDatabase();

  QString driver_name() const override { return QStringLiteral("QPSQL"); }
  void create_tables() override;

private:
  QSqlDatabase m_database;
};

/**************************************************************************************************/

class QcOsmPbfDatabaseImporter : public QcOsmPbfReader
{
public:
  QcOsmPbfDatabaseImporter(const QString & pbf_path, QcOsmDatabase & database);

  void enter_node_transactions();
  // void yield_node(int64_t node_index, int64_t longitude, int64_t latitude, const QVector<KeyValPair> & attributes);
  void yield_node(int64_t node_index, int64_t longitude, int64_t latitude);
  // void yield_node_attribute(int64_t node_index, int32_t key_id, int32_t key_val_id);
  // // void yield_node_metadata(int64_t i, int64_t version, int64_t timestamp, int64_t changeset, int64_t uid, int64_t user_sid);
  void leave_node_transactions();

  // void enter_way_transactions();
  // void yield_way(int64_t way_id, const QVector<int64_t> & node_ids, const QVector<KeyValPair> & attributes);
  // // void yield_way_metadata(int64_t i, int64_t version, int64_t timestamp, int64_t changeset, int64_t uid, int64_t user_sid);
  // void leave_way_transactions();

  // void enter_relation_transactions();
  // void yield_relation(int64_t relation_id,
  //                            const QVector<int32_t> & roles_sid,
  //                            const QVector<int64_t> & member_ids,
  //                            const QVector<OSMPBF::Relation::MemberType> & types,
  //                            const QVector<KeyValPair> & attributes);
  // // void yield_relation_metadata(int64_t i, int64_t version, int64_t timestamp, int64_t changeset, int64_t uid, int64_t user_sid);
  // void leave_relation_transactions();

private:
  QSqlQuery prepare_node_query();

private:
  QcOsmDatabase & m_database;
  QSqlQuery m_transaction_query;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __OSM_DATABASE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
