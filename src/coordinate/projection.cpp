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

#include "projection.h"

#include "laea.h"
#include "mercator.h"
#include "wgs84.h"

#include <QtMath>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

#ifdef WITH_PROJ4
QcProjection4::QcProjection4(const QString & definition, projCtx context)
  : m_definition(definition),
    m_projection(nullptr)
{
  qInfo() << definition;

  // cf. https://trac.osgeo.org/proj/wiki/ThreadSafety
  if (!context)
    context = pj_get_default_ctx();
  // context = pj_ctx_alloc();

  m_projection = pj_init_plus_ctx(context, definition.toStdString().c_str());
}

QcProjection4::~QcProjection4()
{
  qInfo() << m_definition;
  // Fixme: segfault
  // if (m_projection)
  //   pj_free(m_projection);
}

void
QcProjection4::transform(const QcProjection4 & proj2, double & x, double & y) const
{
  double z = .0;
  return transform(proj2, x, y, z);
}

void
QcProjection4::transform(const QcProjection4 & proj2, double & x, double & y, double & z) const
{
  // int pj_transform( projPJ src, projPJ dst, long point_count, int point_offset,
  //                 double *x, double *y, double *z );

  long point_count = 1;
  int point_offset = 1;
  int error = pj_transform(m_projection, proj2.m_projection, point_count, point_offset, &x, &y, &z);
  if (error != 0)
    throw; // Fixme: (pj_strerrno(error))
}

bool
QcProjection4::is_latlong() const {
  return pj_is_latlong(m_projection);
}
#endif

/**************************************************************************************************/

QMap<QString, QcProjection *> QcProjection::m_instances;

const QcProjection *
QcProjection::by_srid(const QString & srid)
{
  if (m_instances.isEmpty())
    init();
  if (m_instances.contains(srid))
    return m_instances.value(srid);
  else {
    qWarning() << "SRID not found";
    return nullptr;
  }
}

void
QcProjection::init()
{
  register_projection(new QcWgs84Projection());
  register_projection(new QcWebMercatorProjection());
  register_projection(new QcLaeaProjection_3571());
}

void
QcProjection::register_projection(QcProjection * projection)
{
  const QString & srid = projection->srid();
  if (!m_instances.contains(srid))
    m_instances.insert(srid, projection);
}


QcProjection::QcProjection()
  : m_srid(),
    m_extent(),
    m_preserve_bit(),
    m_projection4()
{}

QcProjection::QcProjection(const QString & srid,
                           const QcInterval2DDouble & extent,
                           PreserveBit preserve_bit)
  : m_srid(srid),
    m_extent(extent),
    m_preserve_bit(preserve_bit)
#ifdef WITH_PROJ4
  , m_projection4(new QcProjection4(proj4_definition()))
#endif
{
  // qInfo() << "Build " + srid + " projection";
}

QcProjection::QcProjection(const QcProjection & other)
  : m_srid(other.m_srid),
    m_extent(other.m_extent),
    m_preserve_bit(other.m_preserve_bit)
#ifdef WITH_PROJ4
  , m_projection4(other.m_projection4)
#endif
{}

// Fixme: default
QcProjection &
QcProjection::operator=(const QcProjection & other)
{
  if (this != &other) {
    m_srid = other.m_srid;
    m_extent = other.m_extent;
    m_preserve_bit = other.m_preserve_bit;
#ifdef WITH_PROJ4
    m_projection4 = other.m_projection4;
#endif
  }

  return *this;
}

bool
QcProjection::operator==(const QcProjection & other) const
{
  return (m_srid == other.m_srid and
          m_extent == other.m_extent and
          m_preserve_bit == other.m_preserve_bit);
}

QcGeoCoordinate
QcProjection::coordinate(double x, double y) const
{
  return QcGeoCoordinate(this, x, y);
}

#ifdef WITH_PROJ4
QString
QcProjection::proj4_definition() const
{
  return QString("+init=") + srid();
}
#endif

/**************************************************************************************************/

QcGeoCoordinateTrait::QcGeoCoordinateTrait()
  : m_x(qQNaN()), m_y(qQNaN())
{}

QcGeoCoordinateTrait::QcGeoCoordinateTrait(const QcGeoCoordinateTrait & other)
  : m_x(other.m_x), m_y(other.m_y)
{}

// Fixme: default
QcGeoCoordinateTrait &
QcGeoCoordinateTrait::operator=(const QcGeoCoordinateTrait & other)
{
  if (this != &other) {
    m_x = other.m_x;
    m_y = other.m_y;
  }

  return *this;
}

bool
QcGeoCoordinateTrait::operator==(const QcGeoCoordinateTrait & other) const
{
  bool x_equal = ((qIsNaN(m_x) and qIsNaN(other.m_x))
                  || qFuzzyCompare(m_x, other.m_x));
  bool y_equal = ((qIsNaN(m_y) and qIsNaN(other.m_y))
                  || qFuzzyCompare(m_y, other.m_y));

  return (x_equal and y_equal);
}

bool
QcGeoCoordinateTrait::is_valid() const
{
  return !(qIsNaN(m_x) or qIsNaN(m_y));
}

#ifdef WITH_PROJ4
void
QcGeoCoordinateTrait::transform(QcGeoCoordinateTrait & other) const
{
  const QcProjection4 projection4_from = projection().projection4();
  const QcProjection4 projection4_to = other.projection().projection4();
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

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const QcGeoCoordinateTrait & coordinate)
{
  QDebugStateSaver saver(debug);
  double x = coordinate.x();
  double y = coordinate.y();
  QString class_name = QLatin1Literal("QcGeoCoordinate<") + coordinate.projection().srid() + QLatin1Literal(">(");

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
QDataStream & operator<<(QDataStream & stream, const QcGeoCoordinateTrait & coordinate)
{
  stream << coordinate.x();
  stream << coordinate.y();
  return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream & operator>>(QDataStream & stream, QcGeoCoordinateTrait & coordinate)
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

QcGeoCoordinate::QcGeoCoordinate(const QcProjection * projection, double x, double y)
  : QcGeoCoordinateTrait(),
    m_projection(projection)
{
  if (m_projection->is_valid_xy(x, y)) {
    set_x(x);
    set_y(y);
  } else
    throw std::invalid_argument("Invalid coordinate");
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
