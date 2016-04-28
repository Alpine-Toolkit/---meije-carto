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
#include <stdexcept>

#include <QDebug>

#include "qtcarto_global.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T> class QcInterval;

template <typename T> QcInterval<T> operator+(const QcInterval<T> & interval, T dx);
template <typename T> QcInterval<T> operator-(const QcInterval<T> & interval, T dx);
template <typename T> QcInterval<T> operator|(const QcInterval<T> & interval1, const QcInterval<T> & interval2);
template <typename T> QcInterval<T> operator&(const QcInterval<T> & interval1, const QcInterval<T> & interval2);

template <typename T>
class QC_EXPORT QcInterval
{
 public:
  QcInterval();
  QcInterval(T inf, T sup);
  QcInterval(const QcInterval<T> & other);

  QcInterval<T> &operator=(const QcInterval<T> & other);

  inline T inf() const { return m_inf; }
  inline T sup() const { return m_sup; }

  void set_inf(T value);
  void set_sup(T value);

  inline bool is_empty() const { return m_empty; }
  inline bool is_not_empty() const { return !m_empty; }

  T length() const;
  T center() const;

  bool operator==(const QcInterval<T> & other) const;
  bool operator!=(const QcInterval<T> & other) const;

  bool operator<(const QcInterval<T> & other) const;
  bool operator>(const QcInterval<T> & other) const;

  QcInterval<T> & operator+=(T dx);
  QcInterval<T> & operator-=(T dx);

  friend QcInterval<T> operator+<>(const QcInterval<T> & interval, T dx);
  friend QcInterval<T> operator-<>(const QcInterval<T> & interval, T dx);

  void enlarge(T dx);

  // Test if x is in the interval?
  bool contains(T x) const;
  T truncate(T x) const;
  T wrap(T x) const;

  // Test whether the interval intersects with i2?
  bool intersect(const QcInterval<T> & other) const;
  // Test whether the interval is included in i1?
  bool is_included_in(const QcInterval<T> & other) const;
  // Test whether the interval is outside of i2?
  bool is_outside_of(const QcInterval<T> & other) const;

  // Intersection
  QcInterval<T> & operator&=(const QcInterval<T> & other);
  friend QcInterval<T> operator&<>(const QcInterval<T> & interval1, const QcInterval<T> & interval2);

  QcInterval<T> cut(const QcInterval<T> & other,
		    QcInterval<T> & left,  QcInterval<T> & right,
		    bool & exchanged) const;

  // Union
  QcInterval<T> & operator|=(const QcInterval<T> & other);
  friend QcInterval<T> operator|<>(const QcInterval<T> & interval1, const QcInterval<T> & interval2);

 private:
  T m_inf;
  T m_sup;
  bool m_empty;
};

typedef QcInterval<int> QcIntervalInt;
typedef QcInterval<double> QcIntervalDouble;

template<> int QcIntervalInt::length() const;

template<> double QcIntervalDouble::wrap(double x) const;

template<>
QcIntervalInt QcIntervalInt::cut(const QcIntervalInt & other,
				 QcIntervalInt & left,  QcIntervalInt & right,
				 bool & exchanged) const;

template<>
QcIntervalDouble QcIntervalDouble::cut(const QcIntervalDouble & other,
				       QcIntervalDouble & left, QcIntervalDouble & right,
				       bool & exchanged) const;

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcIntervalInt & interval);
QC_EXPORT QDebug operator<<(QDebug debug, const QcIntervalDouble & interval);
#endif

// #ifndef QT_NO_DATASTREAM
// QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcIntervalInt & interval);
// QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcIntervalInt & interval);
// #endif

/**************************************************************************************************/

template <typename T>
class QC_EXPORT QcInterval2D
{
 public:
  QcInterval2D();
  QcInterval2D(T x_inf, T x_sup, T y_inf, T y_sup);
  QcInterval2D(const QcInterval<T> & x, const QcInterval<T> & y);
  QcInterval2D(const QcInterval2D<T> & other);

  QcInterval2D<T> & operator=(const QcInterval2D<T> & other);

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

  bool is_empty() const;

  bool operator==(const QcInterval2D<T> & other) const;
  bool operator!=(const QcInterval2D<T> & other) const;

  void enlarge(T dx);

  // Test if x is in the interval?
  bool contains(T x, T y) const;
  // Test whether the interval intersects with i2?
  bool intersect(const QcInterval2D<T> & other) const;
  // Test whether the interval is included in i1?
  bool is_included_in(const QcInterval2D<T> & other) const;
  // Test whether the interval is outside of i2?
  bool is_outside_of(const QcInterval2D<T> & other) const;

  // Intersection
  QcInterval2D<T> & operator&=(const QcInterval2D<T> & other);

  // Union
  QcInterval2D<T> & operator|=(const QcInterval2D<T> & other);

 private:
  QcInterval<T> m_x;
  QcInterval<T> m_y;
};

typedef QcInterval2D<int> QcInterval2DInt;
typedef QcInterval2D<double> QcInterval2DDouble;

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcInterval2DInt & interval);
QC_EXPORT QDebug operator<<(QDebug debug, const QcInterval2DDouble & interval);
#endif

// #ifndef QT_NO_DATASTREAM
// QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcInterval2DInt & interval);
// QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcInterval2DInt & interval);
// #endif

// QT_END_NAMESPACE

/**************************************************************************************************/

#ifndef QC_MANUAL_INSTANTIATION
#include "interval.hxx"
#endif

/**************************************************************************************************/

#endif /* __INTERVAL_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
