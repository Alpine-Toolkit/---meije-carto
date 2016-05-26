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

#include <QtMath>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

#ifdef WITH_PROJ4
QcProjection4::QcProjection4(const QString & definition, projCtx context)
  : m_projection(nullptr)
{
  // cf. https://trac.osgeo.org/proj/wiki/ThreadSafety
  if (!context)
    context = pj_get_default_ctx();
  // context = pj_ctx_alloc();

  m_projection = pj_init_plus_ctx(context, definition.toStdString().c_str());
}

QcProjection4::~QcProjection4()
{
  if (m_projection)
    pj_free(m_projection);
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

QcProjection::QcProjection(const QString & srid,
                           const QcInterval2DDouble & extent,
                           PreserveBit preserve_bit)
  : m_srid(srid),
    m_extent(extent),
    m_preserve_bit(preserve_bit)
{
  // qInfo() << "Build " + srid + " projection";
}

#ifdef WITH_PROJ4
QString
QcProjection::proj4_definition() const
{
  return QString("+init=") + srid();
}

QcProjection4 &
QcProjection::projection4() const
{
  if (!m_projection4)
    m_projection4 = new QcProjection4(proj4_definition()); // Fixme: never deleted ?

  return *m_projection4;
}
#endif

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
