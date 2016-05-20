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

#ifndef __INTERVAL_HXX__
#define __INTERVAL_HXX__

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T>
QcInterval<T>::QcInterval()
  : m_inf(0), m_sup(0), m_empty(true)
{}

template <typename T>
QcInterval<T>::QcInterval(T inf, T sup)
  : m_inf(0), m_sup(0), m_empty(false)
{
  if (inf <= sup) {
    m_inf = inf;
    m_sup = sup;
  }
  else
    throw std::invalid_argument("inf > sup");
}

template <typename T>
QcInterval<T>::QcInterval(const QcInterval<T> & other)
  : m_inf(other.m_inf), m_sup(other.m_sup), m_empty(other.m_empty)
{}

template <typename T>
QcInterval<T> &
QcInterval<T>::operator=(const QcInterval<T> & other)
{
  if (this != &other) {
    m_inf = other.m_inf;
    m_sup = other.m_sup;
    m_empty = other.m_empty;
  }

  return *this;
}

template <typename T>
inline void
QcInterval<T>::set_inf(T value)
{
  // Fixme: check
  m_inf = value;
}

template <typename T>
inline void
QcInterval<T>::set_sup(T value)
{
  m_sup = value;
}

template <typename T>
inline T
QcInterval<T>::length() const
{
  return m_sup - m_inf;
}

template <typename T>
inline T
QcInterval<T>::center() const
{
  return (m_sup + m_inf) / 2; // Fixme: double ?
}

template <typename T>
bool
QcInterval<T>::operator==(const QcInterval<T> & other) const
{
  return (m_empty == other.m_empty) and (m_inf == other.m_inf) and (m_sup == other.m_sup);
}

template <typename T>
inline bool
QcInterval<T>::operator!=(const QcInterval<T> & other) const
{
  return !operator==(other);
}

template <typename T>
inline bool
QcInterval<T>::operator<(const QcInterval<T> & other) const
{
  return m_sup < other.m_inf;
}

template <typename T>
inline bool
QcInterval<T>::operator>(const QcInterval<T> & other) const
{
  return other.m_sup < m_inf;
}

template <typename T>
QcInterval<T> &
QcInterval<T>::operator+=(T dx)
{
  if (! m_empty) {
    m_inf += dx;
    m_sup += dx;
  }
  return *this;
}

template <typename T>
QcInterval<T>
operator+(const QcInterval<T> & interval, T dx)
{
  if (interval.m_empty)
    return QcInterval<T>();
  else
    return QcInterval<T>(interval.m_inf + dx, interval.m_sup + dx);
}

template <typename T>
QcInterval<T> &
QcInterval<T>::operator-=(T dx)
{
  if (! m_empty) {
    m_inf -= dx;
    m_sup -= dx;
  }
  return *this;
}

template <typename T>
QcInterval<T>
operator-(const QcInterval<T> & interval, T dx)
{
  if (interval.m_empty)
    return QcInterval<T>();
  else
    return QcInterval<T>(interval.m_inf - dx, interval.m_sup - dx);
}

template <typename T>
void
QcInterval<T>::enlarge(T dx)
{
  if (! m_empty) {
    m_inf -= dx;
    m_sup += dx;
  }
}

// Test if x is in the interval?
template <typename T>
bool
QcInterval<T>::contains(T x) const
{
  if (m_empty)
    return false;
  else
    return m_inf <= x and x <= m_sup;
}

template <typename T>
T
QcInterval<T>::truncate(T x) const
{
  if (x > m_sup)
    return m_sup;
  else if (x < m_inf)
    return m_inf;
  else
    return x;
}

template <typename T>
T
QcInterval<T>::wrap(T x) const
{
  if (contains(x))
    return x;
  else if (x > m_inf)
    return m_inf + ((x - m_inf) % length());
  else
    return m_sup - ((m_inf - x) % length());
}

// Test whether the interval intersects with i2?
template <typename T>
bool
QcInterval<T>::intersect(const QcInterval<T> & other) const
{
  if (m_empty or other.m_empty)
    return false;
  else
    return m_inf <= other.m_sup and other.m_inf <= m_sup;
}

// Test whether the interval is included in i1?
template <typename T>
bool
QcInterval<T>::is_included_in(const QcInterval<T> & other) const
{
  return other.m_inf <= m_inf and m_sup <= other.m_sup;
}

// Test whether the interval is outside of i2?
template <typename T>
bool
QcInterval<T>::is_outside_of(const QcInterval<T> & other) const
{
  if (m_empty or other.m_empty)
    return false;
  else
    return m_inf < other.m_inf and other.m_sup < m_sup;
}

// Intersection
template <typename T>
QcInterval<T> &
QcInterval<T>::operator&=(const QcInterval<T> & other)
{
  if (intersect(other)) {
    m_inf = std::max(m_inf, other.m_inf);
    m_sup = std::min(m_sup, other.m_sup);
  }
  else {
    m_inf = 0;
    m_sup = 0;
    m_empty = true;
  }
  return *this;
}

template <typename T>
QcInterval<T>
operator&(const QcInterval<T> & interval1, const QcInterval<T> & interval2)
{
  if (interval1.intersect(interval2)) {
    return QcInterval<T>(std::max(interval1.m_inf, interval2.m_inf),
			 std::min(interval1.m_sup, interval2.m_sup));
  }
  else {
    return QcInterval<T>();
  }
}

// Union
template <typename T>
QcInterval<T> &
QcInterval<T>::operator|=(const QcInterval<T> & other) {
  if (!(m_empty or other.m_empty)) {
    m_inf = std::min(m_inf, other.m_inf);
    m_sup = std::max(m_sup, other.m_sup);
  }
  else if (m_empty and !other.m_empty) {
    m_inf = other.m_inf;
    m_sup = other.m_sup;
  }
  return *this;
}

template <typename T>
QcInterval<T>
operator|(const QcInterval<T> & interval1, const QcInterval<T> & interval2)
{
  if (!(interval1.m_empty or interval2.m_empty))
    return QcInterval<T>(std::min(interval1.m_inf, interval2.m_inf),
			 std::max(interval1.m_sup, interval2.m_sup));
  else if (interval1.m_empty and !interval2.m_empty)
    return QcInterval<T>(*interval2);
  else
    return QcInterval<T>(*interval1);
}

/**************************************************************************************************/

template<> inline int QcIntervalInt::length() const {
  return m_sup - m_inf +1;
}

/**************************************************************************************************/

template <typename T>
QcInterval2D<T>::QcInterval2D()
  : m_x(), m_y()
{}

template <typename T>
QcInterval2D<T>::QcInterval2D(T x_inf, T x_sup, T y_inf, T y_sup)
  : m_x(x_inf, x_sup), m_y(y_inf, y_sup)
{}

template <typename T>
QcInterval2D<T>::QcInterval2D(const QcInterval<T> & x, const QcInterval<T> & y)
  : m_x(x), m_y(y)
{}

template <typename T>
QcInterval2D<T>::QcInterval2D(const QcInterval2D<T> & other)
  : m_x(other.m_x), m_y(other.m_y)
{}

template <typename T>
QcInterval2D<T> &
QcInterval2D<T>::operator=(const QcInterval2D<T> & other)
{
  if (this != &other) {
    m_x = other.m_x;
    m_y = other.m_y;
  }

  return *this;
}

template <typename T>
inline bool
QcInterval2D<T>::is_empty() const {
  return m_x.is_empty() or m_y.is_empty();
}

template <typename T>
bool
QcInterval2D<T>::operator==(const QcInterval2D<T> & other) const {
  return (m_x == other.m_x) and (m_y == other.m_y);
}

template <typename T>
inline bool
QcInterval2D<T>::operator!=(const QcInterval2D<T> & other) const
{
  return !operator==(other);
}

template <typename T>
void
QcInterval2D<T>::enlarge(T dx)
{
  m_x.enlarge(dx);
  m_y.enlarge(dx);
}

// Test if x is in the interval?
template <typename T>
bool
QcInterval2D<T>::contains(T x, T y) const
{
  return m_x.contains(x) and m_y.contains(y);
}

// Test whether the interval intersects with i2?
template <typename T>
bool
QcInterval2D<T>::intersect(const QcInterval2D<T> & other) const
{
  return m_x.intersect(other.m_x) and m_y.intersect(other.m_y);
}

// Test whether the interval is included in i1?
template <typename T>
bool
QcInterval2D<T>::is_included_in(const QcInterval2D<T> & other) const
{
  return m_x.is_included_in(other.m_x) and m_y.is_included_in(other.m_y);
}

// Test whether the interval is outside of i2?
template <typename T>
bool
QcInterval2D<T>::is_outside_of(const QcInterval2D<T> & other) const
{
  return m_x.is_outside_of(other.m_x) and m_y.is_outside_of(other.m_y);
}

// Intersection
template <typename T>
QcInterval2D<T> &
QcInterval2D<T>::operator&=(const QcInterval2D<T> & other)
{
  m_x &= other.m_x;
  m_y &= other.m_y;
  return *this;
}

/*
  QcInterval2D<T> operator&(const QcInterval2D<T> & interval1, const QcInterval2D<T> & T interval2) {
  if (interval1.intersect(interval2)) {
  return QcInterval2D(interval1.m_x & interval2.m_x,
  interval1.m_y & interval2.m_y);
  }
  else {
  return QcInterval2D();
  }
  }
*/

// Union
template <typename T>
QcInterval2D<T> &
QcInterval2D<T>::operator|=(const QcInterval2D<T> & other)
{
  m_x |= other.m_x;
  m_y |= other.m_y;
  return *this;
}

/*
  QcInterval2D<T> operator|(const QcInterval2D<T> & interval1, const QcInterval2D<T> & T interval2) {
  if (!(interval1.is_empty() or interval2.is_empty()))
  return QcInterval2D(interval1.m_x | interval2.m_x,
  interval1.m_y | interval2.m_y);
  else if (interval1.empty() and !interval2.is_empty())
  return QcInterval<T>(*interval2);
  else
  return QcInterval<T>(*interval1);
  }
*/

/**************************************************************************************************/

template <typename T>
QcInterval3D<T>::QcInterval3D()
  : QcInterval2D<T>(),
    m_z()
{}

template <typename T>
QcInterval3D<T>::QcInterval3D(T x_inf, T x_sup, T y_inf, T y_sup, T z_inf, T z_sup)
  : QcInterval2D<T>(x_inf, x_sup, y_inf, y_sup),
    m_z(z_inf, z_sup)
{}

template <typename T>
QcInterval3D<T>::QcInterval3D(const QcInterval<T> & x, const QcInterval<T> & y, const QcInterval<T> & z)
  : QcInterval2D<T>(x, y),
    m_z(z)
{}

template <typename T>
QcInterval3D<T>::QcInterval3D(const QcInterval3D<T> & other)
  : QcInterval2D<T>(other),
    m_z(other.m_z)
{}

template <typename T>
QcInterval2D<T>
QcInterval3D<T>::to_2d() const
{
  return QcInterval2D<T>(this->x(), this->y());
}

template <typename T>
QcInterval3D<T> &
QcInterval3D<T>::operator=(const QcInterval3D<T> & other)
{
  if (this != &other) {
    QcInterval2D<T>::operator=(other);
    m_z = other.m_z;
  }

  return *this;
}

template <typename T>
inline bool
QcInterval3D<T>::is_empty() const {
  return QcInterval2D<T>::is_empty() or m_z.is_empty();
}

template <typename T>
bool
QcInterval3D<T>::operator==(const QcInterval3D<T> & other) const {
  return QcInterval2D<T>::operator=(other) and (m_z == other.m_z);
}

template <typename T>
inline bool
QcInterval3D<T>::operator!=(const QcInterval3D<T> & other) const
{
  return !operator==(other);
}

template <typename T>
void
QcInterval3D<T>::enlarge(T dx)
{
  QcInterval2D<T>::enlarge(dx);
  m_z.enlarge(dx);
}

// Test if x is in the interval?
template <typename T>
bool
QcInterval3D<T>::contains(T x, T y, T z) const
{
  return QcInterval2D<T>::contains(x, y) and m_z.contains(z);
}

// Test whether the interval intersects with i2?
template <typename T>
bool
QcInterval3D<T>::intersect(const QcInterval3D<T> & other) const
{
  return QcInterval2D<T>::intersect(other) and m_z.intersect(other.m_z);
}

// Test whether the interval is included in i1?
template <typename T>
bool
QcInterval3D<T>::is_included_in(const QcInterval3D<T> & other) const
{
  return QcInterval2D<T>::is_included_in(other) and m_z.is_included_in(other.m_z);
}

// Test whether the interval is outside of i2?
template <typename T>
bool
QcInterval3D<T>::is_outside_of(const QcInterval3D<T> & other) const
{
  return QcInterval2D<T>::is_outside_of(other) and m_z.is_outside_of(other.m_z);
}

// Intersection
template <typename T>
QcInterval3D<T> &
QcInterval3D<T>::operator&=(const QcInterval3D<T> & other)
{
  QcInterval2D<T>::operator&=(other);
  m_z &= other.m_z;
  return *this;
}

/*
  QcInterval3D<T> operator&(const QcInterval3D<T> & interval1, const QcInterval3D<T> & T interval2) {
  if (interval1.intersect(interval2)) {
  return QcInterval3D(interval1.m_x & interval2.m_x,
  interval1.m_y & interval2.m_y);
  }
  else {
  return QcInterval3D();
  }
  }
*/

// Union
template <typename T>
QcInterval3D<T> &
QcInterval3D<T>::operator|=(const QcInterval3D<T> & other)
{
  QcInterval2D<T>::operator&=(other);
  m_z |= other.m_z;
  return *this;
}

/**************************************************************************************************/

// QT_END_NAMESPACE

/**************************************************************************************************/

#endif /* __INTERVAL_HXX__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
