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

#include <cmath>

#include <QDataStream>
#include <QDebug>
#include <qnumeric.h>

#include "geo_coordinate.h"

/**************************************************************************************************/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline static double deg_to_rad(double degrees)
{
  return degrees / 180. * M_PI;
}

inline static double rad_to_deg(double radians)
{
  return radians / M_PI * 180.;
}

inline static double haversine(double theta)
{
  // hav = sin(.5 * theta)**2
  return .5*(1 - cos(theta));
}

/**************************************************************************************************/

double
QcGeoAngle::to_decimal(int degrees, int minutes, double seconds)
{
  return degrees + (minutes + seconds / 60.) / 60.;
}

/*
void
QcGeoAngle::to_sexagesimal(double angle, int &degrees, int &minutes, double &second)
{
  double _degrees, _minutes;
  f = modf(angle, *_degrees);
  f = modf(f * 60, *_minutes);
  second = f * 60;

  degrees = cast<int>(_degrees);
  minutes = cast<int>(_minutes);
}
*/

QcGeoAngle::QcGeoAngle(double degrees, int minutes, double seconds)
  : m_value(seconds)
{
  // Fixme: check input
  m_value = to_decimal(degrees, minutes, seconds);
}

QcGeoAngle::~QcGeoAngle()
{}

QcGeoAngle &
QcGeoAngle::operator=(const QcGeoAngle &other)
{
  if (this != &other) {
    m_value = other.m_value;
  }

  return *this;
}

bool
QcGeoAngle::operator==(const QcGeoAngle &other) const
{
  return qFuzzyCompare(m_value, other.m_value);;
}

QcGeoSexagesimalAngle
QcGeoAngle::sexagesimal() const {
  // to_sexagesimal(m_value, degrees, minutes, second);

  double degrees, minutes, second, f;
  f = modf(m_value, &degrees);
  f = modf(f * 60, &minutes);
  second = f * 60;

  return QcGeoSexagesimalAngle(static_cast<int>(degrees), static_cast<int>(minutes), second);
}

/**************************************************************************************************/

QcGeoSexagesimalAngle::QcGeoSexagesimalAngle(int degrees, int minutes, double seconds)
  : m_degrees(0), m_minutes(0), m_seconds(qQNaN())
{
  if (is_valid_degrees(degrees) and is_valid_minutes(minutes)) {
    m_degrees = degrees;
    m_minutes = minutes;
    m_seconds = seconds;
  }
}

QcGeoSexagesimalAngle::QcGeoSexagesimalAngle(const QcGeoSexagesimalAngle &other)
: m_degrees(other.m_degrees), m_minutes(other.m_minutes), m_seconds(other.m_seconds)
{}

QcGeoSexagesimalAngle::~QcGeoSexagesimalAngle()
{}

QcGeoSexagesimalAngle &
QcGeoSexagesimalAngle::operator=(const QcGeoSexagesimalAngle &other)
{
  if (this != &other) {
    m_degrees = other.m_degrees;
    m_minutes = other.m_minutes;
    m_seconds = other.m_seconds;
  }

  return *this;
}

bool
QcGeoSexagesimalAngle::operator==(const QcGeoSexagesimalAngle &other) const
{
  bool degrees_equal = m_minutes == other.m_degrees;
  bool minutes_equal = m_minutes == other.m_minutes;
  bool seconds_equal = qFuzzyCompare(m_seconds, other.m_seconds);

  return (degrees_equal && minutes_equal && seconds_equal);
}

QcGeoAngle
QcGeoSexagesimalAngle::decimal() const {
  double degrees = QcGeoAngle::to_decimal(m_degrees, m_minutes, m_seconds);

  return QcGeoAngle(degrees);
}

/**************************************************************************************************/

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84(double latitude, double longitude)
  : m_latitude(qQNaN()), m_longitude(qQNaN())
{
  if (is_valid_latitude(latitude) && is_valid_longitude(longitude)) {
    m_latitude = latitude;
    m_longitude = longitude;
  }
}

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84()
  : QcGeoCoordinateWGS84(.0, .0)
{}

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84(const QcGeoCoordinateWGS84 &other)
  : m_latitude(other.m_latitude), m_longitude(other.m_longitude)
{}

QcGeoCoordinateWGS84::~QcGeoCoordinateWGS84()
{}

QcGeoCoordinateWGS84 &
QcGeoCoordinateWGS84::operator=(const QcGeoCoordinateWGS84 &other)
{
  if (this != &other) {
    m_latitude = other.m_latitude;
    m_longitude = other.m_longitude;
  }

  return *this;
}

bool
QcGeoCoordinateWGS84::operator==(const QcGeoCoordinateWGS84 &other) const
{
  bool latitude_equal = ((qIsNaN(m_latitude) && qIsNaN(other.m_latitude))
			 || qFuzzyCompare(m_latitude, other.m_latitude));
  bool longitude_equal = ((qIsNaN(m_longitude) && qIsNaN(other.m_longitude))
			  || qFuzzyCompare(m_longitude, other.m_longitude));

  // Fixme: compact
  if (!qIsNaN(m_longitude) && ((m_latitude == 90.0) || (m_latitude == -90.0)))
    longitude_equal = true;

  return (latitude_equal && longitude_equal);
}

QcGeoCoordinateMercator
QcGeoCoordinateWGS84::mercator() const
{
  double x = deg_to_rad(m_longitude);
  double y = log(tan(deg_to_rad(m_latitude)/2 + M_PI/4));
  x *= EQUATORIAL_RADIUS;
  y *= EQUATORIAL_RADIUS;
  // y = R/2 * math.log((1 + sin(latitude))/(1 - sin(latitude))

  return QcGeoCoordinateMercator(x, y);
}

/*!
  Returns the distance (in meters) from this coordinate to the coordinate
  specified by \a other.

  This calculation returns the great-circle distance between the two
  coordinates, with an assumption that the Earth is spherical for the
  purpose of this calculation.

  Returns 0 if the type of this coordinate or the type of \a other is
  QcGeoCoordinateWGS84::InvalidCoordinate.
*/
double QcGeoCoordinateWGS84::distance_to(const QcGeoCoordinateWGS84 &other) const
{
  /*
  if (type() == QcGeoCoordinateWGS84::InvalidCoordinate || other.type() == QcGeoCoordinateWGS84::InvalidCoordinate) {
    return 0;
  }
  */

  // Haversine formula https://en.wikipedia.org/wiki/Haversine_formula

  double latitude1 = deg_to_rad(m_latitude);
  double latitude2 = deg_to_rad(other.m_latitude);

  double delta_latitude = deg_to_rad(other.m_latitude - m_latitude);
  double delta_longitude = deg_to_rad(other.m_longitude - m_longitude);

  double haversine_delta_latitude = haversine(delta_latitude);
  double haversine_delta_longitude = haversine(delta_longitude);

  double f = haversine_delta_latitude + cos(latitude1) * cos(latitude2) * haversine_delta_longitude;
  return EQUATORIAL_DIAMETER * asin(sqrt(f)); // EARTH_MEAN_RADIUS
}

/*!
  Returns the azimuth (or bearing) in degrees from this coordinate to the
  coordinate specified by \a other.

  The bearing returned is the bearing from the origin to \a other along the
  great-circle between the two coordinates. There is an assumption that the
  Earth is spherical for the purpose of this calculation.

  Returns 0 if the type of this coordinate or the type of \a other is
  QcGeoCoordinateWGS84::InvalidCoordinate.
*/
double
QcGeoCoordinateWGS84::azimuth_to(const QcGeoCoordinateWGS84 &other) const
{
  /*
  if (type() == QcGeoCoordinateWGS84::InvalidCoordinate || other.type() == QcGeoCoordinateWGS84::InvalidCoordinate) {
    return 0;
  }
  */

  // Fixme: find reference

  double delta_longitude = deg_to_rad(other.m_longitude - m_longitude);
  double latitude1 = deg_to_rad(m_latitude);
  double latitude2 = deg_to_rad(other.m_latitude);

  double y = sin(delta_longitude) * cos(latitude2);
  double x = cos(latitude1) * sin(latitude2) - sin(latitude1) * cos(latitude2) * cos(delta_longitude);
  double theta = atan2(y, x);

  // Since atan2 returns values in the range [-π, +π] (that is, [-180°, +180°]),
  // to normalise the result to a compass bearing in the range [0°, 360°]
  // use (theta + 360) % 360

  // Fixme: check int -> double ...
  double whole;
  double fraction = modf(rad_to_deg(theta), &whole);
  return double((int(whole) + 360) % 360) + fraction;
}

/*!
  Returns the coordinate that is reached by traveling \a distance meters
  from the current coordinate at \a azimuth (or bearing) along a great-circle.
  There is an assumption that the Earth is spherical for the purpose of this
  calculation.

  Returns an invalid coordinate if this coordinate is invalid.
*/
QcGeoCoordinateWGS84
QcGeoCoordinateWGS84::at_distance_and_azimuth(double distance, double _azimuth) const
{
  /*
  if (!isValid())
    return QcGeoCoordinateWGS84();
  */

  // Fixme: find reference
  // http://www.movable-type.co.uk/scripts/latlong.html

  double latitude1 = deg_to_rad(m_latitude);
  double longitude1 = deg_to_rad(m_longitude);
  double azimuth = deg_to_rad(_azimuth);

  double cos_latitude1 = cos(latitude1);
  double sin_latitude1 = sin(latitude1);
  double cos_azimuth = cos(azimuth);
  double sin_azimuth = sin(azimuth);

  double delta = distance / EQUATORIAL_RADIUS; // EARTH_MEAN_RADIUS
  double cos_delta = cos(delta);
  double sin_delta = sin(delta);

  double latitude2_rad = asin(sin_latitude1 * cos_delta + cos_latitude1 * sin_delta * cos_azimuth);
  double longitude2_rad = longitude1 + atan2(sin_azimuth * sin_delta * cos_latitude1,
					     cos_delta - sin_latitude1 * sin(latitude2_rad));

  double latitude2 = rad_to_deg(latitude2_rad);
  double longitude2 = rad_to_deg(longitude2_rad);
  if (longitude2 > 180.0)
    longitude2 -= 360.0;
  else if (longitude2 < -180.0)
    longitude2 += 360.0;

  return QcGeoCoordinateWGS84(latitude2, longitude2);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const QcGeoCoordinateWGS84 &coordinate)
{
    QDebugStateSaver saver(debug);
    double latitude = coordinate.latitude();
    double longitude = coordinate.longitude();

    debug.nospace() << "QcGeoCoordinateWGS84(";
    if (qIsNaN(latitude))
        debug << '?';
    else
        debug << latitude;
    debug << ", ";
    if (qIsNaN(longitude))
        debug << '?';
    else
        debug << longitude;
    debug << ')';
    return debug;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &stream, const QcGeoCoordinateWGS84 &coordinate)
{
    stream << coordinate.latitude();
    stream << coordinate.longitude();
    return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator>>(QDataStream &stream, QcGeoCoordinateWGS84 &coordinate)
{
    double value;
    stream >> value;
    coordinate.set_latitude(value);
    stream >> value;
    coordinate.set_longitude(value);
    return stream;
}
#endif

/**************************************************************************************************/

QcGeoCoordinateMercator::QcGeoCoordinateMercator(double x, double y)
  : m_x(x), m_y(y)
{
  /*
  if (is_valid_x(x) && is_valid_y(y)) {
    m_x = x;
    m_y = y;
  }
  */
}

QcGeoCoordinateMercator::QcGeoCoordinateMercator()
  : QcGeoCoordinateMercator(.0, .0)
{}

QcGeoCoordinateMercator::QcGeoCoordinateMercator(const QcGeoCoordinateMercator &other)
  : m_x(other.m_x), m_y(other.m_y)
{}

QcGeoCoordinateMercator::~QcGeoCoordinateMercator()
{}

QcGeoCoordinateMercator &
QcGeoCoordinateMercator::operator=(const QcGeoCoordinateMercator &other)
{
  if (this != &other) {
    m_x = other.m_x;
    m_y = other.m_y;
  }

  return *this;
}

bool
QcGeoCoordinateMercator::operator==(const QcGeoCoordinateMercator &other) const
{
  bool x_equal = qFuzzyCompare(m_x, other.m_x);
  bool y_equal = qFuzzyCompare(m_y, other.m_y);

  return (x_equal && y_equal);
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
