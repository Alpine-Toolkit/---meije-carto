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

#ifndef __INTERVAL_H__
#define __INTERVAL_H__

/**************************************************************************************************/

#include <algorithm>

// #include <QtCore/QMetaType>

#include "qtcarto_global.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T>
class QC_EXPORT QcInterval
{
 public:
  QcInterval()
    : m_inf(0), m_sup(0), m_empty(true)
    {}

  QcInterval(T inf, T sup)
    : m_inf(0), m_sup(0), m_empty(false)
    {
      if (inf <= sup) {
	m_inf = inf;
	m_sup = sup;
      }
      else
	throw std::invalid_argument("inf > sup");
    }

  QcInterval(const QcInterval<T> & other)
    : m_inf(other.m_inf), m_sup(other.m_sup), m_empty(other.m_empty)
    {}

  QcInterval<T> &operator=(const QcInterval<T> & other)
    {
      if (this != &other) {
	m_inf = other.m_inf;
	m_sup = other.m_sup;
	m_empty = other.m_empty;
      }

      return *this;
    }

  inline T inf() const {
    return m_inf;
  }

  inline void set_inf(T value) {
    // Fixme: check
    m_inf = value;
  }

  inline T sup() const {
    return m_sup;
  }

  inline void set_sup(T value) {
    m_sup = value;
  }

  inline bool is_empty() const {
    return m_empty;
  }

  inline T length() const {
    return m_sup - m_inf;
  }

  inline T center() const {
    return (m_sup + m_inf) / 2; // Fixme: double ?
  }

  bool operator==(const QcInterval<T> & other) const {
    return (m_empty == other.m_empty) && (m_inf == other.m_inf) && (m_sup == other.m_sup);
  }
  inline bool operator!=(const QcInterval<T> & other) const
  {
    return !operator==(other);
  }

  bool operator<(const QcInterval<T> & other) const {
    return m_sup < other.m_inf;
  }

  bool operator>(const QcInterval<T> & other) const {
    return other.m_sup < m_inf;
  }

  QcInterval<T> & operator+=(T dx) {
    if (! m_empty) {
      m_inf += dx;
      m_sup += dx;
    }
    return *this;
  }

  friend QcInterval<T> operator+(const QcInterval<T> & interval, T dx)
  {
    if (interval.m_empty)
      return QcInterval();
    else
      return QcInterval(interval.m_inf + dx, interval.m_sup + dx);
  }

  QcInterval<T> & operator-=(T dx) {
    if (! m_empty) {
      m_inf -= dx;
      m_sup -= dx;
    }
    return *this;
  }

  friend QcInterval<T> operator-(const QcInterval<T> & interval, T dx)
  {
    if (interval.m_empty)
      return QcInterval();
    else
      return QcInterval(interval.m_inf - dx, interval.m_sup - dx);
  }

  void enlarge(T dx) {
    if (! m_empty) {
      m_inf -= dx;
      m_sup += dx;
    }
  }

  // Test if x is in the interval?
  bool contains(T x) const {
    if (m_empty)
      return false;
    else
      return m_inf <= x && x <= m_sup;
  }

  // Test whether the interval intersects with i2?
  bool intersect(const QcInterval<T> & other) {
    if (m_empty || other.m_empty)
      return false;
    else
      return m_inf <= other.sup && other.inf <= m_sup;
  }

  // Test whether the interval is included in i1?
  bool is_included_in(const QcInterval<T> & other) {
    return other.inf <= m_inf && m_sup <= other.sup;
  }

  // Test whether the interval is outside of i2?
  bool is_outside_of(const QcInterval<T> & other) {
    if (m_empty || other.m_empty)
      return false;
    else
      return m_inf < other.inf && other.sup < m_sup;
  }

  // Intersection
  QcInterval<T> & operator&=(const QcInterval<T> & other) {
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

  friend QcInterval<T> operator&(const QcInterval<T> & interval1, const QcInterval<T> & interval2)
  {
    if (interval1.intersect(interval2)) {
      return QcInterval(std::max(interval1.m_inf, interval2.m_inf),
			std::min(interval1.m_sup, interval2.m_sup));
    }
    else {
      return QcInterval();
    }
  }

  // Union
  QcInterval<T> & operator|=(const QcInterval<T> & other) {
    if (!(m_empty || other.m_empty)) {
      m_inf = std::min(m_inf, other.m_inf);
      m_sup = std::max(m_sup, other.m_sup);
    }
    else if (m_empty && !other.m_empty) {
      m_inf = other.m_inf;
      m_sup = other.m_sup;
    }
    return *this;
  }

  friend QcInterval<T> operator|(const QcInterval<T> & interval1, const QcInterval<T> & interval2)
  {
    if (!(interval1.m_empty || interval2.m_empty))
      return QcInterval(std::min(interval1.m_inf, interval2.m_inf),
			std::max(interval1.m_sup, interval2.m_sup));
    else if (interval1.m_empty && !interval2.m_empty)
      return QcInterval(*interval2);
    else
      return QcInterval(*interval1);
  }

 private:
  T m_inf;
  T m_sup;
  bool m_empty;
};

/**************************************************************************************************/

template <typename T>
class QC_EXPORT QcInterval2D
{
 public:
  QcInterval2D()
    : m_x(), m_y()
    {}

  QcInterval2D(T x_inf, T x_sup, T y_inf, T y_sup)
    : m_x(x_inf, x_sup), m_y(y_inf, y_sup)
    {}

  QcInterval2D(const QcInterval<T> & x, const QcInterval<T> & y)
    : m_x(x), m_y(y)
    {}

  QcInterval2D(const QcInterval2D & other)
    : m_x(other.m_x), m_y(other.m_y)
    {}

  QcInterval2D & operator=(const QcInterval2D & other)
    {
      if (this != &other) {
	m_x = other.m_x;
	m_y = other.m_y;
      }

      return *this;
    }

  inline QcInterval<T> & x() {
    return m_x;
  }

  inline QcInterval<T> & y() {
    return m_y;
  }

  inline const QcInterval<T> & x() const {
    return m_x;
  }

  inline const QcInterval<T> & y() const {
    return m_y;
  }

  inline bool is_empty() const {
    return x.is_empty() || y.is_empty();
  }

  bool operator==(const QcInterval2D & other) const {
    return (m_x == other.m_x) && (m_y == other.m_y);
  }
  inline bool operator!=(const QcInterval2D & other) const
  {
    return !operator==(other);
  }

  void enlarge(T dx) {
    m_x.enlarge(dx);
    m_y.enlarge(dx);
  }

  // Test if x is in the interval?
  bool contains(T x, T y) const {
    return m_x.contains(x) && m_y.contains(y);
  }

  // Test whether the interval intersects with i2?
  bool intersect(const QcInterval2D & other) {
    return m_x.intersect(other.m_x) && m_y.intersect(other.m_y);
  }

  // Test whether the interval is included in i1?
  bool is_included_in(const QcInterval2D & other) {
    return m_x.is_included_in(other.m_x) && m_y.is_included_in(other.m_y);
  }

  // Test whether the interval is outside of i2?
  bool is_outside_of(const QcInterval2D & other) {
    return m_x.is_outside_of(other.m_x) && m_y.is_outside_of(other.m_y);
  }

  // Intersection
  QcInterval2D & operator&=(const QcInterval2D & other) {
    m_x &= other.m_x;
    m_y &= other.m_y;
    return *this;
  }

  /*
  QcInterval2D operator&(const QcInterval2D & interval1, const QcInterval2D & T interval2) {
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
  QcInterval2D & operator|=(const QcInterval2D & other) {
    m_x |= other.m_x;
    m_y |= other.m_y;
    return *this;
  }

  /*
  QcInterval2D operator|(const QcInterval2D & interval1, const QcInterval2D & T interval2) {
    if (!(interval1.is_empty() || interval2.is_empty()))
      return QcInterval2D(interval1.m_x | interval2.m_x,
			  interval1.m_y | interval2.m_y);
    else if (interval1.empty() && !interval2.is_empty())
      return QcInterval(*interval2);
    else:
      return QcInterval(*interval1);
  }
  */

 private:
  QcInterval<T> m_x;
  QcInterval<T> m_y;
};

typedef QcInterval<int> QcIntervalInt;
typedef QcInterval<double> QcIntervalDouble;

typedef QcInterval2D<int> QcInterval2DInt;
typedef QcInterval2D<double> QcInterval2DDouble;

// QT_END_NAMESPACE

/**************************************************************************************************/

#endif /* __INTERVAL_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
