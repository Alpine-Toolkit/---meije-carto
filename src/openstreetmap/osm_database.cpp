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

#include "osm_database.h"
#include "geo_data_format/wkb.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcOsmDatabase::QcOsmDatabase(const QcDatabaseConnectionData & connection_data)
  : QcNetworkDatabase()
{
  open(connection_data);
}

QcOsmDatabase::~QcOsmDatabase()
{}

void
QcOsmDatabase::create_tables()
{
  qInfo() << "Create OSM planet tables";

  // # https://www.sqlite.org/autoinc.html
  // # Fixme: how to handle auto-increment overflow ?

  QList<QString> queries;

  if (!create_extension(QStringLiteral("postgis")))
    qCritical() << "Cannot creat postgis extension";
  if (!create_extension(QStringLiteral("hstore")))
    qCritical() << "Cannot creat hstore extension";

  const QString node_schema =
    "CREATE TABLE planet_osm_point ("
    "osm_id bigint,"
    // wood text,
    // z_order integer,
    "way geometry(Point, 900913)"
    ");";
  queries << node_schema;

  execute_queries(queries);
}

/**************************************************************************************************/

QcOsmPbfDatabaseImporter::QcOsmPbfDatabaseImporter(const QString & pbf_path, QcOsmDatabase & database)
  : QcOsmPbfReader(pbf_path),
  m_database(database)
{}

QSqlQuery
QcOsmPbfDatabaseImporter::prepare_node_query()
{
  const QString sql_query = "INSERT INTO planet_osm_point (osm_id, way) VALUES (?, ST_GeomFromWKB(?, 900913));";
  return m_database.prepare_query(sql_query);
}

void
QcOsmPbfDatabaseImporter::enter_node_transactions()
{
  m_transaction_query = prepare_node_query();
}

void
QcOsmPbfDatabaseImporter::yield_node(int64_t node_index, int64_t longitude, int64_t latitude)
{
  QcWgsCoordinate coordinate = to_wgs(longitude, latitude);
  QcWkbPoint point(coordinate.longitude(), coordinate.latitude());
  qDebug() << "node" << node_index << coordinate;

  m_transaction_query.addBindValue(static_cast<qint64>(node_index), QSql::In);
  m_transaction_query.addBindValue(point.to_wkb(true), QSql::In); // | QSql::Binary
  if (!m_transaction_query.exec())
    qWarning() << m_transaction_query.lastError().text();
}

void
QcOsmPbfDatabaseImporter::leave_node_transactions()
{
  m_database.commit();
}

/**************************************************************************************************/

/*
CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;
CREATE EXTENSION IF NOT EXISTS hstore WITH SCHEMA public;
CREATE EXTENSION IF NOT EXISTS postgis WITH SCHEMA public;

CREATE TABLE planet_osm_line (
    osm_id bigint,
    ... text,
    z_order integer,
    way_area real,
    way geometry(LineString,900913)
);

CREATE TABLE planet_osm_point (
    osm_id bigint,
    ... text,
    z_order integer,
    way geometry(Point,900913)
);

CREATE TABLE planet_osm_polygon (
    osm_id bigint,
    ... text,
    z_order integer,
    way_area real,
    way geometry(Geometry,900913)
);

CREATE TABLE planet_osm_roads (
    osm_id bigint,
    ... text,
    z_order integer,
    way_area real,
    way geometry(LineString,900913)
);

CREATE INDEX planet_osm_line_ferry ON planet_osm_line USING gist (way) WHERE (route = 'ferry'::text);
CREATE INDEX planet_osm_line_index ON planet_osm_line USING gist (way) WITH (fillfactor='100');
CREATE INDEX planet_osm_line_name ON planet_osm_line USING gist (way) WHERE (name IS NOT NULL);
CREATE INDEX planet_osm_line_river ON planet_osm_line USING gist (way) WHERE (waterway = 'river'::text);
CREATE INDEX planet_osm_point_index ON planet_osm_point USING gist (way) WITH (fillfactor='100');
CREATE INDEX planet_osm_point_place ON planet_osm_point USING gist (way) WHERE ((place IS NOT NULL) AND (name IS NOT NULL));
CREATE INDEX planet_osm_polygon_index ON planet_osm_polygon USING gist (way) WITH (fillfactor='100');
CREATE INDEX planet_osm_polygon_military ON planet_osm_polygon USING gist (way) WHERE (landuse = 'military'::text);
CREATE INDEX planet_osm_polygon_name ON planet_osm_polygon USING gist (way) WHERE (name IS NOT NULL);
CREATE INDEX planet_osm_polygon_nobuilding ON planet_osm_polygon USING gist (way) WHERE (building IS NULL);
CREATE INDEX planet_osm_polygon_way_area_z6 ON planet_osm_polygon USING gist (way) WHERE (way_area > (59750)::double precision);
CREATE INDEX planet_osm_roads_admin ON planet_osm_roads USING gist (way) WHERE (boundary = 'administrative'::text);
CREATE INDEX planet_osm_roads_admin_low ON planet_osm_roads USING gist (way) WHERE ((boundary = 'administrative'::text) AND (admin_level = ANY (ARRAY['0'::text, '1'::text, '2'::text, '3'::text, '4'::text])));
CREATE INDEX planet_osm_roads_index ON planet_osm_roads USING gist (way) WITH (fillfactor='100');
CREATE INDEX planet_osm_roads_roads_ref ON planet_osm_roads USING gist (way) WHERE ((highway IS NOT NULL) AND (ref IS NOT NULL));
*/

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
