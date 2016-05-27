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

#include "mercator.h"

#include "wgs84.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

/**************************************************************************************************/

QcGeoCoordinateWebMercator::QcGeoCoordinateWebMercator(double x, double y)
  : QcGeoCoordinateTemplate(x, y)
{}

QcGeoCoordinateWebMercator::QcGeoCoordinateWebMercator()
  : QcGeoCoordinateTemplate(.0, .0)
{}

QcGeoCoordinateWGS84
QcGeoCoordinateWebMercator::wgs84() const
{
  double longitude = x() / EQUATORIAL_RADIUS;
  double _y = y() / EQUATORIAL_RADIUS;
  double latitude = 2*atan(exp(_y)) - M_HALF_PI;

  return QcGeoCoordinateWGS84(qRadiansToDegrees(longitude), qRadiansToDegrees(latitude));
}

QcGeoCoordinatePseudoWebMercator
QcGeoCoordinateWebMercator::pseudo_web_mercator() const
{
  double _x = x() + HALF_EQUATORIAL_PERIMETER;
  double _y = HALF_EQUATORIAL_PERIMETER - y();

  return QcGeoCoordinatePseudoWebMercator(_x, _y);
}

QcGeoCoordinateNormalisedWebMercator
QcGeoCoordinateWebMercator::normalised_web_mercator() const
{
  // Fixme: duplicated code
  double _x = (x() + HALF_EQUATORIAL_PERIMETER) * INVERSE_EQUATORIAL_PERIMETER;
  double _y = (HALF_EQUATORIAL_PERIMETER - y()) * INVERSE_EQUATORIAL_PERIMETER;

  return QcGeoCoordinateNormalisedWebMercator(_x, _y);
}

/**************************************************************************************************/

QcGeoCoordinatePseudoWebMercator::QcGeoCoordinatePseudoWebMercator(double x, double y)
  : QcGeoCoordinateTemplate()
{
  // Fixme: right or in viewport ???
  /*
  if (is_valid_x(x) and is_valid_x(y)) {
    x() = x;
    y() = y;
  }
  */

  // Adjust coordinate if outside domain
  set_x(cls_projection.x_extent().wrap(x));
  set_y(cls_projection.y_extent().truncate(y));
}

QcGeoCoordinatePseudoWebMercator::QcGeoCoordinatePseudoWebMercator()
  : QcGeoCoordinateTemplate(.0, .0)
{}

QcGeoCoordinateWGS84
QcGeoCoordinatePseudoWebMercator::wgs84() const
{
  return web_mercator().wgs84();
}

QcGeoCoordinateWebMercator
QcGeoCoordinatePseudoWebMercator::web_mercator() const
{
  double _x = x() - HALF_EQUATORIAL_PERIMETER;
  double _y = HALF_EQUATORIAL_PERIMETER - y();

  return QcGeoCoordinateWebMercator(_x, _y);
}

QcGeoCoordinateNormalisedWebMercator
QcGeoCoordinatePseudoWebMercator::normalised_web_mercator() const
{
  double _x = x() * INVERSE_EQUATORIAL_PERIMETER;
  double _y = y() * INVERSE_EQUATORIAL_PERIMETER;

  return QcGeoCoordinateNormalisedWebMercator(_x, _y);
}

/**************************************************************************************************/

QcGeoCoordinateNormalisedWebMercator::QcGeoCoordinateNormalisedWebMercator(double x, double y)
  : QcGeoCoordinateTemplate(x, y)
{}

QcGeoCoordinateNormalisedWebMercator::QcGeoCoordinateNormalisedWebMercator()
  : QcGeoCoordinateTemplate(.0, .0)
{}

QcGeoCoordinateWebMercator
QcGeoCoordinateNormalisedWebMercator::web_mercator() const
{
  double _x = x() * EQUATORIAL_PERIMETER - HALF_EQUATORIAL_PERIMETER;
  double _y = HALF_EQUATORIAL_PERIMETER - y() * EQUATORIAL_PERIMETER;

  return QcGeoCoordinateWebMercator(_x, _y);
}

QcGeoCoordinatePseudoWebMercator
QcGeoCoordinateNormalisedWebMercator::pseudo_web_mercator() const
{
  double _x = x() * EQUATORIAL_PERIMETER;
  double _y = y() * EQUATORIAL_PERIMETER;

  return QcGeoCoordinatePseudoWebMercator(_x, _y);
}

QcGeoCoordinateWGS84
QcGeoCoordinateNormalisedWebMercator::wgs84() const
{
  // double longitude = M_2PI * x() - M_PI;
  // double latitude = 2*atan(exp(M_PI - M_2PI * y())) - M_HALF_PI;

  // return QcGeoCoordinateWGS84(qRadiansToDegrees(longitude), qRadiansToDegrees(latitude));

  return web_mercator().wgs84();
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
