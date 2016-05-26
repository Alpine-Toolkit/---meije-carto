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

#include "wgs84.h"

#include "earth.h"
#include "mercator.h"

#include <QDataStream>
#include <QDebug>

/**************************************************************************************************/

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84(double longitude, double latitude)
  : QcGeoCoordinate(longitude, latitude)
{}

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84()
  : QcGeoCoordinate(.0, .0)
{}

QcGeoCoordinateWGS84::QcGeoCoordinateWGS84(QcGeoSexagesimalAngle & longitude, QcGeoSexagesimalAngle & latitude)
  : QcGeoCoordinate(longitude.decimal(), latitude.decimal())
{}

bool
QcGeoCoordinateWGS84::operator==(const QcGeoCoordinateWGS84 & other) const
{
  bool longitude_equal = ((qIsNaN(longitude()) and qIsNaN(other.longitude()))
			  || qFuzzyCompare(longitude(), other.longitude()));
  bool latitude_equal = ((qIsNaN(latitude()) and qIsNaN(other.latitude()))
                         || qFuzzyCompare(latitude(), other.latitude()));

  // Fixme: compact
  if (!qIsNaN(longitude()) and ((latitude() == 90.0) || (latitude() == -90.0)))
    longitude_equal = true;

  return (longitude_equal and latitude_equal);
}

QcGeoCoordinateWebMercator
QcGeoCoordinateWGS84::web_mercator() const
{
  // 128/pi 2^level (longitude() + pi) px
  // 128/pi 2^level (pi - ln(tan(latitude/2 + pi/4))) px

  double x = qDegreesToRadians(longitude());
  double y = log(tan(qDegreesToRadians(latitude())/2 + M_PI/4));
  x *= EQUATORIAL_RADIUS;
  y *= EQUATORIAL_RADIUS;
  // y = R/2 * math.log((1 + sin(latitude))/(1 - sin(latitude))

  return QcGeoCoordinateWebMercator(x, y);
}

QcGeoCoordinatePseudoWebMercator
QcGeoCoordinateWGS84::pseudo_web_mercator() const
{
  return web_mercator().pseudo_web_mercator(); // shift
}

QcGeoCoordinateNormalisedWebMercator
QcGeoCoordinateWGS84::normalised_web_mercator() const
{
  // 128/pi 2^level (longitude() + pi) px
  // 128/pi 2^level (pi - ln(tan(latitude/2 + pi/4))) px

  // Fixme: duplicated code
  double x = qDegreesToRadians(longitude());
  double y = log(tan(qDegreesToRadians(latitude())/2 + M_PI/4));
  x /= M_2PI;
  y /= M_2PI;
  x += .5;
  y = .5 - y;

  return QcGeoCoordinateNormalisedWebMercator(x, y);
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
double QcGeoCoordinateWGS84::distance_to(const QcGeoCoordinateWGS84 & other) const
{
  /*
  if (type() == QcGeoCoordinateWGS84::InvalidCoordinate || other.type() == QcGeoCoordinateWGS84::InvalidCoordinate) {
    return 0;
  }
  */

  // Haversine formula https://en.wikipedia.org/wiki/Haversine_formula

  double latitude1 = qDegreesToRadians(latitude());
  double latitude2 = qDegreesToRadians(other.latitude());

  double delta_latitude = qDegreesToRadians(other.latitude() - latitude());
  double delta_longitude = qDegreesToRadians(other.longitude() - longitude());

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
QcGeoCoordinateWGS84::azimuth_to(const QcGeoCoordinateWGS84 & other) const
{
  /*
  if (type() == QcGeoCoordinateWGS84::InvalidCoordinate || other.type() == QcGeoCoordinateWGS84::InvalidCoordinate) {
    return 0;
  }
  */

  // Fixme: find reference

  double delta_longitude = qDegreesToRadians(other.longitude() - longitude());
  double latitude1 = qDegreesToRadians(latitude());
  double latitude2 = qDegreesToRadians(other.latitude());

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

  double latitude1 = qDegreesToRadians(latitude());
  double longitude1 = qDegreesToRadians(longitude());
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

/**************************************************************************************************/

QcGeoElevationCoordinateWGS84::QcGeoElevationCoordinateWGS84(double longitude, double latitude, double elevation)
  : QcGeoCoordinateWGS84(longitude, latitude),
    QcElevation(elevation)
{
}

QcGeoElevationCoordinateWGS84::QcGeoElevationCoordinateWGS84()
  : QcGeoCoordinateWGS84(),
    QcElevation()
{}

QcGeoElevationCoordinateWGS84::QcGeoElevationCoordinateWGS84(QcGeoSexagesimalAngle & longitude,
                                                             QcGeoSexagesimalAngle & latitude,
                                                             double elevation)
  : QcGeoCoordinateWGS84(longitude.decimal(), latitude.decimal()),
    QcElevation(elevation)
{}

QcGeoElevationCoordinateWGS84::QcGeoElevationCoordinateWGS84(const QcGeoElevationCoordinateWGS84 & other)
  : QcGeoCoordinateWGS84(other.longitude(), other.latitude()),
    QcElevation(other.elevation())
{}

QcGeoElevationCoordinateWGS84::~QcGeoElevationCoordinateWGS84()
{}

// Fixme: default
QcGeoElevationCoordinateWGS84 &
QcGeoElevationCoordinateWGS84::operator=(const QcGeoElevationCoordinateWGS84 & other)
{
  if (this != &other) {
    QcGeoCoordinateWGS84::operator=(other);
    QcElevation::operator=(other);
  }

  return *this;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const QcGeoElevationCoordinateWGS84 & coordinate)
{
    QDebugStateSaver saver(debug);
    double longitude = coordinate.longitude();
    double latitude = coordinate.latitude();
    double elevation = coordinate.elevation();

    debug.nospace() << "QcGeoElevationCoordinateWGS84(";
    if (qIsNaN(longitude))
        debug << '?';
    else
        debug << longitude;
    debug << ", ";
    if (qIsNaN(latitude))
        debug << '?';
    else
        debug << latitude;
    debug << ", ";
    if (qIsNaN(elevation))
        debug << '?';
    else
        debug << elevation;
    debug << ')';

    return debug;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream & operator<<(QDataStream & stream, const QcGeoElevationCoordinateWGS84 & coordinate)
{
    stream << coordinate.longitude();
    stream << coordinate.latitude();
    stream << coordinate.elevation();
    return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream & operator>>(QDataStream & stream, QcGeoElevationCoordinateWGS84 & coordinate)
{
    double value;
    stream >> value;
    coordinate.set_longitude(value);
    stream >> value;
    coordinate.set_latitude(value);
    stream >> value;
    coordinate.set_elevation(value);
    return stream;
}
#endif

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
