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

#ifndef __LAEA_H__
#define __LAEA_H__

/**************************************************************************************************/

#include "earth.h"
#include "coordinate/projection.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcWgsCoordinate;

/**************************************************************************************************/

/*! Lambert Azimuthal Equal-Area (LAEA) Projection
 *
 */

/*!
 * http://epsg.io/3571
 *
 * Unit: metre
 * Geodetic CRS: WGS 84
 * Datum: World Geodetic System 1984
 * Ellipsoid: WGS 84
 * Prime meridian: Greenwich
 * Data source: OGP
 * Scope: Arctic research.
 * Remarks: For studies of Bering Sea area.
 * Area of use: Northern hemisphere - north of 45°N, including Arctic.
 * Coordinate system: Cartesian 2D CS for north polar azimuthal lonO 180°E. Axes: X,Y. Orientations: X along 90°W, Y along 0°E meridians. UoM: m.
 * Center coordinates 0.00 2768558.07
 * Projected bounds:
 *   8533.49 -4889327.36
 *   -974.38 -558277.56
 * WGS84 bounds:
 *   -180.0 45.0
 *    180.0 90.0
 *
 * Northern hemisphere - north of 45°N, including Arctic.
 */
class QC_EXPORT QcLaeaProjection_3571 : public QcProjection // LaeaProjection
{
 public:
  QcLaeaProjection_3571()
    : QcProjection(QLatin1Literal("epsg:3571"),
                   QLatin1Literal("North Pole LAEA Bering Sea"),
                   QcVectorDouble(180, 90),
                   // QcVector2DDouble(90, 0), // X along 90°W
                   // QcVector2DDouble(0, 0), // Y along 0°E
                   QcInterval2DDouble(-180, 180,
                                      45, 90),
                   QcInterval2DDouble(-HALF_EQUATORIAL_PERIMETER, HALF_EQUATORIAL_PERIMETER,
                                      -HALF_EQUATORIAL_PERIMETER, HALF_EQUATORIAL_PERIMETER),
                   QLatin1Literal("m"),
                   ProjectionSurface::Azimuthal,
                   PreserveBit::PreserveArea)
    {}
};

/**************************************************************************************************/

/*
class QC_EXPORT QcLaeaCoordinate_3571 : public QcGeoCoordinateTemplate<QcLaeaProjection_3571>
{
  Q_GADGET;

  Q_PROPERTY(double x READ y WRITE set_x)
  Q_PROPERTY(double y READ x WRITE set_y)
  Q_PROPERTY(bool is_valid READ is_valid)

 public:
  QcLaeaProjection_3571();
  QcLaeaProjection_3571(double x, double y);
};
*/

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __LAEA_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
