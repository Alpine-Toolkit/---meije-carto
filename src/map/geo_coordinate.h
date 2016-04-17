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

// Fixme: qreal ???

/**************************************************************************************************/

#ifndef GEO_COORDINATE_H
#define GEO_COORDINATE_H

/**************************************************************************************************/

#include <QtCore/QMetaType>
#include <QString>

#include "qtcarto_global.h"
#include "map/earth.h"
#include "math/interval.h"
#include "math/vector.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

//#ifdef PROJ4
# include "proj_api.h"
//#endif

/**************************************************************************************************/

class QDebug;
class QDataStream;

class QcGeoCoordinateMercator;
class QcGeoCoordinateNormalisedMercator;

/**************************************************************************************************/

class QC_EXPORT QcGeoSexagesimalAngle
{
 public:
  static double to_decimal(int degrees, int minutes=0, double seconds=0);
  static void to_sexagesimal(double angle, int & degrees, int & minutes, double & seconds);

  inline static bool is_valid_degrees(int degrees) {
    // Fixme: -0 sign, 360 ? modulo ?
    return -360 <= degrees && degrees <= 360;
  }

  inline static bool is_valid_minutes(int degrees) {
    return 0 <= degrees && degrees <= 60;
  }

 public:
  QcGeoSexagesimalAngle(int degrees, int minutes=0, double seconds=0);
  QcGeoSexagesimalAngle(double degrees);
  QcGeoSexagesimalAngle(const QcGeoSexagesimalAngle & other);
  ~QcGeoSexagesimalAngle();

  QcGeoSexagesimalAngle & operator=(const QcGeoSexagesimalAngle & other);

  bool operator==(const QcGeoSexagesimalAngle & other) const;
  inline bool operator!=(const QcGeoSexagesimalAngle & other) const {
    return !operator==(other);
  }

  inline int degrees() const {
    return m_degrees;
  }
  inline void set_degrees(int degrees) {
    if (is_valid_degrees(degrees))
      m_degrees = degrees;
  }
  void set_degrees(double degrees);

  inline int minutes() const {
      return m_degrees;
  }
  inline void set_minutes(int minutes) {
    if (is_valid_minutes(minutes))
      m_minutes = minutes;
  }

  inline double seconds() const {
    return m_degrees;
  }
  inline void set_seconds(double seconds) {
    m_seconds = seconds;
  }

  double decimal() const;

 private:
  int m_degrees;
  int m_minutes; // Fixme: unsigned
  double m_seconds;
};

/**************************************************************************************************/

//#ifdef WITH_PROJ4
class QC_EXPORT QcProjection
{
 public:
  QcProjection(const QString & definition, projCtx context=nullptr);
  ~QcProjection();

  void transform(QcProjection & proj2, double & x, double & y) const;

  bool is_latlong() const;

 private:
  projPJ m_projection;
};
//#endif WITH_PROJ4

/**************************************************************************************************/

class QC_EXPORT QcGeoCoordinate
{
 public:
  virtual const char * srid() = 0;
  virtual QString proj4_definition() {
    return QString("+init=") + srid();
  };

  virtual double coordinate1() const = 0;
  virtual void set_coordinate1(double value) = 0;

  virtual double coordinate2() const = 0;
  virtual void set_coordinate2(double value) = 0;

//#ifdef WITH_PROJ4
 private:
  QcProjection *m_projection = nullptr;

 public:
  inline QcProjection & projection() {
    if (!m_projection)
      m_projection = new QcProjection(proj4_definition());

    return *m_projection;
  }
  void transform(QcGeoCoordinate & other);
//#endif WITH_PROJ4
};

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

class QC_EXPORT QcGeoCoordinateWGS84 : public QcGeoCoordinate
{
  Q_GADGET;

  Q_PROPERTY(double longitude READ longitude WRITE set_longitude)
  Q_PROPERTY(double latitude READ latitude WRITE set_latitude)
  /* Q_PROPERTY(bool isValid READ isValid) */

 public:

  // static
  inline const char *srid() {
    return "epsg:4326";
  };

  /*
  const char *proj4_definition() {
    return "+init=epsg:4326";
    // return "+proj=longlat +datum=WGS84 +no_defs";
  };
  */


  static constexpr double westward_longitude = -180.;
  static constexpr double eastward_longitude =  180.;

  static constexpr double equator_latitude = 0;
  static constexpr double south_pole_latitude = -90;
  static constexpr double north_pole_latitude =  90.;

  static inline QcIntervalDouble longitude_interval() {
    return QcIntervalDouble(westward_longitude, eastward_longitude);
  }

  static inline QcIntervalDouble latitude_interval() {
    return QcIntervalDouble(south_pole_latitude, north_pole_latitude);
  }

  static inline QcInterval2DDouble domain() {
    return QcInterval2DDouble(longitude_interval(), latitude_interval());
  }

  inline static bool is_valid_longitude(double longitude) {
    return westward_longitude <= longitude && longitude <= eastward_longitude;
  }

  inline static bool is_valid_latitude(double latitude) {
    return south_pole_latitude <= latitude && latitude <= north_pole_latitude;
  }

 public:
  QcGeoCoordinateWGS84();
  QcGeoCoordinateWGS84(double longitude, double latitude);
  QcGeoCoordinateWGS84(QcGeoSexagesimalAngle & longitude, QcGeoSexagesimalAngle & latitude);
  QcGeoCoordinateWGS84(const QcGeoCoordinateWGS84 & other);
  ~QcGeoCoordinateWGS84();

  QcGeoCoordinateWGS84 & operator=(const QcGeoCoordinateWGS84 & other);

  bool operator==(const QcGeoCoordinateWGS84 & other) const;
  inline bool operator!=(const QcGeoCoordinateWGS84 & other) const {
    return !operator==(other);
  }

  /* bool isValid() const; */

  inline double coordinate1() const {
    return m_longitude;
  }
  inline void set_coordinate1(double value) {
    m_longitude = value;
  }

  inline double coordinate2() const {
    return m_latitude;
  }
  inline void set_coordinate2(double value) {
    m_latitude = value;
  }

  inline void set_longitude(double longitude) {
    m_longitude = longitude;
  }
  inline double longitude() const {
    return m_longitude;
  }
  inline QcGeoSexagesimalAngle sexagesimal_longitude() const {
    return QcGeoSexagesimalAngle(m_longitude);
  }

  inline void set_latitude(double latitude) {
    m_latitude = latitude;
  }
  inline double latitude() const {
    return m_latitude;
  }
  inline QcGeoSexagesimalAngle sexagesimal_latitude() const {
    return QcGeoSexagesimalAngle(m_latitude);
  }

  QcGeoCoordinateMercator mercator() const;
  QcGeoCoordinateNormalisedMercator normalised_mercator() const;

  Q_INVOKABLE double distance_to(const QcGeoCoordinateWGS84 & other) const;
  Q_INVOKABLE double azimuth_to(const QcGeoCoordinateWGS84 & other) const;
  Q_INVOKABLE QcGeoCoordinateWGS84 at_distance_and_azimuth(double distance, double azimuth) const;

  /* Q_INVOKABLE QString toString(CoordinateFormat format = DegreesMinutesSecondsWithHemisphere) const; */

 private:
  // double[2] coordinates
  double m_longitude;
  double m_latitude;
};

// Q_DECLARE_TYPEINFO(QcGeoCoordinateWGS84, Q_MOVABLE_TYPE);

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug, const QcGeoCoordinateWGS84 &);
#endif

#ifndef QT_NO_DATASTREAM
QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcGeoCoordinateWGS84 & coordinate);
QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcGeoCoordinateWGS84 & coordinate);
#endif

/**************************************************************************************************/

class QC_EXPORT QcGeoCoordinateMercator : public QcGeoCoordinate
{
  Q_GADGET;

  Q_PROPERTY(double x READ x WRITE set_x)
  Q_PROPERTY(double y READ y WRITE set_y)
  /* Q_PROPERTY(bool isValid READ isValid) */

 public:

  inline const char *srid() {
    return "epsg:3857";
  };

  // const char *proj4_definition() {
  //   return "+init=epsg:3857";
  //   // return "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs";
  // };

  // argmax(latitude) is computed for y(latitude max) = pi * EQUATORIAL_RADIUS
  // argmax(latitude) = 2*atan(exp(pi)) - pi/2 = 85.051 128 779 806 6
  static constexpr double latitude_max = 85.0511287798060;

  static inline QcIntervalDouble longitude_interval() {
    return QcIntervalDouble(QcGeoCoordinateWGS84::westward_longitude, QcGeoCoordinateWGS84::eastward_longitude);
  }

  static inline QcIntervalDouble latitude_interval() {
    return QcIntervalDouble(-latitude_max, latitude_max);
  }

  static inline QcInterval2DDouble wgs84_domain() {
    return QcInterval2DDouble(longitude_interval(), latitude_interval());
  }

  static inline QcIntervalDouble x_interval() {
    return QcIntervalDouble(-HALF_EQUATORIAL_PERIMETER, HALF_EQUATORIAL_PERIMETER);
  }

  static inline QcIntervalDouble y_interval() {
    return QcIntervalDouble(-HALF_EQUATORIAL_PERIMETER, HALF_EQUATORIAL_PERIMETER);
  }

  static inline QcInterval2DDouble domain() {
    return QcInterval2DDouble(x_interval(), y_interval());
  }

  inline static bool is_valid_x(double x) {
    return -HALF_EQUATORIAL_PERIMETER <= x && x <= HALF_EQUATORIAL_PERIMETER;
  }

 public:

  QcGeoCoordinateMercator();
  QcGeoCoordinateMercator(double x, double y);
  QcGeoCoordinateMercator(const QcGeoCoordinateMercator & other);
  ~QcGeoCoordinateMercator();

  QcGeoCoordinateMercator & operator=(const QcGeoCoordinateMercator & other);

  bool operator==(const QcGeoCoordinateMercator & other) const;
  inline bool operator!=(const QcGeoCoordinateMercator & other) const {
    return !operator==(other);
  }

  inline double coordinate1() const {
    return m_x;
  }
  inline void set_coordinate1(double value) {
    m_x = value;
  }

  inline double coordinate2() const {
    return m_y;
  }
  inline void set_coordinate2(double value) {
    m_y = value;
  }

  inline void set_x(double x) {
    m_x = x;
  }
  inline double x() const {
    return m_x;
  }

  inline void set_y(double y) {
    m_y = y;
  }
  inline double y() const {
    return m_y;
  }

  QcGeoCoordinateWGS84 wgs84() const;
  QcGeoCoordinateNormalisedMercator normalised_mercator() const;

  inline QcVectorDouble vector() const {
    return QcVectorDouble(m_x, m_y);
  }

 private:
  double m_x;
  double m_y;
};

// Q_DECLARE_TYPEINFO(QcGeoCoordinateMercator, Q_MOVABLE_TYPE);

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcGeoCoordinateMercator & coordinate);
#endif

#ifndef QT_NO_DATASTREAM
QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcGeoCoordinateMercator & coordinate);
QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcGeoCoordinateMercator & coordinate);
#endif

/**************************************************************************************************/

/*
 * Normalised Mercator coordinate lies in [0, 1] interval in the web mercator rest frame.
 * This normalised coordinate can be converted to a matrix index by multiplying by 2^level.
 *
 */
class QC_EXPORT QcGeoCoordinateNormalisedMercator : public QcGeoCoordinate
{
  Q_GADGET;

  Q_PROPERTY(double x READ x WRITE set_x)
  Q_PROPERTY(double y READ y WRITE set_y)
  /* Q_PROPERTY(bool isValid READ isValid) */

 public:

  inline const char *srid() {
    return "epsg:none";
  };

  static inline QcIntervalDouble x_interval() {
    return QcIntervalDouble(0, 1);
  }

  static inline QcIntervalDouble y_interval() {
    return QcIntervalDouble(0, 1);
  }

  static inline QcInterval2DDouble domain() {
    return QcInterval2DDouble(x_interval(), y_interval());
  }

  inline static bool is_valid_x(double x) {
    return 0 <= x && x <= 1.;
  }

 public:

  QcGeoCoordinateNormalisedMercator();
  QcGeoCoordinateNormalisedMercator(double x, double y);
  QcGeoCoordinateNormalisedMercator(const QcGeoCoordinateNormalisedMercator & other);
  ~QcGeoCoordinateNormalisedMercator();

  QcGeoCoordinateNormalisedMercator & operator=(const QcGeoCoordinateNormalisedMercator & other);

  bool operator==(const QcGeoCoordinateNormalisedMercator & other) const;
  inline bool operator!=(const QcGeoCoordinateNormalisedMercator & other) const {
    return !operator==(other);
  }

  inline double coordinate1() const {
    return m_x;
  }
  inline void set_coordinate1(double value) {
    m_x = value;
  }

  inline double coordinate2() const {
    return m_y;
  }
  inline void set_coordinate2(double value) {
    m_y = value;
  }

  inline void set_x(double x) {
    m_x = x;
  }
  inline double x() const {
    return m_x;
  }

  inline void set_y(double y) {
    m_y = y;
  }
  inline double y() const {
    return m_y;
  }

  QcGeoCoordinateMercator mercator() const;
  QcGeoCoordinateWGS84 wgs84() const;

  inline QcVectorDouble vector() const {
    return QcVectorDouble(m_x, m_y);
  }

 private:
  double m_x;
  double m_y;
};

// Q_DECLARE_TYPEINFO(QcGeoCoordinateNormalisedMercator, Q_MOVABLE_TYPE);

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcGeoCoordinateNormalisedMercator & coordinate);
#endif

#ifndef QT_NO_DATASTREAM
QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcGeoCoordinateNormalisedMercator & coordinate);
QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcGeoCoordinateNormalisedMercator & coordinate);
#endif

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

  double elevation() const {
    return m_elevation;
  }
  void set_elevation(double elevation) {
    m_elevation = elevation;
  }

 private:
  double m_elevation;
};

/**************************************************************************************************/

class QC_EXPORT QcGeoElevationCoordinateWGS84 : public QcGeoCoordinateWGS84, public QcElevation
{
  Q_GADGET;

  Q_PROPERTY(double longitude READ longitude WRITE set_longitude)
  Q_PROPERTY(double latitude READ latitude WRITE set_latitude)
  Q_PROPERTY(double elevation READ elevation WRITE set_elevation)
  /* Q_PROPERTY(bool isValid READ isValid) */

 public:
  QcGeoElevationCoordinateWGS84();
  QcGeoElevationCoordinateWGS84(double longitude, double latitude, double elevation);
  QcGeoElevationCoordinateWGS84(QcGeoSexagesimalAngle & longitude, QcGeoSexagesimalAngle & latitude, double elevation);
  QcGeoElevationCoordinateWGS84(const QcGeoElevationCoordinateWGS84 & other);
  ~QcGeoElevationCoordinateWGS84();

  QcGeoElevationCoordinateWGS84 & operator=(const QcGeoElevationCoordinateWGS84 & other);

  bool operator==(const QcGeoElevationCoordinateWGS84 & other) const;
};

// Q_DECLARE_TYPEINFO(QcGeoElevationCoordinateWGS84, Q_MOVABLE_TYPE);

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug, const QcGeoElevationCoordinateWGS84 &);
#endif

#ifndef QT_NO_DATASTREAM
QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcGeoElevationCoordinateWGS84 & coordinate);
QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcGeoElevationCoordinateWGS84 & coordinate);
#endif

/**************************************************************************************************/

// QT_END_NAMESPACE

// Q_DECLARE_METATYPE(QcGeoCoordinateWGS84)
// Q_DECLARE_METATYPE(QcGeoCoordinateNormalisedMercator)

/**************************************************************************************************/

#endif // GEO_COORDINATE_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
