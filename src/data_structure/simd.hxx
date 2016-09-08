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

#ifndef __SIMD_HXX__
#define __SIMD_HXX__

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T>
QcSimdPair<T>::QcSimdPair()
  : QcSimdPair(0, 0)
{
  // qInfo() << "QcSimdPair()" << *this << this;
}

template <typename T>
QcSimdPair<T>::QcSimdPair(T x, T y)
  : m_x(x), m_y(y)
{
  // qInfo() << "QcSimdPair(x, y)" << *this << this;
}

template <typename T>
QcSimdPair<T>::QcSimdPair(const QcSimdPair<T> & other)
  : m_x(other.m_x), m_y(other.m_y)
{
  // qInfo() << "QcSimdPair(const QcSimdPair<T> & other)" << *this << &other << this;
}

template <typename T>
QcSimdPair<T>::QcSimdPair(const QPoint & other)
  : m_x(other.x()), m_y(other.y())
{}

template <typename T>
QcSimdPair<T>::QcSimdPair(const QPointF & other)
  : m_x(other.x()), m_y(other.y())
{}

template <typename T>
QcSimdPair<T>::QcSimdPair(const QVector2D & other)
  : m_x(other.x()), m_y(other.y())
{}

template <typename T>
QcSimdPair<T>::~QcSimdPair()
{
  // qInfo() << "~QcSimdPair()" << *this << this;
}

template <typename T>
QcSimdPair<T> &
QcSimdPair<T>::operator=(const QcSimdPair<T> & other)
{
  // qInfo() << "operator=(const QcSimdPair<T> & other)" << *this;

  if (this != &other) {
    m_x = other.m_x;
    m_y = other.m_y;
  }

  return *this;
}

template <typename T>
QPointF
QcSimdPair<T>::to_pointf() const
{
  return QPointF(m_x, m_y);
}

template <typename T>
inline T
QcSimdPair<T>::x() const
{
  return m_x;
}

template <typename T>
inline void
QcSimdPair<T>::set_x(T value)
{
  m_x = value;
}

template <typename T>
inline T
QcSimdPair<T>::y() const
{
  return m_y;
}

template <typename T>
inline void QcSimdPair<T>::set_y(T value)
{
  m_y = value;
}

template <typename T>
T &
QcSimdPair<T>::operator[](int i)
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
QcSimdPair<T>::operator[](int i) const
{
  if (i == 0)
    return m_x;
  else if (i == 1)
    return m_y;
  else
    throw std::invalid_argument("invalid index");
}

template <typename T>
bool
QcSimdPair<T>::operator==(const QcSimdPair<T> & other) const
{
  return (m_x == other.m_x) and (m_y == other.m_y);
}

template <typename T>
inline bool
QcSimdPair<T>::operator!=(const QcSimdPair<T> & other) const
{
  return !operator==(other);
}

template <typename T>
QcSimdPair<T> &
QcSimdPair<T>::operator+=(const QcSimdPair<T> & other)
{
  m_x += other.m_x;
  m_y += other.m_y;
  return *this;
}

template <typename T>
QcSimdPair<T>
operator+(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2)
{
  return QcSimdPair<T>(simd1.m_x + simd2.m_x, simd1.m_y + simd2.m_y);
}

template <typename T>
QcSimdPair<T> &
QcSimdPair<T>::operator-=(const QcSimdPair<T> & other) {
  m_x -= other.m_x;
  m_y -= other.m_y;
  return *this;
}

template <typename T>
QcSimdPair<T>
operator-(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2)
{
  return QcSimdPair<T>(simd1.m_x - simd2.m_x, simd1.m_y - simd2.m_y);
}

template <typename T>
QcSimdPair<T> &
QcSimdPair<T>::operator*=(T factor)
{
  m_x *= factor;
  m_y *= factor;
  return *this;
}

template <typename T>
QcSimdPair<T>
operator*(const QcSimdPair<T> & simd, T factor)
{
  return QcSimdPair<T>(simd.m_x * factor, simd.m_y * factor);
}

template <typename T>
QcSimdPair<T> &
QcSimdPair<T>::operator/=(T factor)
{
  m_x /= factor;
  m_y /= factor;
  return *this;
}

template <typename T>
QcSimdPair<T>
operator/(const QcSimdPair<T> & simd, T factor)
{
  return QcSimdPair<T>(simd.m_x / factor, simd.m_y / factor);
}

template <typename T>
QcSimdPair<T> &
QcSimdPair<T>::operator*=(const QcSimdPair<T> & other)
{
  m_x *= other.m_x;
  m_y *= other.m_y;
  return *this;
}

template <typename T>
QcSimdPair<T>
operator*(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2)
{
  return QcSimdPair<T>(simd1.m_x * simd2.m_x, simd1.m_y * simd2.m_y);
}

template <typename T>
QcSimdPair<T> &
QcSimdPair<T>::operator/=(const QcSimdPair<T> & other)
{
  m_x /= other.m_x;
  m_y /= other.m_y;
  return *this;
}

template <typename T>
QcSimdPair<T>
operator/(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2)
{
  return QcSimdPair<T>(simd1.m_x / simd2.m_x, simd1.m_y / simd2.m_y);
}

/**************************************************************************************************/

template <typename T>
QcSimdTriplet<T>::QcSimdTriplet()
  : QcSimdTriplet(0, 0, 0)
{
  // qInfo() << "QcSimdTriplet()" << *this << this;
}

template <typename T>
QcSimdTriplet<T>::QcSimdTriplet(T x, T y, T z)
  : QcSimdPair<T>(x, y),
    m_z(z)
{
  // qInfo() << "QcSimdTriplet(x, y, z)" << *this << this;
}

template <typename T>
QcSimdTriplet<T>::QcSimdTriplet(const QcSimdTriplet<T> & other)
  : QcSimdPair<T>(other),
    m_z(other.m_z)
{
  // qInfo() << "QcSimdTriplet(const QcSimdTriplet<T> & other)" << *this << &other << this;
}

template <typename T>
QcSimdTriplet<T>::~QcSimdTriplet()
{
  // qInfo() << "~QcSimdTriplet()" << *this << this;
}

template <typename T>
QcSimdTriplet<T> &
QcSimdTriplet<T>::operator=(const QcSimdTriplet<T> & other)
{
  // qInfo() << "operator=(const QcSimdTriplet<T> & other)" << *this;

  if (this != &other) {
    QcSimdPair<T>::operator=(other);
    m_z = other.m_z;
  }

  return *this;
}

template <typename T>
inline T
QcSimdTriplet<T>::z() const
{
  return m_z;
}

template <typename T>
inline void
QcSimdTriplet<T>::set_z(T value)
{
  m_z = value;
}

template <typename T>
T &
QcSimdTriplet<T>::operator[](int i)
{
  if (i == 0)
    return this->x();
  else if (i == 1)
    return this->y();
  else if (i == 2)
    return m_z;
  else
    throw std::invalid_argument("invalid index");
}

template <typename T>
T
QcSimdTriplet<T>::operator[](int i) const
{
  if (i == 0)
    return this->x();
  else if (i == 1)
    return this->y();
  else if (i == 2)
    return m_z;
  else
    throw std::invalid_argument("invalid index");
}

template <typename T>
bool
QcSimdTriplet<T>::operator==(const QcSimdTriplet<T> & other) const
{
  return QcSimdPair<T>::operator==(other) and (m_z == other.m_z);
}

/*
template <typename T>
inline bool
QcSimdTriplet<T>::operator!=(const QcSimdTriplet<T> & other) const
{
  return !operator==(other);
}
*/

template <typename T>
QcSimdTriplet<T> &
QcSimdTriplet<T>::operator+=(const QcSimdTriplet<T> & other)
{
  QcSimdPair<T>::operator+(other);
  m_z += other.m_z;
  return *this;
}

template <typename T>
QcSimdTriplet<T>
operator+(const QcSimdTriplet<T> & simd1, const QcSimdTriplet<T> & simd2)
{
  return QcSimdTriplet<T>(simd1.x() + simd2.x(),
                       simd1.y() + simd2.y(),
                       simd1.m_z + simd2.m_z);
}

template <typename T>
QcSimdTriplet<T> &
QcSimdTriplet<T>::operator-=(const QcSimdTriplet<T> & other) {
  QcSimdPair<T>::operator-=(other);
  m_z -= other.m_z;
  return *this;
}

template <typename T>
QcSimdTriplet<T>
operator-(const QcSimdTriplet<T> & simd1, const QcSimdTriplet<T> & simd2)
{
  return QcSimdTriplet<T>(simd1.x() - simd2.x(),
                       simd1.y() - simd2.y(),
                       simd1.m_z - simd2.m_z
                       );
}

template <typename T>
QcSimdTriplet<T> &
QcSimdTriplet<T>::operator*=(T factor)
{
  QcSimdPair<T>::operator*=(factor);
  m_z *= factor;
  return *this;
}

template <typename T>
QcSimdTriplet<T>
operator*(const QcSimdTriplet<T> & simd, T factor)
{
  return QcSimdTriplet<T>(simd.x() * factor,
                       simd.y() * factor,
                       simd.m_z * factor);
}

template <typename T>
QcSimdTriplet<T> &
QcSimdTriplet<T>::operator/=(T factor)
{
  QcSimdPair<T>::operator/=(factor);
  m_z /= factor;
  return *this;
}

template <typename T>
QcSimdTriplet<T>
operator/(const QcSimdTriplet<T> & simd, T factor)
{
  return QcSimdTriplet<T>(simd.x() / factor,
                       simd.y() / factor,
                       simd.m_z / factor
                       );
}

/**************************************************************************************************/

#endif /* __SIMD_HXX__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
