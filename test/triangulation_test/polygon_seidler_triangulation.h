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

#ifndef __POLYGON_SEIDLER_TRIANGULATION_H__
#define __POLYGON_SEIDLER_TRIANGULATION_H__

/**************************************************************************************************/

// #include <cmath>

// #include <QtCore/QMetaType>
// #include <QVector>
// #include <QList>

// #include "qtcarto_global.h"
// #include "geometry/segment.h"
// #include "geometry/vector.h"
// #include "math/interval.h"
// #include "math/qc_math.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

// class QC_EXPORT QcTriangulation
// {
// };

int triangulate_polygon(int n, double vertices[][2], int triangles[][3]);

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __POLYGON_SEIDLER_TRIANGULATION_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
