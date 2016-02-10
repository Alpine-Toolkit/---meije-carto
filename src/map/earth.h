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

#ifndef EARTH_H
#define EARTH_H

/**************************************************************************************************/

#include "qtcarto_global.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

// Equatorial radius (half major axis) of the ellipsoid
constexpr double EQUATORIAL_RADIUS = 6378137.0; // m
constexpr double EQUATORIAL_DIAMETER = 2 * EQUATORIAL_RADIUS; // m
constexpr double HALF_EQUATORIAL_PERIMETER = M_PI * EQUATORIAL_RADIUS; // m
constexpr double EQUATORIAL_PERIMETER = 2 * HALF_EQUATORIAL_PERIMETER; // m

// QT_END_NAMESPACE

/**************************************************************************************************/

#endif // EARTH_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
