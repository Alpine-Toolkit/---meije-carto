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

#ifndef __SIMD_H__
#define __SIMD_H__

/**************************************************************************************************/

#include <cmath>

// #include <QtCore/QMetaType>
#include <QtMath>
#include <QDebug>
#include <QPointF>
#include <QVector2D>

#include "qtcarto_global.h"
#include "math/qc_math.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T> class QcSimdPair;

template <typename T> QcSimdPair<T> operator+(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2);
template <typename T> QcSimdPair<T> operator-(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2);
template <typename T> QcSimdPair<T> operator*(const QcSimdPair<T> & simd, T factor);
template <typename T> QcSimdPair<T> operator/(const QcSimdPair<T> & simd, T factor);
template <typename T> QcSimdPair<T> operator*(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2);
template <typename T> QcSimdPair<T> operator/(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2);

template <typename T>
class QC_EXPORT QcSimdPair
{
 public:
  static constexpr int dimension() { return 2; }

 public:
  QcSimdPair();
  QcSimdPair(T x, T y);
  QcSimdPair(const QcSimdPair<T> & other);
  QcSimdPair(const QPoint & other);
  QcSimdPair(const QPointF & other);
  QcSimdPair(const QVector2D & other);
  ~QcSimdPair();

  QcSimdPair<T> & operator=(const QcSimdPair<T> & other);

  QPointF to_pointf() const;

  // bool is_valid();

  T x() const;
  T y() const;
  void set_x(T value);
  void set_y(T value);

  T & operator[](int i);
  T operator[](int i) const;

  bool operator==(const QcSimdPair<T> & other) const;
  bool operator!=(const QcSimdPair<T> & other) const;

  QcSimdPair<T> & operator+=(const QcSimdPair<T> & other);
  QcSimdPair<T> & operator-=(const QcSimdPair<T> & other);
  QcSimdPair<T> & operator*=(T factor);
  QcSimdPair<T> & operator/=(T factor);
  QcSimdPair<T> & operator*=(const QcSimdPair<T> & other);
  QcSimdPair<T> & operator/=(const QcSimdPair<T> & other);

  friend QcSimdPair<T> operator+<>(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2);
  friend QcSimdPair<T> operator-<>(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2);
  friend QcSimdPair<T> operator*<>(const QcSimdPair<T> & simd, T factor);
  friend QcSimdPair<T> operator/<>(const QcSimdPair<T> & simd, T factor);
  friend QcSimdPair<T> operator*<>(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2);
  friend QcSimdPair<T> operator/<>(const QcSimdPair<T> & simd1, const QcSimdPair<T> & simd2);

 private:
  T m_x;
  T m_y;
};

typedef QcSimdPair<int32_t> QcSimdPairInt32;
typedef QcSimdPair<int> QcSimdPairInt; // Fixme: double computation ???
typedef QcSimdPair<double> QcSimdPairDouble;

Q_DECLARE_METATYPE(QcSimdPairInt);
Q_DECLARE_METATYPE(QcSimdPairDouble);

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcSimdPairDouble & simd);
#endif

// #ifndef QT_NO_DATASTREAM
// QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcSimdPair & simd);
// QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcSimdPair & simd);
// #endif

/**************************************************************************************************/

template <typename T> class QcSimdTriplet;

template <typename T> QcSimdTriplet<T> operator+(const QcSimdTriplet<T> & simd1, const QcSimdTriplet<T> & simd2);
template <typename T> QcSimdTriplet<T> operator-(const QcSimdTriplet<T> & simd1, const QcSimdTriplet<T> & simd2);
template <typename T> QcSimdTriplet<T> operator*(const QcSimdTriplet<T> & simd, T factor);
template <typename T> QcSimdTriplet<T> operator/(const QcSimdTriplet<T> & simd, T factor);

template <typename T>
class QC_EXPORT QcSimdTriplet : public QcSimdPair<T>
{
 public:
  static constexpr int dimension() { return 3; }

 public:
  QcSimdTriplet();
  QcSimdTriplet(T x, T y, T z);
  QcSimdTriplet(const QcSimdTriplet<T> & other);
  ~QcSimdTriplet();

  QcSimdTriplet<T> & operator=(const QcSimdTriplet<T> & other);

  QcSimdPair<T> to_2d() const;

  T z() const;
  void set_z(T value);

  T & operator[](int i);
  T operator[](int i) const;

  bool operator==(const QcSimdTriplet<T> & other) const;
  bool operator!=(const QcSimdTriplet<T> & other) const;

  QcSimdTriplet<T> & operator+=(const QcSimdTriplet<T> & other);
  QcSimdTriplet<T> & operator-=(const QcSimdTriplet<T> & other);
  QcSimdTriplet<T> & operator*=(T factor);
  QcSimdTriplet<T> & operator/=(T factor);

  friend QcSimdTriplet<T> operator+<>(const QcSimdTriplet<T> & simd1, const QcSimdTriplet<T> & simd2);
  friend QcSimdTriplet<T> operator-<>(const QcSimdTriplet<T> & simd1, const QcSimdTriplet<T> & simd2);
  friend QcSimdTriplet<T> operator*<>(const QcSimdTriplet<T> & simd, T factor);
  friend QcSimdTriplet<T> operator/<>(const QcSimdTriplet<T> & simd, T factor);

 private:
  T m_z;
};

typedef QcSimdTriplet<int> QcSimdTripletInt; // Fixme: double computation ???
typedef QcSimdTriplet<double> QcSimdTripletDouble;

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcSimdTripletDouble & simd);
#endif

// #ifndef QT_NO_DATASTREAM
// QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcSimdTriplet & simd);
// QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcSimdTriplet & simd);
// #endif

/**************************************************************************************************/

// QT_END_NAMESPACE

/**************************************************************************************************/

#ifndef QC_MANUAL_INSTANTIATION
#include "simd.hxx"
#endif

/**************************************************************************************************/

#endif /* __SIMD_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
