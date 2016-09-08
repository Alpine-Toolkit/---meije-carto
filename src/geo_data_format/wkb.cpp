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

#include "wkb.h"

#include <QtDebug>

#include <QtEndian>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

const QString Point("Point");
const QString LineString("LineString");
const QString Polygon("Polygon");
const QString MultiPoint("MultiPoint");
const QString MultiLineString("MultiLineString");
const QString MultiPolygon("MultiPolygon");
const QString GeometryCollection("GeometryCollection");
const QString Empty("Empty");

/**************************************************************************************************/

class QcWktParser
{
public:
  QcWktParser(const QString & stream, int location = 0)
    : m_location(location),
    m_location_end(stream.size()),
    m_stream(stream)
  {}

  inline bool is_location_valid() {
    return m_location < m_location_end;
  }

  inline bool is_location_not_valid() {
    return !is_location_valid();
  }

  inline QChar lookup_current_char() const {
    return m_stream[m_location];
  }

  inline bool current_char_is_space() const {
    return lookup_current_char() == ' ';
  }

  void
  move_forward()
  {
    m_location++;
    if (is_location_not_valid())
      throw_parser_error(QStringLiteral("unexpected end"));
  }

  QChar
  read_next_char()
  {
    move_forward();
    return lookup_current_char();
  }

  void
  skip_space()
  {
    while (is_location_valid() and current_char_is_space())
      m_location++;
  }

  int
  find_next_char(const QString & chars)
  {
    int location = m_location;
    while(location < m_location_end) {
      if (chars.contains(m_stream[location]))
        return location;
      location++;
    }
    return -1;
  }

  inline QString slice_stream(int start, int stop) {
    return m_stream.mid(start, stop - start);
  }

  bool match_string(const QString & substring)
  {
    int sublocation = 0;
    int sublocation_end = substring.size();
    int location = m_location;
    while (sublocation < sublocation_end and location < m_location_end)
      if (substring[sublocation++].toLower() != m_stream[location++].toLower())
        return false;
    m_location = location;
    return true;
  }

  bool are_string_equal(const QString & string1, const QString & string2) {
    return string1.compare(string2, Qt::CaseInsensitive) == 0;
  }

  void
  throw_parser_error(const QString & error_message)
  {
    qDebug() << QStringLiteral("Error") << m_stream.left(m_location) << '\n' << m_stream.right(m_location_end - m_location);
    throw std::invalid_argument(error_message.toStdString().c_str());
  }

  QcWkbGeometryType
  parse_geometry_type()
  {
    qDebug() << "parse_header";

    // Geometry type must be followed by a space or a '('
    int location = find_next_char(QStringLiteral(" ("));
    if (location == -1)
      throw_parser_error(QStringLiteral("expected ' ' or '('"));
    QString type_name = slice_stream(m_location, location);
    m_location = location;

    // Lookup for Z, M or ZM prefix
    QString type_name_dimension;
    if (lookup_current_char() == ' ') {
      skip_space();
      QChar next_char = lookup_current_char().toLower();
      if (next_char == 'z' or next_char == 'm') {
        type_name_dimension = next_char;
        if (next_char == 'z') {
          next_char = read_next_char().toLower();
          if (next_char == 'm')
            type_name_dimension += 'm';
        }
        skip_space();
      }
    } // else '(

    qDebug() << "geometry type is" << type_name;
    QcWkbGeometryType type;
    if (are_string_equal(type_name, Point))
      type = QcWkbGeometryType::Point;
    else if (are_string_equal(type_name, LineString))
      type = QcWkbGeometryType::LineString;
    else if (are_string_equal(type_name, Polygon))
      type = QcWkbGeometryType::Polygon;
    else if (are_string_equal(type_name, MultiPoint))
      type = QcWkbGeometryType::MultiPoint;
    else if (are_string_equal(type_name, MultiLineString))
      type = QcWkbGeometryType::MultiLineString;
    else if (are_string_equal(type_name, MultiPolygon))
      type = QcWkbGeometryType::MultiPolygon;
    else if (are_string_equal(type_name, GeometryCollection))
      type = QcWkbGeometryType::GeometryCollection;
    else
      throw std::invalid_argument("bad type");

    return type;
  }

  QcVectorDouble
  parse_point()
  {
    qDebug() << "parse_point";

    // Start after (
    // a space is mandatory in the grammar in order to separate coordinate components
    int separator_position = m_stream.indexOf(' ', m_location);
    if (separator_position == -1)
      throw_parser_error(QStringLiteral("expected ' '"));
    double x = slice_stream(m_location, separator_position).toDouble();

    skip_space();

    m_location = separator_position + 1;
    int next_location = find_next_char(QStringLiteral(",)")); // space ?
    if (next_location == -1)
      throw_parser_error(QStringLiteral("expected ',' or ')'"));
    double y = slice_stream(m_location, next_location).toDouble();

    m_location = next_location;
    qDebug() << x << y;
    return QcVectorDouble(x, y);
  }

  QcVector3DDouble
  parse_point_3d()
  {
    qDebug() << "parse_point_3d";

    // Start after (
    // a space is mandatory in the grammar in order to separate coordinate components
    int separator_position = m_stream.indexOf(' ', m_location);
    if (separator_position == -1)
      throw_parser_error(QStringLiteral("expected ' '"));
    double x = slice_stream(m_location, separator_position).toDouble();

    skip_space();

    m_location = separator_position + 1;
    int next_location = find_next_char(QStringLiteral(",)")); // space ?
    if (next_location == -1)
      throw_parser_error(QStringLiteral("expected ',' or ')'"));
    double y = slice_stream(m_location, next_location).toDouble();

    skip_space();

    m_location = separator_position + 1;
    next_location = find_next_char(QStringLiteral(",)")); // space ?
    if (next_location == -1)
      throw_parser_error(QStringLiteral("expected ',' or ')'"));
    double z = slice_stream(m_location, next_location).toDouble();

    m_location = next_location;
    qDebug() << x << y << z;
    return QcVector3DDouble(x, y, z);
  }

  QcVectorDoubleList
  parse_points()
  {
    qDebug() << "parse_points";

    QcVectorDoubleList points;
    // Start after (
    QChar next_char = lookup_current_char();
    bool use_parenthesis_syntax = next_char == '(';
    while (m_location < m_location_end and next_char != ')') {
      if (use_parenthesis_syntax) {
        if (lookup_current_char() == '(')
          m_location++;
        else
          throw_parser_error(QStringLiteral("expected '('"));
      }
      points << parse_point();
      if (use_parenthesis_syntax) {
        if (lookup_current_char() == ')')
          m_location++;
        else
          throw_parser_error(QStringLiteral("expected ')'"));
      }
      skip_space();
      next_char = lookup_current_char();
      if (next_char == ',')
        m_location++;
      skip_space();
    }

    // qDebug() << points;
    return points;
  }

  QList<QcVectorDoubleList>
  parse_list_of_list()
  {
    qDebug() << "parse_list_of_list";

    QList<QcVectorDoubleList> list;

    while (lookup_current_char() != ')') {
      if (lookup_current_char() == '(') {
        m_location++;
        list << parse_points();
      } else
        throw_parser_error(QStringLiteral("expected '('"));
      if (lookup_current_char() == ')')
        m_location++;
      else
        throw_parser_error(QStringLiteral("expected ')'"));
      if (lookup_current_char() == ',')
        m_location++;
      skip_space();
    }

    qDebug() << list;

    return list;
  }

  QcWkbGeometryObject *
  parse_geometry()
  {
    QcWkbGeometryType type = parse_geometry_type();
    QcWkbGeometryObject * geometry_object = QcWkbGeometryObject::new_geometry_object(type);
    if (!geometry_object)
      throw_parser_error(QStringLiteral("Unimplemented geometry type"));
    parse_geometry_values(geometry_object);
    return geometry_object;
  }

  QList<const QcWkbGeometryObject *>
  parse_geometries()
  {
    qDebug() << "parse_geometries";

    QList<const QcWkbGeometryObject *> geometries;

    while (is_location_valid()) {
      skip_space();
      geometries << parse_geometry();
      skip_space();
      QChar next_char = lookup_current_char();
      if (next_char == ',')
        m_location++;
      else if (next_char == ')')
        break;
      else
        throw_parser_error(QStringLiteral("expected ',' or ')'"));
    }

    return geometries;
  }

  void
  parse_geometry_values(QcWkbGeometryObject * geometry_object)
  {
    // Must be located on '('

    qDebug() << "parse_geometry_values";

    QChar next_char = lookup_current_char();
    qDebug() << "open" << m_location << next_char;
    if (next_char == '(') {
      m_location++; // check
      skip_space();
      geometry_object->set_from_wkt(this);
    } else if (match_string(Empty)) {
      return;
    } else
      throw_parser_error(QStringLiteral("expected '(' or 'EMPTY'"));

    skip_space();
    next_char = lookup_current_char();
    qDebug() << "close" << m_location << next_char;
    if (next_char == ')') {
      m_location++;
    } else
      throw_parser_error(QStringLiteral("expected ')'"));
  }

private:
  int m_location;
  int m_location_end;
  const QString & m_stream;
};

/**************************************************************************************************/

QcWkbGeometryObject::QcWkbGeometryObject()
  : m_srid(-1)
{}

bool
QcWkbGeometryObject::read_byte_order(QDataStream & stream)
{
  quint8 byte_order;
  stream >> byte_order;
  bool is_big_endian = byte_order == 0; // Fixme: WkbByteOrder::Xdr
  qDebug() << "is big endian" << is_big_endian;
  stream.setByteOrder(is_big_endian ? QDataStream::BigEndian : QDataStream::LittleEndian);
  return is_big_endian;
}

void
QcWkbGeometryObject::write_byte_order(QDataStream & stream, bool use_big_endian)
{
  stream << static_cast<quint8>(use_big_endian ? 0 : 1);
  stream.setByteOrder(use_big_endian ? QDataStream::BigEndian : QDataStream::LittleEndian);
}

QcWkbGeometryType
QcWkbGeometryObject::read_header(QDataStream & stream, bool & is_ewkb)
{
  read_byte_order(stream);

  quint32 type;
  stream >> type;

  is_ewkb = type & static_cast<quint32>(QcWkbGeometryType::SRID);
  if (is_ewkb)
    type -= static_cast<quint32>(QcWkbGeometryType::SRID);

  qDebug() << "geometry type is" << type;
  switch (type) {
  case static_cast<quint32>(QcWkbGeometryType::Point):
    return QcWkbGeometryType::Point;
  case static_cast<quint32>(QcWkbGeometryType::PointM):
    return QcWkbGeometryType::PointM;
  case static_cast<quint32>(QcWkbGeometryType::LineString):
    return QcWkbGeometryType::LineString;
  case static_cast<quint32>(QcWkbGeometryType::Polygon):
    return QcWkbGeometryType::Polygon;
  case static_cast<quint32>(QcWkbGeometryType::MultiPoint):
    return QcWkbGeometryType::MultiPoint;
  case static_cast<quint32>(QcWkbGeometryType::MultiLineString):
    return QcWkbGeometryType::MultiLineString;
  case static_cast<quint32>(QcWkbGeometryType::MultiPolygon):
    return QcWkbGeometryType::MultiPolygon;
  case static_cast<quint32>(QcWkbGeometryType::GeometryCollection):
    return QcWkbGeometryType::GeometryCollection;
  default:
    throw std::invalid_argument("bad type");
  }
}

int
QcWkbGeometryObject::read_srid(QDataStream & stream)
{
  quint32 srid;
  stream >> srid;
  qDebug() << "SRID" << srid;
  return srid;
}

void
QcWkbGeometryObject::write_srid(QDataStream & stream)
{
  if (has_srid())
    stream << static_cast<quint32>(m_srid);
}

void
QcWkbGeometryObject::write_header(QDataStream & stream, bool use_big_endian, bool use_ewkb) const
{
  write_byte_order(stream, use_big_endian);
  quint32 type = static_cast<quint32>(geometry_type());
  if (use_ewkb) {
    if (!has_srid())
      throw std::invalid_argument("ewkb require a valid srid");;
    type |= static_cast<quint32>(QcWkbGeometryType::SRID);
  }
  stream << type;
  if (use_ewkb)
    stream << static_cast<quint32>(srid());
}

QcWkbGeometryObject *
QcWkbGeometryObject::new_geometry_object(QcWkbGeometryType type)
{
  switch (type) {
  case QcWkbGeometryType::Point:
    return new QcWkbPoint;
    break;
  case QcWkbGeometryType::Polygon:
    return new QcWkbPolygon;
    break;
  case QcWkbGeometryType::LineString:
    return new QcWkbLineString;
    break;
  case QcWkbGeometryType::MultiPoint:
    return new QcWkbMultiPoint;
    break;
  case QcWkbGeometryType::GeometryCollection:
    return new QcWkbGeometryCollection;
    break;
  default:
    return nullptr;
  }
}

QcWkbGeometryObject *
QcWkbGeometryObject::read_geometry_object(QDataStream & stream)
{
  bool is_ewkb;
  QcWkbGeometryType type = read_header(stream, is_ewkb);

  QcWkbGeometryObject * object;
  switch (type) {
  case QcWkbGeometryType::Point:
    object = new QcWkbPoint;
    break;
  case QcWkbGeometryType::Polygon:
    object = new QcWkbPolygon;
    break;
  case QcWkbGeometryType::LineString:
    object = new QcWkbLineString;
    break;
  case QcWkbGeometryType::MultiPoint:
    object = new QcWkbMultiPoint;
    break;
  default:
    object = nullptr;
  }

  if (object)
    object->set_from_binary(stream);

  if (is_ewkb)
    object->set_srid(read_srid(stream));

  return object;
}

void
QcWkbGeometryObject::init_from_binary(const QByteArray & bytes)
{
  QDataStream stream(bytes);
  bool is_ewkb;
  QcWkbGeometryType type = read_header(stream, is_ewkb);
  if (is_ewkb)
    set_srid(read_srid(stream));
  if (type != geometry_type())
    throw std::invalid_argument("wrong type");
  set_from_binary(stream);
}

QcVectorDouble
QcWkbGeometryObject::read_point(QDataStream & stream)
{
  double x, y;
  stream >> x >> y;
  return QcVectorDouble(x, y);
}

QcVector3DDouble
QcWkbGeometryObject::read_point_3d(QDataStream & stream)
{
  double x, y, z;
  stream >> x >> y >> z;
  return QcVector3DDouble(x, y, z);
}

/*
QcVector4DDouble
QcWkbGeometryObject::read_point_4d(QDataStream & stream)
{
  double x, y, z, m;
  stream >> x >> y >> z >> m;
  return QcVector4DDouble(x, y, z, m);
}
*/

void
QcWkbGeometryObject::write_point(QDataStream & stream, const QcVectorDouble & point)
{
  stream << point.x() << point.y();
}

void
QcWkbGeometryObject::write_point_3d(QDataStream & stream, const QcVector3DDouble & point)
{
  stream << point.x() << point.y() << point.z();
}

/*
void
QcWkbGeometryObject::write_point_4d(QDataStream & stream, const QcVector4DDouble & point)
{
  stream << point.x() << point.y() << point.z() << point.t();
}
*/

void
QcWkbGeometryObject::read_points(QDataStream & stream, QcVectorDoubleList & points)
{
  quint32 number_of_points;
  stream >> number_of_points;
  for (quint32 i = 0; i < number_of_points; i++)
    points << read_point(stream);
}

void
QcWkbGeometryObject::write_points(QDataStream & stream, const QcVectorDoubleList & points)
{
  stream << static_cast<quint32>(points.size());
  for (const auto & point : points)
    write_point(stream, point);
}

QByteArray
QcWkbGeometryObject::to_wkb(bool use_big_endian) const
{
  QByteArray bytes;
  QDataStream stream(&bytes, QIODevice::WriteOnly);
  to_binary(stream, use_big_endian);
  return bytes;
}

QcWkbGeometryObject *
QcWkbGeometryObject::init_from_wkt(const QString & stream)
{
  QcWktParser parser(stream);
  return parser.parse_geometry();
}

QString
QcWkbGeometryObject::srid_to_ewkt() const
{
  if (has_srid())
    return QStringLiteral("SRID=") + QString::number(m_srid) + ';';
  else
    return QString();
}

QString
QcWkbGeometryObject::point_to_wkt(const QcVectorDouble & point)
{
  QString wkt;
  wkt += QString::number(point.x()) + ' ' + QString::number(point.y());
  return wkt;
}

QString
QcWkbGeometryObject::point_3d_to_wkt(const QcVector3DDouble & point)
{
  QString wkt;
  wkt += point_to_wkt(point) + ' ' + QString::number(point.z());
  return wkt;
}

/*
QString
QcWkbGeometryObject::point_4d_to_wkt(const QcVector4DDouble & point)
{
  QString wkt;
  wkt += point_3d_to_wkt(point) + ' ' + QString::number(point.t());
  return wkt;
}
*/

QString
QcWkbGeometryObject::points_to_wkt(const QcVectorDoubleList & points)
{
  QString wkt;
  if (points.size()) {
    wkt += '(';
    int i = 0;
    for (const auto & point : points) {
      if (i++)
        wkt += QStringLiteral(", ");
      wkt += point_to_wkt(point);
    }
    wkt += ')';
  }
  return wkt;
}

/**************************************************************************************************/

QcWkbPoint::QcWkbPoint()
  : QcWkbGeometryObject(),
    m_point()
{}

QcWkbPoint::QcWkbPoint(const QByteArray & bytes)
  : QcWkbPoint()
{
  init_from_binary(bytes);
}

QcWkbPoint::QcWkbPoint(double x, double y)
  : QcWkbGeometryObject(),
    m_point(x, y)
{}

QcWkbPoint::QcWkbPoint(const QcWkbPoint & other)
  : QcWkbPoint(other.x(), other.y())
{}

QcWkbPoint &
QcWkbPoint::operator=(const QcWkbPoint & other)
{
  if (this != &other) {
    m_point = other.m_point;
  }

  return *this;
}

bool
QcWkbPoint::operator==(const QcWkbPoint & other) const
{
  return m_point == other.m_point;
}

void
QcWkbPoint::set_from_wkt(QcWktParser * parser)
{
  m_point = parser->parse_point();
}

void
QcWkbPoint::set_from_binary(QDataStream & stream)
{
  m_point = read_point(stream);
}

void
QcWkbPoint::to_binary(QDataStream & stream, bool use_big_endian) const
{
  write_header(stream, use_big_endian);
  write_point(stream, m_point);
}

QString
QcWkbPoint::to_wkt() const
{
  QString wkt = srid_to_ewkt();
  wkt += geometry_type_name() + '(' + point_to_wkt(m_point) + ')';
  return wkt;
}

/**************************************************************************************************/

QcWkbPointM::QcWkbPointM()
  : QcWkbGeometryObject(),
    m_point()
{}

QcWkbPointM::QcWkbPointM(const QByteArray & bytes)
  : QcWkbPointM()
{
  init_from_binary(bytes);
}

QcWkbPointM::QcWkbPointM(double x, double y, double m)
  : QcWkbGeometryObject(),
  m_point(x, y, m)
{}

QcWkbPointM::QcWkbPointM(const QcWkbPointM & other)
  : QcWkbPointM(other.x(), other.y(), other.m())
{}

QcWkbPointM &
QcWkbPointM::operator=(const QcWkbPointM & other)
{
  if (this != &other) {
    m_point = other.m_point;
  }

  return *this;
}

bool
QcWkbPointM::operator==(const QcWkbPointM & other) const
{
  return m_point == other.m_point;
}

void
QcWkbPointM::set_from_wkt(QcWktParser * parser)
{
  m_point = parser->parse_point_3d();
}

void
QcWkbPointM::set_from_binary(QDataStream & stream)
{
  m_point = read_point_3d(stream);
}

void
QcWkbPointM::to_binary(QDataStream & stream, bool use_big_endian) const
{
  write_header(stream, use_big_endian);
  write_point_3d(stream, m_point);
}

QString
QcWkbPointM::to_wkt() const
{
  QString wkt;
  wkt += geometry_type_name() + '(' + point_3d_to_wkt(m_point) + ')';
  return wkt;
}

/*
QcWkbPointM::QcWkbPointM()
  : QcWkbPoint(),
  m_m()
{}

QcWkbPointM::QcWkbPointM(const QByteArray & bytes)
  : QcWkbPointM()
{
  init_from_binary(bytes);
}

QcWkbPointM::QcWkbPointM(double x, double y, double m)
  : QcWkbPoint(x, y),
    m_m(m)
{}

QcWkbPointM::QcWkbPointM(const QcWkbPointM & other)
  : QcWkbPointM(other.x(), other.y(), other.m())
{}

QcWkbPointM &
QcWkbPointM::operator=(const QcWkbPointM & other)
{
  if (this != &other) {
    QcWkbPoint::operator=(other);
    m_m = other.m_m;
  }

  return *this;
}

bool
QcWkbPointM::operator==(const QcWkbPointM & other) const
{
  return QcWkbPoint::operator==(operator) and m_m == other.m_m;
}

void
QcWkbPointM::set_from_wkt(QcWktParser * parser)
{
  m_point = parser->parse_point();
}

void
QcWkbPointM::set_from_binary(QDataStream & stream)
{
  m_point = read_point(stream);
}

void
QcWkbPointM::to_binary(QDataStream & stream, bool use_big_endian) const
{
  write_header(stream, use_big_endian);
  write_point(stream, m_point);
}

QString
QcWkbPointM::to_wkt() const
{
  QString wkt;
  wkt += geometry_type_name() + '(' + point_to_wkt(m_point) + ')';
  return wkt;
}
*/

/**************************************************************************************************/

QcWkbPointList::QcWkbPointList()
  : QcWkbGeometryObject(),
  m_points()
{}

QcWkbPointList::QcWkbPointList(const QcWkbPointList & other)
  : QcWkbGeometryObject(),
  m_points(other.m_points)
{}

QcWkbPointList &
QcWkbPointList::operator=(const QcWkbPointList & other)
{
  if (this != &other) {
    m_points = other.m_points;
  }

  return *this;
}

void
QcWkbPointList::set_from_wkt(QcWktParser * parser)
{
  m_points = parser->parse_points();
}

void
QcWkbPointList::set_from_binary(QDataStream & stream)
{
  read_points(stream, m_points);
}

void
QcWkbPointList::to_binary(QDataStream & stream, bool use_big_endian) const
{
  write_header(stream, use_big_endian);
  write_points(stream, m_points);
}

QString
QcWkbPointList::to_wkt() const
{
  QString wkt;
  wkt += geometry_type_name();
  if (m_points.size())
    wkt += points_to_wkt(m_points);
  else {
    wkt += ' ' + Empty;
  }
  return wkt;
}

/**************************************************************************************************/

QcWkbLineString::QcWkbLineString()
  : QcWkbPointList()
{}

QcWkbLineString::QcWkbLineString(const QByteArray & bytes)
  : QcWkbLineString()
{
  init_from_binary(bytes);
}

QcWkbLineString::QcWkbLineString(const QcWkbLineString & other)
  : QcWkbPointList(other)
{}

QcWkbLineString &
QcWkbLineString::operator=(const QcWkbLineString & other)
{
  if (this != &other) {
    QcWkbPointList::operator=(other);
  }

  return *this;
}

/**************************************************************************************************/

QcWkbPolygon::QcWkbPolygon()
  : QcWkbGeometryObject(),
  m_rings()
{}

QcWkbPolygon::QcWkbPolygon(const QByteArray & bytes)
  : QcWkbPolygon()
{
  init_from_binary(bytes);
}

QcWkbPolygon::QcWkbPolygon(const QcWkbPolygon & other)
  : QcWkbGeometryObject(),
  m_rings(other.m_rings)
{}

QcWkbPolygon &
QcWkbPolygon::operator=(const QcWkbPolygon & other)
{
  if (this != &other) {
    m_rings = other.m_rings;
  }

  return *this;
}

void
QcWkbPolygon::set_from_wkt(QcWktParser * parser)
{
  m_rings = parser->parse_list_of_list();
}

void
QcWkbPolygon::set_from_binary(QDataStream & stream)
{
  quint32 number_of_rings;
  stream >> number_of_rings;
  for (quint32 i = 0; i < number_of_rings; i++) {
    QcVectorDoubleList ring;
    read_points(stream, ring);
    m_rings << ring; // Fixme: copy ?
  }
}

void
QcWkbPolygon::to_binary(QDataStream & stream, bool use_big_endian) const
{
  write_header(stream, use_big_endian);
  stream << static_cast<quint32>(m_rings.size());
  for (const auto & ring : m_rings)
    write_points(stream, ring);
}

void
QcWkbPolygon::add_ring(const QcVectorDoubleList & ring)
{
  m_rings << ring;
}

QString
QcWkbPolygon::to_wkt() const
{
  QString wkt;
  wkt += geometry_type_name();
  if (m_rings.size()) {
    wkt += '(';
    int i = 0;
    for (const auto & ring : m_rings) {
      if (i++)
        wkt += QStringLiteral(", ");
      wkt += points_to_wkt(ring);
    }
    wkt += ')';
  } else {
    wkt += ' ' + Empty;
  }
  return wkt;
}

/**************************************************************************************************/

QcWkbMultiPoint::QcWkbMultiPoint()
  : QcWkbPointList()
{}

QcWkbMultiPoint::QcWkbMultiPoint(const QByteArray & bytes)
  : QcWkbMultiPoint()
{
  init_from_binary(bytes);
}

QcWkbMultiPoint::QcWkbMultiPoint(const QcWkbMultiPoint & other)
  : QcWkbPointList(other)
{}

QcWkbMultiPoint &
QcWkbMultiPoint::operator=(const QcWkbMultiPoint & other)
{
  if (this != &other) {
    QcWkbPointList::operator=(other);
  }

  return *this;
}

/**************************************************************************************************/

QcWkbGeometryCollection::QcWkbGeometryCollection()
  : QcWkbGeometryObject()
{}

QcWkbGeometryCollection::QcWkbGeometryCollection(const QByteArray & bytes)
  : QcWkbGeometryObject()
{
  init_from_binary(bytes);
}

QcWkbGeometryCollection::QcWkbGeometryCollection(const QcWkbGeometryCollection & other)
  : QcWkbGeometryObject(),
  m_geometries(other.m_geometries)
{}

QcWkbGeometryCollection &
QcWkbGeometryCollection::operator=(const QcWkbGeometryCollection & other)
{
  if (this != &other) {
    m_geometries = other.m_geometries;
  }

  return *this;
}

void
QcWkbGeometryCollection::set_from_wkt(QcWktParser * parser)
{
  m_geometries = parser->parse_geometries();
}

void
QcWkbGeometryCollection::set_from_binary(QDataStream & stream)
{
  quint32 number_of_geometries;
  stream >> number_of_geometries;

  for (quint32 i = 0; i < number_of_geometries; i++) {
    QcWkbGeometryObject * object = read_geometry_object(stream);
    if (object)
      m_geometries << object;
  }
}

void
QcWkbGeometryCollection::add_geometry(const QcWkbGeometryObject & geometry_object)
{
  m_geometries << &geometry_object;
}

void
QcWkbGeometryCollection::to_binary(QDataStream & stream, bool use_big_endian) const
{
  write_header(stream, use_big_endian);
  stream << static_cast<quint32>(m_geometries.size());
  for (const auto * geometry : m_geometries)
    geometry->to_binary(stream, use_big_endian);
}

QString
QcWkbGeometryCollection::to_wkt() const
{
  QString wkt;
  wkt += geometry_type_name();
  wkt += '(';
  if (m_geometries.size()) {
    int i = 0;
    for (const auto * geometry : m_geometries) {
      if (i++)
        wkt += QStringLiteral(", ");
      wkt += geometry->to_wkt();
    }
    wkt += ')';
  } else
    wkt += ' ' + Empty;
  return wkt;
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
