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

#ifndef __MERCATOR_H__
#define __MERCATOR_H__

/**************************************************************************************************/

#include "earth.h"
#include "coordinate/projection.h"

// #include <QtCore/QMetaType>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QDebug;
class QDataStream;

class QcGeoCoordinateWGS84;
class QcGeoCoordinateWebMercator;
class QcGeoCoordinatePseudoWebMercator;
class QcGeoCoordinateNormalisedWebMercator;

/**************************************************************************************************/

/*!
 * http://epsg.io/3857
 *
 * Unit: metre
 * Geodetic CRS: WGS 84
 * Datum: World Geodetic System 1984
 * Ellipsoid: WGS 84
 * Prime meridian: Greenwich
 * Data source: OGP
 * Scope: Certain Web mapping and visualisation applications. It is
 *   not a recognised geodetic system: for that see ellipsoidal Mercator
 *   CRS code 3395 (WGS 84 / World Mercator).
 * Remarks: Uses spherical development of ellipsoidal
 *   coordinates. Relative to WGS 84 / World Mercator (CRS code 3395)
 *   errors of 0.7 percent in scale and differences in northing of up to
 *   43km in the map (equivalent to 21km on the ground) may arise.
 * Area of use: World between 85.06°S and 85.06°N.
 * Coordinate system: Cartesian 2D CS. Axes: easting, northing (X,Y). Orientations: east, north. UoM: m.
 * Center coordinates
 *   0.00 -0.00
 * Projected bounds:
 *   -20026376.39 -20048966.10
 *    20026376.39  20048966.10
 * WGS84 bounds:
 *   -180.0 -85.06
 *    180.0  85.06
 * World between 85.06°S and 85.06°N.
 */

class QC_EXPORT QcWebMercatorProjection : public QcProjection
{
  /*
  const char *proj4_definition() {
    return "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs";
  };
  */

 public:
  // argmax(latitude) is computed for y(latitude max) = pi * EQUATORIAL_RADIUS
  // argmax(latitude) = 2*atan(exp(pi)) - pi/2 = 85.051 128 779 806 6
  static constexpr double latitude_max = 85.0511287798060;

 public:
  QcWebMercatorProjection()
    : QcProjection(QLatin1Literal("epsg:3857"),
                   QLatin1Literal("Web Mercator"),
                   QcVectorDouble(0, 0),
                   // QcVectorDouble(0, 180), // X towards E
                   // QcVectorDouble(0, 90), // Y towards N
                   QcInterval2DDouble(-180, 180,
                                      -latitude_max, latitude_max),
                   QcInterval2DDouble(-HALF_EQUATORIAL_PERIMETER, HALF_EQUATORIAL_PERIMETER,
                                      -HALF_EQUATORIAL_PERIMETER, HALF_EQUATORIAL_PERIMETER),
                   QLatin1Literal("m"),
                   ProjectionSurface::Cylindrical,
                   PreserveBit::PreserveDirection)
    {}
};


class QC_EXPORT QcGeoCoordinateWebMercator : public QcGeoCoordinateTemplate<QcWebMercatorProjection>
{
  Q_GADGET;

  Q_PROPERTY(double x READ y WRITE set_x)
  Q_PROPERTY(double y READ x WRITE set_y)
  Q_PROPERTY(bool is_valid READ is_valid)

 public:
  QcGeoCoordinateWebMercator();
  QcGeoCoordinateWebMercator(double x, double y);

  QcGeoCoordinateWGS84 wgs84() const;
  QcGeoCoordinatePseudoWebMercator pseudo_web_mercator() const;
  QcGeoCoordinateNormalisedWebMercator normalised_web_mercator() const;
};

// Q_DECLARE_TYPEINFO(QcGeoCoordinateWebMercator, Q_MOVABLE_TYPE);

/**************************************************************************************************/

class QC_EXPORT QcPseudoMercatorProjection : public QcProjection
{
 public:
  QcPseudoMercatorProjection()
    : QcProjection(QLatin1Literal("pseudo-mercator"), // Fxime: name ???
                   QLatin1Literal("Pseudo Web Mercator"),
                   QcVectorDouble(0, 0),
                   // QcVectorDouble(0, 180), // X towards E
                   // QcVectorDouble(0, 90), // Y towards N
                   QcInterval2DDouble(-180, 180,
                                      -QcWebMercatorProjection::latitude_max, QcWebMercatorProjection::latitude_max),
                   QcInterval2DDouble(0, EQUATORIAL_PERIMETER,
                                      0, EQUATORIAL_PERIMETER),
                   QLatin1Literal("m"),
                   ProjectionSurface::Cylindrical,
                   PreserveBit::PreserveDirection,
                   false)
    {}
};


class QC_EXPORT QcGeoCoordinatePseudoWebMercator : public QcGeoCoordinateTemplate<QcPseudoMercatorProjection>
{
  Q_GADGET;

  Q_PROPERTY(double x READ y WRITE set_x)
  Q_PROPERTY(double y READ x WRITE set_y)
  Q_PROPERTY(bool is_valid READ is_valid)

 public:
  QcGeoCoordinatePseudoWebMercator();
  QcGeoCoordinatePseudoWebMercator(double x, double y);

  QcGeoCoordinateWGS84 wgs84() const;
  QcGeoCoordinateWebMercator web_mercator() const;
  QcGeoCoordinateNormalisedWebMercator normalised_web_mercator() const;
};

// Q_DECLARE_TYPEINFO(QcGeoCoordinatePseudoWebMercator, Q_MOVABLE_TYPE);

/**************************************************************************************************/

/*
 * Normalised WebMercator coordinate lies in [0, 1] interval in the web mercator rest frame.
 * This normalised coordinate can be converted to a matrix index by multiplying by 2^level.
 *
 */

class QC_EXPORT QcNormalisedMercatorProjection : public QcProjection
{
 public:
  QcNormalisedMercatorProjection()
    : QcProjection(QLatin1Literal("normalised-mercator"),
                   QLatin1Literal("Normalised Web Mercator"),
                   QcVectorDouble(0, 0),
                   // QcVectorDouble(0, 180), // X towards E
                   // QcVectorDouble(0, 90), // Y towards N
                   QcInterval2DDouble(-180, 180,
                                      -QcWebMercatorProjection::latitude_max, QcWebMercatorProjection::latitude_max),
                   QcInterval2DDouble(0, 1, 0, 1),
                   QLatin1Literal("m"),
                   ProjectionSurface::Cylindrical,
                   PreserveBit::PreserveDirection,
                   false)
    {}
};


class QC_EXPORT QcGeoCoordinateNormalisedWebMercator : public QcGeoCoordinateTemplate<QcNormalisedMercatorProjection>
{
  Q_GADGET;

  Q_PROPERTY(double x READ y WRITE set_x)
  Q_PROPERTY(double y READ x WRITE set_y)
  Q_PROPERTY(bool is_valid READ is_valid)

 public:
  QcGeoCoordinateNormalisedWebMercator();
  QcGeoCoordinateNormalisedWebMercator(double x, double y);

  QcGeoCoordinateWGS84 wgs84() const;
  QcGeoCoordinateWebMercator web_mercator() const;
  QcGeoCoordinatePseudoWebMercator pseudo_web_mercator() const;
};

// Q_DECLARE_TYPEINFO(QcGeoCoordinateNormalisedWebMercator, Q_MOVABLE_TYPE);

/**************************************************************************************************/

// QC_END_NAMESPACE

// Q_DECLARE_METATYPE(QcGeoCoordinateWebMercator)
// Q_DECLARE_METATYPE(QcGeoCoordinatePseudoWebMercator)
// Q_DECLARE_METATYPE(QcGeoCoordinateNormalisedWebMercator)

/**************************************************************************************************/

#endif /* __MERCATOR_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
