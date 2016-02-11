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

#ifndef __RATIONAL_H__
#define __RATIONAL_H__

/**************************************************************************************************/

#include <cmath>

// #include <QtCore/QMetaType>
#include <QDebug>

#include "qtcarto_global.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

unsigned int euclidian_recursive_gcd(unsigned int u, unsigned int v);
unsigned int euclidian_iterative_gcd(unsigned int u, unsigned int v);
unsigned int binary_recursive_gcd(unsigned int u, unsigned int v);
unsigned int binary_iterative_gcd(unsigned int u, unsigned int v);

/**************************************************************************************************/

template <typename T>
class QC_EXPORT QcRational
{
 public:
  QcRational()
    : QcRational(0, 0)
    {}

  QcRational(T numerator, T denominator)
    : m_numerator(numerator), m_denominator(denominator)
    {}

  QcRational(const QcRational<T> & other)
    : m_numerator(other.m_numerator), m_denominator(other.m_denominator)
    {}

  ~QcRational()
    {}

  QcRational<T> & operator=(const QcRational<T> & other)
    {
      if (this != &other) {
	m_numerator = other.m_numerator;
	m_denominator = other.m_denominator;
      }

      return *this;
    }

  inline T numerator() const {
    return m_numerator;
  }

  inline void set_numerator(T numerator) {
    m_numerator = numerator;
  }

  inline T denominator() const {
    return m_denominator;
  }

  inline void set_denominator(T denominator) {
    m_denominator = denominator;
  }

  double to_double() const {
    if (is_null())
      return .0;
    else if (is_one())
      return 1.;
    else if (is_infinite())
      // return +/- inf
      throw std::invalid_argument("Denominator is null");
    else
      return m_numerator / m_denominator;
  }

  inline bool is_null() const {
    return m_numerator == 0;
  }

  inline bool is_one() const {
    return m_numerator == m_denominator;
  }

  inline bool is_infinite() const {
    return m_denominator == 0;
  }

  bool operator==(const QcRational<T> & other) const {
    return (m_numerator == other.m_numerator) && (m_denominator == other.m_denominator);
  }
  inline bool operator!=(const QcRational<T> & other) const
  {
    return !operator==(other);
  }

  void simplify()

  QcRational<T> & operator+=(const QcRational<T> & other) {
    m_numerator *= other.m_denominator;
    m_denominator *= other.m_denominator;
    m_numerator += other.m_numerator * m_denominator
    return *this;
  }

  friend QcRational<T> operator+(const QcRational<T> & rational1, const QcRational<T> & rational2)
  {
    return QcRational(rational1.m_numerator * rational2.m_denominator + rational2.m_numerator * rational1*m_denominator,
		      rational1.m_denominator * rational2.m_denominator);
  }

  QcRational<T> & operator-=(const QcRational<T> & other) {
    m_numerator *= other.m_denominator;
    m_denominator *= other.m_denominator;
    m_numerator -= other.m_numerator * m_denominator
    return *this;
  }

  friend QcRational<T> operator-(const QcRational<T> & rational1, const QcRational<T> & rational2)
  {
    return QcRational(rational1.m_numerator * rational2.m_denominator - rational2.m_numerator * rational1*m_denominator,
		      rational1.m_denominator * rational2.m_denominator);
  }

  QcRational<T> & operator*=(const QcRational<T> & other) {
    m_numerator *= other.m_numerator;
    m_denominator *= other.m_denominator;
    return *this;
  }

  friend QcRational<T> operator*(const QcRational<T> & rational1, const QcRational<T> & rational2)
  {
    return QcRational(rational1.m_numerator * rational2.m_numerator,
		      rational1.m_denominator * rational2.m_denominator);
  }

  QcRational<T> & operator/=(const QcRational<T> & other) {
    m_numerator *= other.m_denominator;
    m_denominator *= other.m_numerator;
    return *this;
  }

  friend QcRational<T> operator/(const QcRational<T> & rational1, const QcRational<T> & rational2)
  {
    return QcRational(rational1.m_numerator * rational2.m_denominator,
		      rational1.m_denominator * rational2.m_numerator);
  }

  QcRational<T> & operator*=(T factor) {
    m_numerator *= factor;
    return *this;
  }

  friend QcRational<T> operator*(const QcRational<T> & rational, T factor)
  {
    return QcRational(rational.m_numerator * factor, rational.m_denominator);
  }

  QcRational<T> & operator/=(T factor) {
    m_denominator *= factor;
    return *this;
  }

  friend QcRational<T> operator/(const QcRational<T> & rational, T factor)
  {
    return QcRational(rational.m_numerator, rational.m_denominator * factor);
  }

 private:
  T m_numerator;
  T m_denominator;
};

typedef QcRational<int> QcRationalInt;
// typedef QcRational<double> QcRationalDouble;

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcRationalInt & rational)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug.nospace() << "QcRationalInt(";
  debug << rational.numerator();
  debug << ", ";
  debug << rational.denominator();
  debug << ')';

  return debug;
}
#endif

// #ifndef QT_NO_DATASTREAM
// QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcRational & rational);
// QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcRational & rational);
// #endif

/**************************************************************************************************/

// QT_END_NAMESPACE

/**************************************************************************************************/

#endif /* __RATIONAL_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
