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

class QC_EXPORT QcRational
{
 public:
  QcRational();
  QcRational(unsigned int numerator, unsigned int denominator, int sign = 1);
  QcRational(const QcRational & other);
  ~QcRational();

  QcRational & operator=(const QcRational & other);

  inline unsigned int numerator() const {
    return m_numerator;
  }

  inline void set_numerator(unsigned int numerator) {
    m_numerator = numerator;
  }

  inline unsigned int denominator() const {
    return m_denominator;
  }

  inline void set_denominator(unsigned int denominator) {
    m_denominator = denominator;
  }

  inline int sign() const {
    return m_denominator;
  }

  inline void set_sign(int sign) {
    m_sign = sign;
  }

  inline void inverse_sign() {
    m_sign = -m_sign;
  }

  inline bool is_negative() const {
    return m_sign < 0;
  }

  inline bool is_positive() const {
    return m_sign >= 0;
  }

  inline bool is_null() const {
    return m_numerator == 0;
  }

  inline bool is_infinite() const {
    return m_denominator == 0;
  }

  inline bool is_one() const {
    return m_numerator == m_denominator;
  }

  inline bool is_integer() const {
    return m_denominator == 1;
  }

  inline bool same_sign(const QcRational & other) const {
    return m_sign == other.m_sign;
  }

  inline bool operator==(const QcRational & other) const {
    return same_sign(other) && (m_numerator == other.m_numerator) && (m_denominator == other.m_denominator);
  }

  inline bool operator!=(const QcRational & other) const
  {
    return !operator==(other);
  }

  double to_double() const;
  void simplify();

  QcRational & operator*=(const QcRational & other);
  QcRational & operator*=(unsigned int factor);
  QcRational & operator+=(const QcRational & other);
  QcRational & operator-=(const QcRational & other);
  QcRational & operator/=(const QcRational & other);
  QcRational & operator/=(unsigned int factor);
  friend QcRational operator*(const QcRational & rational, unsigned int factor);
  friend QcRational operator*(const QcRational & rational1, const QcRational & rational2);
  friend QcRational operator+(const QcRational & rational1, const QcRational & rational2);
  friend QcRational operator-(const QcRational & rational1, const QcRational & rational2);
  friend QcRational operator/(const QcRational & rational, unsigned int factor);
  friend QcRational operator/(const QcRational & rational1, const QcRational & rational2);

 private:
  unsigned int m_numerator;
  unsigned int m_denominator;
  int m_sign;
};

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcRational & rational)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug.nospace() << "QcRational(";
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
