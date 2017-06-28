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

#ifndef __OSM_PBF_H__
#define __OSM_PBF_H__

/**************************************************************************************************/

#include "coordinate/wgs84.h"

#include <QDataStream>
#include <QFile>
#include <QStringList>
#include <QPair>

// this is the header to pbf format
#include <osmpbf/osmpbf.h>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcOsmPbfReader
{
public:
  typedef QPair<int32_t, int32_t> KeyValPair;

public:
  QcOsmPbfReader(const QString & pbf_path);
  virtual ~QcOsmPbfReader();

  void read_file(bool read_nodes, bool read_ways, bool read_relations, bool read_metadatas);

  virtual void enter_node_transactions() {}
  virtual void yield_node(int64_t node_index, int64_t longitude, int64_t latitude, const QVector<KeyValPair> & attributes) {}
  virtual void yield_node(int64_t node_index, int64_t longitude, int64_t latitude) {}
  virtual void yield_node_attribute(int64_t node_index, int32_t key_id, int32_t key_val_id) {}
  // virtual void yield_node_metadata(int64_t i, int64_t version, int64_t timestamp, int64_t changeset, int64_t uid, int64_t user_sid) {}
  virtual void leave_node_transactions() {}

  virtual void enter_way_transactions() {}
  virtual void yield_way(int64_t way_id, const QVector<int64_t> & node_ids, const QVector<KeyValPair> & attributes) {}
  // virtual void yield_way_metadata(int64_t i, int64_t version, int64_t timestamp, int64_t changeset, int64_t uid, int64_t user_sid) {}
  virtual void leave_way_transactions() {}

  virtual void enter_relation_transactions() {}
  virtual void yield_relation(int64_t relation_id,
                              const QVector<int32_t> & roles_sid,
                              const QVector<int64_t> & member_ids,
                              const QVector<OSMPBF::Relation::MemberType> & types,
                              const QVector<KeyValPair> & attributes) {}
  // virtual void yield_relation_metadata(int64_t i, int64_t version, int64_t timestamp, int64_t changeset, int64_t uid, int64_t user_sid) {}
  virtual void leave_relation_transactions() {}

  QcWgsCoordinate to_wgs(int64_t longitude, int64_t latitude);
  QcWgsCoordinateSmallFootprint to_wgs_small(int64_t longitude, int64_t latitude);

  inline const QString & string(int32_t id) const { return m_string_table[id]; }

private:
  void zlib_inflate();
  void lzma_inflate();
  void read_blob_header();
  void read_blob();
  void read_osm_header();
  void read_osm_data();
  void read_nodes(OSMPBF::PrimitiveGroup primitive_group);
  void read_dense_nodes(OSMPBF::PrimitiveGroup primitive_group);
  void read_ways(OSMPBF::PrimitiveGroup primitive_group);
  void read_relations(OSMPBF::PrimitiveGroup primitive_group);
  inline int64_t to_timestamp(int64_t timestamp) { return timestamp * m_date_granularity; }

private:
  QString m_pbf_path;

  bool m_read_nodes;
  bool m_read_ways;
  bool m_read_relations;
  bool m_read_metadatas;

  uint64_t m_number_of_primitive_groups;
  uint64_t m_number_of_node_primitive_groups;
  uint64_t m_number_of_dense_node_primitive_groups;
  uint64_t m_number_of_way_primitive_groups;
  uint64_t m_number_of_relation_primitive_groups;
  uint64_t m_number_of_nodes;
  uint64_t m_number_of_ways;
  uint64_t m_number_of_relations;

  QDataStream m_data_stream;

  // buffer for reading a compressed blob from file
  char * m_buffer;
  // buffer for decompressing the blob
  char * m_unpack_buffer;
  int32_t m_buffer_size;

  // pbf struct of a BlobHeader
  OSMPBF::BlobHeader m_blob_header;
  // pbf struct of a Blob
  OSMPBF::Blob m_blob;
  // pbf struct of an OSM HeaderBlock
  OSMPBF::HeaderBlock m_header_block;
  // pbf struct of an OSM PrimitiveBlock
  OSMPBF::PrimitiveBlock m_primitive_block;

  int32_t m_granularity;
  // Offset value between the output coordinates coordinates and the granularity grid in unites of nanodegrees.
  int64_t m_latitude_offset;
  int64_t m_longitude_offset;
  // Granularity of dates, normally represented in units of milliseconds since the 1970 epoch.
  int32_t m_date_granularity;

  QStringList m_string_table; // Fixme: QVector<QString> ?
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __OSM_PBF_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
