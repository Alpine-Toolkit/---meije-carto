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

#ifndef __WSG84_COORDINATE_H__
#define __WSG84_COORDINATE_H__

/**************************************************************************************************/

#include "coordinate/sexagesimal_angle.h"
#include "coordinate/projection.h"

#include <QGeoCoordinate>
#include <QtCore/QMetaType>

/**************************************************************************************************/

class QcWebMercatorCoordinate;
class QcPseudoWebMercatorCoordinate;
class QcNormalisedWebMercatorCoordinate;

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

/*
 * Definitions:
 *
 *   Longitude (λ) is a geographic coordinate that specifies the
 *   east-west position of a point on the Earth's surface. Meridians
 *   (lines running from the North Pole to the South Pole) connect
 *   points with the same longitude. By convention, one of these, the
 *   Prime Meridian, which passes through the Royal Observatory,
 *   Greenwich, England, was allocated the position of zero degrees
 *   longitude. The longitude of other places is measured as the angle
 *   east or west from the Prime Meridian, ranging from 0° at the
 *   Prime Meridian to +180° eastward and −180° westward.
 *
 *  Latitude (φ) is a geographic coordinate that specifies the
 *  north-south position of a point on the Earth's surface. Latitude
 *  is an angle which ranges from 0° at the Equator to 90° (North or
 *  South) at the poles. Lines of constant latitude, or parallels, run
 *  east-west as circles parallel to the equator. The North Pole has a
 *  latitude of 90° north (written 90° N or +90°), and the South Pole
 *  has a latitude of 90° south (written 90° S or −90°).
 *
 */

/**************************************************************************************************/

/*!
 *
 * Unit: degree (supplier to define representation)
 * Geodetic CRS: WGS 84
 * Datum: World Geodetic System 1984
 * Ellipsoid: WGS 84
 * Prime meridian: Greenwich
 * Data source: OGP
 * Information source: EPSG. See 3D CRS for original information source.
 * Revision date: 2007-08-27
 * Scope: Horizontal component of 3D system. Used by the GPS satellite navigation system and for NATO military geodetic surveying.
 * Area of use: World.
 * Coordinate system: Ellipsoidal 2D CS. Axes: latitude, longitude. Orientations: north, east. UoM: degree
 * Center coordinates
 *   0.00000000 0.00000000
 * WGS84 bounds:
 *  -180.0 -90.0
 *   180.0  90.0
 */

class QC_EXPORT QcWgs84Projection : public QcProjection
{
  /*
  const char *proj4_definition() {
    return "+init=epsg:4326";
    // return "+proj=longlat +datum=WGS84 +no_defs";
  };
  */

 public:
  static constexpr double westward_longitude = -180.;
  static constexpr double eastward_longitude =  180.;

  static constexpr double equator_latitude = 0;
  static constexpr double south_pole_latitude = -90;
  static constexpr double north_pole_latitude =  90.;

 public:
  QcWgs84Projection()
    : QcProjection(QLatin1Literal("epsg:4326"),
                   QLatin1Literal("WGS 84"),
                   QcVectorDouble(), // Fixme: not projection, ellipsoidal cs
                   // QcVectorDouble(),
                   // QcVectorDouble(),
                   QcInterval2DDouble(),
                   QcInterval2DDouble(westward_longitude, eastward_longitude,
                                      south_pole_latitude, north_pole_latitude),
                   QLatin1Literal("degrees"),
                   ProjectionSurface::Spherical,
                   PreserveBit::PreserveAll)
    {}

  inline const QcIntervalDouble & longitude_interval() const {
    return x_projected_interval();
  }

  inline const QcIntervalDouble & latitude_interval() const {
    return y_projected_interval();
  }

  inline bool is_valid_longitude(double longitude) const {
    return is_valid_x(longitude);
  }

   inline bool is_valid_latitude(double latitude) const {
    return is_valid_y(latitude);
  }
};

/**************************************************************************************************/

class QC_EXPORT QcWgsCoordinate : public QcGeoCoordinateTemplate<QcWgs84Projection>
{
  Q_GADGET;

  Q_PROPERTY(double longitude READ longitude WRITE set_longitude)
  Q_PROPERTY(double latitude READ latitude WRITE set_latitude)
  Q_PROPERTY(bool is_valid READ is_valid)

 public:
  QcWgsCoordinate();
  QcWgsCoordinate(double longitude, double latitude);
  QcWgsCoordinate(QcGeoSexagesimalAngle & longitude, QcGeoSexagesimalAngle & latitude);
  QcWgsCoordinate(const QcVectorDouble & vector);

  bool operator==(const QcWgsCoordinate & other) const;

  QcWgsCoordinate(const QGeoCoordinate & coordinate)
    : QcWgsCoordinate(coordinate.longitude(), coordinate.latitude()) {}
  inline QGeoCoordinate to_qt() const { return QGeoCoordinate(latitude(), longitude()); }

  inline void set_longitude(double longitude) { set_x(longitude); }
  inline double longitude() const { return x(); }
  inline QcGeoSexagesimalAngle sexagesimal_longitude() const {
    return QcGeoSexagesimalAngle(longitude());
  }

  inline void set_latitude(double latitude) { set_y(latitude); }
  inline double latitude() const { return y(); }
  inline QcGeoSexagesimalAngle sexagesimal_latitude() const {
    return QcGeoSexagesimalAngle(latitude());
  }

  QcWebMercatorCoordinate web_mercator() const;
  QcPseudoWebMercatorCoordinate pseudo_web_mercator() const;
  QcNormalisedWebMercatorCoordinate normalised_web_mercator() const;

  Q_INVOKABLE double distance_to(const QcWgsCoordinate & other) const;
  Q_INVOKABLE double azimuth_to(const QcWgsCoordinate & other) const;
  Q_INVOKABLE QcWgsCoordinate at_distance_and_azimuth(double distance, double azimuth) const;

  // Q_INVOKABLE QString toString(CoordinateFormat format = DegreesMinutesSecondsWithHemisphere) const;
};

// Q_DECLARE_TYPEINFO(QcWgsCoordinate, Q_MOVABLE_TYPE);

/**************************************************************************************************/

class QC_EXPORT QcElevation
{
 public:
  QcElevation() : m_elevation(.0) {}
  QcElevation(double elevation) : m_elevation(elevation) {}
  QcElevation(const QcElevation & other) : m_elevation(other.m_elevation) {}

  QcElevation & operator=(const QcElevation & other) {
    m_elevation = other.m_elevation;
    return *this;
  }

  bool operator==(const QcElevation & other) const {
    return qFuzzyCompare(m_elevation, other.m_elevation);
  }

  double elevation() const { return m_elevation; }
  void set_elevation(double elevation) { m_elevation = elevation; }

 private:
  double m_elevation;
};

/**************************************************************************************************/

class QC_EXPORT QcWgsElevationCoordinate : public QcWgsCoordinate, public QcElevation
{
  Q_GADGET;

  Q_PROPERTY(double longitude READ longitude WRITE set_longitude)
  Q_PROPERTY(double latitude READ latitude WRITE set_latitude)
  Q_PROPERTY(double elevation READ elevation WRITE set_elevation)
  Q_PROPERTY(bool is_valid READ is_valid)

 public:
  QcWgsElevationCoordinate();
  QcWgsElevationCoordinate(double longitude, double latitude, double elevation);
  QcWgsElevationCoordinate(QcGeoSexagesimalAngle & longitude, QcGeoSexagesimalAngle & latitude, double elevation);
  QcWgsElevationCoordinate(const QcWgsElevationCoordinate & other);
  ~QcWgsElevationCoordinate();

  QcWgsElevationCoordinate & operator=(const QcWgsElevationCoordinate & other);

  bool operator==(const QcWgsElevationCoordinate & other) const;
};

// Q_DECLARE_TYPEINFO(QcWgsElevationCoordinate, Q_MOVABLE_TYPE);

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug, const QcWgsElevationCoordinate &);
#endif

#ifndef QT_NO_DATASTREAM
QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcWgsElevationCoordinate & coordinate);
QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcWgsElevationCoordinate & coordinate);
#endif

/**************************************************************************************************/

// QT_END_NAMESPACE

Q_DECLARE_METATYPE(QcWgsCoordinate)

/**************************************************************************************************/

#endif // __WSG84_COORDINATE_H__

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
