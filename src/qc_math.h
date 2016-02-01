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

#ifndef QC_MATH_H
#define QC_MATH_H

/**************************************************************************************************/

#include <cmath>

/**************************************************************************************************/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**************************************************************************************************/

inline static double haversine(double theta)
{
  return .5*(1 - cos(theta)); // = sin(theta / 2)**2
}

/**************************************************************************************************/

#endif // QC_MATH_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
