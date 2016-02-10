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

#ifndef QC_MATH_H
#define QC_MATH_H

/**************************************************************************************************/

#include <cmath>

/**************************************************************************************************/

#ifndef M_PI
// constepxr double
#define M_PI 3.14159265358979323846
#endif

#ifndef M_2PI
// constepxr double
#define M_2PI 2 * M_PI
#endif

/**************************************************************************************************/

// Fixme: static ?
inline static double
haversine(double theta)
{
  return .5*(1 - cos(theta)); // = sin(theta / 2)**2
}

/**************************************************************************************************/

inline static
double
middle(double a, double b)
{
  return .5*(a + b);
}

/**************************************************************************************************/

// Fixme: sign_of ?
inline static
double
sign(double x)
{
  return copysign(1., x);
}

/**************************************************************************************************/

inline static
double
epsilon_float(double a, double b, double epsilon = 1e-3)
{
  return fabs(a-b) <= epsilon;
}

/**************************************************************************************************/

// Clamp x in the range [-1.,1].
double trignometric_clamp(double x);

/**************************************************************************************************/

inline static
bool
is_in_trignometric_range(double x)
{
  return -1. <= x && x <= 1.;
}

/**************************************************************************************************/

#endif // QC_MATH_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
