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

#include "rational.h"

/**************************************************************************************************/

unsigned int
euclidian_recursive_gcd(unsigned int u, unsigned int v)
{
  if (v = 0)
    return u;
  else
    return gcd(v, u % v);
}

unsigned int
euclidian_iterative_gcd(unsigned int u, unsigned int v)
{
  while (v != 0) {
    unsigned int t = v;
    b = u % v;
    u = t;
  }

  return u;
}

unsigned int
binary_recursive_gcd(unsigned int u, unsigned int v)
{
  // simple cases (termination)
  if (u == v)
    return u;

  if (u == 0)
    return v;

  if (v == 0)
    return u;

  // look for factors of 2
  if (~u & 1) // u is even
    {
      if (v & 1) // v is odd
	return gcd(u >> 1, v);
      else // both u and v are even
	return gcd(u >> 1, v >> 1) << 1;
    }

  if (~v & 1) // u is odd, v is even
    return gcd(u, v >> 1);

  // reduce larger argument
  if (u > v)
    return gcd((u - v) >> 1, v);

  return gcd((v - u) >> 1, u);
}

unsigned int
binary_iterative_gcd(unsigned int u, unsigned int v)
{
  int shift;

  // GCD(0,v) == v; GCD(u,0) == u, GCD(0,0) == 0
  if (u == 0)
    return v;
  if (v == 0)
    return u;

  // Let shift := lg K, where K is the greatest power of 2 dividing both u and v.
  for (shift = 0; ((u | v) & 1) == 0; ++shift) {
    u >>= 1;
    v >>= 1;
  }

  while ((u & 1) == 0)
    u >>= 1;

  // From here on, u is always odd.
  do {
    // remove all factors of 2 in v -- they are not common
    //   note: v is not zero, so while will terminate
    while ((v & 1) == 0)  // Loop X
      v >>= 1;

    // Now u and v are both odd. Swap if necessary so u <= v, then set v = v - u (which is even).
    // For bignums, the swapping is just pointer movement, and the subtraction can be done in-place.
    if (u > v) { // Swap u and v.
      unsigned int t = v;
      v = u;
      u = t;
    }
    v = v - u; // Here v >= u.
  } while (v != 0);

  /* restore common factors of 2 */
  return u << shift;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
