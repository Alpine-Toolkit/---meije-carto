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

#ifndef __LINE_H__
#define __LINE_H__

/**************************************************************************************************/

// #include <QtCore/QMetaType>
#include <QDebug>

#include "qtcarto_global.h"
#include "vector.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T>
class QC_EXPORT QcLine
{
 public:
  typedef QcVector<T> Vector;
  typedef QcLine<T> Line;

 public:
  static Line from_two_points(Vector point1, Vector point2) {
    return QcLine(point1, point2 - point1);
  }

 public:
  QcLine()
    : m_point(), m_vector()
    {}

  QcLine(Vector point, Vector vector)
    : m_point(point), m_vector(vector)
  {}

  QcLine(const Line & other)
    : m_point(other.m_point), m_vector(other.m_vector)
    {}

  ~QcLine()
    {}

  Line & operator=(const Line & other)
    {
      if (this != &other) {
	m_point = other.m_point;
	m_vector = other.m_vector;
      }

      return *this;
    }

  inline const Vector & point() const {
    return m_point;
  }

  inline void set_point(Vector value) {
    m_point = value;
  }

  inline const Vector & vector() const {
    return m_vector;
  }

  inline void set_vector(Vector value) {
    m_vector = value;
  }

  // Return the Point corresponding to the curvilinear abscissa s
  T point_at_s(T s) const {
    return m_point + (m_vector * s);
  }

  // Compute distance between two abscissae
  T compute_distance_between_abscissae(T s0, T s1) const {
    return fabs(s1 - s0) * m_vector.magnitude();
  }

  // Return y corresponding to x
  T get_y_from_x(T x) {
    return m_vector.tan() * (x - m_point.x()) + m_point.y();
  }

  // Return x corresponding to y
  T get_x_from_y(T y) {
    return m_vector.inverse_tan() * (y - m_point.y()) + m_point.x();
  }

  // Fixme: is_parallel_to
  // Self is parallel to other
  bool is_parallel(const Line & other) const {
    return m_vector.is_parallel(other.m_vector);
  }

  // Self is orthogonal to other
  bool is_orthogonal(const Line & other) {
    return m_vector.is_orthogonal(other.m_vector);
  }

  // Return the shifted parallel line
  Line shifted_parallel_line(T shift) {
    Vector n = m_vector.rotate_counter_clockwise_90();
    n.normalise();
    Vector point = m_point + n*shift;

    return Line(point, m_vector);
  }

  // Return the orthogonal line at abscissa s
  Line orthogonal_line_at_abscissa(T s) {
    Vector point = m_pointoint_at_s(s);
    Vector vector = m_vector.rotate_counter_clockwise_90();

    return Line(point, vector);
  }

  // Return the intersection abscissae between l1 and l2
  bool intersection_abscissae(const Line & l1, const Line & l2, T & s1, T & s2) {
    // l1 = p1 + s1*v1
    // l2 = p2 + s2*v2
    // delta = p2 - p1 = s1*v1 - s2*v2
    // delta x v2 = s1 * v1 x v2
    // delta x v1 = s2 * v1 x v2

    if (l1.is_parallel(l2))
      return false;
    else {
      T n = 1. / l1.m_vector.cross(l2.m_vector);
      Vector delta = l2.m_point - l1.m_pointoint_at_s;
      s1 = delta.cross(l2.m_vector) * n;
      s2 = delta.cross(l1.m_vector) * n;
      return true;
    }
  }

  // Return the intersection Point between self and other
  bool intersection(const Line & other, Vector & point) {
    T s0, s1;
    bool intersect = intersection_abscissae(other, s0, s1);
    if (intersect)
      return false;
    else {
      point = point_at_s(s0);
      return true;
    }
  }

  // Return the abscissa corresponding to the perpendicular projection of a point to the line
  T projected_abscissa(const Vector & point) {
    Vector delta = point - m_point;
    return delta.projection_on(m_vector);
  }

  // Return the distance of a point to the line
  T distance_to_line(const Vector & point) {
    Vector delta = point - m_point;
    return delta.deviation_with(m_vector);
  }

  // Return the distance of a point to the line
  void distance_and_abscissa_to_line(const Vector & point, T & d, T & s) {
    Vector delta = point - m_point;
    d = delta.deviation_with(m_vector);
    s = delta.projection_on(m_vector);
  }

 private:
  Vector m_point;
  Vector m_vector;
};

typedef QcLine<int> QcLineInt;
typedef QcLine<double> QcLineDouble;

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcLineDouble & line)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug.nospace() << "QcLineDouble(";
  debug << line.point();
  debug << ", ";
  debug << line.vector();
  debug << ')';

  return debug;
}
#endif

// #ifndef QT_NO_DATASTREAM
// QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcLine & line);
// QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcLine & line);
// #endif

/**************************************************************************************************/

// QT_END_NAMESPACE

/**************************************************************************************************/

#endif /* __LINE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
