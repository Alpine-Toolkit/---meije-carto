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

#ifndef __VECTOR_HXX__
#define __VECTOR_HXX__

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

template <typename T>
QcVector<T>::QcVector()
  : QcVector(0, 0)
{
  // qInfo() << "QcVector()" << *this << this;
}

template <typename T>
QcVector<T>::QcVector(T x, T y)
  : m_x(x), m_y(y)
{
  // qInfo() << "QcVector(x, y)" << *this << this;
}

template <typename T>
QcVector<T>::QcVector(const QcVector<T> & other)
  : m_x(other.m_x), m_y(other.m_y)
{
  // qInfo() << "QcVector(const QcVector<T> & other)" << *this << &other << this;
}

template <typename T>
QcVector<T>::~QcVector()
{
  // qInfo() << "~QcVector()" << *this << this;
}

template <typename T>
QcVector<T> &
QcVector<T>::operator=(const QcVector<T> & other)
{
  // qInfo() << "operator=(const QcVector<T> & other)" << *this;

  if (this != &other) {
    m_x = other.m_x;
    m_y = other.m_y;
  }

  return *this;
}

template <typename T>
inline T
QcVector<T>::x() const
{
  return m_x;
}

template <typename T>
inline void
QcVector<T>::set_x(T value)
{
  m_x = value;
}

template <typename T>
inline T
QcVector<T>::y() const
{
  return m_y;
}

template <typename T>
inline void QcVector<T>::set_y(T value)
{
  m_y = value;
}

template <typename T>
T &
QcVector<T>::operator[](int i)
{
  if (i == 0)
    return m_x;
  else if (i == 1)
    return m_y;
  else
    throw std::invalid_argument("invalid index");
}

template <typename T>
T
QcVector<T>::operator[](int i) const
{
  if (i == 0)
    return m_x;
  else if (i == 1)
    return m_y;
  else
    throw std::invalid_argument("invalid index");
}

template <typename T>
inline T
QcVector<T>::magnitude_square() const
{
  return m_x * m_x + m_y * m_y;
}

template <typename T>
inline T
QcVector<T>::magnitude() const
{
  return sqrt(magnitude_square());
}

template <typename T>
void
QcVector<T>::normalise()
{
  *this /= magnitude();
}

template <typename T>
QcVector<T>
QcVector<T>::normalised() const
{
  return *this / magnitude();
}

template <typename T>
inline bool
QcVector<T>::is_null() const
{
  return m_x == 0 && m_y == 0;
}

template <typename T>
bool
QcVector<T>::operator==(const QcVector<T> & other) const
{
  return (m_x == other.m_x) && (m_y == other.m_y);
}

template <typename T>
inline bool
QcVector<T>::operator!=(const QcVector<T> & other) const
{
  return !operator==(other);
}

template <typename T>
QcVector<T> &
QcVector<T>::operator+=(const QcVector<T> & other)
{
  m_x += other.m_x;
  m_y += other.m_y;
  return *this;
}

template <typename T>
QcVector<T>
operator+(const QcVector<T> & vector1, const QcVector<T> & vector2)
{
  return QcVector<T>(vector1.m_x + vector2.m_x, vector1.m_y + vector2.m_y);
}

template <typename T>
QcVector<T> &
QcVector<T>::operator-=(const QcVector<T> & other) {
  m_x -= other.m_x;
  m_y -= other.m_y;
  return *this;
}

template <typename T>
QcVector<T>
operator-(const QcVector<T> & vector1, const QcVector<T> & vector2)
{
  return QcVector<T>(vector1.m_x - vector2.m_x, vector1.m_y - vector2.m_y);
}

template <typename T>
QcVector<T> &
QcVector<T>::operator*=(T factor)
{
  m_x *= factor;
  m_y *= factor;
  return *this;
}

template <typename T>
QcVector<T>
operator*(const QcVector<T> & vector, T factor)
{
  return QcVector<T>(vector.m_x * factor, vector.m_y * factor);
}

template <typename T>
QcVector<T> &
QcVector<T>::operator/=(T factor)
{
  m_x /= factor;
  m_y /= factor;
  return *this;
}

template <typename T>
QcVector<T>
operator/(const QcVector<T> & vector, T factor)
{
  return QcVector<T>(vector.m_x / factor, vector.m_y / factor);
}

// Return the orientation in degree
template <typename T>
T
QcVector<T>::orientation() const
{
  /*
   * 2 | 1
   * - + -
   * 4 | 3
   *
   *       | 1    | 2         | 3    | 4         |
   * x     | +    | -         | +    | -         |
   * y     | +    | +         | -    | -         |
   * tan   | +    | -         | -    | +         |
   * atan  | +    | -         | -    | +         |
   * theta | atan | atan + pi | atan | atan - pi |
   */

  if (is_null())
    throw std::invalid_argument("Null Vector");

  if (m_x == 0)
    return copysign(90, m_y);
  else if (m_y == 0)
    return m_x >= 0 ? 0 : 180;
  else {
    orientation = qRadiansToDegrees(atan(tan()));
    if (m_x < 0) {
      if (m_y > 0)
	orientation += 180;
      else
	orientation -= 180;
    }
    return orientation;
  }
}

// Return a new vector equal to self rotated of angle degree in the counter clockwise direction
template <typename T>
QcVector<T>
QcVector<T>::rotate_counter_clockwise(T angle) const
{
  T radians = qDegreesToRadians(angle);
  T c = cos(radians);
  T s = sin(radians);

  T xp = c * m_x - s * m_y;
  T yp = s * m_x + c * m_y;

  return QcVector<T>(xp, yp);
}

// Return a new vector equal to self rotated of 90 degree in the counter clockwise direction
template <typename T>
QcVector<T>
QcVector<T>::rotate_counter_clockwise_90() const
{
  return QcVector<T>(-m_y, m_x);
}

// Return a new vector equal to  rotated of 90 degree in the clockwise direction
template <typename T>
QcVector<T>
QcVector<T>::rotate_clockwise_90() const {
  return QcVector<T>(m_y, -m_x);
}

// Return a new vector equal to  rotated of 180 degree
// parity
template <typename T>
QcVector<T>
QcVector<T>::rotate_180() const
{
  return QcVector<T> (-m_x, -m_y);
}

template <typename T>
QcVector<T>
QcVector<T>::mirror_x() const
{
  return QcVector<T>(-m_x, m_y);
}

template <typename T>
QcVector<T>
QcVector<T>::mirror_y() const
{
  return QcVector<T>(m_x, -m_y);
}

// Return the tangent
template <typename T>
T
QcVector<T>::tan() const
{
  if (m_x != 0)
    return m_y / m_x;
  else
    throw std::invalid_argument("x is null");
}

// Return the inverse tangent
template <typename T>
T
QcVector<T>::inverse_tan() const
{
  if (m_y != 0)
    return m_x / m_y;
  else
    throw std::invalid_argument("y is null");
}

// Return the dot product of  with other
template <typename T>
T
QcVector<T>::dot(const QcVector<T> & other) const
{
  return m_x * other.m_x + m_y * other.m_y;
}

// Return the cross product of  with other
template <typename T>
T
QcVector<T>::cross(const QcVector<T> & other) const
{
  return m_x * other.m_y - m_y * other.m_x;
}

// is parallel with other
template <typename T>
bool
QcVector<T>::is_parallel(const QcVector<T> & other) const
{
  // Fixme: round
  return round(cross(other), 7) == 0;
}

// is orthogonal with other
template <typename T>
bool
QcVector<T>::is_orthogonal(const QcVector<T> & other) const
{
  // Fixme: round
  return round(dot(other), 7) == 0;
}

// Return the cosinus of  with direction
template <typename T>
T
QcVector<T>::cos_with(const QcVector<T> & direction) const
{
  T cos = dot(direction) / (direction.magnitude() * magnitude());
  return trignometric_clamp(cos);
}

//  Return the projection of  on direction
template <typename T>
T
QcVector<T>::projection_on(const QcVector<T> & direction) const
{
  return dot(direction) / direction.magnitude();
}

// Return the sinus of  with other
template <typename T>
T
QcVector<T>::sin_with(const QcVector<T> & direction) const
{
  // turn from direction to
  T sin = direction.cross(*this) / (direction.magnitude() * magnitude());
  return trignometric_clamp(sin);
}

// Return the tan of  with other
template <typename T>
T
QcVector<T>::tan_with(const QcVector<T> & direction) const
{
  // Fixme: direction <-> this
  return direction.cross(*this) / direction.dot(*this);
}

// Return the atan of  with other
template <typename T>
T
QcVector<T>::atan_with(const QcVector<T> & direction) const
{
  return atan(tan_with(direction));
}

// Return the deviation of  with other
template <typename T>
T
QcVector<T>::deviation_with(const QcVector<T> & direction) const
{
  return direction.cross(*this) / direction.magnitude();
}

template <typename T>
T
QcVector<T>::radians_with(const QcVector<T> & direction) const
{
  // Fixme: check all cases
  // -> angle_with
  //  Return the angle of on direction

  T angle = acos(cos_with(direction));
  T angle_sign = sign(sin_with(direction));

  return angle_sign * angle;
}

template <typename T>
T
QcVector<T>::orientation_with(const QcVector<T> & direction) const
{
  return qRadiansToDegrees(radians_with(direction));
}

template <typename T>
QcInterval2D<T>
QcVector<T>::to_interval() const
{
  return QcInterval2D<T>(m_x, m_x, m_y, m_y);
}

/**************************************************************************************************/

#endif /* __VECTOR_HXX__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
