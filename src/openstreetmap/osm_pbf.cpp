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

#include "osm_pbf.h"

#include <QtDebug>

#include <cstring>

// zlib compression is used inside the pbf blobs
#include <zlib.h>

/**************************************************************************************************/

class DeltaCodedInt64
{
public:
  DeltaCodedInt64()
    : m_last_value(0)
  {}

  inline int64_t update(int64_t value) {
    value += m_last_value;
    m_last_value = value;
    return value;
  }

  inline int64_t operator()() const { return m_last_value; }

private:
  int64_t m_last_value;
};

/**************************************************************************************************/

QcOsmPbfReader::QcOsmPbfReader(const QString & pbf_path)
  : m_pbf_path(pbf_path),
  m_buffer(nullptr),
  m_unpack_buffer(nullptr)
{
  // qDebug() << pbf_path;
}

QcOsmPbfReader::~QcOsmPbfReader()
{}

/* Unzip a blob a return its size
 */
void
QcOsmPbfReader::zlib_inflate()
{
  // the size of the compressesd data
  int32_t size = m_blob.zlib_data().size();

  // tell about the compressed data
  qDebug().nospace() << "  contains zlib-compressed data: " << size << " bytes";
  qDebug().nospace() << "  uncompressed size: " << m_blob.raw_size() << " bytes";

  // zlib information
  z_stream z;

  // next byte to decompress
  z.next_in = (unsigned char *) m_blob.zlib_data().c_str();

  // number of bytes to decompress
  z.avail_in = size;

  // place of next decompressed byte
  z.next_out = (unsigned char *) m_unpack_buffer;

  // space for decompressed data
  z.avail_out = m_blob.raw_size();

  // misc
  z.zalloc = Z_NULL;
  z.zfree = Z_NULL;
  z.opaque = Z_NULL;

  if (inflateInit(&z) != Z_OK)
    qCritical() << "  failed to init zlib stream";
  if (inflate(&z, Z_FINISH) != Z_STREAM_END)
    qCritical() << "  failed to inflate zlib stream";
  if (inflateEnd(&z) != Z_OK)
    qCritical() << "  failed to deinit zlib stream";

  m_buffer_size = z.total_out;
}

void
QcOsmPbfReader::lzma_inflate()
{
  // tell about the compressed data
  qDebug().nospace() << "  contains lzma-compressed data: " << m_blob.lzma_data().size() << " bytes";
  qDebug().nospace() << "  uncompressed size: " << m_blob.raw_size() << " bytes";

  // issue a warning, lzma compression is not yet supported
  qCritical() << "  lzma-decompression is not supported";
}

QcWgsCoordinate
QcOsmPbfReader::to_wgs(int64_t longitude, int64_t latitude)
{
  double _longitude = .000000001 * (m_longitude_offset + (m_granularity * longitude));
  double _latitude = .000000001 * (m_latitude_offset + (m_granularity * latitude));

  return QcWgsCoordinate(_longitude, _latitude);
}

QcWgsCoordinateSmallFootprint
QcOsmPbfReader::to_wgs_small(int64_t longitude, int64_t latitude)
{
  // PBF scale by 1e-9
  int32_t _longitude = (m_longitude_offset + (m_granularity * longitude)) / 100;
  int32_t _latitude = (m_latitude_offset + (m_granularity * latitude)) / 100;

  return QcWgsCoordinateSmallFootprint(_longitude, _latitude);
}

void
QcOsmPbfReader::read_file(bool read_nodes, bool read_ways, bool read_relations, bool read_metadatas)
{
  m_read_nodes = read_nodes;
  m_read_ways = read_ways;
  m_read_relations = read_relations;
  m_read_metadatas = read_metadatas;

  m_number_of_primitive_groups = 0;
  m_number_of_node_primitive_groups = 0;
  m_number_of_dense_node_primitive_groups = 0;
  m_number_of_way_primitive_groups = 0;
  m_number_of_relation_primitive_groups = 0;
  m_number_of_nodes = 0;
  m_number_of_ways = 0;
  m_number_of_relations = 0;

  m_buffer = new char[OSMPBF::max_uncompressed_blob_size];
  m_unpack_buffer = new char[OSMPBF::max_uncompressed_blob_size];

  QFile file(m_pbf_path);
  if (!file.open(QIODevice::ReadOnly)) {
    qCritical() << "can't open file" << m_pbf_path;
    return ;
  }

  m_data_stream.setDevice(&file);
  // Set network byte-order
  m_data_stream.setByteOrder(QDataStream::BigEndian);

  // read while the file has not reached its end
  while (!file.atEnd()) {
    /* A file contains a header followed by a sequence of fileblocks.
     *
     * The design is intended to allow future random-access to the
     * contents of the file and skipping past not-understood or
     * unwanted data.
     *
     * The format is a repeating sequence of:
     *  - int4: length of the BlobHeader message in network byte order
     *  - serialized BlobHeader message
     *  - serialized Blob message (size is given in the header)
     */
    read_blob_header();
    read_blob();
  }

  // clean up the protobuf lib
  google::protobuf::ShutdownProtobufLibrary();

  qDebug() << "File Statistics\n"
           << "Primitive Groups" <<  m_number_of_primitive_groups << "\n"
           << " for Node" <<  m_number_of_node_primitive_groups << "\n"
           << " for Dense Node" <<  m_number_of_dense_node_primitive_groups << "\n"
           << " for Way" <<  m_number_of_way_primitive_groups << "\n"
           << " for Relation" <<  m_number_of_relation_primitive_groups << "\n"
           << "Number of nodes" <<  m_number_of_nodes << "\n"
           << "Number of ways" <<  m_number_of_ways << "\n"
           << "Number of relations" <<  m_number_of_relations;
}

void
QcOsmPbfReader::read_blob_header()
{
  // read the blob-header size
  int32_t blob_header_size;
  m_data_stream >> blob_header_size;
  // Fixme: end of file reached

  // ensure the blob-header is smaller then MAX_BLOB_HEADER_SIZE
  if (blob_header_size > OSMPBF::max_blob_header_size)
    qCritical() << "blob-header-size is bigger then allowed (" << blob_header_size << "  > " << OSMPBF::max_blob_header_size << ")";

  // read the blob-header from the file
  if (m_data_stream.readRawData(m_buffer, blob_header_size) == -1)
    qCritical() << "unable to read blob-header from file";

  // parse the blob-header from the read-buffer
  if (!m_blob_header.ParseFromArray(m_buffer, blob_header_size))
    qCritical() << "unable to parse blob header";

  // tell about the blob-header
  qDebug().nospace() << "BlobHeader (" << blob_header_size << " bytes)" << " type = " << m_blob_header.type().c_str();

  // optional indexdata
  if (m_blob_header.has_indexdata())
    qDebug().nospace() << "  indexdata = " << m_blob_header.indexdata().size() << "bytes";
}

void
QcOsmPbfReader::read_blob()
{
  // size of the following blob
  int32_t data_size = m_blob_header.datasize();
  qDebug().nospace() << "  datasize = " << data_size;

  // ensure the blob is smaller then MAX_BLOB_SIZE
  if (data_size > OSMPBF::max_uncompressed_blob_size)
    qCritical() << "blob-size is bigger then allowed (" << data_size << " > " << OSMPBF::max_uncompressed_blob_size << ")";

  // read the blob from the file
  if (m_data_stream.readRawData(m_buffer, data_size) == -1)
    qCritical() << "unable to read blob from file";

  // parse the blob from the read-buffer
  if (!m_blob.ParseFromArray(m_buffer, data_size))
    qCritical() << "unable to parse blob";

  // tell about the blob-header
  qDebug().nospace() << "Blob (" << data_size << " bytes)";

  // set when we find at least one data stream
  bool found_data = false;

  // if the blob has uncompressed data
  if (m_blob.has_raw()) {
    // we have at least one datastream
    found_data = true;

    // size of the blob-data
    m_buffer_size = m_blob.raw().size();

    // check that raw_size is set correctly
    if (m_buffer_size != m_blob.raw_size())
      qWarning() << "  reports wrong raw_size: " << m_blob.raw_size() << " bytes";

    // tell about the blob-data
    qDebug().nospace() << "  contains uncompressed data: " << m_buffer_size << " bytes";

    // copy the uncompressed data over to the unpack_buffer
    memcpy(m_unpack_buffer, m_buffer, m_buffer_size);
  }

  // if the blob has zlib-compressed data
  if (m_blob.has_zlib_data()) {
    // issue a warning if there is more than one data steam, a blob may only contain one data stream
    if (found_data)
      qWarning() << "  contains several data streams";

    // we have at least one datastream
    found_data = true;

    // unpacked size
    zlib_inflate();
  }

  // if the blob has lzma-compressed data
  if (m_blob.has_lzma_data()) {
    // issue a warning if there is more than one data steam, a blob may only contain one data stream
    if (found_data)
      qWarning() << "  contains several data streams";

    // we have at least one datastream
    found_data = true;

    // unpacked size
    lzma_inflate();
  }

  // check we have at least one data-stream
  if (!found_data)
    qCritical() << "  does not contain any known data stream";

  // switch between different blob-types
  auto blob_type = m_blob_header.type();
  if (blob_type == "OSMHeader")
    // The Blob contains a serialized HeaderBlock message.
    // Every fileblock must have one of these blocks before the first 'OSMData' block.
    read_osm_header();
  else if (blob_type == "OSMData")
    // The Blob contains a serialized PrimitiveBlock message.
    read_osm_data();
  else
    // unknown blob type
    qWarning() << "  unknown blob type: " << m_blob_header.type().c_str();
}

void
QcOsmPbfReader::read_osm_header()
{
  // tell about the OSMHeader blob
  qDebug() << "  OSMHeader";

  // parse the HeaderBlock from the blob
  if (!m_header_block.ParseFromArray(m_unpack_buffer, m_buffer_size))
    qCritical() << "unable to parse header block";

  // tell about the bbox
  if (m_header_block.has_bbox()) {
    OSMPBF::HeaderBBox bbox = m_header_block.bbox();
    qDebug().nospace() << "    bbox: "
             << (double) bbox.left() / OSMPBF::lonlat_resolution << ", "
             << (double) bbox.bottom() / OSMPBF::lonlat_resolution << ", "
             << (double) bbox.right() / OSMPBF::lonlat_resolution << ", "
             << (double) bbox.top() / OSMPBF::lonlat_resolution;
  }

  // tell about the required features
  for (int i = 0, l = m_header_block.required_features_size(); i < l; i++) {
    qDebug().nospace() << "    required_feature: " << m_header_block.required_features(i).c_str();
  }

  // tell about the optional features
  for (int i = 0, l = m_header_block.optional_features_size(); i < l; i++) {
    qDebug().nospace() << "    optional_feature: " << m_header_block.optional_features(i).c_str();
  }

  // tell about the writing program
  if (m_header_block.has_writingprogram())
    qDebug().nospace() << "    writingprogram: " << m_header_block.writingprogram().c_str();

  // tell about the source
  if (m_header_block.has_source())
    qDebug().nospace() << "    source: ", m_header_block.source().c_str();
}

void
QcOsmPbfReader::read_osm_data()
{
  // tell about the OSMData blob
  qDebug() << "  OSMData";

  // parse the PrimitiveBlock from the blob
  if (!m_primitive_block.ParseFromArray(m_unpack_buffer, m_buffer_size))
    qCritical() << "unable to parse primitive block";

  // tell about the block's meta info
  m_granularity = m_primitive_block.granularity(); // default: 100
  m_latitude_offset = m_primitive_block.lat_offset(); // default: 0
  m_longitude_offset = m_primitive_block.lon_offset(); // default: 0
  m_date_granularity = m_primitive_block.date_granularity(); // default: 1000
  // qDebug().nospace() << "    granularity: " << m_granularity;
  // qDebug().nospace() << "    lat_offset: " << m_latitude_offset;
  // qDebug().nospace() << "    lon_offset: "  << m_longitude_offset;
  // qDebug().nospace() << "    date_granularity: " << m_date_granularity;

  // tell about the stringtable
  m_string_table.clear();
  OSMPBF::StringTable string_table = m_primitive_block.stringtable();
  qDebug().nospace() << "    stringtable: " << string_table.s_size() << " items";
  for (int i = 0; i < string_table.s_size(); i++) {
    QString string = string_table.s(i).c_str();
    // qDebug().nospace() << "      string[" << i << "] " << string ;
    m_string_table << string;
  }

  // number of PrimitiveGroups
  int number_of_primitive_groups = m_primitive_block.primitivegroup_size();
  m_number_of_primitive_groups += number_of_primitive_groups;
  qDebug().nospace() << "    primitivegroups: " << number_of_primitive_groups << " groups";

  // iterate over all PrimitiveGroups
  for (int i = 0, l = number_of_primitive_groups; i < l; i++) {
    /* A PrimitiveGroup MUST NEVER contain different types of
     * objects. So either it contains many Node messages, or a
     * DenseNode message, or many Way messages, or many Relation
     * messages, or many ChangeSet messages.
     */

    // one PrimitiveGroup from the the Block
    OSMPBF::PrimitiveGroup primitive_group = m_primitive_block.primitivegroup(i);

    bool found_items = false;

    // tell about nodes
    int number_of_nodes = primitive_group.nodes_size();
    if (number_of_nodes > 0) {
      found_items = true;
      m_number_of_node_primitive_groups++;
      m_number_of_nodes += number_of_nodes;
      qDebug().nospace() << "      primitive nodes: " << number_of_nodes;
      if (m_read_nodes)
        read_nodes(primitive_group);
    }

    // tell about dense nodes
    if (primitive_group.has_dense()) {
      found_items = true;
      m_number_of_dense_node_primitive_groups++;
      int number_of_nodes = primitive_group.dense().id_size();
      m_number_of_nodes += number_of_nodes;
      qDebug().nospace() << "      primitive dense nodes: " << number_of_nodes;
      if (m_read_nodes)
        read_dense_nodes(primitive_group);
    }

    // tell about ways
    if (primitive_group.ways_size() > 0) {
      found_items = true;
      m_number_of_way_primitive_groups++;
      int number_of_ways = primitive_group.ways_size();
      m_number_of_ways += number_of_ways;
      qDebug().nospace() << "      primitive ways: " << number_of_ways;
      if (m_read_ways)
        read_ways(primitive_group);
    }

    // tell about relations
    if (primitive_group.relations_size() > 0) {
      found_items = true;
      m_number_of_relation_primitive_groups++;
      int number_of_relations = primitive_group.relations_size();
      m_number_of_relations += number_of_relations;
      qDebug().nospace() << "      primitive relations: " << primitive_group.relations_size();
      if (m_read_relations)
        read_relations(primitive_group);
    }

    if (!found_items)
      qWarning() << "      contains no items";
  }
}

void
QcOsmPbfReader::read_nodes(OSMPBF::PrimitiveGroup primitive_group)
{
  enter_node_transactions();

  int number_of_nodes = primitive_group.nodes_size();
  for (int i = 0; i < number_of_nodes; i++) {
    OSMPBF::Node node = primitive_group.nodes(i);
    int64_t node_id = node.id();
    int64_t longitude = node.lon();
    int64_t latitude = node.lat();
    // qDebug() << "        node " << i << node_id << to_wgs(longitude, latitude);

    int number_of_attributes = node.keys_size();
    QVector<KeyValPair> attributes(number_of_attributes);
    for (int i = 0; i < number_of_attributes; i++) {
      int32_t key_id = node.keys(i);
      int32_t val_id = node.vals(i);
      // qDebug() << "key_val" << node_id << m_string_table[key_id] << m_string_table[val_id];
    }

    yield_node(node_id, longitude, latitude, attributes);

    if (m_read_metadatas and node.has_info()) {
      // qDebug().nospace() << "        with meta-info";
      OSMPBF::Info info = node.info();
      int32_t version = info.version();
      int64_t timestamp = to_timestamp(info.timestamp());
      int64_t changeset = info.changeset();
      int32_t uid = info.uid();
      int32_t user_sid = info.user_sid();
      // bool visible = info.visible();
      // qDebug() << "Meta information:" << version << timestamp << changeset << uid << user_sid;
      // yield_node_metadata(node_id, version, timestamp, changeset, uid, user_sid);
    }
  }

  leave_node_transactions();
}

void
QcOsmPbfReader::read_dense_nodes(OSMPBF::PrimitiveGroup primitive_group)
{
  enter_node_transactions();

  OSMPBF::DenseNodes dense_node = primitive_group.dense();

  DeltaCodedInt64 node_id;
  DeltaCodedInt64 longitude;
  DeltaCodedInt64 latitude;
  int number_of_nodes = dense_node.id_size();
  for (int i = 0; i < number_of_nodes; i++) {
    node_id.update(dense_node.id(i));
    longitude.update(dense_node.lon(i));
    latitude.update(dense_node.lat(i));
    // qDebug() << "        dense node" << node_id() << to_wgs(longitude(), latitude());
    yield_node(node_id(), longitude(), latitude());
  }

  // The storage pattern is: ((<keyid> <valid>)* '0' )*
  bool is_key = true;
  int key_id = 0;
  for (int i = 0, l = dense_node.keys_vals_size(), node_index = 0;
       i < l; i++) {
    int32_t key_val_id = dense_node.keys_vals(i);
    if (key_val_id) {
      if (is_key)
        key_id = key_val_id;
      else {
        // qDebug() << "        attr" << node_index << m_string_table[key_id] << "=" << m_string_table[key_val_id];
        yield_node_attribute(node_index, key_id, key_val_id);
      }
      is_key = not(is_key);
    }
    else
      node_index++;
  }

  if (m_read_metadatas and dense_node.has_denseinfo()) {
    // qDebug().nospace() << "        with meta-info";
    OSMPBF::DenseInfo dense_info = dense_node.denseinfo();
    DeltaCodedInt64 timestamp;
    DeltaCodedInt64 changeset;
    DeltaCodedInt64 uid;
    DeltaCodedInt64 user_sid;
    for (int i = 0; i < number_of_nodes; i++) {
      int32_t version = dense_info.version(i);
      timestamp.update(to_timestamp(dense_info.timestamp(i)));
      changeset.update(dense_info.changeset(i));
      uid.update(dense_info.uid(i));
      user_sid.update(dense_info.user_sid(i));
      // bool visible = dense_info.visible(i);
      // qDebug() << "Meta information:" << version << timestamp() << changeset() << uid() << user_sid();
      // yield_node_metadata(i, version, timestamp(), changeset(), uid(), user_sid());
    }
  }

  leave_node_transactions();
}

void
QcOsmPbfReader::read_ways(OSMPBF::PrimitiveGroup primitive_group)
{
  enter_way_transactions();

  int number_of_ways = primitive_group.ways_size();
  for (int i = 0; i < number_of_ways; i++) {
    OSMPBF::Way way = primitive_group.ways(i);
    int64_t way_id = way.id();

    QVector<int64_t> node_ids(way.refs_size());
    int j = 0;
    DeltaCodedInt64 node_id;
    for (auto ref : way.refs()) {
      node_ids[j++] = node_id.update(ref);
    }

    // qDebug().nospace() << "way" << i << way_id << node_ids;

    int number_of_attributes = way.keys_size();
    QVector<KeyValPair> attributes(number_of_attributes);
    for (int i = 0; i < number_of_attributes; i++) {
      int32_t key_id = way.keys(i);
      int32_t val_id = way.vals(i);
      // qDebug() << "  key_val" << way_id << m_string_table[key_id] << m_string_table[val_id];
      attributes[i] = KeyValPair(key_id, val_id);
    }

    yield_way(way_id, node_ids, attributes);

    if (m_read_metadatas and way.has_info()) {
      // qDebug().nospace() << "        with meta-info";
      OSMPBF::Info info = way.info();
      int32_t version = info.version();
      int64_t timestamp = to_timestamp(info.timestamp());
      int64_t changeset = info.changeset();
      int32_t uid = info.uid();
      int32_t user_sid = info.user_sid();
      // bool visible = info.visible();
      // qDebug() << "Meta information:" << version << timestamp << changeset << uid << user_sid;
      // yield_way_metadata(way_id, version, timestamp, changeset, uid, user_sid);
    }
  }

  leave_way_transactions();
}

void
QcOsmPbfReader::read_relations(OSMPBF::PrimitiveGroup primitive_group)
{
  enter_relation_transactions();

  int number_of_relations = primitive_group.relations_size();
  for (int i = 0; i < number_of_relations; i++) {
    OSMPBF::Relation relation = primitive_group.relations(i);
    int64_t relation_id = relation.id();

    QVector<int32_t> roles_sid(relation.roles_sid_size());
    QVector<int64_t> member_ids(roles_sid.size());
    QVector<OSMPBF::Relation::MemberType> types(roles_sid.size());
    // QStringList roles;
    DeltaCodedInt64 member_id;
    for (int i = 0, l = relation.roles_sid_size(); i < l; i++) {
      int32_t role_sid = relation.roles_sid(i);
      roles_sid[i] = role_sid;
      member_ids[i] = member_id.update(relation.memids(i));
      types[i] = relation.types(i);
      // roles << m_string_table[role_sid];
    }

    // qDebug().nospace() << "relation" << i << relation_id << roles_sid << roles << member_ids << types;

    int number_of_attributes = relation.keys_size();
    QVector<KeyValPair> attributes(number_of_attributes);
    for (int i = 0; i < number_of_attributes; i++) {
      int32_t key_id = relation.keys(i);
      int32_t val_id = relation.vals(i);
      // qDebug() << "  key_val" << relation_id << m_string_table[key_id] << m_string_table[val_id];
      attributes[i] = KeyValPair(key_id, val_id);
    }

    yield_relation(relation_id, roles_sid, member_ids, types, attributes);

    if (m_read_metadatas and relation.has_info()) {
      // qDebug().nospace() << "        with meta-info";
      OSMPBF::Info info = relation.info();
      int32_t version = info.version();
      int64_t timestamp = to_timestamp(info.timestamp());
      int64_t changeset = info.changeset();
      int32_t uid = info.uid();
      int32_t user_sid = info.user_sid();
      // bool visible = info.visible();
      // qDebug() << "Meta information:" << version << timestamp << changeset << uid << user_sid;
      // yield_relation_metadata(way_id, version, timestamp, changeset, uid, user_sid);
    }
  }

  leave_relation_transactions();
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
