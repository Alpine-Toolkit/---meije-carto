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

QcWebMercatorCoordinate::QcWebMercatorCoordinate(double _x, double y)
// : QcGeoCoordinateTemplate(x, y)
{
  // Adjust coordinate if outside domain
  set_x(cls_projection.x_projected_interval().wrap(_x));
  set_y(cls_projection.y_projected_interval().truncate(y));

  qInfo() << "x adjust" << static_cast<int>(_x) << static_cast<int>(x());
}

QcWebMercatorCoordinate::QcWebMercatorCoordinate()
  : QcGeoCoordinateTemplate(.0, .0)
{}

QcWgsCoordinate
QcWebMercatorCoordinate::wgs84() const
{
  double longitude = x() / EQUATORIAL_RADIUS;
  double _y = y() / EQUATORIAL_RADIUS;
  double latitude = 2*atan(exp(_y)) - M_HALF_PI;

  return QcWgsCoordinate(qRadiansToDegrees(longitude), qRadiansToDegrees(latitude));
}

QcPseudoWebMercatorCoordinate
QcWebMercatorCoordinate::pseudo_web_mercator() const
{
  double _x = x() + HALF_EQUATORIAL_PERIMETER;
  double _y = HALF_EQUATORIAL_PERIMETER - y();

  return QcPseudoWebMercatorCoordinate(_x, _y);
}

QcNormalisedWebMercatorCoordinate
QcWebMercatorCoordinate::normalised_web_mercator() const
{
  // Fixme: duplicated code
  double _x = (x() + HALF_EQUATORIAL_PERIMETER) * INVERSE_EQUATORIAL_PERIMETER;
  double _y = (HALF_EQUATORIAL_PERIMETER - y()) * INVERSE_EQUATORIAL_PERIMETER;

  return QcNormalisedWebMercatorCoordinate(_x, _y);
}

/**************************************************************************************************/

QcPseudoWebMercatorCoordinate::QcPseudoWebMercatorCoordinate(double x, double y)
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
  set_x(cls_projection.x_projected_interval().wrap(x));
  set_y(cls_projection.y_projected_interval().truncate(y));
}

QcPseudoWebMercatorCoordinate::QcPseudoWebMercatorCoordinate()
  : QcGeoCoordinateTemplate(.0, .0)
{}

QcWgsCoordinate
QcPseudoWebMercatorCoordinate::wgs84() const
{
  return web_mercator().wgs84();
}

QcWebMercatorCoordinate
QcPseudoWebMercatorCoordinate::web_mercator() const
{
  double _x = x() - HALF_EQUATORIAL_PERIMETER;
  double _y = HALF_EQUATORIAL_PERIMETER - y();

  return QcWebMercatorCoordinate(_x, _y);
}

QcNormalisedWebMercatorCoordinate
QcPseudoWebMercatorCoordinate::normalised_web_mercator() const
{
  double _x = x() * INVERSE_EQUATORIAL_PERIMETER;
  double _y = y() * INVERSE_EQUATORIAL_PERIMETER;

  return QcNormalisedWebMercatorCoordinate(_x, _y);
}

/**************************************************************************************************/

QcNormalisedWebMercatorCoordinate::QcNormalisedWebMercatorCoordinate(double x, double y)
  : QcGeoCoordinateTemplate(x, y)
{}

QcNormalisedWebMercatorCoordinate::QcNormalisedWebMercatorCoordinate()
  : QcGeoCoordinateTemplate(.0, .0)
{}

QcWebMercatorCoordinate
QcNormalisedWebMercatorCoordinate::web_mercator() const
{
  double _x = x() * EQUATORIAL_PERIMETER - HALF_EQUATORIAL_PERIMETER;
  double _y = HALF_EQUATORIAL_PERIMETER - y() * EQUATORIAL_PERIMETER;

  return QcWebMercatorCoordinate(_x, _y);
}

QcPseudoWebMercatorCoordinate
QcNormalisedWebMercatorCoordinate::pseudo_web_mercator() const
{
  double _x = x() * EQUATORIAL_PERIMETER;
  double _y = y() * EQUATORIAL_PERIMETER;

  return QcPseudoWebMercatorCoordinate(_x, _y);
}

QcWgsCoordinate
QcNormalisedWebMercatorCoordinate::wgs84() const
{
  // double longitude = M_2PI * x() - M_PI;
  // double latitude = 2*atan(exp(M_PI - M_2PI * y())) - M_HALF_PI;

  // return QcWgsCoordinate(qRadiansToDegrees(longitude), qRadiansToDegrees(latitude));

  return web_mercator().wgs84();
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
