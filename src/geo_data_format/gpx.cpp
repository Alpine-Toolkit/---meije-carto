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

#include "gpx.h"
#include "waypoint.h"

#include <QFile>
#include <QtDebug>

/**************************************************************************************************/

QcGpx::QcGpx()
  : m_creator(),
    m_version(),
    m_name(),
    m_description(),
    m_author(),
    m_link(),
    m_time(),
    m_keywords(),
    m_bounds(),
    m_waypoints(),
    m_routes(),
    m_tracks()
{}

QcGpx::QcGpx(const QcGpx & other)
  : m_creator(other.m_creator),
    m_version(other.m_version),
    m_name(other.m_name),
    m_description(other.m_description),
    m_author(other.m_author),
    m_link(other.m_link),
    m_time(other.m_time),
    m_keywords(other.m_keywords),
    m_bounds(other.m_bounds),
    m_waypoints(other.m_waypoints),
    m_routes(other.m_routes),
    m_tracks(other.m_tracks)
{}

QcGpx::~QcGpx()
{}

QcGpx &
QcGpx::operator=(const QcGpx & other)
{
  if (this != &other) {
    m_creator = other.m_creator;
    m_version = other.m_version;
    m_name = other.m_name;
    m_description = other.m_description;
    m_author = other.m_author;
    m_link = other.m_link;
    m_time = other.m_time;
    m_keywords = other.m_keywords;
    m_bounds = other.m_bounds;
    m_waypoints = other.m_waypoints;
    m_routes = other.m_routes;
    m_tracks = other.m_tracks;
  }

  return *this;
}

const QString &
QcGpx::creator() const
{
  return m_creator;
}

void
QcGpx::set_creator(const QString & creator)
{
  m_creator = creator;
}

const QString &
QcGpx::version() const
{
  return m_version;
}

void
QcGpx::set_version(const QString & version)
{
  m_version = version;
}

const QString &
QcGpx::name() const
{
  return m_name;
}

void
QcGpx::set_name(const QString & name)
{
  m_name = name;
}

const QString &
QcGpx::description() const
{
  return m_description;
}

void
QcGpx::set_description(const QString & description)
{
  m_description = description;
}

const QString &
QcGpx::author() const
{
  return m_author;
}

void
QcGpx::set_author(const QString & author)
{
  m_author = author;
}

const QString &
QcGpx::link() const
{
  return m_link;
}

void
QcGpx::set_link(const QString & link)
{
  m_link = link;
}

QDateTime
QcGpx::time() const
{
  return m_time;
}

void
QcGpx::set_time(const QDateTime & time)
{
  m_time = time;
}

const QString &
QcGpx::keywords() const
{
  return m_keywords;
}

void
QcGpx::set_keywords(const QString & keywords)
{
  m_keywords = keywords;
}

QcInterval2DDouble
QcGpx::bounds() const
{
  return m_bounds;
}

void
QcGpx::set_bounds(const QcInterval2DDouble & bounds)
{
  m_bounds = bounds;
}

const QcWayPointList &
QcGpx::waypoints() const
{
  return m_waypoints;
}

const QList<QcRoute> &
QcGpx::routes() const
{
  return m_routes;
}

const QList<QcTrack> &
QcGpx::tracks() const
{
  return m_tracks;
}

void
QcGpx::add_waypoint(const QcWayPoint & waypoint)
{
  m_waypoints.append(waypoint);
}

void
QcGpx::add_route(const QcRoute & route)
{
  m_routes.append(route);
}

void
QcGpx::add_track(const QcTrack & track)
{
  m_tracks.append(track);
}


/**************************************************************************************************/

const QString GPX_ELEMENT = "gpx";
const QString METADATA_ELEMENT = "metadata";
const QString WAYPOINT_ELEMENT = "wpt";
const QString ROUTE_ELEMENT = "rte";
const QString ROUTE_POINT_ELEMENT = "rtept";
const QString TRACK_ELEMENT = "trk";
const QString TRACK_SEGMENT_ELEMENT = "trkseg";
const QString TRACK_POINT_ELEMENT = "trkpt";

const QString CREATOR_ATTRIBUTE = "creator";
const QString LATITUDE_ATTRIBUTE = "latitude";
const QString LONGITUDE_ATTRIBUTE = "longitude";
const QString MAX_LATITUDE_ATTRIBUTE = "maxlat";
const QString MAX_LONGITUDE_ATTRIBUTE = "maxlon";
const QString MIN_LATITUDE_ATTRIBUTE = "minlat";
const QString MIN_LONGITUDE_ATTRIBUTE = "minlon";
const QString VERSION_ATTRIBUTE = "version";

const QString AGE_OF_DGPS_DATA_ELEMENT = "ageofdgpsdata";
const QString AUTHOR_ELEMENT = "author";
const QString BOUNDS_ELEMENT = "bounds";
const QString COMMENT_ELEMENT = "cmt";
const QString COPYRIGHT_ELEMENT = "copyright";
const QString DESCRIPTION_ELEMENT = "desc";
const QString DGPS_ID_ELEMENT = "dgpsid";
const QString ELEVATION_ELEMENT = "ele";
const QString EXTENSIONS_ELEMENT = "extensions";
const QString FIX_ELEMENT = "fix";
const QString GEOID_HEIGHT_ELEMENT = "geoidheight";
const QString HDOP_ELEMENT = "hdop";
const QString KEYWORDS_ELEMENT = "keywords";
const QString LINK_ELEMENT = "link";
const QString MAGNETIC_VARIATION_ELEMENT = "magvar";
const QString NAME_ELEMENT = "name";
const QString NUMBER_ELEMENT = "number";
const QString PDOP_ELEMENT = "pdop";
const QString SATELLITE_ELEMENT = "sat";
const QString SOURCE_ELEMENT = "src";
const QString SYMBOL_ELEMENT = "sym";
const QString TIME_ELEMENT = "time";
const QString TYPE_ELEMENT = "type";
const QString VDOP_ELEMENT = "vdop";

/**************************************************************************************************/

QcGpxReader::QcGpxReader()
  : m_reader()
{}

QcGpxReader::~QcGpxReader()
{}

QcGpx
QcGpxReader::read(const QString & gpx_path)
{
  qInfo() << "QcGpxReader::read";

  QcGpx gpx;

  QFile file(gpx_path);
  if (!file.open(QIODevice::ReadOnly))
    goto parse_error;

  m_reader.clear();
  m_reader.setDevice(&file);

  if (m_reader.readNext() != QXmlStreamReader::StartDocument)
    goto parse_error;
  if (! m_reader.read_match_start_element(GPX_ELEMENT))
    goto parse_error;
  gpx.set_creator(m_reader.get_attribute(CREATOR_ATTRIBUTE));
  gpx.set_version(m_reader.get_attribute(VERSION_ATTRIBUTE));

  while (! m_reader.read_match_end_element(GPX_ELEMENT))
    if (m_reader.isStartElement()) {
      QStringRef ename = m_reader.name();
      if (ename == METADATA_ELEMENT)
        read_metadata(gpx);
      else if (ename == WAYPOINT_ELEMENT)
        gpx.add_waypoint(read_waypoint(WAYPOINT_ELEMENT));
      else if (ename == ROUTE_ELEMENT)
        gpx.add_route(read_route());
      else if (ename == TRACK_ELEMENT)
        gpx.add_track(read_track());
      else if (ename == EXTENSIONS_ELEMENT)
        ;
      else
        goto parse_error;
    }

 parse_error:
  return gpx;
}

void
QcGpxReader::read_metadata(QcGpx & gpx)
{
  qInfo() << "QcGpxReader::read_metadata";

  while (! m_reader.read_match_end_element(METADATA_ELEMENT))
    if (m_reader.isStartElement()) {
      QStringRef ename = m_reader.name();
      if (ename == NAME_ELEMENT)
        gpx.set_name(m_reader.readElementText());
      else if (ename == DESCRIPTION_ELEMENT)
        gpx.set_description(m_reader.readElementText());
      else if (ename == AUTHOR_ELEMENT)
        ;
      else if (ename == COPYRIGHT_ELEMENT)
        ;
      else if (ename == LINK_ELEMENT)
        ;
      else if (ename == TIME_ELEMENT)
        gpx.set_time(m_reader.read_date());
      else if (ename == KEYWORDS_ELEMENT)
        gpx.set_keywords(m_reader.readElementText());
      else if (ename == BOUNDS_ELEMENT)
        read_bounds(gpx);
      else if (ename == EXTENSIONS_ELEMENT)
        ;
      // else
      //   goto parse_error;
    }
}

void
QcGpxReader::read_bounds(QcGpx & gpx)
{
  qInfo() << "QcGpxReader::read_bounds";
  double latitude_inf = m_reader.get_double_attribute(MIN_LATITUDE_ATTRIBUTE);
  double longitude_inf = m_reader.get_double_attribute(MIN_LONGITUDE_ATTRIBUTE);
  double latitude_sup = m_reader.get_double_attribute(MAX_LATITUDE_ATTRIBUTE);
  double longitude_sup = m_reader.get_double_attribute(MAX_LONGITUDE_ATTRIBUTE);
  gpx.set_bounds(QcInterval2DDouble(longitude_inf, longitude_sup, latitude_inf, latitude_sup));
}

QcWayPoint
QcGpxReader::read_waypoint(const QString & element)
{
  qInfo() << "QcGpxReader::read_waypoint" << element;

  QcWayPoint waypoint;

  double latitude = m_reader.get_double_attribute(LATITUDE_ATTRIBUTE);
  double longitude = m_reader.get_double_attribute(LONGITUDE_ATTRIBUTE);
  QcGeoElevationCoordinateWGS84 coordinate(longitude, latitude, .0);

  while (! m_reader.read_match_end_element(element))
    if (m_reader.isStartElement()) {
      QStringRef ename = m_reader.name();
      if (ename == ELEVATION_ELEMENT)
        coordinate.set_elevation(m_reader.read_double());
      else if (ename == TIME_ELEMENT)
        waypoint.set_time(m_reader.read_date());
      else if (ename == MAGNETIC_VARIATION_ELEMENT)
        waypoint.set_magnetic_variation(m_reader.read_double());
      else if (ename == GEOID_HEIGHT_ELEMENT)
        waypoint.set_geoid_height(m_reader.read_double());
      else if (ename == NAME_ELEMENT)
        waypoint.set_name(m_reader.readElementText());
      else if (ename == COMMENT_ELEMENT)
        waypoint.set_comment(m_reader.readElementText());
      else if (ename == DESCRIPTION_ELEMENT)
        waypoint.set_description(m_reader.readElementText());
      else if (ename == SOURCE_ELEMENT)
        waypoint.set_source(m_reader.readElementText());
      else if (ename == LINK_ELEMENT)
        ;
      else if (ename == SYMBOL_ELEMENT)
        waypoint.set_symbol(m_reader.readElementText());
      else if (ename == TYPE_ELEMENT)
        waypoint.set_type(m_reader.readElementText());
      else if (ename == FIX_ELEMENT)
        waypoint.set_fix_type(m_reader.readElementText());
      else if (ename == SATELLITE_ELEMENT)
        waypoint.set_number_of_satellites(m_reader.read_int());
      else if (ename == HDOP_ELEMENT)
        waypoint.set_hdop(m_reader.read_double());
      else if (ename == VDOP_ELEMENT)
        waypoint.set_vdop(m_reader.read_double());
      else if (ename == PDOP_ELEMENT)
        waypoint.set_pdop(m_reader.read_double()) ;
      else if (ename == AGE_OF_DGPS_DATA_ELEMENT)
        waypoint.set_age_of_dgps_data(m_reader.read_double());
      else if (ename == DGPS_ID_ELEMENT)
        waypoint.set_dgps_id(m_reader.read_double());
      else if (ename == EXTENSIONS_ELEMENT)
        ;
    }

  waypoint.set_coordinate(coordinate);

  // parse_waypoint_error:
  return waypoint;
}

bool
QcGpxReader::read_route_metadata(QcRouteMetaData & route, const QStringRef & ename)
{
  qInfo() << "QcGpxReader::read_route_metadata";

  if (ename == NAME_ELEMENT)
    route.set_name(m_reader.readElementText());
  else if (ename == COMMENT_ELEMENT)
    route.set_comment(m_reader.readElementText());
  else if (ename == DESCRIPTION_ELEMENT)
    route.set_description(m_reader.readElementText());
  else if (ename == SOURCE_ELEMENT)
    route.set_source(m_reader.readElementText());
  else if (ename == LINK_ELEMENT)
    ;
  else if (ename == NUMBER_ELEMENT)
    route.set_number(m_reader.read_int());
  else if (ename == TYPE_ELEMENT)
    route.set_type(m_reader.readElementText());
  else if (ename == EXTENSIONS_ELEMENT)
    ;
  else
    return false;

  return true;
}

QcRoute
QcGpxReader::read_route()
{
  qInfo() << "QcGpxReader::read_route";

  QcRoute route;

  while (! m_reader.read_match_end_element(ROUTE_ELEMENT))
    if (m_reader.isStartElement()) {
      QStringRef ename = m_reader.name();
      if (! read_route_metadata(route, ename)) {
        if (ename == ROUTE_POINT_ELEMENT)
          route.add_waypoint(read_waypoint(ROUTE_POINT_ELEMENT));
      }
    }

  return route;
}

QcTrack
QcGpxReader::read_track()
{
  qInfo() << "QcGpxReader::read_track";

  QcTrack track;

  while (! m_reader.read_match_end_element(TRACK_ELEMENT))
    if (m_reader.isStartElement()) {
      QStringRef ename = m_reader.name();
      if (! read_route_metadata(track, ename)) {
        if (ename == TRACK_SEGMENT_ELEMENT)
          track.add_segment(read_track_segment());
      }
    }

  return track;
}

QcWayPointList
QcGpxReader::read_track_segment()
{
  qInfo() << "QcGpxReader::read_track_segment";

  QcWayPointList segment;

  while (! m_reader.read_match_end_element(TRACK_SEGMENT_ELEMENT))
    if (m_reader.isStartElement()) {
      QStringRef ename = m_reader.name();
      if (ename == TRACK_POINT_ELEMENT)
          segment.append(read_waypoint(TRACK_POINT_ELEMENT));
      else if (ename == EXTENSIONS_ELEMENT)
        ;
      }

  return segment;
}

/**************************************************************************************************/

QcGpxWriter::QcGpxWriter()
  : m_writer()
{}

QcGpxWriter::~QcGpxWriter()
{}

void
QcGpxWriter::write(const QcGpx & gpx, const QString & gpx_path)
{
  qInfo() << "QcGpxWriter::read";

  QFile file(gpx_path);
  if (!file.open(QIODevice::WriteOnly))
    return; // Fixme:

  m_writer.setDevice(&file);

  m_writer.setAutoFormatting(true);

  m_writer.writeStartDocument();
  m_writer.writeStartElement(GPX_ELEMENT);
  m_writer.writeAttribute(QLatin1Literal("xsi:schemaLocation"), QLatin1Literal("http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd"));
  m_writer.writeAttribute(QLatin1Literal("xmlns:xsi"), QLatin1Literal("http://www.w3.org/2001/XMLSchema-instance"));
  m_writer.writeAttribute(QLatin1Literal("xmlns"), QLatin1Literal("http://www.topografix.com/GPX/1/1"));
  m_writer.writeAttribute(CREATOR_ATTRIBUTE, QLatin1Literal("Alpine ToolKit")); // gpx.creator()
  m_writer.writeAttribute(VERSION_ATTRIBUTE, QLatin1Literal("1.1")); // gpx.version()

  write_metadata(gpx);

  for (const QcWayPoint & waypoint : gpx.waypoints())
    write_waypoint(waypoint);
  for (const QcRoute & route : gpx.routes())
    write_route(route);
  for (const QcTrack & track : gpx.tracks())
    write_track(track);

  // EXTENSIONS_ELEMENT

  m_writer.writeEndElement(); // GPX_ELEMENT
  m_writer.writeEndDocument();
}

void
QcGpxWriter::write_metadata(const QcGpx & gpx)
{
  m_writer.writeStartElement(METADATA_ELEMENT);
  m_writer.writeTextElement(NAME_ELEMENT, gpx.name());
  m_writer.writeTextElement(DESCRIPTION_ELEMENT, gpx.description());
  // AUTHOR_ELEMENT
  // COPYRIGHT_ELEMENT
  // LINK_ELEMENT
  m_writer.writeTextElement(TIME_ELEMENT, gpx.time().toString(Qt::ISODate));
  m_writer.writeTextElement(KEYWORDS_ELEMENT, gpx.keywords());
  m_writer.writeStartElement(BOUNDS_ELEMENT);
  const QcInterval2DDouble & bounds = gpx.bounds();
  m_writer.writeAttribute(MIN_LATITUDE_ATTRIBUTE, QString::number(bounds.y().inf()));
  m_writer.writeAttribute(MIN_LONGITUDE_ATTRIBUTE, QString::number(bounds.x().inf()));
  m_writer.writeAttribute(MAX_LATITUDE_ATTRIBUTE, QString::number(bounds.y().sup()));
  m_writer.writeAttribute(MAX_LONGITUDE_ATTRIBUTE, QString::number(bounds.x().sup()));
  m_writer.writeEndElement(); // BOUNDS_ELEMENT
  m_writer.writeEndElement(); // METADATA_ELEMENT
}

void
QcGpxWriter::write_waypoint(const QcWayPoint & waypoint)
{
  const QcGeoElevationCoordinateWGS84 & coordinate = waypoint.coordinate();
  m_writer.writeAttribute(LATITUDE_ATTRIBUTE, QString::number(coordinate.latitude()));
  m_writer.writeAttribute(LONGITUDE_ATTRIBUTE, QString::number(coordinate.longitude()));
  m_writer.writeTextElement(ELEVATION_ELEMENT, QString::number(coordinate.elevation()));
  m_writer.writeTextElement(TIME_ELEMENT, waypoint.time().toString(Qt::ISODate));
  m_writer.writeTextElement(MAGNETIC_VARIATION_ELEMENT, QString::number(waypoint.magnetic_variation()));
  m_writer.writeTextElement(GEOID_HEIGHT_ELEMENT, QString::number(waypoint.geoid_height()));
  m_writer.writeTextElement(NAME_ELEMENT, waypoint.name());
  m_writer.writeTextElement(COMMENT_ELEMENT, waypoint.comment());
  m_writer.writeTextElement(DESCRIPTION_ELEMENT, waypoint.description());
  m_writer.writeTextElement(SOURCE_ELEMENT, waypoint.source());
  // LINK_ELEMENT
  m_writer.writeTextElement(SYMBOL_ELEMENT, waypoint.symbol());
  m_writer.writeTextElement(TYPE_ELEMENT, waypoint.type());
  m_writer.writeTextElement(FIX_ELEMENT, waypoint.fix_type());
  m_writer.writeTextElement(SATELLITE_ELEMENT, QString::number(waypoint.number_of_satellites()));
  m_writer.writeTextElement(HDOP_ELEMENT, QString::number(waypoint.hdop()));
  m_writer.writeTextElement(VDOP_ELEMENT, QString::number(waypoint.vdop()));
  m_writer.writeTextElement(PDOP_ELEMENT, QString::number(waypoint.pdop()));
  m_writer.writeTextElement(AGE_OF_DGPS_DATA_ELEMENT, QString::number(waypoint.age_of_dgps_data()));
  m_writer.writeTextElement(DGPS_ID_ELEMENT, QString::number(waypoint.dgps_id()));
  // EXTENSIONS_ELEMENT
}

void
QcGpxWriter::write_route_metadata(const QcRouteMetaData & metadata)
{
  m_writer.writeTextElement(NAME_ELEMENT, metadata.name());
  m_writer.writeTextElement(COMMENT_ELEMENT, metadata.comment());
  m_writer.writeTextElement(DESCRIPTION_ELEMENT, metadata.description());
  m_writer.writeTextElement(SOURCE_ELEMENT, metadata.source());
  // LINK_ELEMENT
  m_writer.writeTextElement(NUMBER_ELEMENT, QString::number(metadata.number()));
  m_writer.writeTextElement(TYPE_ELEMENT, metadata.type());
  // EXTENSIONS_ELEMENT
}

void
QcGpxWriter::write_route(const QcRoute & route)
{
  m_writer.writeStartElement(ROUTE_ELEMENT);
  write_route_metadata(route);
  for (const QcWayPoint & waypoint : route.waypoints()) {
    m_writer.writeStartElement(ROUTE_POINT_ELEMENT);
    write_waypoint(waypoint);
    m_writer.writeEndElement(); // ROUTE_POINT_ELEMENT
  }
  m_writer.writeEndElement(); // ROUTE_ELEMENT
}

void
QcGpxWriter::write_track(const QcTrack & track)
{
  m_writer.writeStartElement(TRACK_ELEMENT);
  write_route_metadata(track);
  for (const QcWayPointList & segment : track.segments()) {
    m_writer.writeStartElement(TRACK_SEGMENT_ELEMENT);
    for (const QcWayPoint & waypoint : segment) {
      m_writer.writeStartElement(TRACK_POINT_ELEMENT);
      write_waypoint(waypoint);
      m_writer.writeEndElement(); // TRACK_POINT_ELEMENT
    }
    m_writer.writeEndElement(); // TRACK_SEGMENT_ELEMENT
  }
  m_writer.writeEndElement(); // TRACK_ELEMENT
}


/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
