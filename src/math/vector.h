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

#ifndef __VECTOR_H__
#define __VECTOR_H__

/**************************************************************************************************/

#include <cmath>

// #include <QtCore/QMetaType>
#include <QtMath>
#include <QDebug>

#include "qtcarto_global.h"
#include "math/qc_math.h"
#include "math/interval.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T> class QcVector;

template <typename T> QcVector<T> operator+(const QcVector<T> & vector1, const QcVector<T> & vector2);
template <typename T> QcVector<T> operator-(const QcVector<T> & vector1, const QcVector<T> & vector2);
template <typename T> QcVector<T> operator*(const QcVector<T> & vector, T factor);
template <typename T> QcVector<T> operator/(const QcVector<T> & vector, T factor);

template <typename T>
class QC_EXPORT QcVector
{
 public:
  QcVector();
  QcVector(T x, T y);
  QcVector(const QcVector<T> & other);
  ~QcVector();

  QcVector<T> & operator=(const QcVector<T> & other);

  T x() const;
  T y() const;
  void set_x(T value);
  void set_y(T value);

  T & operator[](size_t i);
  T operator[](size_t i) const;

  T magnitude_square() const;
  T magnitude() const;
  void normalise();
  QcVector<T> normalised() const;
  inline bool is_null() const;

  bool operator==(const QcVector<T> & other) const;
  inline bool operator!=(const QcVector<T> & other) const;

  QcVector<T> & operator+=(const QcVector<T> & other);
  QcVector<T> & operator-=(const QcVector<T> & other);
  QcVector<T> & operator*=(T factor);
  QcVector<T> & operator/=(T factor);

  friend QcVector<T> operator+<>(const QcVector<T> & vector1, const QcVector<T> & vector2);
  friend QcVector<T> operator-<>(const QcVector<T> & vector1, const QcVector<T> & vector2);
  friend QcVector<T> operator*<>(const QcVector<T> & vector, T factor);
  friend QcVector<T> operator/<>(const QcVector<T> & vector, T factor);

  // Return the orientation in degree
  T orientation() const;
  // Return a new vector equal to self rotated of angle degree in the counter clockwise direction
  QcVector<T> rotate_counter_clockwise(T angle) const;
  // Return a new vector equal to self rotated of 90 degree in the counter clockwise direction
  QcVector<T> rotate_counter_clockwise_90() const;
  // Return a new vector equal to  rotated of 90 degree in the clockwise direction
  QcVector<T> rotate_clockwise_90() const;
  // Return a new vector equal to  rotated of 180 degree
  // parity
  QcVector<T> rotate_180() const;
  QcVector<T> mirror_x() const;
  QcVector<T> mirror_y() const;

  // Return the tangent
  T tan() const;
  // Return the inverse tangent
  T inverse_tan() const;

  // Return the dot product of  with other
  T dot(const QcVector<T> & other) const;
  // Return the cross product of  with other
  T cross(const QcVector<T> & other) const;
  // is parallel with other
  bool is_parallel(const QcVector<T> & other) const;
  // is orthogonal with other
  bool is_orthogonal(const QcVector<T> & other) const;
  // Return the cosinus of  with direction
  T cos_with(const QcVector<T> & direction) const;
  //  Return the projection of  on direction
  T projection_on(const QcVector<T> & direction) const;
  //  Return the sinus of  with other
  T sin_with(const QcVector<T> & direction) const;
  //  Return the deviation of  with other
  T deviation_with(const QcVector<T> & direction) const;
  T orientation_with(const QcVector<T> & direction) const;

  QcInterval2D<T> to_interval() const;

 private:
  T m_x;
  T m_y;
};

typedef QcVector<int> QcVectorInt;
typedef QcVector<double> QcVectorDouble;

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcVectorDouble & vector);
#endif

// #ifndef QT_NO_DATASTREAM
// QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcVector & vector);
// QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcVector & vector);
// #endif

/**************************************************************************************************/

// QT_END_NAMESPACE

/**************************************************************************************************/

#ifndef QC_MANUAL_INSTANTIATION
#include "vector.hxx"
#endif

/**************************************************************************************************/

#endif /* __VECTOR_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
