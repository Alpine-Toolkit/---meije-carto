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

#ifndef __SEGMENT_H__
#define __SEGMENT_H__

/**************************************************************************************************/

#include <cmath>
#include "math/qc_math.h"

// #include <QtCore/QMetaType>
#include <QDebug>

#include "qtcarto_global.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T>
class QC_EXPORT QcSegment
{
 public:
  typedef QcVector<T> Point;
  typedef QcSegment<T> Segment;

  QcSegment()
    : m_p1(), m_p2()
    {}

  QcSegment(const Point & p1, const Point & p2)
    : m_p1(p1), m_p2(p2)
    {}

  QcSegment(const Segment & other)
    : m_p1(other.m_p1), m_p2(other.m_p2)
    {}

  ~QcSegment()
    {}

  Segment & operator=(const Segment & other)
    {
      if (this != &other) {
	m_p1 = other.m_p1;
	m_p2 = other.m_p2;
      }

      return *this;
    }

  inline const Point & p1() const {
    return m_p1;
  }

  inline Point & p1() {
    return m_p1;
  }

  inline void set_p1(const Point & point) {
    m_p1 = point;
  }

  inline Point & p2() {
    return m_p2;
  }

  inline const Point & p2() const {
    return m_p2;
  }

  inline void set_p2(const Point & point) {
    m_p2 = point;
  }

 private:
  Point m_p1;
  Point m_p2;
};

typedef QcSegment<double> QcSegmentDouble;

/**************************************************************************************************/

// QT_END_NAMESPACE

/**************************************************************************************************/

#endif /* __SEGMENT_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
