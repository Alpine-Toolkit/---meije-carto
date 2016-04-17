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

const QString AGE_OF_DGPS_DATA_ELEMENT = "ageofdgpsdata";
const QString COMMENT_ELEMENT = "cmt";
const QString DESCRIPTION_ELEMENT = "desc";
const QString DGPS_ID_ELEMENT = "dgpsid";
const QString ELEVATION_ELEMENT = "ele";
const QString EXTENSIONS_ELEMENT = "extensions";
const QString FIX_ELEMENT = "fix";
const QString GEOID_HEIGHT_ELEMENT = "geoidheight";
const QString HDOP_ELEMENT = "hdop";
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

const QString _ELEMENT = "";

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
  gpx.set_creator(m_reader.get_attribute(QLatin1String("creator")));
  gpx.set_version(m_reader.get_attribute(QLatin1String("version")));

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
      else if (ename == QLatin1String("author"))
        ;
      else if (ename == QLatin1String("copyright"))
        ;
      else if (ename == LINK_ELEMENT)
        ;
      else if (ename == TIME_ELEMENT)
        gpx.set_time(m_reader.read_date());
      else if (ename == QLatin1String("keywords"))
        gpx.set_keywords(m_reader.readElementText());
      else if (ename == QLatin1String("bounds"))
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
  double latitude_inf = m_reader.get_double_attribute(QLatin1String("minlat"));
  double longitude_inf = m_reader.get_double_attribute(QLatin1String("minlon"));
  double latitude_sup = m_reader.get_double_attribute(QLatin1String("maxlat"));
  double longitude_sup = m_reader.get_double_attribute(QLatin1String("maxlon"));
  gpx.set_bounds(QcInterval2DDouble(longitude_inf, longitude_sup, latitude_inf, latitude_sup));
}

QcWayPoint
QcGpxReader::read_waypoint(const QString & element)
{
  qInfo() << "QcGpxReader::read_waypoint" << element;

  QcWayPoint waypoint;

  double latitude = m_reader.get_double_attribute(QLatin1String("latitude"));
  double longitude = m_reader.get_double_attribute(QLatin1String("longitude"));
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
          route.add_point(read_waypoint(ROUTE_POINT_ELEMENT));
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

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
