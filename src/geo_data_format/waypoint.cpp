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

#include "waypoint.h"

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcWayPoint::QcWayPoint()
  : m_coordinate(),
    m_time(),
    m_magnetic_variation(),
    m_geoid_height(),
    m_name(),
    m_comment(),
    m_description(),
    m_source(),
    m_link(),
    m_symbol(),
    m_type(),
    m_fix_type(),
    m_number_of_satellites(),
    m_hdop(),
    m_vdop(),
    m_pdop(),
    m_age_of_dgps_data(),
    m_dgps_id()
{}

QcWayPoint::QcWayPoint(const QcWayPoint & other)
  : m_coordinate(other.m_coordinate),
    m_time(other.m_time),
    m_magnetic_variation(other.m_magnetic_variation),
    m_geoid_height(other.m_geoid_height),
    m_name(other.m_name),
    m_comment(other.m_comment),
    m_description(other.m_description),
    m_source(other.m_source),
    m_link(other.m_link),
    m_symbol(other.m_symbol),
    m_type(other.m_type),
    m_fix_type(other.m_fix_type),
    m_number_of_satellites(other.m_number_of_satellites),
    m_hdop(other.m_hdop),
    m_vdop(other.m_vdop),
    m_pdop(other.m_pdop),
    m_age_of_dgps_data(other.m_age_of_dgps_data),
    m_dgps_id(other.m_dgps_id)
{}

QcWayPoint::~QcWayPoint()
{}

QcWayPoint &
QcWayPoint::operator=(const QcWayPoint & other)
{
  if (this != &other) {
    m_coordinate = other.m_coordinate;
    m_time = other.m_time;
    m_magnetic_variation = other.m_magnetic_variation;
    m_geoid_height = other.m_geoid_height;
    m_name = other.m_name;
    m_comment = other.m_comment;
    m_description = other.m_description;
    m_source = other.m_source;
    m_link = other.m_link;
    m_symbol = other.m_symbol;
    m_type = other.m_type;
    m_fix_type = other.m_fix_type;
    m_number_of_satellites = other.m_number_of_satellites;
    m_hdop = other.m_hdop;
    m_vdop = other.m_vdop;
    m_pdop = other.m_pdop;
    m_age_of_dgps_data = other.m_age_of_dgps_data;
    m_dgps_id = other.m_dgps_id;
  }

   return *this;
}

// Elevation (in meters) of the point.
QcGeoElevationCoordinateWGS84
QcWayPoint::coordinate() const
{
  return m_coordinate;
}

void
QcWayPoint::set_coordinate(const QcGeoElevationCoordinateWGS84 & coordinate)
{
  m_coordinate = coordinate;
}

/* Creation/modification timestamp for element. Date and time in are
 * in Univeral Coordinated Time (UTC), not local time! Conforms to ISO
 * 8601 specification for date/time representation. Fractional seconds
 * are allowed for millisecond timing in tracklogs.
 */
QDateTime
QcWayPoint::time() const
{
  return m_time;
}

void
QcWayPoint::set_time(const QDateTime & time)
{
  m_time = time;
}

// Magnetic variation (in degrees) at the point.
qreal
QcWayPoint::magnetic_variation() const
{
  return m_magnetic_variation;
}

void
QcWayPoint::set_magnetic_variation(qreal magnetic_variation)
{
  m_magnetic_variation = magnetic_variation;
}

// Height (in meters) of geoid (mean sea level) above WGS84 earth
// ellipsoid. As defined in NMEA GGA message.
qreal
QcWayPoint::geoid_height() const
{
  return m_geoid_height;
}

void
QcWayPoint::set_geoid_height(qreal geoid_height)
{
  m_geoid_height = geoid_height;
}

/* The GPS name of the waypoint. This field will be transferred to
 * and from the GPS. GPX does not place restrictions on the length
 * of this field or the characters contained in it. It is up to the
 * receiving application to validate the field before sending it to
 * the GPS.
 */
const QString &
QcWayPoint::name() const
{
  return m_name;
}

void
QcWayPoint::set_name(const QString & name)
{
  m_name = name;
}

// GPS waypoint comment. Sent to GPS as comment.
const QString &
QcWayPoint::comment() const
{
  return m_comment;
}

void
QcWayPoint::set_comment(const QString & comment)
{
  m_comment = comment;
}

// A text description of the element. Holds additional information
// about the element intended for the user, not the GPS.
const QString &
QcWayPoint::description() const
{
  return m_description;
}

void
QcWayPoint::set_description(const QString & description)
{
  m_description = description;
}

// Source of data. Included to give user some idea of reliability and
// accuracy of data. "Garmin eTrex", "USGS quad Boston North", e.g.
const QString &
QcWayPoint::source() const
{
  return m_source;
}

void
QcWayPoint::set_source(const QString & source)
{
  m_source = source;
}

// Link to additional information about the waypoint.
const QList<QString>
QcWayPoint::link() const
{
  return m_link;
}

void
QcWayPoint::add_link(const QString & link)
{
  m_link.append(link);
}

// Text of GPS symbol name. For interchange with other programs, use
// the exact spelling of the symbol as displayed on the GPS. If the
// GPS abbreviates words, spell them out.
const QString &
QcWayPoint::symbol() const
{
  return m_symbol;
}

void
QcWayPoint::set_symbol(const QString & symbol)
{
  m_symbol = symbol;
}

// Type (classification) of the waypoint.
const QString &
QcWayPoint::type() const
{
  return m_type;
}

void
QcWayPoint::set_type(const QString & type)
{
  m_type = type;
}

// Type of GPX fix.
const QString &
QcWayPoint::fix_type() const
{
  return m_fix_type;
}

void
QcWayPoint::set_fix_type(const QString & fix_type)
{
  m_fix_type = fix_type;
}

// Number of satellites used to calculate the GPX fix.
int
QcWayPoint::number_of_satellites() const
{
  return m_number_of_satellites;
}

void
QcWayPoint::set_number_of_satellites(int number_of_satellites)
{
  m_number_of_satellites = number_of_satellites;
}

// Horizontal dilution of precision.
qreal
QcWayPoint::hdop() const
{
  return m_hdop;
}

void
QcWayPoint::set_hdop(qreal hdop)
{
  m_hdop = hdop;
}

// Vertical dilution of precision.
qreal
QcWayPoint::vdop() const
{
  return m_vdop;
}

void
QcWayPoint::set_vdop(qreal vdop)
{
  m_vdop = vdop;
}

// Position dilution of precision.
qreal
QcWayPoint::pdop() const
{
  return m_pdop;
}

void
QcWayPoint::set_pdop(qreal pdop)
{
  m_pdop = pdop;
}

// Number of seconds since last DGPS update.
qreal
QcWayPoint::age_of_dgps_data() const
{
  return m_age_of_dgps_data;
}

void
QcWayPoint::set_age_of_dgps_data(qreal age_of_dgps_data)
{
  m_age_of_dgps_data = age_of_dgps_data;
}

// ID of DGPS station used in differential correction.
int
QcWayPoint::dgps_id() const
{
  return m_dgps_id;
}

void
QcWayPoint::set_dgps_id(int dgps_id)
{
  m_dgps_id = dgps_id;
}

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
