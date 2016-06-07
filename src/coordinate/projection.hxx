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

#ifndef __PROJECTION_HXX__
#define __PROJECTION_HXX__

/**************************************************************************************************/

#include <QDataStream>
#include <QDebug>

/**************************************************************************************************/

template <typename Projection>
const Projection QcGeoCoordinateTemplate<Projection>::cls_projection;

template <typename Projection>
QcGeoCoordinateTemplate<Projection>::QcGeoCoordinateTemplate(double x, double y)
  : QcGeoCoordinateTrait()
{
  if (cls_projection.is_valid_xy(x, y)) {
    set_x(x);
    set_y(y);
  } else {
    qWarning() << "Invalid coordinate" << cls_projection.srid() << x << y;
    throw std::invalid_argument("Invalid coordinate");
  }
}

template <typename Projection>
QcGeoCoordinateTemplate<Projection>::QcGeoCoordinateTemplate(const QcVectorDouble & vector)
  : QcGeoCoordinateTemplate(vector.x(), vector.y())
{}

/**************************************************************************************************/

#endif /* __PROJECTION_HXX__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
