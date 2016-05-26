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

#ifndef __PROJECTION_HXX__
#define __PROJECTION_HXX__

/**************************************************************************************************/

#include <QDataStream>
#include <QDebug>

/**************************************************************************************************/

template <typename Projection>
const Projection QcGeoCoordinate<Projection>::projection;

template <typename Projection>
QcGeoCoordinate<Projection>::QcGeoCoordinate()
  : m_x(qQNaN()), m_y(qQNaN())
{}

template <typename Projection>
QcGeoCoordinate<Projection>::QcGeoCoordinate(double x, double y)
  : m_x(qQNaN()), m_y(qQNaN())
{
  if (projection.is_valid_xy(x, y)) {
    m_x = x;
    m_y = y;
  } else
    throw std::invalid_argument("Invalid coordinate");
}

template <typename Projection>
QcGeoCoordinate<Projection>::QcGeoCoordinate(const QcGeoCoordinate & other)
  : m_x(other.m_x), m_y(other.m_y)
{}

template <typename Projection>
QcGeoCoordinate<Projection>::~QcGeoCoordinate()
{}

// Fixme: default
template <typename Projection>
QcGeoCoordinate<Projection> &
QcGeoCoordinate<Projection>::operator=(const QcGeoCoordinate<Projection> & other)
{
  if (this != &other) {
    m_x = other.m_x;
    m_y = other.m_y;
  }

  return *this;
}

template <typename Projection>
bool
QcGeoCoordinate<Projection>::operator==(const QcGeoCoordinate<Projection> & other) const
{
  bool x_equal = ((qIsNaN(m_x) and qIsNaN(other.m_x))
                  || qFuzzyCompare(m_x, other.m_x));
  bool y_equal = ((qIsNaN(m_y) and qIsNaN(other.m_y))
                  || qFuzzyCompare(m_y, other.m_y));

  return (x_equal and y_equal);
}

template <typename Projection>
bool
QcGeoCoordinate<Projection>::is_valid() const
{
  return !(qIsNaN(m_x) or qIsNaN(m_y));
}

/*
#ifdef WITH_PROJ4
template <typename Projection>
void
QcGeoCoordinate<Projection>::transform(QcGeoCoordinate<Projection> & other)
{
  const QcProjection4 projection4_from = projection.projection4();
  const QcProjection4 projection4_to = other.projection.projection4();
  double _x = x();
  double _y = y();
  if (projection4_from.is_latlong()) {
    _x = qDegreesToRadians(_x);
    _y = qDegreesToRadians(_y);
  }
  projection4_from.transform(projection4_to, _x, _y);
  other.set_x(_x);
  other.set_y(_y);
}
#endif
*/

#ifndef QT_NO_DEBUG_STREAM
template <typename Projection>
QDebug operator<<(QDebug debug, const QcGeoCoordinate<Projection> & coordinate)
{
  QDebugStateSaver saver(debug);
  double x = coordinate.x();
  double y = coordinate.y();
  QString class_name = QLatin1Literal("QcGeoCoordinate<") + coordinate.projection.srid() + QLatin1Literal(">(");

  debug.nospace() << class_name.toStdString().c_str();
  if (qIsNaN(x))
    debug << '?';
  else
    debug << x;
  debug << ", ";
  if (qIsNaN(y))
    debug << '?';
  else
    debug << y;
  debug << ')';

  return debug;
}
#endif

#ifndef QT_NO_DATASTREAM
template <typename Projection>
QDataStream & operator<<(QDataStream & stream, const QcGeoCoordinate<Projection> & coordinate)
{
  stream << coordinate.x();
  stream << coordinate.y();
  return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
template <typename Projection>
QDataStream & operator>>(QDataStream & stream, QcGeoCoordinate<Projection> & coordinate)
{
  double value;
  stream >> value;
  coordinate.set_x(value);
  stream >> value;
  coordinate.set_y(value);
  return stream;
}
#endif

/**************************************************************************************************/

#endif /* __PROJECTION_HXX__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
