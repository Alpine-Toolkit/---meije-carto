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

#include <cmath>

#include <QDataStream>
#include <QDebug>
#include <QtMath>
#include <qnumeric.h>

/**************************************************************************************************/

#include "math/qc_math.h"
#include "geo_coordinate.h"

/**************************************************************************************************/

double
QcGeoSexagesimalAngle::to_decimal(int degrees, int minutes, double seconds)
{
  return degrees + (minutes + seconds / 60.) / 60.;
}

void
QcGeoSexagesimalAngle::to_sexagesimal(double angle, int &degrees, int &minutes, double &seconds)
{
  double _degrees, _minutes, f;
  f = modf(angle, &_degrees);
  f = modf(f * 60, &_minutes);
  seconds = f * 60;

  degrees = int(_degrees);
  minutes = int(_minutes);
}

QcGeoSexagesimalAngle::QcGeoSexagesimalAngle(double degrees)
  : m_degrees(0), m_minutes(0), m_seconds(qQNaN())
{
  to_sexagesimal(degrees, m_degrees, m_minutes, m_seconds);
}

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

// Fixme: default
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

void
QcGeoSexagesimalAngle::set_degrees(double degrees)
{
  to_sexagesimal(degrees, m_degrees, m_minutes, m_seconds);
}

double
QcGeoSexagesimalAngle::decimal() const {
  return to_decimal(m_degrees, m_minutes, m_seconds);
}

/**************************************************************************************************/

//#ifdef WITH_PROJ4
QcProjection::QcProjection(const QString & definition, projCtx context)
  : m_projection(nullptr)
{
  // cf. https://trac.osgeo.org/proj/wiki/ThreadSafety
  if (!context)
    context = pj_get_default_ctx();
  // context = pj_ctx_alloc();

  m_projection = pj_init_plus_ctx(context, definition.toStdString().c_str());
}

QcProjection::~QcProjection()
{
  if (m_projection)
    pj_free(m_projection);
}

void
QcProjection::transform(QcProjection &proj2, double &x, double &y) const
{
  // int pj_transform( projPJ src, projPJ dst, long point_count, int point_offset,
  //                 double *x, double *y, double *z );

  long point_count = 1;
  int point_offset = 1;
  double z = 0;
  int error = pj_transform(m_projection, proj2.m_projection, point_count, point_offset, &x, &y, &z);
  if (error != 0)
    throw; // Fixme: (pj_strerrno(error))
}

bool
QcProjection::is_latlong() const {
  return pj_is_latlong(m_projection);
}
//#endif WITH_PROJ4

/**************************************************************************************************/

void
QcGeoCoordinate::transform(QcGeoCoordinate & other)
{
  double _coordinate1 = coordinate1();
  double _coordinate2 = coordinate2();
  if (projection().is_latlong()) {
      _coordinate1 = qDegreesToRadians(_coordinate1);
      _coordinate2 = qDegreesToRadians(_coordinate2);
    }
  projection().transform(other.projection(), _coordinate1, _coordinate2);
  other.set_coordinate1(_coordinate1);
  other.set_coordinate2(_coordinate2);
}

/**************************************************************************************************/

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84(double longitude, double latitude)
  : m_longitude(qQNaN()), m_latitude(qQNaN())
{
  if (is_valid_longitude(longitude) && is_valid_latitude(latitude)) {
    m_longitude = longitude;
    m_latitude = latitude;
  }
}

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84()
  : QcGeoCoordinateWGS84(.0, .0)
{}

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84(QcGeoSexagesimalAngle &longitude, QcGeoSexagesimalAngle &latitude)
  : QcGeoCoordinateWGS84(longitude.decimal(), latitude.decimal())
{}

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84(const QcGeoCoordinateWGS84 &other)
  : m_longitude(other.m_longitude), m_latitude(other.m_latitude)
{}

QcGeoCoordinateWGS84::~QcGeoCoordinateWGS84()
{}

// Fixme: default
QcGeoCoordinateWGS84 &
QcGeoCoordinateWGS84::operator=(const QcGeoCoordinateWGS84 &other)
{
  if (this != &other) {
    m_longitude = other.m_longitude;
    m_latitude = other.m_latitude;
  }

  return *this;
}

bool
QcGeoCoordinateWGS84::operator==(const QcGeoCoordinateWGS84 &other) const
{
  bool longitude_equal = ((qIsNaN(m_longitude) && qIsNaN(other.m_longitude))
			  || qFuzzyCompare(m_longitude, other.m_longitude));
  bool latitude_equal = ((qIsNaN(m_latitude) && qIsNaN(other.m_latitude))
			 || qFuzzyCompare(m_latitude, other.m_latitude));

  // Fixme: compact
  if (!qIsNaN(m_longitude) && ((m_latitude == 90.0) || (m_latitude == -90.0)))
    longitude_equal = true;

  return (longitude_equal && latitude_equal);
}

QcGeoCoordinateMercator
QcGeoCoordinateWGS84::mercator() const
{
  double x = qDegreesToRadians(m_longitude);
  double y = log(tan(qDegreesToRadians(m_latitude)/2 + M_PI/4));
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

  double latitude1 = qDegreesToRadians(m_latitude);
  double latitude2 = qDegreesToRadians(other.m_latitude);

  double delta_latitude = qDegreesToRadians(other.m_latitude - m_latitude);
  double delta_longitude = qDegreesToRadians(other.m_longitude - m_longitude);

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

  double delta_longitude = qDegreesToRadians(other.m_longitude - m_longitude);
  double latitude1 = qDegreesToRadians(m_latitude);
  double latitude2 = qDegreesToRadians(other.m_latitude);

  double y = sin(delta_longitude) * cos(latitude2);
  double x = cos(latitude1) * sin(latitude2) - sin(latitude1) * cos(latitude2) * cos(delta_longitude);
  double theta = atan2(y, x);

  // Since atan2 returns values in the range [-π, +π] (that is, [-180°, +180°]),
  // to normalise the result to a compass bearing in the range [0°, 360°]
  // use (theta + 360) % 360

  // Fixme: check int -> double ...
  double whole;
  double fraction = modf(qRadiansToDegrees(theta), &whole);
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

  double latitude1 = qDegreesToRadians(m_latitude);
  double longitude1 = qDegreesToRadians(m_longitude);
  double azimuth = qDegreesToRadians(_azimuth);

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

  double latitude2 = qRadiansToDegrees(latitude2_rad);
  double longitude2 = qRadiansToDegrees(longitude2_rad);
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
    double longitude = coordinate.longitude();
    double latitude = coordinate.latitude();

    debug.nospace() << "QcGeoCoordinateWGS84(";
    if (qIsNaN(longitude))
        debug << '?';
    else
        debug << longitude;
    debug << ", ";
    if (qIsNaN(latitude))
        debug << '?';
    else
        debug << latitude;
    debug << ')';

    return debug;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &stream, const QcGeoCoordinateWGS84 &coordinate)
{
    stream << coordinate.longitude();
    stream << coordinate.latitude();
    return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator>>(QDataStream &stream, QcGeoCoordinateWGS84 &coordinate)
{
    double value;
    stream >> value;
    coordinate.set_longitude(value);
    stream >> value;
    coordinate.set_latitude(value);
    return stream;
}
#endif

/**************************************************************************************************/

QcGeoCoordinateMercator::QcGeoCoordinateMercator(double x, double y)
  : m_x(qQNaN()), m_y(qQNaN())
{
  if (is_valid_x(x) && is_valid_x(y)) {
    m_x = x;
    m_y = y;
  }
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

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const QcGeoCoordinateMercator &coordinate)
{
    QDebugStateSaver saver(debug); // Fixme: ???
    double x = coordinate.x();
    double y = coordinate.y();

    debug.nospace() << "QcGeoCoordinateMercator(";
    if (qIsNaN(x))
        debug << '?';
    else
        debug << x;
    debug << ", ";
    if (qIsNaN(y))
        debug << '?';
    else
        debug << y;
    debug << ')';

    return debug;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &stream, const QcGeoCoordinateMercator &coordinate)
{
    stream << coordinate.x();
    stream << coordinate.y();
    return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator>>(QDataStream &stream, QcGeoCoordinateMercator &coordinate)
{
    double value;
    stream >> value;
    coordinate.set_x(value);
    stream >> value;
    coordinate.set_y(value);
    return stream;
}
#endif

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
