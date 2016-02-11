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
  if (v == 0)
    return u;
  else
    return euclidian_recursive_gcd(v, u % v);
}

unsigned int
euclidian_iterative_gcd(unsigned int u, unsigned int v)
{
  while (v != 0) {
    unsigned int t = v;
    v = u % v;
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
	return binary_recursive_gcd(u >> 1, v);
      else // both u and v are even
	return binary_recursive_gcd(u >> 1, v >> 1) << 1;
    }

  if (~v & 1) // u is odd, v is even
    return binary_recursive_gcd(u, v >> 1);

  // reduce larger argument
  if (u > v)
    return binary_recursive_gcd((u - v) >> 1, v);

  return binary_recursive_gcd((v - u) >> 1, u);
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

/**************************************************************************************************/

QcRational::QcRational()
  : QcRational(0, 0)
{}

QcRational::QcRational(unsigned int numerator, unsigned int denominator, int sign)
  : m_numerator(numerator), m_denominator(denominator), m_sign(sign < 0 ? -1 : 1)
{}

QcRational::QcRational(const QcRational & other)
  : m_numerator(other.m_numerator), m_denominator(other.m_denominator), m_sign(other.m_sign)
{}

QcRational::~QcRational()
{}

QcRational &
QcRational::operator=(const QcRational & other)
{
  if (this != &other) {
    m_numerator = other.m_numerator;
    m_denominator = other.m_denominator;
  }

  return *this;
}

double
QcRational::to_double() const
{
  if (is_null())
    return .0;
  else if (is_one())
    return m_sign;
  else if (is_infinite())
    // return +/- inf
    throw std::invalid_argument("Denominator is null");
  else if (is_integer())
    return m_sign * m_numerator;
  else
    return m_sign * (m_numerator / m_denominator);
}

void
QcRational::simplify()
{
  unsigned int gcd = binary_iterative_gcd(m_numerator, m_denominator);
  m_numerator /= gcd;
  m_denominator /= gcd;
}

QcRational &
QcRational::operator+=(const QcRational & other) {
  m_denominator *= other.m_denominator;
  unsigned int a = m_numerator * other.m_denominator;
  unsigned int b = other.m_numerator * m_denominator;
  if (same_sign(other)) {
    m_numerator = a + b;
  }
  else if (is_negative()) {
    if (b > a) {
      m_numerator = b - a;
      m_sign = 1;
    }
    else {
      m_numerator = a - b;
    }
  }
  else {
    if (a > b) {
      m_numerator = a - b;
    }
    else {
      m_numerator = a - b;
      m_sign = -1;
    }
  }
  return *this;
}

QcRational
operator+(const QcRational & rational1, const QcRational & rational2)
{
  QcRational rational(rational1);
  rational += rational2;
  return rational;
}

QcRational &
QcRational::operator-=(const QcRational & other) {
  QcRational rational2(other);
  rational2.inverse_sign();
  *this += rational2;
  return *this;
}

QcRational
operator-(const QcRational & rational1, const QcRational & rational2)
{
  QcRational rational(rational1);
  rational -= rational2;
  return rational;
}

QcRational &
QcRational::operator*=(const QcRational & other) {
  m_numerator *= other.m_numerator;
  m_denominator *= other.m_denominator;
  m_sign *= other.m_sign;
  return *this;
}

QcRational
operator*(const QcRational & rational1, const QcRational & rational2)
{
  return QcRational(rational1.m_numerator * rational2.m_numerator,
		    rational1.m_denominator * rational2.m_denominator,
		    rational1.m_sign * rational2.m_sign);
}

QcRational &
QcRational::operator/=(const QcRational & other) {
  m_numerator *= other.m_denominator;
  m_denominator *= other.m_numerator;
  m_sign *= other.m_sign;
  return *this;
}

QcRational
operator/(const QcRational & rational1, const QcRational & rational2)
{
  return QcRational(rational1.m_numerator * rational2.m_denominator,
		    rational1.m_denominator * rational2.m_numerator,
		    rational1.m_sign * rational2.m_sign);
}

QcRational &
QcRational::operator*=(unsigned int factor) {
  m_numerator *= factor;
  return *this;
}

QcRational
operator*(const QcRational & rational, unsigned int factor)
{
  return QcRational(rational.m_numerator * factor, rational.m_denominator);
}

QcRational &
QcRational::operator/=(unsigned int factor)
{
  m_denominator *= factor;
  return *this;
}

QcRational
operator/(const QcRational & rational, unsigned int factor)
{
  return QcRational(rational.m_numerator, rational.m_denominator * factor);
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
